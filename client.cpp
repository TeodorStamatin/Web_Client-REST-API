#include <bits/stdc++.h>
#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "server.h"

/**
    Funtion used to get the command from stdin and convert it into a number.
    It maps the input to the corresponding int codes.
*/
int get_command_type(char *command) {

	if (strncmp(command, "register", 8) == 0)
        return 1;
	else if (strncmp(command, "login", 5) == 0)
		return 2;
	else if (strncmp(command, "enter_library", 13) == 0)
		return 3;
	else if (strncmp(command, "get_books", 9) == 0)
		return 4;
	else if (strncmp(command, "get_book", 8) == 0)
		return 5;
	else if (strncmp(command, "add_book", 8) == 0)
		return 6;
	else if (strncmp(command, "delete_book", 11) == 0)
		return 7;
	else if (strncmp(command, "logout", 6) == 0)
		return 8;
	else if (strncmp(command, "exit", 4) == 0)
		return 9;
    else return 0;
}

int main(void) {

    // initialing the token and the cookie
    int sockfd;
    char *login_cookie = NULL;
    char *jwt_token = NULL;
    bool exit = false;

    while (1) {

        // getting the connection to the server using ip and port
        sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);

        char buff[LINELEN];
	    char command[LINELEN];
	    char dummy[LINELEN];

        memset(buff, 0, LINELEN);
        fgets(buff, LINELEN, stdin);

        // if no command is read, we continue
	    int rc = sscanf(buff, "%s %[^\n\t ]", command, dummy);
        if(rc != 1) {
            printf("Invalid command.\n");
            continue;
        }

        int command_type = get_command_type(command);

        // switch case for the command type
        switch(command_type) {
            case 1:
                register_command(sockfd);
                break;
            case 2:
                login_command(sockfd, &login_cookie);
                break;
            case 3:
                jwt_token = enter_library_command(sockfd, login_cookie);
                break;
            case 4:
                get_books_command(sockfd, jwt_token);
                break;
            case 5:
                get_book_command(sockfd, jwt_token);
                break;
            case 6:
                add_book_command(sockfd, jwt_token);
                break;
            case 7:
                delete_book_command(sockfd, jwt_token);
                break;
            case 8:
                logout_command(sockfd, &login_cookie, &jwt_token);
                break;
            case 9:
                exit = true;
                return 0;
            default:
                printf("Invalid command.\n");
        }

        // if the command is exit, we close the connection and break the loop
        close(sockfd);
        if(exit)
            break;
    }

    close_connection(sockfd);
    free(login_cookie);
    free(jwt_token);
    return 0;
}