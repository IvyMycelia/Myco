#include "../../include/libs/http_server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <time.h>
#include "../../include/utils/shared_utilities.h"

// Custom HTTP server implementation to replace libmicrohttpd
// This provides a lightweight, dependency-free HTTP server

// Global server state
static HttpServer* g_server = NULL;
static bool g_server_running = false;

// Create HTTP server
HttpServer* http_server_create(int port) {
    HttpServer* server = shared_malloc_safe(sizeof(HttpServer), "http_server", "http_server_create", 0);
    if (!server) return NULL;
    
    server->port = port;
    server->socket_fd = -1;
    server->running = false;
    server->routes = NULL;
    server->route_count = 0;
    server->max_connections = 100;
    
    return server;
}

// Add route to server
int http_server_add_route(HttpServer* server, const char* method, const char* path, 
                         HttpRouteHandler handler) {
    if (!server || !method || !path || !handler) return 0;
    
    // Expand routes array
    server->routes = shared_realloc_safe(server->routes, 
        (server->route_count + 1) * sizeof(HttpRoute), "http_server", "http_server_add_route", 0);
    if (!server->routes) return 0;
    
    HttpRoute* route = &server->routes[server->route_count];
    route->method = shared_strdup(method);
    route->path = shared_strdup(path);
    route->handler = handler;
    
    if (!route->method || !route->path) {
        if (route->method) shared_free_safe(route->method, "http_server", "http_server_add_route", 0);
        if (route->path) shared_free_safe(route->path, "http_server", "http_server_add_route", 0);
        return 0;
    }
    
    server->route_count++;
    return 1;
}

// Parse HTTP request
static HttpRequest* parse_http_request(const char* request_data, size_t data_len) {
    HttpRequest* request = shared_malloc_safe(sizeof(HttpRequest), "http_server", "parse_http_request", 0);
    if (!request) return NULL;
    
    // Initialize request
    request->method = NULL;
    request->path = NULL;
    request->headers = NULL;
    request->body = NULL;
    request->query_string = NULL;
    
    // Find end of headers
    const char* header_end = strstr(request_data, "\r\n\r\n");
    if (!header_end) {
        shared_free_safe(request, "http_server", "parse_http_request", 0);
        return NULL;
    }
    
    // Parse request line
    const char* line_end = strchr(request_data, '\r');
    if (line_end) {
        size_t line_len = line_end - request_data;
        char* request_line = shared_malloc_safe(line_len + 1, "http_server", "parse_http_request", 0);
        if (request_line) {
            strncpy(request_line, request_data, line_len);
            request_line[line_len] = '\0';
            
            // Parse method, path, version
            char* method_end = strchr(request_line, ' ');
            if (method_end) {
                size_t method_len = method_end - request_line;
                request->method = shared_malloc_safe(method_len + 1, "http_server", "parse_http_request", 0);
                if (request->method) {
                    strncpy(request->method, request_line, method_len);
                    request->method[method_len] = '\0';
                }
                
                char* path_start = method_end + 1;
                char* path_end = strchr(path_start, ' ');
                if (path_end) {
                    size_t path_len = path_end - path_start;
                    request->path = shared_malloc_safe(path_len + 1, "http_server", "parse_http_request", 0);
                    if (request->path) {
                        strncpy(request->path, path_start, path_len);
                        request->path[path_len] = '\0';
                    }
                }
            }
            shared_free_safe(request_line, "http_server", "parse_http_request", 0);
        }
    }
    
    // Extract headers
    size_t header_len = header_end - request_data;
    request->headers = shared_malloc_safe(header_len + 1, "http_server", "parse_http_request", 0);
    if (request->headers) {
        strncpy(request->headers, request_data, header_len);
        request->headers[header_len] = '\0';
    }
    
    // Extract body
    const char* body_start = header_end + 4; // Skip \r\n\r\n
    size_t body_len = data_len - (body_start - request_data);
    if (body_len > 0) {
        request->body = shared_malloc_safe(body_len + 1, "http_server", "parse_http_request", 0);
        if (request->body) {
            strncpy(request->body, body_start, body_len);
            request->body[body_len] = '\0';
        }
    }
    
    return request;
}

// Create HTTP response string
static char* create_http_response(int status_code, const char* content_type, 
                                const char* body, size_t* response_len) {
    const char* status_text = "OK";
    if (status_code == 404) status_text = "Not Found";
    else if (status_code == 500) status_text = "Internal Server Error";
    
    size_t body_len = body ? strlen(body) : 0;
    size_t total_len = 512 + body_len; // Base headers + body
    
    char* response = shared_malloc_safe(total_len, "http_server", "create_http_response", 0);
    if (!response) return NULL;
    
    snprintf(response, total_len,
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n%s",
        status_code, status_text, content_type, body_len, body ? body : "");
    
    *response_len = strlen(response);
    return response;
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
    HttpRequest* request = parse_http_request(buffer, bytes_received);
    if (!request) {
        close(client_fd);
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
        char* http_response = create_http_response(response->status_code, response->content_type,
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
    shared_free_safe(request, "http_server", "handle_client_connection", 0);
    
    close(client_fd);
}

// Start HTTP server
int http_server_start(HttpServer* server) {
    if (!server || server->running) return 0;
    
    // Create socket
    server->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socket_fd < 0) return 0;
    
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
        return 0;
    }
    
    // Listen for connections
    if (listen(server->socket_fd, server->max_connections) < 0) {
        close(server->socket_fd);
        return 0;
    }
    
    server->running = true;
    g_server = server;
    g_server_running = true;
    
    printf("HTTP server started on port %d\n", server->port);
    
    // Main server loop
    while (g_server_running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(server->socket_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd >= 0) {
            handle_client_connection(client_fd, server);
        }
    }
    
    return 1;
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
