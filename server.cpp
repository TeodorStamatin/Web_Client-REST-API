#include <bits/stdc++.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "server.h"
#include "requests.h"
#include "helpers.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * Function used to send a HTTP request to the server and receive the response.
 * It also handles the case when the request is a GET, POST or DELETE.
 * @param body - The JSON body to send with the request (only for POST).
 * @param sockfd - The socket file descriptor.
 * @param host - The host of the server.
 * @param url - The url of the request.
 * @param method - The method of the request.
 * @param cookies - The cookies to send with the request (only for GET).
 * @param token - The token to send with the request (only for GET).
 * @return The response from the server.
*/
char *send_and_receive(nlohmann::json body, int sockfd, const char *host, const char *url, const char *method, char *cookies, char *token) {
    char *message = NULL;
    char *response = NULL;

    if (strcmp(method, "GET") == 0) {
        message = compute_get_request(host, url, cookies, token);
    } else if (strcmp(method, "POST") == 0) {
        char *body_buff = (char *)malloc(body.dump().length() + 1);
        if (!body_buff) {
            fprintf(stderr, "malloc() failed");
            return NULL;
        }
        strcpy(body_buff, body.dump().c_str());
        message = compute_post_request(host, url, "application/json", body_buff, cookies, token);
        free(body_buff);
    } else if (strcmp(method, "DELETE") == 0) {
        message = delete_request(host, url, cookies, token);
    }

    if (message != NULL) {
        send_to_server(sockfd, message);
        response = receive_from_server(sockfd);
        free(message);
    }

    return response;
}

/**
 * Function used to register a user.
 * It reads the username and password from stdin and sends a POST request to the server.
 * @param sockfd - The socket file descriptor.
*/
void register_command(int sockfd) {
    char username[LINELEN];
    char password[LINELEN];
    char buff[LINELEN];
    char dummy[LINELEN];
    int rc;

    printf("username=");
    fgets(buff, LINELEN, stdin);

    rc = sscanf(buff, "%s %[^\n\t ]", username, dummy);
    if (rc != 1)
        printf("EROARE : Invalid username!\n");

    printf("password=");
    fgets(buff, LINELEN, stdin);

    rc = sscanf(buff, "%s %[^\n\t ]", password, dummy);
    if (rc != 1)
        printf("EROARE : Invalid password!\n");

    json body;
    body["username"] = username;
    body["password"] = password;

    char *response = send_and_receive(body, sockfd, SERVER, REGISTER, "POST", NULL, NULL);

    if (strstr(response, "error") != NULL) {
        json error = json::parse(strstr(response, "{"));
        printf("EROARE : %s\n", error.value("error", "").c_str());
    } else {
        printf("SUCCES : Utilizator înregistrat cu succes!\n");
    }

    free(response);
}

/**
 * Function used to login a user.
 * It reads the username and password from stdin and sends a POST request to the server.
 * @param sockfd - The socket file descriptor.
 * @param cookie - The cookie received from the server.
*/
void login_command(int sockfd, char **cookie) {
    char username[LINELEN];
    char password[LINELEN];
    char buff[LINELEN];
    char dummy[LINELEN];
    int rc;

    printf("username=");
    fgets(buff, LINELEN, stdin);

    rc = sscanf(buff, "%s %[^\n\t ]", username, dummy);
    if (rc != 1)
        printf("EROARE : Invalid username!\n");

    printf("password=");
    fgets(buff, LINELEN, stdin);

    rc = sscanf(buff, "%s %[^\n\t ]", password, dummy);
    if (rc != 1)
        printf("EROARE : Invalid password!\n");

    json body;
    body["username"] = username;
    body["password"] = password;

    char *response = send_and_receive(body, sockfd, SERVER, LOGIN, "POST", NULL, NULL);

    if (strstr(response, "error")) {
        json error = json::parse(strstr(response, "{"));
        printf("EROARE : %s\n", error.value("error", "").c_str());
    } else {
        char *cookie_start = strstr(response, "Set-Cookie: ");
        if (cookie_start != NULL) {
            char dummy[LINELEN];
            sscanf(cookie_start, "Set-Cookie: %s", dummy);

            *cookie = (char *)malloc(strlen(dummy) + 1);
            if (!*cookie) {
                fprintf(stderr, "malloc() failed");
            } else {
                strcpy(*cookie, strtok(dummy, ";"));
                printf("SUCCES : Utilizatorul a fost logat cu succes!\n");
            }
        }
    }

    free(response);
}

/**
 * Function used to enter the library.
 * It sends a GET request to the server and returns a token on successful access.
 * @param sockfd - The socket file descriptor.
 * @param cookie - The cookie received from the server.
 * @return The token received from the server.
*/
char *enter_library_command(int sockfd, char* cookie) {

    char *response = send_and_receive(NULL, sockfd, SERVER, LIBRARY, "GET", cookie, NULL);

    char *token  = NULL;

    if (strstr(response, "error")) {
        json error = json::parse(strstr(response, "{"));
        printf("EROARE : %s\n", error.value("error", "").c_str());
    } else {
        char *token_start = strstr(response, "token\":\"");
        if (token_start != NULL) {
            char dummy[LINELEN];
            sscanf(token_start, "token\":\"%[^\"]", dummy);

            token = (char *)malloc(strlen(dummy) + 1);
            if (!token) {
                fprintf(stderr, "malloc() failed");
            } else {
                strcpy(token, dummy);
                printf("SUCCES : Utilizatorul are acces la biblioteca!\n");
            }
        } else {
            printf("EROARE : Tokenul nu a fost gasit!\n");
        }
    }

    free(response);
    return token;
}

