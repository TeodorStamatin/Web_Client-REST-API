# Project4 PCOM - Web

## About

This project implements a C++ client to interact with a REST API exposed by an HTTP server. The
client sends HTTP requests to an online library and allows users to make operations such as
registration, login, library access, viewing and adding books, deleting books, and logging out.

Firstly, we read every command from stdin until the entered command is "exit". In this case, the
whole program will shut down. We open a connection with the server for every command using the
server ip and port, and we execute the commands depeding on their corresponding code :

#### 1. REGISTER
This function registers a new user by sending a POST request to the server with the username and
password. In case that the credentials contain whitespaces, newline or tabs, the command will fail
and it will abort. It constructs a JSON object using the "nlohmann" library and sends a request to
the server, and checks the response for any errors.

#### 2. LOGIN
This function will try to log in the user by sending a POST request to the server with the username
and password. In case that the credentials contain whitespaces, newline or tabs, the command will
fail and it will abort. It constructs a JSON object using the "nlohmann" library and sends a
request to the server, and checks the response for any errors. If the login is successful, it
extracts the session cookie from the response and stores it.

#### 3. ENTER_LIBRARY
This function requests access to the library by sending a GET request to the server. It returns a
token on successful access. It mainly calls the "send_and_receive" function to get the response
from the server and checks for a potential error. If there is none, it extracts the token.

#### 4. GET_BOOKS
This function retrieves the list of books from the library by sending a GET request to the server.
It calls "send_and_receive" with the GET method, server information, and the token to send the
request to the server. It processes the server's response to check for errors and if the request is
successful, it extracts the list of books from the response, formats it as a JSON string, and
prints it.

#### 5. GET_BOOK
This function retrieves the details of a specific book from the library by sending a GET request to
the server. It asks the user to enter the book id and constructs the request url using the default
path and the book id. It calls "send_and_receive" with the GET method and the token to send the
request to the server. It scans the server's response to check for errors. If the request is
successful, it extracts the book details from the response, formats it as a JSON string, and prints
it.

#### 6. ADD_BOOK
This function adds a new book to the library by sending a POST request to the server with the book
details. The user needs to enter the book details : title, author, genre, publisher, and page
count. If any of these fields are wrong, the command is aborted. If they are valid, it constructs
a JSON object containing the book details. It calls send_and_receive with the JSON body and POST
method to send the request to the server. It scans the server's response to check for errors.

#### 7. DELETE_BOOK
This function deletes an existing book from the library by sending a DELETE request to the server
with the book ID. It asks the user to enter the book id and checks it's validity. It constructs
the request url using the default path and the book id. It calls "send_and_receive" with the DELETE
method and the token to send the request to the server. It processes the server's response to check
for errors.

#### 8. LOGOUT
This function logs out the current user by sending a GET request to the server and frees the cookie
and token. It calls "send_and_receive" with the GET method, cookie, and the token to send the
request to the server. It scans the server's response to check for errors. If the logout is
successful, it frees the memory allocated for the cookie and token.

#### 9. EXIT
If the user enters the "exit" command, the program closes the connection to the server, breaks the
infinite loop and frees the memory for the token and cookie.

## JSON Usage

For this project I used JSON for the "Nlohmann" library, being easy to use for parsing the response from the server, also being recommended on the assignment's page.
https://github.com/nlohmann/json

### More - https://pcom.pages.upb.ro/enunt-tema4/
