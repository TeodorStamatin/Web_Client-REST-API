#ifndef __SERVER_H__
#define __SERVER_H__
#include <nlohmann/json.hpp>

char *send_and_receive(nlohmann::json body, int sockfd, const char *host, const char *url, const char *method, char *cookies, char *token);

void register_command(int sockfd);

void login_command(int sockfd, char **cookie);

char *enter_library_command(int sockfd, char *cookie);

void get_books_command(int sockfd, char *token);

void get_book_command(int sockfd, char *token);

void add_book_command(int sockfd, char *token);

void delete_book_command(int sockfd, char *token);

void logout_command(int sockfd, char **cookie, char **token);

#endif