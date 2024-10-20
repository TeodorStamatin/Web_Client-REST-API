#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include <nlohmann/json.hpp>

char *compute_get_request(const char *host, const char *url, char *cookies, char *token) {
	char *message = (char *)calloc(BUFLEN, sizeof(char));
	if(!message) {
		fprintf(stderr, "calloc() failed");
	}
	char *line = (char *)calloc(LINELEN, sizeof(char));
	if(!line) {
		fprintf(stderr, "calloc() failed");
	}

	/* Write the method name, URL, request params (if any) and protocol type */
	sprintf(line, "GET %s HTTP/1.1", url);

	compute_message(message, line);

	// Step 2: add the host

	sprintf(line, "Host: %s", host);
	compute_message(message, line);

	// // Step 3 (optional): add headers and/or cookies, according to the protocol format
    // sprintf(line, "User-Agent: Mozilla/5.0");
    // compute_message(message, line);

    // sprintf(line, "Connection: keep-alive");
    // compute_message(message, line);

	if (cookies != NULL) {
		sprintf(line, "Cookie: %s", cookies);
		compute_message(message, line);
	}

	if (token != NULL) {
		sprintf(line, "Authorization: Bearer %s", token);
		compute_message(message, line);
	}

    // Step 4: add final new line
    compute_message(message, "");
    return message;
}

char *compute_post_request(const char *host, const char *url,
						   const char *content_type,
						   const char *body_data,
						   char *cookies,
						   char *token) {
	char *message = (char *)calloc(BUFLEN, sizeof(char));
	if(!message) {
		fprintf(stderr, "calloc() failed");
	}
	char *line = (char *)calloc(LINELEN, sizeof(char));
	if(!line) {
		fprintf(stderr, "calloc() failed");
	}
	char *body_data_buffer = (char *)calloc(LINELEN, sizeof(char));
	if(!body_data_buffer) {
		fprintf(stderr, "calloc() failed");
	}

	// Step 1: write the method name, URL and protocol type
	sprintf(line, "POST %s HTTP/1.1", url);
	compute_message(message, line);

	// Step 2: add the host
	sprintf(line, "Host: %s", host);
	compute_message(message, line);

	/* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)*/
	
	memset(body_data_buffer, 0, LINELEN);
	strcat(body_data_buffer, body_data);

	sprintf(line, "Content-Type: %s", content_type);
	compute_message(message, line);

	sprintf(line, "Content-Length: %lu", strlen(body_data_buffer));
	compute_message(message, line);

	// Step 4 (optional): add cookies
	if (cookies != NULL) {
		sprintf(line, "Cookie: %s", cookies);
		compute_message(message, line);
	}

	if (token != NULL) {
		sprintf(line, "Authorization: Bearer %s", token);
		compute_message(message, line);
	}

	// Step 5: add new line at end of header
    compute_message(message, "");

    // Step 6: add the actual payload data
    // TODO

    memset(line, 0, LINELEN);
    compute_message(message, body_data_buffer);

    free(line);
    return message;
}

char *delete_request(const char *host, const char *url,
						  	 char *cookies,
						  	 char *token) {
	char *message = (char *)calloc(BUFLEN, sizeof(char));
	if(!message) {
		fprintf(stderr, "calloc() failed");
	}
	char *line = (char *)calloc(LINELEN, sizeof(char));
	if(!line) {
		fprintf(stderr, "calloc() failed");
	}

	// Step 1: write the method name, URL, request params (if any) and protocol type
	sprintf(line, "DELETE %s HTTP/1.1", url);

	compute_message(message, line);

	// Step 2: add the host

	sprintf(line, "Host: %s", host);
	compute_message(message, line);

	// // Step 3 (optional): add headers and/or cookies, according to the protocol format
	// sprintf(line, "User-Agent: Mozilla/5.0");
    // compute_message(message, line);

    // sprintf(line, "Connection: keep-alive");
    // compute_message(message, line);

	if (cookies != NULL) {
		sprintf(line, "Cookie: %s", cookies);
		compute_message(message, line);
	}

	if (token != NULL) {
		sprintf(line, "Authorization: Bearer %s", token);
		compute_message(message, line);
	}

    // Step 4: add final new line
    compute_message(message, "");
    return message;
}
