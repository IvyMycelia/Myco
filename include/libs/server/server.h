#ifndef MYCO_SERVER_H
#define MYCO_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include "../core/interpreter.h"

// Server configuration structure
typedef struct {
    int port;
    char* static_dir;
    bool debug;
    bool enable_gzip;
    bool enable_cache;
} ServerConfig;

// Middleware structure
typedef struct Middleware {
    Value function;
    struct Middleware* next;
} Middleware;

// File watcher structure
typedef struct FileWatcher {
    char* watch_path;
    Value callback;
    bool active;
    pthread_t thread;
    time_t last_check;
} FileWatcher;

// Signal handler structure
typedef struct SignalHandler {
    int signal;
    Value callback;
    struct SignalHandler* next;
} SignalHandler;

// Server configuration
typedef struct {
    int port;
    bool running;
    void* daemon; // Custom HTTP server daemon
    Interpreter* interpreter;
    ServerConfig* config;
    Middleware* middleware;
    FileWatcher* file_watcher;
    SignalHandler* signal_handlers;
} MycoServer;

// Route parameter structure
typedef struct RouteParam {
    char* name;
    char* value;
    struct RouteParam* next;
} RouteParam;

// Static route structure
typedef struct StaticRoute {
    char* url_prefix;
    char* file_path;
    bool enable_gzip;
    bool enable_cache;
    int cache_duration; // in seconds
    struct StaticRoute* next;
} StaticRoute;

// Route structure
typedef struct Route {
    char* method;
    char* path;
    char* pattern;  // The original pattern with :param syntax
    RouteParam* params;  // Extracted parameters from the request
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
Value builtin_server_static(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_server_use(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_server_use_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_server_group(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_server_close(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_group_get(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_group_post(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_group_put(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_group_delete(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_server_now(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_server_sleep(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_server_watch(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_server_onSignal(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Request/Response object methods
Value builtin_request_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_request_url(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_request_path(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_request_body(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_request_header(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_request_param(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_request_json(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_request_form(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_request_query(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

Value builtin_response_send(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_response_json(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_response_status(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_response_header(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_response_send_file(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_response_set_header(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_next_function(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
void execute_next_middleware(void);

// Request/Response object creation and management
Value create_request_object(MycoRequest* request);
Value create_response_object(MycoResponse* response);
MycoRequest* parse_http_request(void* connection, const char* url, const char* method);
MycoResponse* create_http_response(void);
void free_request_object(MycoRequest* request);
void free_response_object(MycoResponse* response);
Value execute_myco_function(Interpreter* interpreter, Value function, Value* arg1, Value* arg2);

// Internal server functions
MycoServer* server_create(int port, Interpreter* interpreter);
MycoServer* server_create_with_config(ServerConfig* config, Interpreter* interpreter);
void server_free(MycoServer* server);
ServerConfig* parse_server_config(Value config_obj);
void free_server_config(ServerConfig* config);
Middleware* middleware_create(Value function);
void middleware_free(Middleware* middleware);
void middleware_add(MycoServer* server, Value function);
void execute_middleware(MycoServer* server, Value req_obj, Value res_obj, Value next_func);
int server_handle_request(void* cls, void* connection, const char* url, const char* method, const char* version, const char* upload_data, size_t* upload_data_size, void** con_cls);
Route* route_create(const char* method, const char* path, Value handler);
void route_free(Route* route);
Route* route_match(Route* routes, const char* method, const char* path);
RouteParam* route_param_create(const char* name, const char* value);
void route_param_free(RouteParam* param);
void route_params_free(RouteParam* params);
RouteParam* route_params_find(RouteParam* params, const char* name);
bool route_path_matches(const char* pattern, const char* path, RouteParam** params);
char** split_path(const char* path);
void free_path_segments(char** segments);
bool validate_typed_parameter(const char* value, const char* type);

// Static file serving functions
StaticRoute* static_route_create(const char* url_prefix, const char* file_path);
void static_route_free(StaticRoute* route);
void static_route_add(StaticRoute* route);
StaticRoute* static_route_match(const char* url);
char* get_mime_type(const char* filename);
bool file_exists(const char* path);
char* read_file_content(const char* path, size_t* size);
char* compress_gzip(const char* data, size_t data_size, size_t* compressed_size);
bool should_compress_file(const char* filename);
char* get_cache_headers(int cache_duration);

// File watching functions
FileWatcher* file_watcher_create(const char* path, Value callback);
void file_watcher_free(FileWatcher* watcher);
void* file_watcher_thread(void* arg);

// Signal handling functions
SignalHandler* signal_handler_create(int signal, Value callback);
void signal_handler_free(SignalHandler* handler);
void signal_handler_register(MycoServer* server, int signal_num, Value callback);
void signal_handler_execute(int signal);

// Request body parsing functions
Value parse_json_body(const char* body);
Value parse_form_body(const char* body);
Value parse_query_string(const char* query_string);
char* url_decode(const char* str);

// Library registration function
void server_library_register(Interpreter* interpreter);

#endif // MYCO_SERVER_H
