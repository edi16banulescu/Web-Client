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
#include "parson.h"

char *compute_get_request(char *host, char *url, char *query_params,
                            char *cookies, char *jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL) {
        char aux[200];
        memset(aux, 0, 200);
        strcat(aux, "Cookie: ");
        strcat(aux, cookies);
        sprintf(line, "%s", aux);
        compute_message(message, line);
    }

    if (jwt != NULL) {
        char aux[300];
        memset(aux, 0, 300);
        strcat(aux, "Authorization: Bearer ");
        strcat(aux, jwt);
        sprintf(line, "%s", aux);
        compute_message(message, line);
    }
    // Step 4: add final new line
    free(line);
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char *body_data, 
                                char *cookies, char *jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    if (jwt != NULL) {
        char aux[300];
        memset(aux, 0, 300);
        strcat(aux, "Authorization: Bearer ");
        strcat(aux, jwt);
        sprintf(line, "%s", aux);
        compute_message(message, line);
    }
    
    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    sprintf(line, "Content-Length: %ld", strlen(body_data));
    compute_message(message, line);

    // Step 4 (optional): add cookies
    if (cookies != NULL) {
       
    }
    // Step 5: add new line at end of header
    compute_message(message, "");

    // Step 6: add the actual payload data
    memset(line, 0, LINELEN);
    strcat(message, body_data);

    free(line);
    return message;
}

char *compute_delete_request(char *host, char *url, char *content_type,
                            char *cookies, char *jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "DELETE %s HTTP/1.1", url);

    compute_message(message, line);
    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (jwt != NULL) {
        char aux[300];
        memset(aux, 0, 300);
        strcat(aux, "Authorization: Bearer ");
        strcat(aux, jwt);
        sprintf(line, "%s", aux);
        compute_message(message, line);
    }
    if (cookies != NULL) {
       
    }
    // Step 4: add final new line
    compute_message(message, "");
    free(line);
    return message;
}

char *parser(char *type, char **params) {
    char *serialized_string = NULL;

    // Creez JSON-uri pentru fiecare tip de request si le transform in string
    if(strcmp(type, "register") == 0) {
        JSON_Value *root_value = json_value_init_object();
        JSON_Object *root_object = json_value_get_object(root_value);

        json_object_set_string(root_object, "username", params[0]);
        json_object_set_string(root_object, "password", params[1]);

        serialized_string = json_serialize_to_string_pretty(root_value);
        json_value_free(root_value);
    } else if(strcmp(type, "login") == 0) {
        JSON_Value *root_value = json_value_init_object();
        JSON_Object *root_object = json_value_get_object(root_value);

        json_object_set_string(root_object, "username", params[0]);
        json_object_set_string(root_object, "password", params[1]);

        serialized_string = json_serialize_to_string_pretty(root_value);
        json_value_free(root_value);
    } else if (strcmp(type, "add") == 0) {
        JSON_Value *root_value = json_value_init_object();
        JSON_Object *root_object = json_value_get_object(root_value);

        json_object_set_string(root_object, "title", params[0]);
        json_object_set_string(root_object, "author", params[1]);
        json_object_set_string(root_object, "genre", params[2]);
        json_object_set_number(root_object, "page_count", atoi(params[3]));
        json_object_set_string(root_object, "publisher", params[4]);

        serialized_string = json_serialize_to_string_pretty(root_value);
        json_value_free(root_value);
    }

    return serialized_string;
}
