#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stddef.h>
#include <stdbool.h>

// Custom HTTP client to replace libcurl dependency
// Provides lightweight, dependency-free HTTP functionality

typedef struct {
    int status_code;
    char* body;
    char* headers;
    bool success;
} HttpResponse;

// Core HTTP client functions
HttpResponse* http_client_request(const char* url, const char* method, 
                                const char* headers, const char* body, int timeout_seconds);

// Convenience functions
HttpResponse* http_get(const char* url, const char* headers, int timeout);
HttpResponse* http_post(const char* url, const char* body, const char* headers, int timeout);
HttpResponse* http_put(const char* url, const char* body, const char* headers, int timeout);
HttpResponse* http_delete(const char* url, const char* headers, int timeout);

// Memory management
void http_response_free(HttpResponse* response);

#endif // HTTP_CLIENT_H
