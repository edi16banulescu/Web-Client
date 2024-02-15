#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <poll.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

int main(int argc, char *argv[])
{

    char *message;
    char *response;
    int sockfd;
    char cookie[150];
    memset(cookie, 0, 150);
    char token[300];
    memset(token, 0, 300);

    while(1) {
        char buf[100];
        memset(buf, 0, 100);
        fgets(buf, 100, stdin);

        if (strcmp(buf, "exit\n") == 0) {
            break;
        } else if (strcmp(buf, "register\n") == 0) {
            char username[100];
            char password[100];

            printf("username=");
            fgets(username, 100, stdin);
            printf("password=");
            fgets(password, 100, stdin);

            // elimin \n de la finalul lui username si password
            username[strcspn(username, "\n")] = '\0';
            password[strcspn(password, "\n")] = '\0';

            // verific daca username si password contin whitespace
            if (containsWhitespace(username) == 1 || containsWhitespace(password) == 1) {
                printf("Username/Password cannot contain whitespace\n");
                continue;
            }

            char *body_data[] = {username, password};

            char *serialized_string = parser("register", body_data);

            sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);

            message = compute_post_request("34.254.242.81:8080", "/api/v1/tema/auth/register", 
                                "application/json", serialized_string, NULL, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            close_connection(sockfd);
            free(message);
            json_free_serialized_string(serialized_string);

            char *aux = basic_extract_json_response(response);
            JSON_Value *root_value = json_parse_string(aux);
            if (json_object_get_string(json_object(root_value), "error") != NULL) {
                printf("400 - Bad Request - The username already exists!\n");
            } else {
                printf("200 - OK - User registered successfully!\n");
            }

            free(response);
            json_value_free(root_value);

        } else if (strcmp(buf, "login\n") == 0) {
            char username[100];
            char password[100];

            printf("username=");
            fgets(username, 100, stdin);
            printf("password=");
            fgets(password, 100, stdin);

            // elimin \n de la finalul lui username si password
            username[strcspn(username, "\n")] = '\0';
            password[strcspn(password, "\n")] = '\0'; 

            // verific daca username si password contin whitespace
            if (containsWhitespace(username) == 1 || containsWhitespace(password) == 1) {
                printf("Username/Password cannot contain whitespace\n");
                continue;
            }

            char *body_data[] = {username, password};

            char *serialized_string = parser("login", body_data);

            sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);

            message = compute_post_request("34.254.242.81:8080", "/api/v1/tema/auth/login", 
                                "application/json", serialized_string, NULL, NULL);
            send_to_server(sockfd, message);
            json_free_serialized_string(serialized_string);
            free(message);
            response = receive_from_server(sockfd);
            close_connection(sockfd);

            char *myjson = basic_extract_json_response(response);
            JSON_Value *root_value = json_parse_string(myjson);

            if (json_object_get_string(json_object(root_value), "error") != NULL) {
                printf("400 - Bad Request - Wrong username or password!\n");
            } else {
                printf("200 - OK - Welcome Back!\n");

                // extrag cookie-ul din raspuns
                char *aux = strstr(response, "Set-Cookie: ");
                if(aux != NULL) {
                    char *aux2 = strchr(aux, ';');
                    strncpy(cookie, aux + 12, aux2 - aux - 12);
                }
            }

            json_value_free(root_value);
            free(response);

        } else if (strcmp(buf, "enter_library\n") == 0) {
            sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);

            message = compute_get_request("34.254.242.81:8080", "/api/v1/tema/library/access", 
                                NULL, cookie, NULL);

            send_to_server(sockfd, message);
            free(message);
            response = receive_from_server(sockfd);
            close_connection(sockfd);

            char *aux = basic_extract_json_response(response);
            JSON_Value *root_value = json_parse_string(aux);
            JSON_Object *root_object = json_object(root_value);

            const char *source = json_object_get_string(root_object, "token");
            if (source != NULL) {
                printf("200 - OK - Access to library granted!\n");
                strcpy(token, source);
            } else {
                printf("You are not logged in!\n");
            }

            free(response);
            json_value_free(root_value);
        } else if (strcmp(buf, "get_books\n") == 0) {
            sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);

            message = compute_get_request("34.254.242.81:8080", "/api/v1/tema/library/books", 
                                NULL, NULL, token);

            send_to_server(sockfd, message);
            free(message);
            response = receive_from_server(sockfd);
            close_connection(sockfd);

            char *aux = basic_extract_json_response(response);
            char *aux2 = malloc(strlen(aux) + 1);
            strcpy(aux2, aux);

            JSON_Value *root_value = json_parse_string(aux);

            if (json_object_get_string(json_object(root_value), "error") != NULL) {
                printf("400 - No access!\n");
            } else {
                // adaug '[' la inceputul sirului pentru a putea creea Array-ul de JSON-uri
                if (aux2 != NULL) {
                    for (size_t i = strlen(aux2); i > 0; --i) {
                        aux2[i] = aux2[i - 1];
                    }

                    aux2[0] = '[';
                }

                JSON_Value *root_value = json_parse_string(aux2);

                printf("200 - OK - Books:\n");
                JSON_Array *array = json_value_get_array(root_value);
                for (size_t i = 0; i < json_array_get_count(array); i++) {
                    JSON_Object *obj = json_array_get_object(array, i);
                    int id = (int)json_object_get_number(obj, "id");
                    const char *title = json_object_get_string(obj, "title");
                    printf("id: %d, title: %s\n", id, title);
                }
            }

            free(aux2);
            json_value_free(root_value);
            free(response);
        } else if (strncmp(buf, "get_book", 8) == 0) {
            char id[100];
            printf("id=");
            fgets(id, 100, stdin);
            id[strcspn(id, "\n")] = '\0';

            // verific daca id-ul este numeric
            if (isNumeric(id) == 0) {
                printf("The ID must be numeric!\n");
                continue;
            }

            // creez url-ul pentru request
            char url[100] = "/api/v1/tema/library/books/";
            strcat(url, id);

            sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);

            message = compute_get_request("34.254.242.81:8080", url, 
                                NULL, NULL, token);

            send_to_server(sockfd, message);
            free(message);
            response = receive_from_server(sockfd);
            close_connection(sockfd);

            char *aux = basic_extract_json_response(response);
            JSON_Value *root_value = json_parse_string(aux);

            if (json_object_get_string(json_object(root_value), "error") != NULL) {
                if (strncmp(json_object_get_string(json_object(root_value), "error"), "No book", 7) == 0) {
                    printf("Book with ID : %s doesn't exit!\n", id);
                } else {
                    printf("400 - No access!\n");
                }
            } else {
                JSON_Object *obj = json_object(root_value);

                printf("200 - OK - Book:\n");
                printf("id=%d, title=%s, author=%s, genre=%s, page_count=%d, publisher=%s\n", 
                    (int)json_object_get_number(obj, "id"), json_object_get_string(obj, "title"),
                    json_object_get_string(obj, "author"), json_object_get_string(obj, "genre"),
                    (int)json_object_get_number(obj, "page_count"), json_object_get_string(obj, "publisher"));
            }

            json_value_free(root_value);
            free(response);
        } else if (strcmp(buf, "add_book\n") == 0) {
            char title[100];
            char author[100];
            char genre[100];
            char publisher[100];
            char page_count[100];
            printf("title=");
            fgets(title, 100, stdin);
            printf("author=");
            fgets(author, 100, stdin);
            printf("genre=");
            fgets(genre, 100, stdin);
            printf("publisher=");
            fgets(publisher, 100, stdin);
            printf("page_count=");
            fgets(page_count, 100, stdin);

            // elimin '\n' de la sfarsitul fiecarui sir
            title[strcspn(title, "\n")] = '\0';
            author[strcspn(author, "\n")] = '\0';
            genre[strcspn(genre, "\n")] = '\0';
            publisher[strcspn(publisher, "\n")] = '\0';
            page_count[strcspn(page_count, "\n")] = '\0';

            // verific daca page_count este numeric si daca toate campurile sunt completate
            if (isNumeric(page_count) == 0) {
                printf("Page count must be a number!\n");
                continue;
            } else if (strlen(title) == 0 || strlen(author) == 0 || strlen(genre) == 0 || strlen(publisher) == 0) {
                printf("All fields must be completed!\n");
                continue;
            }

            char *body_data[] = {title, author, genre, page_count, publisher};
            char *serialized_string = parser("add", body_data);

            sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);

            message = compute_post_request("34.254.242.81:8080", "/api/v1/tema/library/books", 
                                "application/json", serialized_string, NULL, token);

            json_free_serialized_string(serialized_string);
            send_to_server(sockfd, message);
            free(message);
            response = receive_from_server(sockfd);
            close_connection(sockfd);

            char *aux = basic_extract_json_response(response);
            JSON_Value *root_value = json_parse_string(aux);
            if (json_object_get_string(json_object(root_value), "error") != NULL) {
                printf("400 - No access\n");
            } else {
                printf("200 - OK - Book added\n");
            }

            json_value_free(root_value);
            free(response);
        } else if (strncmp(buf, "delete_book", 11) == 0) {
            char id[100];
            printf("id=");
            fgets(id, 100, stdin);
            id[strcspn(id, "\n")] = '\0';

            // verific daca id-ul este numeric
            if (isNumeric(id) == 0) {
                printf("Id must be a number!\n");
                continue;
            }

            // creez url-ul pentru request
            char url[100] = "/api/v1/tema/library/books/";
            strcat(url, id);

            sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);

            message = compute_delete_request("34.254.242.81:8080", url, "application/json",
                                NULL, token);
            send_to_server(sockfd, message);
            free(message);
            response = receive_from_server(sockfd);
            close_connection(sockfd);

            char *aux = basic_extract_json_response(response);
            JSON_Value *root_value = json_parse_string(aux);
            if (json_object_get_string(json_object(root_value), "error") != NULL) {
                printf("400 - Something went wrong\n");
            } else {
                printf("200 - OK - Book deleted\n");
            }

            json_value_free(root_value);
            free(response);
        } else if (strcmp(buf, "logout\n") == 0) {
            sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);

            message = compute_get_request("34.254.242.81:8080", "/api/v1/tema/auth/logout", 
                                NULL, cookie, NULL);

            send_to_server(sockfd, message);
            free(message);
            response = receive_from_server(sockfd);
            close_connection(sockfd);

            char *aux = basic_extract_json_response(response);
            JSON_Value *root_value = json_parse_string(aux);

            if (json_object_get_string(json_object(root_value), "error") != NULL) {
                printf("You are not logged in!\n");
            } else {
                printf("200 - OK - Logged out\n");
            }

            json_value_free(root_value);
            free(response);

            memset(token, 0, 300);
            memset(cookie, 0, 150);
        } else {
            printf("Invalid command\n");
        }
    }

    return 0;
}