/**
 * Function used to get all the books from the library.
 * It sends a GET request to the server and prints the books.
 * @param sockfd - The socket file descriptor.
 * @param token - The token received from the server.
*/
void get_books_command(int sockfd, char *token) {
    
    char *response = send_and_receive(NULL, sockfd, SERVER, BOOKS, "GET", NULL, token);
    
    if (strstr(response, "[")) {
        json books = json::parse(strstr(response, "["));
        char *books_str = (char *)malloc(books.dump(4).length() + 1);
        strcpy(books_str, books.dump(4).c_str());
        printf("%s\n", books_str);
        free(books_str);
    } else {
        json error = json::parse(strstr(response, "{"));
        printf("EROARE : %s\n", error.value("error", "").c_str());
    }

    free(response);
}

/**
 * Function used to get a specific book from the library.
 * It reads the id of the book from stdin and sends a GET request to the server.
 * @param sockfd - The socket file descriptor.
 * @param token - The token received from the server.
*/
void get_book_command(int sockfd, char *token) {
    char id[LINELEN];
    char buff[LINELEN];
    char dummy[LINELEN];
    char path[LINELEN];
    int rc;

    printf("id=");
	fgets(buff, LINELEN, stdin);
	rc = sscanf(buff, "%s %[^\n\t ]", id, dummy);
	if (rc != 1) {
		printf("EROARE : Invalid id.\n");
		return;
	}
    
    strcpy(path, BOOK);
    strcat(path, id);

    char *response = send_and_receive(NULL, sockfd, SERVER, path, "GET", NULL, token);

    json parser = json::parse(strstr(response, "{"));
    if (parser.contains("error")) {
        printf("EROARE : %s\n", parser.value("error", "").c_str());
    } else {
        char *book_str = (char *)malloc(parser.dump(4).length() + 1);
        strcpy(book_str, parser.dump(4).c_str());
        printf("%s\n", book_str);
        free(book_str);
    }

    free(response);
}

/**
 * Function used to add a book to the library.
 * It reads the book details from stdin and sends a POST request to the server.
 * @param sockfd - The socket file descriptor.
 * @param token - The token received from the server.
*/
void add_book_command(int sockfd, char *token) {
    char title[LINELEN];
    char author[LINELEN];
    char genre[LINELEN];
    char publisher[LINELEN];
    char page_count[LINELEN];

    printf("title=");
    fgets(title, LINELEN, stdin);
    title[strcspn(title, "\n")] = '\0';

    printf("author=");
    fgets(author, LINELEN, stdin);
    author[strcspn(author, "\n")] = '\0';

    printf("genre=");
    fgets(genre, LINELEN, stdin);
    genre[strcspn(genre, "\n")] = '\0';

    printf("publisher=");
    fgets(publisher, LINELEN, stdin);
    publisher[strcspn(publisher, "\n")] = '\0';

    printf("page_count=");
    char buff[LINELEN];
    fgets(buff, LINELEN, stdin);
    strtok(buff, "\n");

    if (strspn(buff, "0123456789") == strlen(buff)) {
        strcpy(page_count, buff);
    } else {
        printf("EROARE : Tip de date incorect pentru numarul de pagini\n");
        return;
    }

    if (strlen(title) == 0 || strlen(author) == 0 || strlen(genre) == 0 || strlen(publisher) == 0) {
        printf("EROARE : Date invalide!\n");
        return;
    }

    json body;
    body["title"] = title;
    body["author"] = author;
    body["genre"] = genre;
    body["page_count"] = page_count;
    body["publisher"] = publisher;

    char *response = send_and_receive(body, sockfd, SERVER, BOOKS, "POST", NULL, token);

    if (strstr(response, "error") != NULL) {
        json error = json::parse(strstr(response, "{"));
        printf("EROARE : %s\n", error.value("error", "").c_str());
    } else {
        printf("SUCCES : S-a adaugat cartea %s\n", title);
    }

    free(response);
}

/**
 * Function used to delete a book from the library.
 * It reads the id of the book from stdin and sends a DELETE request to the server.
 * @param sockfd - The socket file descriptor.
 * @param token - The token received from the server.
*/
void delete_book_command(int sockfd, char *token) {
    char id[LINELEN];
    char buff[LINELEN];
    char dummy[LINELEN];
    char path[LINELEN];
    int rc;

    printf("id=");
	fgets(buff, LINELEN, stdin);
	rc = sscanf(buff, "%s %[^\n\t ]", id, dummy);
	if (rc != 1) {
		printf("EROARE : Invalid id!\n");
		return;
	}
    
    strcpy(path, BOOK);
    strcat(path, id);

    char *response = send_and_receive(NULL, sockfd, SERVER, path, "DELETE", NULL, token);

    if (strstr(response, "error") != NULL) {
        json error = json::parse(strstr(response, "{"));
        printf("EROARE : %s\n", error.value("error", "").c_str());
    } else {
        printf("SUCCES : Cartea cu id %s a fost stearsa cu succes!\n", id);
    }

    free(response);
}

/**
 * Function used to logout the current user.
 * It sends a GET request to the server and frees the cookie and token.
 * @param sockfd - The socket file descriptor.
 * @param cookie - The cookie received from the server.
 * @param token - The token received from the server.
*/
void logout_command(int sockfd, char **cookie, char **token) {
    
    char *response = send_and_receive(NULL, sockfd, SERVER, LOGOUT, "GET", *cookie, *token);

    if (strstr(response, "error") != NULL) {
        json error = json::parse(strstr(response, "{"));
        printf("EROARE : %s\n", error.value("error", "").c_str());
    } else {
        free(*cookie);
        free(*token);
        printf("SUCCES : Utilizatorul s-a delogat cu succes!\n");
    }

    free(response);
}