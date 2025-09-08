#ifndef MYCO_SERVER_H
#define MYCO_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <microhttpd.h>
#include "../core/interpreter.h"

// Server configuration
typedef struct {
    int port;
    bool running;
    struct MHD_Daemon* daemon;
    Interpreter* interpreter;
} MycoServer;

// Route structure
typedef struct Route {
    char* method;
    char* path;
    Value handler;  // Store the actual Myco function value
    struct Route* next;
} Route;

// Request object structure
typedef struct {
    char* method;
    char* url;
    char* path;
    char* query_string;
    char* body;
    char** headers;
    size_t header_count;
    char** params;
    size_t param_count;
} MycoRequest;

// Response object structure
typedef struct {
    int status_code;
    char* content_type;
    char* body;
    char** headers;
    size_t header_count;
    bool headers_sent;
} MycoResponse;

// Server library functions
Value builtin_server_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_server_listen(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_server_stop(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_server_get(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_server_post(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_server_put(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_server_delete(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Request/Response object methods
Value builtin_request_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_request_url(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_request_path(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_request_body(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_request_header(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

Value builtin_response_send(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_response_json(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_response_status(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_response_header(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Request/Response object creation and management
Value create_request_object(MycoRequest* request);
Value create_response_object(MycoResponse* response);
MycoRequest* parse_http_request(struct MHD_Connection* connection, const char* url, const char* method);
MycoResponse* create_http_response(void);
void free_request_object(MycoRequest* request);
void free_response_object(MycoResponse* response);
Value execute_myco_function(Interpreter* interpreter, Value function, Value* arg1, Value* arg2);

// Internal server functions
MycoServer* server_create(int port, Interpreter* interpreter);
void server_free(MycoServer* server);
enum MHD_Result server_handle_request(void* cls, struct MHD_Connection* connection, const char* url, const char* method, const char* version, const char* upload_data, size_t* upload_data_size, void** con_cls);
Route* route_create(const char* method, const char* path, Value handler);
void route_free(Route* route);
Route* route_match(Route* routes, const char* method, const char* path);

// Library registration function
void server_library_register(Interpreter* interpreter);

#endif // MYCO_SERVER_H
