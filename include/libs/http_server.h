#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <stddef.h>
#include <stdbool.h>

// Custom HTTP server to replace libmicrohttpd dependency
// Provides lightweight, dependency-free HTTP server functionality

// Forward declarations
typedef struct HttpRequest HttpRequest;
typedef struct HttpResponse HttpResponse;
typedef struct HttpServer HttpServer;
typedef struct HttpRoute HttpRoute;

// HTTP request structure
struct HttpRequest {
    char* method;
    char* path;
    char* headers;
    char* body;
    char* query_string;
};

// HTTP response structure
struct HttpResponse {
    int status_code;
    char* content_type;
    char* body;
};

// HTTP route handler function type
typedef void (*HttpRouteHandler)(HttpRequest* request, HttpResponse* response);

// HTTP route structure
struct HttpRoute {
    char* method;
    char* path;
    HttpRouteHandler handler;
};

// HTTP server structure
struct HttpServer {
    int port;
    int socket_fd;
    bool running;
    HttpRoute* routes;
    size_t route_count;
    int max_connections;
};

// Server management
HttpServer* http_server_create(int port);
int http_server_add_route(HttpServer* server, const char* method, const char* path, 
                         HttpRouteHandler handler);
int http_server_start(HttpServer* server);
void http_server_stop(HttpServer* server);
void http_server_free(HttpServer* server);

#endif // HTTP_SERVER_H
