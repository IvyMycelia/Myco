#ifndef MYCO_HTTP_H
#define MYCO_HTTP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <curl/curl.h>
#include "../core/interpreter.h"

// HTTP method types
typedef enum {
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_PATCH,
    HTTP_HEAD,
    HTTP_OPTIONS
} HttpMethod;

// HTTP response structure
typedef struct {
    int status_code;
    char* status_text;
    char* body;
    char** headers;
    size_t header_count;
    char* content_type;
    long content_length;
    bool success;
    char* error_message;
} HttpResponse;

// HTTP request structure
typedef struct {
    HttpMethod method;
    char* url;
    char** headers;
    size_t header_count;
    char* body;
    int timeout_seconds;
    bool follow_redirects;
    char* user_agent;
} HttpRequest;

// Core HTTP functions
HttpResponse* http_request(HttpRequest* request);
HttpResponse* http_get(const char* url, char** headers, size_t header_count);
HttpResponse* http_post(const char* url, const char* data, char** headers, size_t header_count);
HttpResponse* http_put(const char* url, const char* data, char** headers, size_t header_count);
HttpResponse* http_delete(const char* url, char** headers, size_t header_count);

// Utility functions
void http_free_response(HttpResponse* response);
void http_free_request(HttpRequest* request);
HttpRequest* http_create_request(HttpMethod method, const char* url);
void http_add_header(HttpRequest* request, const char* name, const char* value);
void http_set_body(HttpRequest* request, const char* body);
void http_set_timeout(HttpRequest* request, int seconds);

// Response parsing functions
char* http_get_header(HttpResponse* response, const char* name);
bool http_is_success(HttpResponse* response);
bool http_is_client_error(HttpResponse* response);
bool http_is_server_error(HttpResponse* response);
char* http_get_json(HttpResponse* response);

// Myco library functions
Value builtin_http_get(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_http_post(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_http_put(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_http_delete(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_http_request(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_http_status_ok(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_http_get_header(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_http_get_json(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_http_head(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_http_patch(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_http_options(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Library registration function
void http_library_register(Interpreter* interpreter);

#endif // MYCO_HTTP_H
