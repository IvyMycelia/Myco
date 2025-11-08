#include "../../include/libs/http_server.h"
#include "../../include/libs/server/server.h"
#include "../../include/libs/json.h"
#include "../../include/utils/shared_utilities.h"
#include "../../include/core/interpreter.h"

// Forward declarations for JSON functions
extern Value builtin_json_stringify(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
extern Value builtin_json_parse(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
extern Value builtin_json_validate(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
#include <pthread.h>

// Global mutex for thread-safe interpreter access
static pthread_mutex_t g_interpreter_mutex = PTHREAD_MUTEX_INITIALIZER;

// Forward declarations for static file serving
extern StaticRoute* g_static_routes;
extern StaticRoute* static_route_match(const char* url);
extern bool file_exists(const char* file_path);
extern char* read_file_content(const char* file_path, size_t* file_size);
extern char* get_mime_type(const char* file_path);

// Forward declarations for route handling
extern Route* g_routes;
extern Interpreter* g_interpreter;
extern char* g_response_body;
extern int g_response_status_code;
extern char* g_response_content_type;
Value execute_myco_function(Interpreter* interpreter, Value function, Value* arg1, Value* arg2);
Value builtin_request_param(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_response_json(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_response_status(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <dirent.h>
#include <sys/event.h>
#include <sys/types.h>

// Maximum number of routes
#define MAX_ROUTES 100

// Global server instance
static HttpServer* g_http_server = NULL;
static bool g_server_running = false;
static HttpServer* g_server = NULL;

// Match route path with parameters (e.g., /api/users/:id matches /api/users/123)
static bool match_route_path(const char* route_path, const char* request_path) {
    if (!route_path || !request_path) return false;
    
    const char* route_ptr = route_path;
    const char* request_ptr = request_path;
    
    while (*route_ptr && *request_ptr) {
        if (*route_ptr == ':') {
            // Skip parameter name in route
            while (*route_ptr && *route_ptr != '/') route_ptr++;
            // Skip parameter value in request
            while (*request_ptr && *request_ptr != '/') request_ptr++;
        } else if (*route_ptr == *request_ptr) {
            route_ptr++;
            request_ptr++;
        } else {
            return false;
        }
    }
    
    return (*route_ptr == '\0' && *request_ptr == '\0');
}

// Parse HTTP request from buffer
static HttpRequest* parse_http_request_buffer(const char* buffer, ssize_t length) {
    HttpRequest* request = shared_malloc_safe(sizeof(HttpRequest), "http_server", "parse_http_request", 0);
    if (!request) return NULL;
    
    // Initialize request
    request->method = NULL;
    request->path = NULL;
    request->headers = NULL;
    request->body = NULL;
    request->query_string = NULL;
    
    // Parse first line (method, path, version)
    char* line_end = strstr(buffer, "\r\n");
    if (!line_end) {
        shared_free_safe(request, "http_server", "parse_http_request", 0);
        return NULL;
    }
    
    // Copy first line
    size_t line_len = line_end - buffer;
    char* first_line = shared_malloc_safe(line_len + 1, "http_server", "parse_http_request", 0);
    if (!first_line) {
        shared_free_safe(request, "http_server", "parse_http_request", 0);
        return NULL;
    }
    
    strncpy(first_line, buffer, line_len);
    first_line[line_len] = '\0';
    
    // Parse method and path
    char* method_end = strchr(first_line, ' ');
    if (!method_end) {
        shared_free_safe(first_line, "http_server", "parse_http_request", 0);
        shared_free_safe(request, "http_server", "parse_http_request", 0);
        return NULL;
    }
    
    *method_end = '\0';
    request->method = shared_strdup(first_line);
    
    char* path_start = method_end + 1;
    char* path_end = strchr(path_start, ' ');
    if (!path_end) {
        shared_free_safe(first_line, "http_server", "parse_http_request", 0);
        shared_free_safe(request, "http_server", "parse_http_request", 0);
        return NULL;
    }
    
    *path_end = '\0';
    request->path = shared_strdup(path_start);
    
    // Parse query string
    char* query_start = strchr(request->path, '?');
    if (query_start) {
        *query_start = '\0';
        request->query_string = shared_strdup(query_start + 1);
    }
    
    // Parse headers (simplified - just store the raw headers)
    char* headers_start = line_end + 2;
    char* headers_end = strstr(headers_start, "\r\n\r\n");
    if (headers_end) {
        size_t headers_len = headers_end - headers_start;
        request->headers = shared_malloc_safe(headers_len + 1, "http_server", "parse_http_request", 0);
        if (request->headers) {
            strncpy(request->headers, headers_start, headers_len);
            request->headers[headers_len] = '\0';
        }
        
        // Parse body
        char* body_start = headers_end + 4;
        size_t body_len = length - (body_start - buffer);
        if (body_len > 0) {
            request->body = shared_malloc_safe(body_len + 1, "http_server", "parse_http_request", 0);
            if (request->body) {
                strncpy(request->body, body_start, body_len);
                request->body[body_len] = '\0';
            }
        }
    }
    
    shared_free_safe(first_line, "http_server", "parse_http_request", 0);
    return request;
}

// Create HTTP response string
static char* create_http_response_string(int status_code, const char* content_type, 
                                 const char* body, size_t* response_len) {
    const char* status_text = "OK";
    if (status_code == 404) status_text = "Not Found";
    else if (status_code == 500) status_text = "Internal Server Error";
    
    size_t body_len = body ? strlen(body) : 0;
    char* response = shared_malloc_safe(1024 + body_len, "http_server", "create_http_response", 0);
    if (!response) return NULL;
    
    snprintf(response, 1024 + body_len,
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type, Authorization, X-Requested-With\r\n"
        "Access-Control-Max-Age: 86400\r\n"
        "Connection: close\r\n"
        "\r\n%s",
        status_code, status_text, content_type, body_len, body ? body : "");
    
    *response_len = strlen(response);
    return response;
}

// Bridge function to call Myco route handlers
static void myco_route_handler(HttpRequest* request, HttpResponse* response) {
    
    // Find the matching Myco route
    Route* myco_route = route_match(g_routes, request->method, request->path);
    if (!myco_route) {
        response->status_code = 404;
        response->body = shared_strdup("404 Not Found");
        return;
    }
    
    
    // Check if we have an interpreter and the route has a handler
    if (!g_interpreter || myco_route->handler.type != VALUE_FUNCTION) {
        response->status_code = 500;
        response->body = shared_strdup("Internal Server Error: No handler available");
        return;
    }
    
    // Reset global response data
    g_response_status_code = 200;
    if (g_response_content_type) {
        shared_free_safe(g_response_content_type, "http_server", "myco_route_handler", 0);
        g_response_content_type = NULL;
    }
    if (g_response_body) {
        shared_free_safe(g_response_body, "http_server", "myco_route_handler", 0);
        g_response_body = NULL;
    }
    
    // Create simple request and response objects with methods
    Value req_obj = value_create_object(8);
    value_object_set(&req_obj, "__class_name__", value_create_string("Request"));
    value_object_set(&req_obj, "method", value_create_string(request->method ? request->method : "GET"));
    value_object_set(&req_obj, "path", value_create_string(request->path ? request->path : "/"));
    value_object_set(&req_obj, "body", value_create_string(request->body ? request->body : ""));

    Value res_obj = value_create_object(8);
    value_object_set(&res_obj, "__class_name__", value_create_string("Response"));
    value_object_set(&res_obj, "statusCode", value_create_number(200));
    value_object_set(&res_obj, "contentType", value_create_string("application/json"));
    value_object_set(&res_obj, "json", value_create_builtin_function(builtin_response_json));
    value_object_set(&res_obj, "status", value_create_builtin_function(builtin_response_status));
    value_object_set(&res_obj, "send", value_create_builtin_function(builtin_response_send));
    
    // Execute the Myco handler directly without complex function execution
    // This avoids the memory corruption issues with execute_myco_function
    if (myco_route->handler.type == VALUE_FUNCTION && myco_route->handler.data.function_value.body) {
        // Use the global interpreter but ensure proper isolation
        Interpreter* request_interpreter = g_interpreter;
        
        // Use value_function_call which handles both AST and bytecode functions
        // Prepare arguments array
        Value handler_args[2] = {req_obj, res_obj};
        size_t handler_arg_count = 2;
        
        // Adjust argument count based on function parameter count
        if (myco_route->handler.data.function_value.parameter_count < 2) {
            handler_arg_count = myco_route->handler.data.function_value.parameter_count;
        }
        
        // Call the handler function using value_function_call
        // This will handle both AST and bytecode functions correctly
        Value handler_result = value_function_call(&myco_route->handler, handler_args, handler_arg_count, request_interpreter, 0, 0);
        
        // Check if the function returned a value
        if (request_interpreter->has_return) {
            handler_result = request_interpreter->return_value;
            request_interpreter->has_return = 0; // Reset return flag
        }
        
        // Free handler result
        value_free(&handler_result);
    }
    
    // Get response data from globals
    response->status_code = g_response_status_code;
    if (g_response_content_type) {
        response->content_type = g_response_content_type;
    } else {
        response->content_type = "application/json";
    }
    if (g_response_body) {
        response->body = shared_strdup(g_response_body);
    } else {
        response->body = shared_strdup("{\"message\": \"OK\"}");
    }
    
    // Don't free req_obj and res_obj to avoid memory management issues
    // They will be cleaned up by the interpreter
}

// Handle client connection
static void handle_client_connection(int client_fd, HttpServer* server) {
    char buffer[4096];
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received <= 0) {
        close(client_fd);
        return;
    }
    
    buffer[bytes_received] = '\0';
    
    // Parse HTTP request
    HttpRequest* request = parse_http_request_buffer(buffer, bytes_received);
    if (!request) {
        close(client_fd);
        return;
    }
    
    // Check for static file serving first (only for GET requests)
    if (strcmp(request->method, "GET") == 0) {
        StaticRoute* static_route = static_route_match(request->path);
        if (static_route) {
            // Build the full file path
            char file_path[1024];
            const char* relative_path = request->path + strlen(static_route->url_prefix);
            
            // If the relative path is empty (root path "/"), default to index.html
            if (relative_path[0] == '\0' || strcmp(relative_path, "/") == 0) {
                snprintf(file_path, sizeof(file_path), "%sindex.html", static_route->file_path);
            } else {
                snprintf(file_path, sizeof(file_path), "%s%s", static_route->file_path, relative_path);
            }
            
            // Check if file exists
            if (file_exists(file_path)) {
                size_t file_size;
                char* file_content = read_file_content(file_path, &file_size);
                if (file_content) {
                    // Get MIME type
                    char* mime_type = get_mime_type(file_path);
                    
                    // Create response with file content
                    HttpResponse* response = shared_malloc_safe(sizeof(HttpResponse), "http_server", "handle_client_connection", 0);
                    if (response) {
                        response->status_code = 200;
                        response->content_type = mime_type ? mime_type : "application/octet-stream";
                        response->body = file_content;
                        
                        // Send response
                        size_t response_len;
                        char* http_response = create_http_response_string(response->status_code, response->content_type,
                                                                response->body, &response_len);
                        if (http_response) {
                            send(client_fd, http_response, response_len, 0);
                            shared_free_safe(http_response, "http_server", "handle_client_connection", 0);
                        }
                        
                        // Cleanup response and file content
                        if (response->body) {
                            shared_free_safe(response->body, "http_server", "handle_client_connection", 0);
                        }
                        shared_free_safe(response, "http_server", "handle_client_connection", 0);
                    }
                    
                    // mime_type is a string literal, don't free it
                    close(client_fd);
                    return;
                }
            }
        }
    }
    
    // Handle CORS preflight requests (OPTIONS)
    if (strcmp(request->method, "OPTIONS") == 0) {
        size_t response_len;
        char* http_response = create_http_response_string(200, "text/plain", "", &response_len);
        if (http_response) {
            send(client_fd, http_response, response_len, 0);
            shared_free_safe(http_response, "http_server", "handle_client_connection", 0);
        }
        close(client_fd);
        shared_free_safe(request->method, "http_server", "handle_client_connection", 0);
        shared_free_safe(request->path, "http_server", "handle_client_connection", 0);
        shared_free_safe(request->headers, "http_server", "handle_client_connection", 0);
        shared_free_safe(request->body, "http_server", "handle_client_connection", 0);
        shared_free_safe(request, "http_server", "handle_client_connection", 0);
        return;
    }
    
    // Find matching route
    HttpRouteHandler handler = NULL;
    for (size_t i = 0; i < server->route_count; i++) {
        HttpRoute* route = &server->routes[i];
        if (strcmp(route->method, request->method) == 0 && 
            strcmp(route->path, request->path) == 0) {
            handler = route->handler;
            break;
        }
    }
    
    if (!handler) {
    }
    
    // If no exact match, try to find a route with parameters
    if (!handler) {
        for (size_t i = 0; i < server->route_count; i++) {
            HttpRoute* route = &server->routes[i];
            if (strcmp(route->method, request->method) == 0 && 
                match_route_path(route->path, request->path)) {
                handler = route->handler;
                break;
            }
        }
    }
    
    // Create response
    HttpResponse* response = shared_malloc_safe(sizeof(HttpResponse), "http_server", "handle_client_connection", 0);
    if (response) {
        response->status_code = 200;
        response->content_type = "text/plain";
        response->body = NULL;
        
        if (handler) {
            // Call route handler
            handler(request, response);
        } else {
            // 404 Not Found
            response->status_code = 404;
            response->body = shared_strdup("404 Not Found");
        }
        
        // Send response
        size_t response_len;
        char* http_response = create_http_response_string(response->status_code, response->content_type,
                                                response->body, &response_len);
        if (http_response) {
            send(client_fd, http_response, response_len, 0);
            shared_free_safe(http_response, "http_server", "handle_client_connection", 0);
        }
        
        // Cleanup response
        if (response->body) {
            shared_free_safe(response->body, "http_server", "handle_client_connection", 0);
        }
        shared_free_safe(response, "http_server", "handle_client_connection", 0);
    }
    
    // Cleanup request
    if (request->method) shared_free_safe(request->method, "http_server", "handle_client_connection", 0);
    if (request->path) shared_free_safe(request->path, "http_server", "handle_client_connection", 0);
    if (request->headers) shared_free_safe(request->headers, "http_server", "handle_client_connection", 0);
    if (request->body) shared_free_safe(request->body, "http_server", "handle_client_connection", 0);
    if (request->query_string) shared_free_safe(request->query_string, "http_server", "handle_client_connection", 0);
    shared_free_safe(request, "http_server", "handle_client_connection", 0);
    
    close(client_fd);
}

// Register Myco routes with HTTP server
void http_server_register_myco_routes(HttpServer* server, void* myco_routes) {
    if (!server || !myco_routes) {
        return;
    }
    
    Route* current = (Route*)myco_routes;
    int count = 0;
    
    // Process routes
    while (current && server->route_count < MAX_ROUTES) {
        count++;
        
        // Add route to HTTP server
        HttpRoute* http_route = &server->routes[server->route_count];
        http_route->method = shared_strdup(current->method);
        http_route->path = shared_strdup(current->path);
        http_route->handler = myco_route_handler;
        server->route_count++;
        
        current = current->next;
    }
}

// Create HTTP server
HttpServer* http_server_create(int port) {
    HttpServer* server = shared_malloc_safe(sizeof(HttpServer), "http_server", "http_server_create", 0);
    if (!server) return NULL;
    
    server->port = port;
    server->socket_fd = -1;
    server->running = false;
    server->routes = shared_malloc_safe(sizeof(HttpRoute) * MAX_ROUTES, "http_server", "http_server_create", 0);
    server->route_count = 0;
    server->max_connections = 10;
    
    if (!server->routes) {
        shared_free_safe(server, "http_server", "http_server_create", 0);
        return NULL;
    }
    
    return server;
}

// Add route to HTTP server
int http_server_add_route(HttpServer* server, const char* method, const char* path, 
                         HttpRouteHandler handler) {
    if (!server || server->route_count >= MAX_ROUTES) return 0;
    
    HttpRoute* route = &server->routes[server->route_count];
    route->method = shared_strdup(method);
    route->path = shared_strdup(path);
    route->handler = handler;
    server->route_count++;
    
    return 1;
}

// Start HTTP server
int http_server_start(HttpServer* server) {
    if (!server) return HTTP_SERVER_ERROR_SOCKET_CREATE;
    if (server->running) return HTTP_SERVER_ERROR_ALREADY_RUNNING;
    
    // Create socket
    server->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socket_fd < 0) {
        return HTTP_SERVER_ERROR_SOCKET_CREATE;
    }
    
    // Set socket options
    int opt = 1;
    setsockopt(server->socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // Bind socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server->port);
    
    if (bind(server->socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(server->socket_fd);
        return HTTP_SERVER_ERROR_BIND;
    }
    
    // Listen for connections
    if (listen(server->socket_fd, server->max_connections) < 0) {
        close(server->socket_fd);
        return HTTP_SERVER_ERROR_LISTEN;
    }
    
    // Make socket non-blocking
    int flags = fcntl(server->socket_fd, F_GETFL, 0);
    if (flags < 0) {
        close(server->socket_fd);
        return HTTP_SERVER_ERROR_FCNTL;
    }
    int result = fcntl(server->socket_fd, F_SETFL, flags | O_NONBLOCK);
    if (result < 0) {
        close(server->socket_fd);
        return HTTP_SERVER_ERROR_FCNTL;
    }
    
    server->running = true;
    g_http_server = server;
    g_server_running = true;
    
    return HTTP_SERVER_SUCCESS;
}

// Handle server loop (non-blocking)
int http_server_handle_requests(HttpServer* server) {
    if (!server || !server->running) return 0;
    
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    // Use non-blocking accept
    int client_fd = accept(server->socket_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd >= 0) {
        handle_client_connection(client_fd, server);
        return 1; // Request handled
    } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // No connection available, this is normal for non-blocking socket
        return 0; // No request
    } else {
        // Error occurred
        return -1;
    }
}

// Stop HTTP server
void http_server_stop(HttpServer* server) {
    if (!server || !server->running) return;
    
    g_server_running = false;
    server->running = false;
    
    if (server->socket_fd >= 0) {
        close(server->socket_fd);
        server->socket_fd = -1;
    }
    
    printf("HTTP server stopped\n");
}

// Background thread function to handle requests
void* http_server_background_loop(void* arg) {
    HttpServer* server = (HttpServer*)arg;
    if (!server) {
        printf("ERROR: No server passed to background thread\n");
        return NULL;
    }
    
    printf("HTTP server background thread started for server on port %d\n", server->port);
    printf("Server running: %s\n", server->running ? "true" : "false");
    printf("Server socket_fd: %d\n", server->socket_fd);
    
    while (server->running) {
        
        // Handle any pending requests
        int result = http_server_handle_requests(server);
        if (result < 0) {
            // Error occurred, break the loop
            break;
        }
        
        // Small delay to prevent busy waiting
        usleep(1000); // 1ms delay
    }
    
    
    printf("HTTP server background thread stopped\n");
    return NULL;
}

// Free HTTP server
void http_server_free(HttpServer* server) {
    if (!server) return;
    
    // Stop server if running
    if (server->running) {
        http_server_stop(server);
    }
    
    // Free routes
    for (size_t i = 0; i < server->route_count; i++) {
        HttpRoute* route = &server->routes[i];
        if (route->method) shared_free_safe(route->method, "http_server", "http_server_free", 0);
        if (route->path) shared_free_safe(route->path, "http_server", "http_server_free", 0);
    }
    
    if (server->routes) {
        shared_free_safe(server->routes, "http_server", "http_server_free", 0);
    }
    
    shared_free_safe(server, "http_server", "http_server_free", 0);
}
