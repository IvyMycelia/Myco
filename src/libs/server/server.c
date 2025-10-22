#include "libs/server/server.h"
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include "../../include/core/standardized_errors.h"
#include "../../include/utils/shared_utilities.h"
#include "../../include/libs/json.h"
#include "../../include/libs/http_server.h"
#include "../../include/libs/compression.h"

// Define inotify constants for compatibility
#define IN_MODIFY 0x00000002

// Global server instance (for simplicity, we'll support one server at a time)
static MycoServer* g_server = NULL;
static Route* g_routes = NULL;
static StaticRoute* g_static_routes = NULL;
static Interpreter* g_interpreter = NULL; // Store interpreter reference for route handlers

// Global response body storage
static char* g_response_body = NULL;

// Global current request parameters (for access from request.param())
RouteParam* g_current_request_params = NULL;

// Global current route group prefix
char* g_current_route_prefix = NULL;

// Global request body storage
static char* g_request_body = NULL;

// Global response data for modifications
static int g_response_status_code = 200;
static char* g_response_content_type = NULL;

// Create a new server instance
MycoServer* server_create(int port, Interpreter* interpreter) {
    MycoServer* server = (MycoServer*)shared_malloc_safe(sizeof(MycoServer), "libs", "unknown_function", 35);
    if (!server) return NULL;
    
    server->port = port;
    server->running = false;
    server->daemon = NULL;
    server->interpreter = interpreter;
    server->config = NULL;
    server->middleware = NULL;
    server->file_watcher = NULL;
    server->signal_handlers = NULL;
    
    return server;
}

// Free server resources
void server_free(MycoServer* server) {
    if (!server) return;
    
    if (server->daemon) {
        http_server_stop((HttpServer*)server->daemon);
    }
    
    // Free config
    if (server->config) {
        free_server_config(server->config);
    }
    
    // Free middleware chain
    Middleware* current = server->middleware;
    while (current) {
        Middleware* next = current->next;
        middleware_free(current);
        current = next;
    }
    
    // Free file watcher
    if (server->file_watcher) {
        file_watcher_free(server->file_watcher);
    }
    
    // Free signal handlers
    SignalHandler* current_handler = server->signal_handlers;
    while (current_handler) {
        SignalHandler* next = current_handler->next;
        signal_handler_free(current_handler);
        current_handler = next;
    }
    
    shared_free_safe(server, "libs", "unknown_function", 84);
}

// Create a new route parameter
RouteParam* route_param_create(const char* name, const char* value) {
    RouteParam* param = (RouteParam*)shared_malloc_safe(sizeof(RouteParam), "libs", "unknown_function", 89);
    if (!param) return NULL;
    
    param->name = (name ? strdup(name) : NULL);
    param->value = (value ? strdup(value) : NULL);
    param->next = NULL;
    
    return param;
}

// Free a route parameter
void route_param_free(RouteParam* param) {
    if (!param) return;
    
    shared_free_safe(param->name, "libs", "unknown_function", 103);
    shared_free_safe(param->value, "libs", "unknown_function", 104);
    shared_free_safe(param, "libs", "unknown_function", 105);
}

// Free all route parameters
void route_params_free(RouteParam* params) {
    while (params) {
        RouteParam* next = params->next;
        route_param_free(params);
        params = next;
    }
}

// Find a route parameter by name
RouteParam* route_params_find(RouteParam* params, const char* name) {
    RouteParam* current = params;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Create a new route
Route* route_create(const char* method, const char* path, Value handler) {
    Route* route = (Route*)shared_malloc_safe(sizeof(Route), "libs", "unknown_function", 131);
    if (!route) return NULL;
    
    route->method = (method ? strdup(method) : NULL);
    route->path = (path ? strdup(path) : NULL);
    route->pattern = (path ? strdup(path) : NULL);  // Store the original pattern
    route->params = NULL;  // Initialize params to NULL
    route->handler = value_clone(&handler);  // Clone the function value
    route->next = NULL;
    
    return route;
}

// Free route resources
void route_free(Route* route) {
    if (!route) return;
    
    shared_free_safe(route->method, "libs", "unknown_function", 148);
    shared_free_safe(route->path, "libs", "unknown_function", 149);
    shared_free_safe(route->pattern, "libs", "unknown_function", 150);
    route_params_free(route->params);  // Free all parameters
    value_free(&route->handler);  // Free the function value
    shared_free_safe(route, "libs", "unknown_function", 153);
}

// Add route to the global routes list
void route_add(Route* route) {
    if (!route) return;
    
    if (!g_routes) {
        g_routes = route;
    } else {
        Route* current = g_routes;
        while (current->next) {
            current = current->next;
        }
        current->next = route;
    }
}

// Match a route based on method and path, extracting parameters
Route* route_match(Route* routes, const char* method, const char* path) {
    Route* current = routes;
    while (current) {
        if (strcmp(current->method, method) == 0) {
            // Check if this route matches the path (with parameter extraction)
            RouteParam* extracted_params = NULL;
            if (route_path_matches(current->pattern, path, &extracted_params)) {
                // Clear any existing parameters
                route_params_free(current->params);
                // Set the extracted parameters
                current->params = extracted_params;
                return current;
            }
        }
        current = current->next;
    }
    return NULL;
}

// Check if a route pattern matches a path and extract parameters
bool route_path_matches(const char* pattern, const char* path, RouteParam** params) {
    if (!pattern || !path) return false;
    
    // Simple exact match first
    if (strcmp(pattern, path) == 0) {
        *params = NULL;
        return true;
    }
    
    // Check for parameterized routes (containing :)
    if (strstr(pattern, ":") == NULL) {
        return false;  // No parameters, and not exact match
    }
    
    // Split pattern and path into segments
    char** pattern_segments = split_path(pattern);
    char** path_segments = split_path(path);
    
    if (!pattern_segments || !path_segments) {
        free_path_segments(pattern_segments);
        free_path_segments(path_segments);
        return false;
    }
    
    // Count segments
    int pattern_count = 0;
    int path_count = 0;
    while (pattern_segments[pattern_count]) pattern_count++;
    while (path_segments[path_count]) path_count++;
    
    // Must have same number of segments
    if (pattern_count != path_count) {
        free_path_segments(pattern_segments);
        free_path_segments(path_segments);
        return false;
    }
    
    // Match each segment
    RouteParam* param_list = NULL;
    for (int i = 0; i < pattern_count; i++) {
        if (pattern_segments[i][0] == ':') {
            // This is a parameter - extract the name and value
            char* param_name = pattern_segments[i] + 1;  // Skip the ':'
            char* param_value = path_segments[i];
            
            // Check for typed parameters (e.g., :id:int)
            char* type_separator = strstr(param_name, ":");
            if (type_separator) {
                // Extract parameter name and type
                *type_separator = '\0';  // Null-terminate the name
                char* param_type = type_separator + 1;
                
                // Validate the parameter value against the type
                if (!validate_typed_parameter(param_value, param_type)) {
                    route_params_free(param_list);
                    free_path_segments(pattern_segments);
                    free_path_segments(path_segments);
                    return false;
                }
            }
            
            // Add to parameter list
            RouteParam* new_param = route_param_create(param_name, param_value);
            if (new_param) {
                new_param->next = param_list;
                param_list = new_param;
            }
        } else {
            // This is a literal segment - must match exactly
            if (strcmp(pattern_segments[i], path_segments[i]) != 0) {
                route_params_free(param_list);
                free_path_segments(pattern_segments);
                free_path_segments(path_segments);
                return false;
            }
        }
    }
    
    *params = param_list;
    free_path_segments(pattern_segments);
    free_path_segments(path_segments);
    return true;
}

// Split a path into segments
char** split_path(const char* path) {
    if (!path) return NULL;
    
    // Count segments
    int segment_count = 1;
    for (const char* p = path; *p; p++) {
        if (*p == '/') segment_count++;
    }
    
    // Allocate array
    char** segments = (char**)calloc(segment_count + 1, sizeof(char*));
    if (!segments) return NULL;
    
    // Split the path
    char* path_copy = (path ? strdup(path) : NULL);
    if (!path_copy) {
        shared_free_safe(segments, "libs", "unknown_function", 295);
        return NULL;
    }
    
    int i = 0;
    char* token = strtok(path_copy, "/");
    while (token && i < segment_count) {
        segments[i] = (token ? strdup(token) : NULL);
        token = strtok(NULL, "/");
        i++;
    }
    
    shared_free_safe(path_copy, "libs", "unknown_function", 307);
    return segments;
}

// Free path segments array
void free_path_segments(char** segments) {
    if (!segments) return;
    
    for (int i = 0; segments[i]; i++) {
        shared_free_safe(segments[i], "libs", "unknown_function", 316);
    }
    shared_free_safe(segments, "libs", "unknown_function", 318);
}

// Validate a typed parameter value
bool validate_typed_parameter(const char* value, const char* type) {
    if (!value || !type) return false;
    
    if (strcmp(type, "int") == 0) {
        // Check if the value is a valid integer
        char* endptr;
        long int_val = strtol(value, &endptr, 10);
        return (*endptr == '\0' && int_val >= INT_MIN && int_val <= INT_MAX);
    } else if (strcmp(type, "float") == 0) {
        // Check if the value is a valid float
        char* endptr;
        strtof(value, &endptr);
        return (*endptr == '\0');
    } else if (strcmp(type, "string") == 0) {
        // String type accepts any non-empty value
        return (strlen(value) > 0);
    } else if (strcmp(type, "uuid") == 0) {
        // Basic UUID validation (8-4-4-4-12 format)
        if (strlen(value) != 36) return false;
        for (int i = 0; i < 36; i++) {
            char c = value[i];
            if (i == 8 || i == 13 || i == 18 || i == 23) {
                if (c != '-') return false;
            } else {
                if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
                    return false;
                }
            }
        }
        return true;
    } else if (strcmp(type, "email") == 0) {
        // Basic email validation
        if (strlen(value) == 0) return false;
        char* at_pos = strchr(value, '@');
        if (!at_pos || at_pos == value) return false;
        char* dot_pos = strrchr(at_pos, '.');
        if (!dot_pos || dot_pos <= at_pos + 1) return false;
        return true;
    }
    
    // Unknown type - reject
    return false;
}

// Static file serving functions
StaticRoute* static_route_create(const char* url_prefix, const char* file_path) {
    StaticRoute* route = (StaticRoute*)shared_malloc_safe(sizeof(StaticRoute), "libs", "unknown_function", 368);
    if (!route) return NULL;
    
    route->url_prefix = (url_prefix ? strdup(url_prefix) : NULL);
    route->file_path = (file_path ? strdup(file_path) : NULL);
    route->enable_gzip = false;
    route->enable_cache = false;
    route->cache_duration = 3600; // Default 1 hour
    route->next = NULL;
    
    return route;
}

void static_route_free(StaticRoute* route) {
    if (!route) return;
    shared_free_safe(route->url_prefix, "libs", "unknown_function", 383);
    shared_free_safe(route->file_path, "libs", "unknown_function", 384);
    shared_free_safe(route, "libs", "unknown_function", 385);
}

void static_route_add(StaticRoute* route) {
    if (!route) return;
    
    if (!g_static_routes) {
        g_static_routes = route;
    } else {
        StaticRoute* current = g_static_routes;
        while (current->next) {
            current = current->next;
        }
        current->next = route;
    }
}

StaticRoute* static_route_match(const char* url) {
    StaticRoute* current = g_static_routes;
    while (current) {
        size_t prefix_len = strlen(current->url_prefix);
        // Check for exact prefix match (not just starts with)
        if (strncmp(url, current->url_prefix, prefix_len) == 0) {
            // Make sure it's either exactly the prefix or followed by a slash
            if (url[prefix_len] == '\0' || url[prefix_len] == '/') {
                return current;
            }
        }
        current = current->next;
    }
    return NULL;
}

// Get MIME type based on file extension
char* get_mime_type(const char* filename) {
    if (!filename) return "application/octet-stream";
    
    const char* ext = strrchr(filename, '.');
    if (!ext) return "application/octet-stream";
    
    ext++; // Skip the dot
    
    if (strcmp(ext, "html") == 0 || strcmp(ext, "htm") == 0) return "text/html";
    if (strcmp(ext, "css") == 0) return "text/css";
    if (strcmp(ext, "js") == 0) return "application/javascript";
    if (strcmp(ext, "json") == 0) return "application/json";
    if (strcmp(ext, "png") == 0) return "image/png";
    if (strcmp(ext, "jpg") == 0 || strcmp(ext, "jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, "gif") == 0) return "image/gif";
    if (strcmp(ext, "svg") == 0) return "image/svg+xml";
    if (strcmp(ext, "ico") == 0) return "image/x-icon";
    if (strcmp(ext, "txt") == 0) return "text/plain";
    if (strcmp(ext, "xml") == 0) return "application/xml";
    if (strcmp(ext, "pdf") == 0) return "application/pdf";
    
    return "application/octet-stream";
}

// Check if file exists
bool file_exists(const char* path) {
    if (!path) return false;
    
    FILE* file = fopen(path, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

// Read file content
char* read_file_content(const char* path, size_t* size) {
    if (!path || !size) return NULL;
    
    FILE* file = fopen(path, "rb");
    if (!file) return NULL;
    
    // Get file size
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Allocate buffer
    char* content = (char*)shared_malloc_safe(*size + 1, "libs", "unknown_function", 469);
    if (!content) {
        fclose(file);
        return NULL;
    }
    
    // Read content
    size_t bytes_read = fread(content, 1, *size, file);
    fclose(file);
    
    if (bytes_read != *size) {
        shared_free_safe(content, "libs", "unknown_function", 480);
        return NULL;
    }
    
    content[*size] = '\0';
    return content;
}

// Request body parsing functions
Value parse_json_body(const char* body) {
    if (!body || strlen(body) == 0) {
        return value_create_null();
    }
    
    // Use the JSON library to parse the request body
    Value json_string = value_create_string(body);
    Value parsed_json = builtin_json_parse(NULL, &json_string, 1, 0, 0);
    value_free(&json_string);
    
    if (parsed_json.type == VALUE_NULL) {
        // If JSON parsing fails, return a simple object with the raw body
        Value obj = value_create_object(4);
        value_object_set(&obj, "__class_name__", value_create_string("JSON"));
        value_object_set(&obj, "raw", value_create_string(body));
        return obj;
    }
    
    return parsed_json;
}

Value parse_form_body(const char* body) {
    if (!body || strlen(body) == 0) {
        return value_create_null();
    }
    
    // Parse form data (key=value&key2=value2)
    Value obj = value_create_object(4);
    value_object_set(&obj, "__class_name__", value_create_string("FormData"));
    
    char* body_copy = (body ? strdup(body) : NULL);
    char* token = strtok(body_copy, "&");
    
    while (token) {
        char* equal_pos = strchr(token, '=');
        if (equal_pos) {
            *equal_pos = '\0';
            char* key = url_decode(token);
            char* value = url_decode(equal_pos + 1);
            
            value_object_set(&obj, key, value_create_string(value));
            
            shared_free_safe(key, "libs", "unknown_function", 524);
            shared_free_safe(value, "libs", "unknown_function", 525);
        }
        token = strtok(NULL, "&");
    }
    
    shared_free_safe(body_copy, "libs", "unknown_function", 530);
    return obj;
}

Value parse_query_string(const char* query_string) {
    if (!query_string || strlen(query_string) == 0) {
        return value_create_null();
    }
    
    // Parse query string (?key=value&key2=value2)
    Value obj = value_create_object(4);
    value_object_set(&obj, "__class_name__", value_create_string("QueryParams"));
    
    char* query_copy = (query_string ? strdup(query_string) : NULL);
    char* token = strtok(query_copy, "&");
    
    while (token) {
        char* equal_pos = strchr(token, '=');
        if (equal_pos) {
            *equal_pos = '\0';
            char* key = url_decode(token);
            char* value = url_decode(equal_pos + 1);
            
            value_object_set(&obj, key, value_create_string(value));
            
            shared_free_safe(key, "libs", "unknown_function", 555);
            shared_free_safe(value, "libs", "unknown_function", 556);
        }
        token = strtok(NULL, "&");
    }
    
    shared_free_safe(query_copy, "libs", "unknown_function", 561);
    return obj;
}

char* url_decode(const char* str) {
    if (!str) return NULL;
    
    size_t len = strlen(str);
    char* decoded = (char*)shared_malloc_safe(len + 1, "libs", "unknown_function", 569);
    if (!decoded) return NULL;
    
    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '%' && i + 2 < len) {
            char hex[3] = {str[i+1], str[i+2], '\0'};
            int value = (int)strtol(hex, NULL, 16);
            decoded[j++] = (char)value;
            i += 2;
        } else if (str[i] == '+') {
            decoded[j++] = ' ';
        } else {
            decoded[j++] = str[i];
        }
    }
    decoded[j] = '\0';
    
    return decoded;
}

// HTTP request handler for libmicrohttpd
int server_handle_request(void* cls, void* connection, const char* url, const char* method, const char* version, const char* upload_data, size_t* upload_data_size, 
                                      void** con_cls) {
    
    // Handle request body data
    if (upload_data && upload_data_size && *upload_data_size > 0) {
        // Store the request body globally
        if (g_request_body) {
            shared_free_safe(g_request_body, "libs", "unknown_function", 600);
        }
        g_request_body = (char*)shared_malloc_safe(*upload_data_size + 1, "libs", "unknown_function", 602);
        strncpy(g_request_body, upload_data, *upload_data_size);
        g_request_body[*upload_data_size] = '\0';
        *upload_data_size = 0; // Mark as processed
        return 1;
    }
    
    // Check for static file serving first (only for GET requests)
    if (strcmp(method, "GET") == 0) {
        StaticRoute* static_route = static_route_match(url);
        if (static_route) {
            // Build the full file path
            char file_path[1024];
            snprintf(file_path, sizeof(file_path), "%s%s", static_route->file_path, url + strlen(static_route->url_prefix));
            
            // Check if file exists
            if (file_exists(file_path)) {
                size_t file_size;
                char* file_content = read_file_content(file_path, &file_size);
                if (file_content) {
                    // Get MIME type
                    char* mime_type = get_mime_type(file_path);
                    
                    // Check if we should compress this file
                    bool should_compress = static_route->enable_gzip && should_compress_file(file_path);
                    char* response_content = file_content;
                    size_t response_size = file_size;
                    char* compressed_content = NULL;
                    
                    if (should_compress) {
                        size_t compressed_size;
                        compressed_content = compress_gzip(file_content, file_size, &compressed_size);
                        if (compressed_content) {
                            response_content = compressed_content;
                            response_size = compressed_size;
                        }
                    }
                    
                    // Create response
                    // Note: In our custom implementation, we handle response creation differently
                    // For now, we'll use a simplified approach
                    printf("Serving static file: %s (size: %zu, type: %s)\n", url, response_size, mime_type);
                    
                    // Add compression header if compressed
                    if (should_compress && compressed_content) {
                        printf("Adding Content-Encoding: gzip header\n");
                    }
                    
                    // Add cache headers if enabled
                    if (static_route->enable_cache) {
                        char* cache_headers = get_cache_headers(static_route->cache_duration);
                        if (cache_headers) {
                            // Parse and add cache headers
                            char* line = strtok(cache_headers, "\r\n");
                            while (line) {
                                char* colon = strchr(line, ':');
                                if (colon) {
                                    *colon = '\0';
                                    printf("Adding header: %s = %s\n", line, colon + 1);
                                }
                                line = strtok(NULL, "\r\n");
                            }
                            shared_free_safe(cache_headers, "libs", "unknown_function", 665);
                        }
                    }
                    
                    printf("Sending 200 OK response\n");
                    
                    // Free compressed content if it was created
                    if (compressed_content) {
                        shared_free_safe(compressed_content, "libs", "unknown_function", 674);
                    }
                    
                    return 1;
                }
            }
        }
    }
    
    // Find matching route
    Route* route = route_match(g_routes, method, url);
    if (!route) {
        // Return 404 for unmatched routes
        const char* response = "404 Not Found";
        printf("404 Not Found: %s\n", url);
        // Note: In our custom implementation, we handle 404 responses differently
        return 1;
    }
    
    // Store the matched route's parameters for this request
    RouteParam* request_params = route->params;
    g_current_request_params = request_params;  // Set global for request.param() access
    
    // Parse the HTTP request
    MycoRequest* request = parse_http_request(connection, url, method);
    if (!request) {
        const char* response = "500 Internal Server Error";
        printf("500 Internal Server Error: Failed to parse request\n");
        // Note: In our custom implementation, we handle 500 responses differently
        return 1;
    }
    
    // For now, skip query string parsing - it's complex with libmicrohttpd
    // TODO: Implement proper query string extraction
    
    // Create HTTP response
    MycoResponse* response = create_http_response();
    if (!response) {
        free_request_object(request);
        const char* error_response = "500 Internal Server Error";
        printf("500 Internal Server Error: Failed to create response\n");
        // Note: In our custom implementation, we handle 500 responses differently
        return 1;
    }
    
    
    // Create Myco Request and Response objects
    Value req_obj = create_request_object(request);
    Value res_obj = create_response_object(response);
    
    // Reset global response data
    g_response_status_code = 200;
    if (g_response_content_type) {
        shared_free_safe(g_response_content_type, "libs", "unknown_function", 736);
        g_response_content_type = NULL;
    }
    
    // Execute middleware if any
    if (g_server && g_server->middleware) {
        execute_middleware(g_server, req_obj, res_obj, value_create_null());
    }
    
    // Call the route handler if it exists
    if (route->handler.type == VALUE_FUNCTION && g_interpreter) {
        // Execute the Myco route handler function
        Value handler_result = execute_myco_function(g_interpreter, route->handler, &req_obj, &res_obj);
        value_free(&handler_result);
        
        // After route handler execution, sync any changes from global response data back to the local response object
        value_object_set(&res_obj, "statusCode", value_create_number(g_response_status_code));
        if (g_response_content_type) {
            value_object_set(&res_obj, "contentType", value_create_string(g_response_content_type));
        }
    } else {
        // Default response if no handler
        value_object_set(&res_obj, "body", value_create_string("Hello from Myco Server!"));
    }
    
    // Get the response body and status code
    // Use global response body if available, otherwise fall back to object
    const char* response_body;
    if (g_response_body) {
        response_body = g_response_body;
    } else {
        Value body_val = value_object_get(&res_obj, "body");
        response_body = body_val.type == VALUE_STRING ? body_val.data.string_value : "Hello from Myco Server!";
    }
    
    // Get status code from global response data
    int status_code = g_response_status_code;
    
    
    // Create response
    printf("Sending response: %d, body length: %zu\n", status_code, strlen(response_body));
    // Note: In our custom implementation, we handle response sending differently
    
    // Clean up
    value_free(&req_obj);
    value_free(&res_obj);
    free_request_object(request);
    free_response_object(response);
    
    // Clear global request parameters
    g_current_request_params = NULL;
    
    return 1;
}

// Create a new server
Value builtin_server_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "server.create() requires exactly 1 argument (port or config)", line, column);
        return value_create_null();
    }
    
    Value arg = args[0];
    ServerConfig* config = NULL;
    int port = 8080; // Default port
    
    // Check if argument is a config object/map or just a port number
    if (arg.type == VALUE_OBJECT || arg.type == VALUE_HASH_MAP) {
        // Parse config object
        config = parse_server_config(arg);
        if (!config) {
            std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "Invalid config object", line, column);
            return value_create_null();
        }
        port = config->port;
    } else if (arg.type == VALUE_NUMBER) {
        // Just a port number
        port = (int)arg.data.number_value;
    } else {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "server.create() requires a port number or config object", line, column);
        return value_create_null();
    }
    
    // Free existing server if any
    if (g_server) {
        server_free(g_server);
    }
    
    // Create new server
    if (config) {
        g_server = server_create_with_config(config, interpreter);
    } else {
        g_server = server_create(port, interpreter);
    }
    
    if (!g_server) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "Failed to create server", line, column);
        if (config) free_server_config(config);
        return value_create_null();
    }
    
    // Store interpreter reference for route handlers
    g_interpreter = interpreter;
    
    // Create server object for Myco
    Value server_obj = value_create_object(17);
    value_object_set(&server_obj, "port", value_create_number(port));
    value_object_set(&server_obj, "running", value_create_boolean(false));
    value_object_set(&server_obj, "__class_name__", value_create_string("Server"));
    
    // Add methods to the server object
    value_object_set(&server_obj, "listen", value_create_builtin_function(builtin_server_listen));
    value_object_set(&server_obj, "stop", value_create_builtin_function(builtin_server_stop));
    value_object_set(&server_obj, "use", value_create_builtin_function(builtin_server_use_method));
    value_object_set(&server_obj, "group", value_create_builtin_function(builtin_server_group));
    value_object_set(&server_obj, "close", value_create_builtin_function(builtin_server_close));
    value_object_set(&server_obj, "get", value_create_builtin_function(builtin_server_get));
    value_object_set(&server_obj, "post", value_create_builtin_function(builtin_server_post));
    value_object_set(&server_obj, "put", value_create_builtin_function(builtin_server_put));
    value_object_set(&server_obj, "delete", value_create_builtin_function(builtin_server_delete));
    value_object_set(&server_obj, "static", value_create_builtin_function(builtin_server_static));
    value_object_set(&server_obj, "watch", value_create_builtin_function(builtin_server_watch));
    value_object_set(&server_obj, "onSignal", value_create_builtin_function(builtin_server_onSignal));
    
    return server_obj;
}

// Start the server
Value builtin_server_listen(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "server.listen() requires server object", line, column);
        return value_create_null();
    }
    
    Value server_obj = args[0];
    
    if (!g_server) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "No server created. Call server.create() first", line, column);
        return value_create_null();
    }
    
    if (g_server->running) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "Server is already running", line, column);
        return value_create_null();
    }
    
    // Start the HTTP daemon
    // Start the HTTP daemon using our custom implementation
    HttpServer* http_server = http_server_create(g_server->port);
    if (!http_server) {
        printf("Failed to create HTTP server\n");
        return value_create_null();
    }
    
    // Add a default route handler
    http_server_add_route(http_server, "GET", "/", NULL);
    http_server_add_route(http_server, "POST", "/", NULL);
    http_server_add_route(http_server, "PUT", "/", NULL);
    http_server_add_route(http_server, "DELETE", "/", NULL);
    
    g_server->daemon = (void*)http_server;
    
    if (!g_server->daemon) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "Failed to start server", line, column);
        return value_create_null();
    }
    
    g_server->running = true;
    printf("Server started on port %d\n", g_server->port);
    
    // Update the server object's running property
    value_object_set(&server_obj, "running", value_create_boolean(true));
    
    // Keep the server running by waiting for requests
    printf("Server is running. Press Ctrl+C to stop.\n");
    while (g_server->running) {
        // Sleep for a short time to prevent busy waiting
        usleep(100000); // 100ms
    }
    
    // Server has stopped, exit the program
    printf("Server shutdown complete.\n");
    exit(0);
    
    return value_create_null();
}

// Stop the server
Value builtin_server_stop(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "server.stop() requires server object", line, column);
        return value_create_null();
    }
    
    Value server_obj = args[0];
    
    if (!g_server || !g_server->running) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "Server is not running", line, column);
        return value_create_null();
    }
    
    http_server_stop((HttpServer*)g_server->daemon);
    g_server->daemon = NULL;
    g_server->running = false;
    
    // Update the server object's running property
    value_object_set(&server_obj, "running", value_create_boolean(false));
    
    printf("Server stopped\n");
    return value_create_null();
}

// Register a GET route
Value builtin_server_get(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2 && arg_count != 3) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "server.get() requires exactly 2 arguments (path, handler)", line, column);
        return value_create_null();
    }
    
    // Handle both server library calls (2 args) and server object calls (3 args)
    Value path_val = (arg_count == 3) ? args[1] : args[0];
    Value handler_val = (arg_count == 3) ? args[2] : args[1];
    
    if (path_val.type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "server", "unknown_function", "server.get() path must be a string", line, column);
        return value_create_null();
    }
    
    if (handler_val.type != VALUE_FUNCTION) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "server.get() handler must be a function", line, column);
        return value_create_null();
    }
    
    // Create and add route with handler
    Route* route = route_create("GET", path_val.data.string_value, handler_val);
    if (route) {
        route_add(route);
    }
    
    // Always return server object for chaining
    Value server_obj = value_create_object(17);
    value_object_set(&server_obj, "port", value_create_number(g_server ? g_server->port : 8080));
    value_object_set(&server_obj, "running", value_create_boolean(g_server ? g_server->running : false));
    value_object_set(&server_obj, "__class_name__", value_create_string("Server"));
    
    // Add methods to the server object
    value_object_set(&server_obj, "listen", value_create_builtin_function(builtin_server_listen));
    value_object_set(&server_obj, "stop", value_create_builtin_function(builtin_server_stop));
    value_object_set(&server_obj, "use", value_create_builtin_function(builtin_server_use_method));
    value_object_set(&server_obj, "group", value_create_builtin_function(builtin_server_group));
    value_object_set(&server_obj, "close", value_create_builtin_function(builtin_server_close));
    value_object_set(&server_obj, "get", value_create_builtin_function(builtin_server_get));
    value_object_set(&server_obj, "post", value_create_builtin_function(builtin_server_post));
    value_object_set(&server_obj, "put", value_create_builtin_function(builtin_server_put));
    value_object_set(&server_obj, "delete", value_create_builtin_function(builtin_server_delete));
    value_object_set(&server_obj, "static", value_create_builtin_function(builtin_server_static));
    value_object_set(&server_obj, "watch", value_create_builtin_function(builtin_server_watch));
    value_object_set(&server_obj, "onSignal", value_create_builtin_function(builtin_server_onSignal));
    
    return server_obj;
}

// Register a POST route
Value builtin_server_post(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2 && arg_count != 3) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "server.post() requires exactly 2 arguments (path, handler)", line, column);
        return value_create_null();
    }
    
    // Handle both server library calls (2 args) and server object calls (3 args)
    Value path_val = (arg_count == 3) ? args[1] : args[0];
    Value handler_val = (arg_count == 3) ? args[2] : args[1];
    
    if (path_val.type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "server", "unknown_function", "server.post() path must be a string", line, column);
        return value_create_null();
    }
    
    if (handler_val.type != VALUE_FUNCTION) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "server.post() handler must be a function", line, column);
        return value_create_null();
    }
    
    // Create and add route with handler
    Route* route = route_create("POST", path_val.data.string_value, handler_val);
    if (route) {
        route_add(route);
    }
    
    return value_create_null();
}

// Register a PUT route
Value builtin_server_put(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2 && arg_count != 3) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "server.put() requires exactly 2 arguments (path, handler)", line, column);
        return value_create_null();
    }
    
    // Handle both server library calls (2 args) and server object calls (3 args)
    Value path_val = (arg_count == 3) ? args[1] : args[0];
    Value handler_val = (arg_count == 3) ? args[2] : args[1];
    
    if (path_val.type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "server", "unknown_function", "server.put() path must be a string", line, column);
        return value_create_null();
    }
    
    if (handler_val.type != VALUE_FUNCTION) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "server.put() handler must be a function", line, column);
        return value_create_null();
    }
    
    // Create and add route with handler
    Route* route = route_create("PUT", path_val.data.string_value, handler_val);
    if (route) {
        route_add(route);
    }
    
    return value_create_null();
}

// Register a DELETE route
Value builtin_server_delete(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2 && arg_count != 3) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "server.delete() requires exactly 2 arguments (path, handler)", line, column);
        return value_create_null();
    }
    
    // Handle both server library calls (2 args) and server object calls (3 args)
    Value path_val = (arg_count == 3) ? args[1] : args[0];
    Value handler_val = (arg_count == 3) ? args[2] : args[1];
    
    if (path_val.type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "server", "unknown_function", "server.delete() path must be a string", line, column);
        return value_create_null();
    }
    
    if (handler_val.type != VALUE_FUNCTION) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "server.delete() handler must be a function", line, column);
        return value_create_null();
    }
    
    // Create and add route with handler
    Route* route = route_create("DELETE", path_val.data.string_value, handler_val);
    if (route) {
        route_add(route);
    }
    
    return value_create_null();
}

// Register static file serving
Value builtin_server_static(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2 || arg_count > 4) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "server.static() requires 2-3 arguments (url_prefix, file_path, [options])", line, column);
        return value_create_null();
    }
    
    // Handle both server library calls and server object calls
    Value url_prefix_val = (arg_count >= 3) ? args[1] : args[0];
    Value file_path_val = (arg_count >= 3) ? args[2] : args[1];
    Value options_val = (arg_count >= 4) ? args[3] : value_create_null();
    
    if (url_prefix_val.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "server", "unknown_function", "server.static() first argument must be a string (URL prefix)", line, column);
        return value_create_null();
    }
    
    if (file_path_val.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "server", "unknown_function", "server.static() second argument must be a string (file path)", line, column);
        return value_create_null();
    }
    
    // Create and add static route
    StaticRoute* static_route = static_route_create(url_prefix_val.data.string_value, file_path_val.data.string_value);
    if (!static_route) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "Failed to create static route", line, column);
        return value_create_null();
    }
    
    // Parse options if provided
    if (options_val.type == VALUE_OBJECT || options_val.type == VALUE_HASH_MAP) {
        // Check for gzip option
        Value gzip_key = value_create_string("gzip");
        Value gzip_val = (options_val.type == VALUE_HASH_MAP) ? 
            value_hash_map_get(&options_val, gzip_key) : 
            value_object_get(&options_val, "gzip");
        if (gzip_val.type == VALUE_BOOLEAN) {
            static_route->enable_gzip = gzip_val.data.boolean_value;
        }
        value_free(&gzip_key);
        value_free(&gzip_val);
        
        // Check for cache option
        Value cache_key = value_create_string("cache");
        Value cache_val = (options_val.type == VALUE_HASH_MAP) ? 
            value_hash_map_get(&options_val, cache_key) : 
            value_object_get(&options_val, "cache");
        if (cache_val.type == VALUE_BOOLEAN) {
            static_route->enable_cache = cache_val.data.boolean_value;
        }
        value_free(&cache_key);
        value_free(&cache_val);
        
        // Check for cache duration
        Value cache_duration_key = value_create_string("cacheDuration");
        Value cache_duration_val = (options_val.type == VALUE_HASH_MAP) ? 
            value_hash_map_get(&options_val, cache_duration_key) : 
            value_object_get(&options_val, "cacheDuration");
        if (cache_duration_val.type == VALUE_NUMBER) {
            static_route->cache_duration = (int)cache_duration_val.data.number_value;
        }
        value_free(&cache_duration_key);
        value_free(&cache_duration_val);
    }
    
    static_route_add(static_route);
    
    printf("Static file serving enabled: %s -> %s (gzip: %s, cache: %s)\n", 
           url_prefix_val.data.string_value, file_path_val.data.string_value,
           static_route->enable_gzip ? "yes" : "no",
           static_route->enable_cache ? "yes" : "no");
    return value_create_null();
}

// Request method implementations
Value builtin_request_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "request.method() takes no arguments", line, column);
        return value_create_null();
    }
    
    Value request_obj = args[0];
    if (request_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "request.method() must be called on a request object", line, column);
        return value_create_null();
    }
    
    Value method_val = value_object_get(&request_obj, "method");
    Value result = value_clone(&method_val);
    value_free(&method_val);
    return result;
}

Value builtin_request_url(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "request.url() takes no arguments", line, column);
        return value_create_null();
    }
    
    Value request_obj = args[0];
    if (request_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "request.url() must be called on a request object", line, column);
        return value_create_null();
    }
    
    Value url_val = value_object_get(&request_obj, "url");
    Value result = value_clone(&url_val);
    value_free(&url_val);
    return result;
}

Value builtin_request_path(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "request.path() takes no arguments", line, column);
        return value_create_null();
    }
    
    Value request_obj = args[0];
    if (request_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "request.path() must be called on a request object", line, column);
        return value_create_null();
    }
    
    Value path_val = value_object_get(&request_obj, "path");
    Value result = value_clone(&path_val);
    value_free(&path_val);
    return result;
}

Value builtin_request_body(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "request.body() takes no arguments", line, column);
        return value_create_null();
    }
    
    Value request_obj = args[0];
    if (request_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "request.body() must be called on a request object", line, column);
        return value_create_null();
    }
    
    Value body_val = value_object_get(&request_obj, "body");
    Value result = value_clone(&body_val);
    value_free(&body_val);
    return result;
}

Value builtin_request_header(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "request.header() requires exactly 2 arguments (request, header_name)", line, column);
        return value_create_null();
    }
    
    Value request_obj = args[0];
    Value header_name_val = args[1];
    
    if (request_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INVALID_ARGUMENT, "server", "unknown_function", "request.header() first argument must be a request object", line, column);
        return value_create_null();
    }
    
    if (header_name_val.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "server", "unknown_function", "request.header() second argument must be a string (header name)", line, column);
        return value_create_null();
    }
    
    // Get the request object's internal MycoRequest structure
    Value request_ptr_value = value_object_get(&request_obj, "__request_ptr__");
    if (request_ptr_value.type != VALUE_NUMBER) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "Invalid request object", line, column);
        return value_create_string("");
    }
    
    MycoRequest* request = (MycoRequest*)(uintptr_t)request_ptr_value.data.number_value;
    if (!request || !request->headers) {
        return value_create_string("");
    }
    
    // Search for the requested header (case-insensitive)
    const char* header_name = header_name_val.data.string_value;
    for (size_t i = 0; i < request->header_count; i++) {
        size_t key_index = i * 2;
        size_t value_index = key_index + 1;
        
        if (request->headers[key_index] && request->headers[value_index]) {
            // Case-insensitive comparison
            if (strcasecmp(request->headers[key_index], header_name) == 0) {
                return value_create_string(request->headers[value_index]);
            }
        }
    }
    
    return value_create_string("");
}

Value builtin_request_param(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "request.param() requires exactly 2 arguments (request, param_name)", line, column);
        return value_create_null();
    }
    
    Value request_obj = args[0];
    Value param_name_val = args[1];
    
    if (request_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INVALID_ARGUMENT, "server", "unknown_function", "request.param() first argument must be a request object", line, column);
        return value_create_null();
    }
    
    if (param_name_val.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "server", "unknown_function", "request.param() second argument must be a string (parameter name)", line, column);
        return value_create_null();
    }
    
    // Find the parameter in the current request's parameters
    // We need to store the current request parameters globally for access
    extern RouteParam* g_current_request_params;
    if (g_current_request_params) {
        RouteParam* param = route_params_find(g_current_request_params, param_name_val.data.string_value);
        if (param) {
            return value_create_string(param->value);
        }
    }
    
    // Parameter not found
    return value_create_null();
}

Value builtin_request_json(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "request.json() takes no arguments", line, column);
        return value_create_null();
    }
    
    Value request_obj = args[0];
    
    if (request_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INVALID_ARGUMENT, "server", "unknown_function", "request.json() first argument must be a request object", line, column);
        return value_create_null();
    }
    
    // Get the request body from the request object
    Value body_val = value_object_get(&request_obj, "body");
    if (body_val.type == VALUE_STRING) {
        return parse_json_body(body_val.data.string_value);
    }
    
    return value_create_null();
}

Value builtin_request_form(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "request.form() takes no arguments", line, column);
        return value_create_null();
    }
    
    Value request_obj = args[0];
    
    if (request_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INVALID_ARGUMENT, "server", "unknown_function", "request.form() first argument must be a request object", line, column);
        return value_create_null();
    }
    
    // Get the request body from the request object
    Value body_val = value_object_get(&request_obj, "body");
    if (body_val.type == VALUE_STRING) {
        return parse_form_body(body_val.data.string_value);
    }
    
    return value_create_null();
}


Value builtin_request_query(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1 || arg_count > 2) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "request.query() requires 1 or 2 arguments (request_obj, [param_name])", line, column);
        return value_create_null();
    }
    
    Value request_obj = args[0];
    
    if (request_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INVALID_ARGUMENT, "server", "unknown_function", "request.query() first argument must be a request object", line, column);
        return value_create_null();
    }
    
    // Get the query string from the request object
    Value query_val = value_object_get(&request_obj, "queryString");
    if (query_val.type == VALUE_STRING) {
        Value query_params = parse_query_string(query_val.data.string_value);
        
        // If a specific parameter name is requested
        if (arg_count == 2) {
            Value param_name = args[1];
            if (param_name.type == VALUE_STRING) {
                Value param_value = value_object_get(&query_params, param_name.data.string_value);
                value_free(&query_params);
                return param_value;
            }
        }
        
        return query_params;
    }
    
    return value_create_null();
}

// Response method implementations
Value builtin_response_send(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "response.send() requires exactly 2 arguments (response, data)", line, column);
        return value_create_null();
    }
    
    Value response_obj = args[0];
    Value data_val = args[1];
    
    if (response_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INVALID_ARGUMENT, "server", "unknown_function", "response.send() first argument must be a response object", line, column);
        return value_create_null();
    }
    
    // Set the response body using global storage
    if (data_val.type == VALUE_STRING) {
        // Free existing global response body
        if (g_response_body) {
            shared_free_safe(g_response_body, "libs", "unknown_function", 1366);
        }
        
        // Store the response body globally
        g_response_body = (data_val.data.string_value ? strdup(data_val.data.string_value) : NULL);
        
        // Also set it in the object (for compatibility)
        Value body_value = value_create_string(data_val.data.string_value);
        value_object_set(&response_obj, "body", body_value);
        
        // Global response body is already updated above
    } else {
        // Convert to string
        Value str_val = value_to_string(&data_val);
        
        // Free existing global response body
        if (g_response_body) {
            shared_free_safe(g_response_body, "libs", "unknown_function", 1383);
        }
        
        // Store the response body globally
        g_response_body = (str_val.data.string_value ? strdup(str_val.data.string_value) : NULL);
        
        value_object_set(&response_obj, "body", str_val);
        
        // Global response body is already updated above
    }
    
    return response_obj; // Return response object for chaining
}

Value builtin_response_json(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "response.json() requires exactly 2 arguments (response, data)", line, column);
        return value_create_null();
    }
    
    Value response_obj = args[0];
    Value data_val = args[1];
    
    if (response_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INVALID_ARGUMENT, "server", "unknown_function", "response.json() first argument must be a response object", line, column);
        return value_create_null();
    }
    
    // Set content type to JSON
    value_object_set(&response_obj, "contentType", value_create_string("application/json"));
    
    // Update global response content type
    if (g_response_content_type) {
        shared_free_safe(g_response_content_type, "libs", "unknown_function", 1416);
    }
    g_response_content_type = ("application/json" ? strdup("application/json") : NULL);
    
    // Use JSON library to serialize the data
    Value json_str = builtin_json_stringify(interpreter, &data_val, 1, line, column);
    if (json_str.type == VALUE_NULL) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "Failed to serialize data to JSON", line, column);
        return value_create_null();
    }
    
    // Set the response body to the JSON string
    value_object_set(&response_obj, "body", json_str);
    
    // Also set the global response body for the final response
    if (g_response_body) {
        shared_free_safe(g_response_body, "libs", "unknown_function", 1426);
    }
    g_response_body = (json_str.data.string_value ? strdup(json_str.data.string_value) : NULL);
    
    // Clean up the JSON string value
    value_free(&json_str);
    
    return response_obj; // Return response object for chaining
}

Value builtin_response_status(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "response.status() requires exactly 2 arguments (response, code)", line, column);
        return value_create_null();
    }
    
    Value response_obj = args[0];
    Value code_val = args[1];
    
    if (response_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INVALID_ARGUMENT, "server", "unknown_function", "response.status() first argument must be a response object", line, column);
        return value_create_null();
    }
    
    if (code_val.type != VALUE_NUMBER) {
        std_error_report(ERROR_INVALID_ARGUMENT, "server", "unknown_function", "response.status() second argument must be a number (status code)", line, column);
        return value_create_null();
    }
    
    // Set the status code on the local response object
    value_object_set(&response_obj, "statusCode", value_clone(&code_val));
    
    // Update global response status code
    g_response_status_code = (int)code_val.data.number_value;
    
    // Ensure the returned response object has all methods and class name
    value_object_set(&response_obj, "__class_name__", value_create_string("Response"));
    value_object_set(&response_obj, "send", value_create_builtin_function(builtin_response_send));
    value_object_set(&response_obj, "json", value_create_builtin_function(builtin_response_json));
    value_object_set(&response_obj, "status", value_create_builtin_function(builtin_response_status));
    value_object_set(&response_obj, "header", value_create_builtin_function(builtin_response_header));
    value_object_set(&response_obj, "sendFile", value_create_builtin_function(builtin_response_send_file));
    value_object_set(&response_obj, "setHeader", value_create_builtin_function(builtin_response_set_header));
    
    return response_obj; // Return response object for chaining
}

Value builtin_response_header(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 3) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "response.header() requires exactly 3 arguments (response, name, value)", line, column);
        return value_create_null();
    }
    
    Value response_obj = args[0];
    Value name_val = args[1];
    Value value_val = args[2];
    
    if (response_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INVALID_ARGUMENT, "server", "unknown_function", "response.header() first argument must be a response object", line, column);
        return value_create_null();
    }
    
    if (name_val.type != VALUE_STRING || value_val.type != VALUE_STRING) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "response.header() name and value must be strings", line, column);
        return value_create_null();
    }
    
    // For now, just return - TODO: implement header storage
    return response_obj; // Return response object for chaining
}

Value builtin_response_send_file(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "response.sendFile() requires exactly 2 arguments (response, file_path)", line, column);
        return value_create_null();
    }
    
    Value response_obj = args[0];
    Value file_path_val = args[1];
    
    if (response_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INVALID_ARGUMENT, "server", "unknown_function", "response.sendFile() first argument must be a response object", line, column);
        return value_create_null();
    }
    
    if (file_path_val.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "server", "unknown_function", "response.sendFile() second argument must be a string (file path)", line, column);
        return value_create_null();
    }
    
    // Check if file exists
    if (!file_exists(file_path_val.data.string_value)) {
        std_error_report(ERROR_UNDEFINED_VARIABLE, "server", "unknown_function", "File not found", line, column);
        return value_create_null();
    }
    
    // Read file content
    size_t file_size;
    char* file_content = read_file_content(file_path_val.data.string_value, &file_size);
    if (!file_content) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "Failed to read file", line, column);
        return value_create_null();
    }
    
    // Set content type based on file extension
    char* mime_type = get_mime_type(file_path_val.data.string_value);
    value_object_set(&response_obj, "content_type", value_create_string(mime_type));
    
    // Set the file content as response body
    if (g_response_body) {
        shared_free_safe(g_response_body, "libs", "unknown_function", 1533);
    }
    g_response_body = file_content;  // File content will be freed by libmicrohttpd
    
    return response_obj; // Return response object for chaining
}

Value builtin_response_set_header(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 3) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "response.setHeader() requires exactly 3 arguments (response, name, value)", line, column);
        return value_create_null();
    }
    
    Value response_obj = args[0];
    Value name_val = args[1];
    Value value_val = args[2];
    
    if (response_obj.type != VALUE_OBJECT) {
        std_error_report(ERROR_INVALID_ARGUMENT, "server", "unknown_function", "response.setHeader() first argument must be a response object", line, column);
        return value_create_null();
    }
    
    if (name_val.type != VALUE_STRING || value_val.type != VALUE_STRING) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "response.setHeader() name and value must be strings", line, column);
        return value_create_null();
    }
    
    // Store header in response object (for now, just store as properties)
    char header_key[256];
    snprintf(header_key, sizeof(header_key), "header_%s", name_val.data.string_value);
    value_object_set(&response_obj, header_key, value_create_string(value_val.data.string_value));
    
    return response_obj; // Return response object for chaining
}

// Register the server library
void server_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    // Create server library object
    Value server_lib = value_create_object(12);
    value_object_set(&server_lib, "__class_name__", value_create_string("ServerLibrary"));
    value_object_set(&server_lib, "create", value_create_builtin_function(builtin_server_create));
    value_object_set(&server_lib, "listen", value_create_builtin_function(builtin_server_listen));
    value_object_set(&server_lib, "stop", value_create_builtin_function(builtin_server_stop));
    value_object_set(&server_lib, "get", value_create_builtin_function(builtin_server_get));
    value_object_set(&server_lib, "post", value_create_builtin_function(builtin_server_post));
    value_object_set(&server_lib, "put", value_create_builtin_function(builtin_server_put));
    value_object_set(&server_lib, "delete", value_create_builtin_function(builtin_server_delete));
    value_object_set(&server_lib, "static", value_create_builtin_function(builtin_server_static));
    value_object_set(&server_lib, "now", value_create_builtin_function(builtin_server_now));
    value_object_set(&server_lib, "sleep", value_create_builtin_function(builtin_server_sleep));
    value_object_set(&server_lib, "watch", value_create_builtin_function(builtin_server_watch));
    value_object_set(&server_lib, "onSignal", value_create_builtin_function(builtin_server_onSignal));
    value_object_set(&server_lib, "use", value_create_builtin_function(builtin_server_use));
    
    // Register the library in global environment
    environment_define(interpreter->global_environment, "server", server_lib);
    
    // Register global functions
    environment_define(interpreter->global_environment, "next", value_create_builtin_function(builtin_next_function));
}

// Create a Myco Request object from MycoRequest structure
Value create_request_object(MycoRequest* request) {
    if (!request) return value_create_null();
    
    Value req_obj = value_create_object(8);
    value_object_set(&req_obj, "__class_name__", value_create_string("Request"));
    
    // Set request properties
    if (request->method) {
        value_object_set(&req_obj, "method", value_create_string(request->method));
    } else {
        value_object_set(&req_obj, "method", value_create_string(""));
    }
    
    if (request->url) {
        value_object_set(&req_obj, "url", value_create_string(request->url));
    } else {
        value_object_set(&req_obj, "url", value_create_string(""));
    }
    
    if (request->path) {
        value_object_set(&req_obj, "path", value_create_string(request->path));
    } else {
        value_object_set(&req_obj, "path", value_create_string(""));
    }
    
    if (request->query_string) {
        value_object_set(&req_obj, "queryString", value_create_string(request->query_string));
    } else {
        value_object_set(&req_obj, "queryString", value_create_string(""));
    }
    
    if (request->body) {
        value_object_set(&req_obj, "body", value_create_string(request->body));
    } else {
        value_object_set(&req_obj, "body", value_create_string(""));
    }
    
    // Store internal request pointer for method access
    value_object_set(&req_obj, "__request_ptr__", value_create_number((double)(uintptr_t)request));
    
    // Add request methods
    value_object_set(&req_obj, "header", value_create_builtin_function(builtin_request_header));
    value_object_set(&req_obj, "param", value_create_builtin_function(builtin_request_param));
    value_object_set(&req_obj, "json", value_create_builtin_function(builtin_request_json));
    value_object_set(&req_obj, "form", value_create_builtin_function(builtin_request_form));
    value_object_set(&req_obj, "query", value_create_builtin_function(builtin_request_query));
    value_object_set(&req_obj, "body", value_create_builtin_function(builtin_request_body));
    
    return req_obj;
}

// Create a Myco Response object from MycoResponse structure
Value create_response_object(MycoResponse* response) {
    if (!response) return value_create_null();
    
    Value res_obj = value_create_object(8);
    value_object_set(&res_obj, "__class_name__", value_create_string("Response"));
    
    // Set response properties
    value_object_set(&res_obj, "statusCode", value_create_number(response->status_code));
    
    if (response->content_type) {
        value_object_set(&res_obj, "contentType", value_create_string(response->content_type));
    } else {
        value_object_set(&res_obj, "contentType", value_create_string("text/plain"));
    }
    
    if (response->body) {
        value_object_set(&res_obj, "body", value_create_string(response->body));
    } else {
        value_object_set(&res_obj, "body", value_create_string(""));
    }
    
    // Add response methods
    value_object_set(&res_obj, "send", value_create_builtin_function(builtin_response_send));
    value_object_set(&res_obj, "json", value_create_builtin_function(builtin_response_json));
    value_object_set(&res_obj, "status", value_create_builtin_function(builtin_response_status));
    value_object_set(&res_obj, "header", value_create_builtin_function(builtin_response_header));
    value_object_set(&res_obj, "sendFile", value_create_builtin_function(builtin_response_send_file));
    value_object_set(&res_obj, "setHeader", value_create_builtin_function(builtin_response_set_header));
    
    return res_obj;
}

// Structure to hold header parsing context
typedef struct {
    char** headers;
    size_t index;
    size_t max_count;
} HeaderContext;

// Callback to count headers
static int count_headers_callback(void *cls, int kind, const char *key, const char *value) {
    (void)kind; (void)key; (void)value;
    size_t* count = (size_t*)cls;
    (*count)++;
    return 1;
}

// Callback to store headers
static int store_headers_callback(void *cls, int kind, const char *key, const char *value) {
    (void)kind;
    HeaderContext* ctx = (HeaderContext*)cls;
    
    if (key && value && ctx->index < ctx->max_count * 2) {
        ctx->headers[ctx->index] = strdup(key);
        ctx->headers[ctx->index + 1] = strdup(value);
        ctx->index += 2;
    }
    return 1;
}

// Parse HTTP request from libmicrohttpd connection
MycoRequest* parse_http_request(void* connection, const char* url, const char* method) {
    MycoRequest* request = (MycoRequest*)shared_malloc_safe(sizeof(MycoRequest), "libs", "unknown_function", 1676);
    if (!request) return NULL;
    
    // Initialize request
    request->method = method ? (method ? strdup(method) : NULL) : NULL;
    request->url = url ? (url ? strdup(url) : NULL) : NULL;
    
    // Extract path and query string from URL
    if (url) {
        char* query_pos = strchr(url, '?');
        if (query_pos) {
            // Split URL into path and query string
            size_t path_len = query_pos - url;
            request->path = (char*)shared_malloc_safe(path_len + 1, "libs", "unknown_function", 1689);
            strncpy(request->path, url, path_len);
            request->path[path_len] = '\0';
            
            request->query_string = (query_pos + 1 ? strdup(query_pos + 1) : NULL);
        } else {
            request->path = (url ? strdup(url) : NULL);
            request->query_string = NULL;
        }
    } else {
        request->path = NULL;
        request->query_string = NULL;
    }
    // Set request body from global storage
    request->body = g_request_body ? (g_request_body ? strdup(g_request_body) : NULL) : NULL;
    
    // Parse headers using libmicrohttpd
    request->headers = NULL;
    request->header_count = 0;
    
    // Get headers from libmicrohttpd
    if (connection) {
        // First pass: count headers
        size_t header_count = 0;
        // Note: In our custom implementation, we handle header parsing differently
        // For now, we'll skip header parsing
        header_count = 0;
        
        if (header_count > 0) {
            // Allocate header storage
            request->headers = (char**)shared_malloc_safe(header_count * 2 * sizeof(char*), "libs", "unknown_function", 1725);
            if (request->headers) {
                HeaderContext ctx = {
                    .headers = request->headers,
                    .index = 0,
                    .max_count = header_count
                };
                
                // Second pass: store headers
                // Note: In our custom implementation, we handle header parsing differently
                // For now, we'll skip header storage
                
                request->header_count = header_count;
            }
        }
    }
    
    request->params = NULL;
    request->param_count = 0;
    
    return request;
}

// Create a new HTTP response
MycoResponse* create_http_response(void) {
    MycoResponse* response = (MycoResponse*)shared_malloc_safe(sizeof(MycoResponse), "libs", "unknown_function", 1714);
    if (!response) return NULL;
    
    // Initialize response
    response->status_code = 200;
    response->content_type = ("text/plain" ? strdup("text/plain") : NULL);
    response->body = NULL;
    response->headers = NULL;
    response->header_count = 0;
    response->headers_sent = false;
    
    return response;
}

// Free request object
void free_request_object(MycoRequest* request) {
    if (!request) return;
    
    shared_free_safe(request->method, "libs", "unknown_function", 1732);
    shared_free_safe(request->url, "libs", "unknown_function", 1733);
    shared_free_safe(request->path, "libs", "unknown_function", 1734);
    shared_free_safe(request->query_string, "libs", "unknown_function", 1735);
    shared_free_safe(request->body, "libs", "unknown_function", 1736);
    
    // Free headers
    for (size_t i = 0; i < request->header_count; i++) {
        shared_free_safe(request->headers[i], "libs", "unknown_function", 1740);
    }
    shared_free_safe(request->headers, "libs", "unknown_function", 1742);
    
    // Free params
    for (size_t i = 0; i < request->param_count; i++) {
        shared_free_safe(request->params[i], "libs", "unknown_function", 1746);
    }
    shared_free_safe(request->params, "libs", "unknown_function", 1748);
    
    shared_free_safe(request, "libs", "unknown_function", 1750);
}

// Free response object
void free_response_object(MycoResponse* response) {
    if (!response) return;
    
    shared_free_safe(response->content_type, "libs", "unknown_function", 1757);
    shared_free_safe(response->body, "libs", "unknown_function", 1758);
    
    // Free headers
    for (size_t i = 0; i < response->header_count; i++) {
        shared_free_safe(response->headers[i], "libs", "unknown_function", 1762);
    }
    shared_free_safe(response->headers, "libs", "unknown_function", 1764);
    
    shared_free_safe(response, "libs", "unknown_function", 1766);
}

// Execute a Myco function from C with 3 parameters
Value execute_myco_function_3(Interpreter* interpreter, Value function, Value* arg1, Value* arg2, Value* arg3) {
    if (!interpreter || function.type != VALUE_FUNCTION) {
        return value_create_null();
    }
    
    // Check if this is a built-in function
    if (function.data.function_value.body && function.data.function_value.parameters == NULL) {
        // This is a built-in function - call it directly
        Value (*builtin_func)(Interpreter*, Value*, size_t, int, int) = 
            (Value (*)(Interpreter*, Value*, size_t, int, int))function.data.function_value.body;
        
        // Prepare arguments
        Value args[3] = {*arg1, *arg2, *arg3};
        return builtin_func(interpreter, args, 3, 0, 0);
    }
    
    // Handle user-defined functions
    if (function.data.function_value.body) {
        // Save current environment
        Environment* old_env = interpreter->current_environment;
        
        // Create new environment for function execution
        Environment* func_env = environment_create(function.data.function_value.captured_environment);
        
        // Set up function parameters
        if (function.data.function_value.parameter_count >= 3 && function.data.function_value.parameters) {
            // Set up the first parameter (req)
            if (function.data.function_value.parameters[0] && 
                function.data.function_value.parameters[0]->type == AST_NODE_IDENTIFIER) {
                environment_define(func_env, function.data.function_value.parameters[0]->data.identifier_value, *arg1);
            }
            // Set up the second parameter (res)
            if (function.data.function_value.parameters[1] && 
                function.data.function_value.parameters[1]->type == AST_NODE_IDENTIFIER) {
                environment_define(func_env, function.data.function_value.parameters[1]->data.identifier_value, *arg2);
            }
            // Set up the third parameter (next)
            if (function.data.function_value.parameters[2] && 
                function.data.function_value.parameters[2]->type == AST_NODE_IDENTIFIER) {
                environment_define(func_env, function.data.function_value.parameters[2]->data.identifier_value, *arg3);
            }
        }
        
        // Set current environment
        interpreter->current_environment = func_env;
        
        // Execute function body
        Value result = interpreter_execute(interpreter, function.data.function_value.body);
        
        // If the result is null and we have a return value, use that instead
        if (result.type == VALUE_NULL && interpreter->has_return) {
            result = interpreter->return_value;
            interpreter->has_return = 0;
        }
        
        // Restore environment
        interpreter->current_environment = old_env;
        
        return result;
    }
    
    return value_create_null();
}

// Execute a Myco function from C with 1 parameter
Value execute_myco_function_1(Interpreter* interpreter, Value function, Value* arg1) {
    if (!interpreter || function.type != VALUE_FUNCTION) {
        return value_create_null();
    }
    
    // Check if this is a built-in function
    if (function.data.function_value.body && function.data.function_value.parameters == NULL) {
        // This is a built-in function - call it directly
        Value (*builtin_func)(Interpreter*, Value*, size_t, int, int) = 
            (Value (*)(Interpreter*, Value*, size_t, int, int))function.data.function_value.body;
        
        // Prepare arguments
        Value args[1] = {*arg1};
        return builtin_func(interpreter, args, 1, 0, 0);
    }
    
    // Handle user-defined functions
    if (function.data.function_value.body) {
        // Save current environment
        Environment* old_env = interpreter->current_environment;
        
        // Create new environment for function execution
        Environment* func_env = environment_create(function.data.function_value.captured_environment);
        
        // Set up function parameters
        if (function.data.function_value.parameter_count >= 1 && function.data.function_value.parameters) {
            // Set up the first parameter
            if (function.data.function_value.parameters[0] && 
                function.data.function_value.parameters[0]->type == AST_NODE_IDENTIFIER) {
                environment_define(func_env, function.data.function_value.parameters[0]->data.identifier_value, *arg1);
            }
        }
        
        // Set current environment
        interpreter->current_environment = func_env;
        
        // Execute function body
        Value result = interpreter_execute(interpreter, function.data.function_value.body);
        
        // If the result is null and we have a return value, use that instead
        if (result.type == VALUE_NULL && interpreter->has_return) {
            result = interpreter->return_value;
            interpreter->has_return = 0;
        }
        
        // Restore environment
        interpreter->current_environment = old_env;
        
        return result;
    }
    
    return value_create_null();
}

// Execute a Myco function from C
Value execute_myco_function(Interpreter* interpreter, Value function, Value* arg1, Value* arg2) {
    if (!interpreter || function.type != VALUE_FUNCTION) {
        return value_create_null();
    }
    
    // Check if this is a built-in function
    if (function.data.function_value.body && function.data.function_value.parameters == NULL) {
        // This is a built-in function - call it directly
        Value (*builtin_func)(Interpreter*, Value*, size_t, int, int) = 
            (Value (*)(Interpreter*, Value*, size_t, int, int))function.data.function_value.body;
        
        // Prepare arguments
        Value args[2] = {*arg1, *arg2};
        return builtin_func(interpreter, args, 2, 0, 0);
    }
    
    // Handle user-defined functions
    if (function.data.function_value.body) {
        // Save current environment
        Environment* old_env = interpreter->current_environment;
        
        // Create new environment for function execution
        Environment* func_env = environment_create(function.data.function_value.captured_environment);
        
        // Set up function parameters
        if (function.data.function_value.parameter_count >= 2 && function.data.function_value.parameters) {
            // Set up the first parameter (req)
            if (function.data.function_value.parameters[0] && 
                function.data.function_value.parameters[0]->type == AST_NODE_IDENTIFIER) {
                environment_define(func_env, function.data.function_value.parameters[0]->data.identifier_value, *arg1);
            }
            // Set up the second parameter (res)
            if (function.data.function_value.parameters[1] && 
                function.data.function_value.parameters[1]->type == AST_NODE_IDENTIFIER) {
                environment_define(func_env, function.data.function_value.parameters[1]->data.identifier_value, *arg2);
            }
        }
        
        // Set current environment
        interpreter->current_environment = func_env;
        
        // Execute function body
        Value result = interpreter_execute(interpreter, function.data.function_value.body);
        
        // If the result is null and we have a return value, use that instead
        if (result.type == VALUE_NULL && interpreter->has_return) {
            result = interpreter->return_value;
            interpreter->has_return = 0;
        }
        
        // Restore environment
        interpreter->current_environment = old_env;
        
        return result;
    }
    
    return value_create_null();
}

// Add middleware to server (global function - 2 args)
Value builtin_server_use(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "app.use() requires exactly 2 arguments (server, middleware_function)", line, column);
        return value_create_null();
    }
    
    Value server_obj = args[0];
    Value middleware_func = args[1];
    
    if (middleware_func.type != VALUE_FUNCTION) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "app.use() middleware must be a function", line, column);
        return value_create_null();
    }
    
    if (!g_server) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "No server created. Call server.create() first", line, column);
        return value_create_null();
    }
    
    // Add middleware to server
    middleware_add(g_server, middleware_func);
    
    return value_create_null();
}

// Add middleware to server (method on server object - 1 arg)
Value builtin_server_use_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "server.use() requires exactly 1 argument (middleware_function)", line, column);
        return value_create_null();
    }
    
    Value middleware_func = args[0];
    
    if (middleware_func.type != VALUE_FUNCTION) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "server.use() middleware must be a function", line, column);
        return value_create_null();
    }
    
    if (!g_server) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "No server created. Call server.create() first", line, column);
        return value_create_null();
    }
    
    // Add middleware to server
    middleware_add(g_server, middleware_func);
    
    // Return the server object for chaining
    Value server_obj = value_create_object(17);
    value_object_set(&server_obj, "port", value_create_number(g_server->port));
    value_object_set(&server_obj, "running", value_create_boolean(g_server->running));
    value_object_set(&server_obj, "__class_name__", value_create_string("Server"));
    
    // Add methods to the server object
    value_object_set(&server_obj, "listen", value_create_builtin_function(builtin_server_listen));
    value_object_set(&server_obj, "stop", value_create_builtin_function(builtin_server_stop));
    value_object_set(&server_obj, "use", value_create_builtin_function(builtin_server_use_method));
    value_object_set(&server_obj, "group", value_create_builtin_function(builtin_server_group));
    value_object_set(&server_obj, "close", value_create_builtin_function(builtin_server_close));
    value_object_set(&server_obj, "get", value_create_builtin_function(builtin_server_get));
    value_object_set(&server_obj, "post", value_create_builtin_function(builtin_server_post));
    value_object_set(&server_obj, "put", value_create_builtin_function(builtin_server_put));
    value_object_set(&server_obj, "delete", value_create_builtin_function(builtin_server_delete));
    value_object_set(&server_obj, "static", value_create_builtin_function(builtin_server_static));
    value_object_set(&server_obj, "watch", value_create_builtin_function(builtin_server_watch));
    value_object_set(&server_obj, "onSignal", value_create_builtin_function(builtin_server_onSignal));
    
    return server_obj;
}

// Create route group
Value builtin_server_group(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "app.group() requires exactly 2 arguments (server, prefix)", line, column);
        return value_create_null();
    }
    
    Value server_obj = args[0];
    Value prefix_val = args[1];
    
    if (prefix_val.type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "server", "unknown_function", "app.group() prefix must be a string", line, column);
        return value_create_null();
    }
    
    // Store the prefix globally for route group functions to use
    if (g_current_route_prefix) {
        shared_free_safe(g_current_route_prefix, "libs", "unknown_function", 1992);
    }
    g_current_route_prefix = (prefix_val.data.string_value ? strdup(prefix_val.data.string_value) : NULL);
    
    // Create route group object with group-specific functions
    Value group_obj = value_create_object(5);
    value_object_set(&group_obj, "__class_name__", value_create_string("RouteGroup"));
    value_object_set(&group_obj, "prefix", prefix_val);
    value_object_set(&group_obj, "get", value_create_builtin_function(builtin_group_get));
    value_object_set(&group_obj, "post", value_create_builtin_function(builtin_group_post));
    value_object_set(&group_obj, "put", value_create_builtin_function(builtin_group_put));
    value_object_set(&group_obj, "delete", value_create_builtin_function(builtin_group_delete));
    
    return group_obj;
}

// Close server
Value builtin_server_close(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "app.close() requires server object", line, column);
        return value_create_null();
    }
    
    Value server_obj = args[0];
    
    if (!g_server) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "No server to close", line, column);
        return value_create_null();
    }
    
    // Stop the server
    if (g_server->running && g_server->daemon) {
        http_server_stop((HttpServer*)g_server->daemon);
        g_server->running = false;
        g_server->daemon = NULL;
    }
    
    // Update server object
    value_object_set(&server_obj, "running", value_create_boolean(false));
    
    return value_create_null();
}

// Parse server configuration from Myco object
ServerConfig* parse_server_config(Value config_obj) {
    if (config_obj.type != VALUE_OBJECT && config_obj.type != VALUE_HASH_MAP) {
        return NULL;
    }
    
    ServerConfig* config = (ServerConfig*)shared_malloc_safe(sizeof(ServerConfig), "libs", "unknown_function", 2041);
    if (!config) return NULL;
    
    // Set defaults
    config->port = 8080;
    config->static_dir = NULL;
    config->debug = false;
    config->enable_gzip = false;
    config->enable_cache = false;
    
    // Parse port
    Value port_key = value_create_string("port");
    Value port_val = value_hash_map_get(&config_obj, port_key);
    if (port_val.type == VALUE_NUMBER) {
        config->port = (int)port_val.data.number_value;
    }
    value_free(&port_key);
    
    // Parse static directory
    Value static_dir_key = value_create_string("staticDir");
    Value static_dir_val = value_hash_map_get(&config_obj, static_dir_key);
    if (static_dir_val.type == VALUE_STRING) {
        config->static_dir = (static_dir_val.data.string_value ? strdup(static_dir_val.data.string_value) : NULL);
    }
    value_free(&static_dir_key);
    
    // Parse debug flag
    Value debug_key = value_create_string("debug");
    Value debug_val = value_hash_map_get(&config_obj, debug_key);
    if (debug_val.type == VALUE_BOOLEAN) {
        config->debug = debug_val.data.boolean_value;
    }
    value_free(&debug_key);
    
    // Parse gzip flag
    Value gzip_key = value_create_string("enableGzip");
    Value gzip_val = value_hash_map_get(&config_obj, gzip_key);
    if (gzip_val.type == VALUE_BOOLEAN) {
        config->enable_gzip = gzip_val.data.boolean_value;
    }
    value_free(&gzip_key);
    
    // Parse cache flag
    Value cache_key = value_create_string("enableCache");
    Value cache_val = value_hash_map_get(&config_obj, cache_key);
    if (cache_val.type == VALUE_BOOLEAN) {
        config->enable_cache = cache_val.data.boolean_value;
    }
    value_free(&cache_key);
    
    return config;
}

// Free server configuration
void free_server_config(ServerConfig* config) {
    if (!config) return;
    
    if (config->static_dir) {
        shared_free_safe(config->static_dir, "libs", "unknown_function", 2099);
    }
    
    shared_free_safe(config, "libs", "unknown_function", 2102);
}

// Create server with configuration
MycoServer* server_create_with_config(ServerConfig* config, Interpreter* interpreter) {
    MycoServer* server = server_create(config->port, interpreter);
    if (!server) return NULL;
    
    server->config = config;
    server->middleware = NULL;
    
    return server;
}

// Create middleware
Middleware* middleware_create(Value function) {
    Middleware* middleware = (Middleware*)shared_malloc_safe(sizeof(Middleware), "libs", "unknown_function", 2118);
    if (!middleware) return NULL;
    
    middleware->function = function;
    middleware->next = NULL;
    
    return middleware;
}

// Free middleware
void middleware_free(Middleware* middleware) {
    if (!middleware) return;
    
    value_free(&middleware->function);
    shared_free_safe(middleware, "libs", "unknown_function", 2132);
}

// Add middleware to server
void middleware_add(MycoServer* server, Value function) {
    if (!server) return;
    
    Middleware* new_middleware = middleware_create(function);
    if (!new_middleware) return;
    
    // Add to the end of the middleware chain
    if (!server->middleware) {
        server->middleware = new_middleware;
    } else {
        Middleware* current = server->middleware;
        while (current->next) {
            current = current->next;
        }
        current->next = new_middleware;
    }
}


// Next function for middleware
Value builtin_next_function(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // This function is called when middleware calls next()
    // Execute the next middleware in the chain
    execute_next_middleware();
    return value_create_null();
}

// Middleware function template
Value builtin_middleware_func(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // This is a template function for middleware
    // It expects req, res, next as parameters
    if (arg_count != 3) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "middleware_func expects exactly 3 arguments (req, res, next)", line, column);
        return value_create_null();
    }
    
    Value req = args[0];
    Value res = args[1];
    Value next = args[2];
    
    // Simple middleware that just calls next()
    if (next.type == VALUE_FUNCTION) {
        Value result = execute_myco_function(interpreter, next, &req, &res);
        value_free(&result);
    }
    
    return value_create_null();
}

// Route handler template
Value builtin_route_handler(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // This is a template function for route handlers
    // It expects req, res as parameters
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "route_handler expects exactly 2 arguments (req, res)", line, column);
        return value_create_null();
    }
    
    Value req = args[0];
    Value res = args[1];
    
    // Simple route handler that sets a response
    value_object_set(&res, "body", value_create_string("Hello from route handler!"));
    value_object_set(&res, "statusCode", value_create_number(200));
    
    return value_create_null();
}

// Global middleware execution state
static Middleware* g_current_middleware = NULL;
static MycoServer* g_current_server = NULL;
static Value g_current_req;
static Value g_current_res;

// Execute middleware chain
void execute_middleware(MycoServer* server, Value req_obj, Value res_obj, Value next_func) {
    if (!server || !server->middleware) return;
    
    // Initialize global state for middleware execution
    g_current_server = server;
    g_current_middleware = server->middleware;
    g_current_req = value_create_null();
    g_current_res = value_create_null();
    
    // Copy the request and response objects
    g_current_req = value_clone(&req_obj);
    g_current_res = value_clone(&res_obj);
    
    // Start executing the middleware chain
    execute_next_middleware();
}

// Execute the next middleware in the chain
void execute_next_middleware(void) {
    if (!g_current_middleware) {
        // No more middleware to execute
        return;
    }
    
    Middleware* current = g_current_middleware;
    
    // Move to next middleware for the next() call
    g_current_middleware = current->next;
    
    if (current->function.type == VALUE_FUNCTION) {
        // Create next function for middleware
        Value next_func_obj = value_create_builtin_function(builtin_next_function);
        
        // Check if the middleware function has the expected number of parameters
        if (current->function.data.function_value.parameter_count >= 3) {
            // Execute middleware with req, res, and next parameters
            Value result = execute_myco_function_3(g_interpreter, current->function, &g_current_req, &g_current_res, &next_func_obj);
            value_free(&result);
        } else {
            // Fallback: execute with just req and res if parameter count is wrong
            Value result = execute_myco_function(g_interpreter, current->function, &g_current_req, &g_current_res);
            value_free(&result);
        }
        
        value_free(&next_func_obj);
    }
}

// Route group functions that apply the prefix to routes
Value builtin_group_get(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 3) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "group.get() requires exactly 2 arguments (path, handler)", line, column);
        return value_create_null();
    }
    
    Value group_obj = args[0];  // Group object (ignored)
    Value path_val = args[1];   // Path
    Value handler = args[2];    // Handler
    
    if (path_val.type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "server", "unknown_function", "group.get() path must be a string", line, column);
        return value_create_null();
    }
    
    if (handler.type != VALUE_FUNCTION) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "group.get() handler must be a function", line, column);
        return value_create_null();
    }
    
    // Combine prefix with path
    size_t prefix_len = g_current_route_prefix ? strlen(g_current_route_prefix) : 0;
    char* full_path = shared_malloc_safe(prefix_len + strlen(path_val.data.string_value) + 1, "libs", "unknown_function", 2210);
    if (g_current_route_prefix) {
        strcpy(full_path, g_current_route_prefix);
        strcat(full_path, path_val.data.string_value);
    } else {
        strcpy(full_path, path_val.data.string_value);
    }
    
    // Create route with the full path
    Route* route = route_create("GET", full_path, handler);
    if (!route) {
        shared_free_safe(full_path, "libs", "unknown_function", 2217);
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "Failed to create route", line, column);
        return value_create_null();
    }
    
    // Add route to global routes
    route_add(route);
    shared_free_safe(full_path, "libs", "unknown_function", 2224);
    
    return value_create_null();
}

Value builtin_group_post(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 3) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "group.post() requires exactly 2 arguments (path, handler)", line, column);
        return value_create_null();
    }
    
    Value group_obj = args[0];  // Group object (ignored)
    Value path_val = args[1];   // Path
    Value handler = args[2];    // Handler
    
    if (path_val.type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "server", "unknown_function", "group.post() path must be a string", line, column);
        return value_create_null();
    }
    
    if (handler.type != VALUE_FUNCTION) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "group.post() handler must be a function", line, column);
        return value_create_null();
    }
    
    // Combine prefix with path
    size_t prefix_len = g_current_route_prefix ? strlen(g_current_route_prefix) : 0;
    char* full_path = shared_malloc_safe(prefix_len + strlen(path_val.data.string_value) + 1, "libs", "unknown_function", 2250);
    if (g_current_route_prefix) {
        strcpy(full_path, g_current_route_prefix);
        strcat(full_path, path_val.data.string_value);
    } else {
        strcpy(full_path, path_val.data.string_value);
    }
    
    // Create route with the full path
    Route* route = route_create("POST", full_path, handler);
    if (!route) {
        shared_free_safe(full_path, "libs", "unknown_function", 2257);
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "Failed to create route", line, column);
        return value_create_null();
    }
    
    // Add route to global routes
    route_add(route);
    shared_free_safe(full_path, "libs", "unknown_function", 2264);
    
    return value_create_null();
}

Value builtin_group_put(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 3) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "group.put() requires exactly 2 arguments (path, handler)", line, column);
        return value_create_null();
    }
    
    Value group_obj = args[0];  // Group object (ignored)
    Value path_val = args[1];   // Path
    Value handler = args[2];    // Handler
    
    if (path_val.type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "server", "unknown_function", "group.put() path must be a string", line, column);
        return value_create_null();
    }
    
    if (handler.type != VALUE_FUNCTION) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "group.put() handler must be a function", line, column);
        return value_create_null();
    }
    
    // Combine prefix with path
    size_t prefix_len = g_current_route_prefix ? strlen(g_current_route_prefix) : 0;
    char* full_path = shared_malloc_safe(prefix_len + strlen(path_val.data.string_value) + 1, "libs", "unknown_function", 2290);
    if (g_current_route_prefix) {
        strcpy(full_path, g_current_route_prefix);
        strcat(full_path, path_val.data.string_value);
    } else {
        strcpy(full_path, path_val.data.string_value);
    }
    
    // Create route with the full path
    Route* route = route_create("PUT", full_path, handler);
    if (!route) {
        shared_free_safe(full_path, "libs", "unknown_function", 2297);
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "Failed to create route", line, column);
        return value_create_null();
    }
    
    // Add route to global routes
    route_add(route);
    shared_free_safe(full_path, "libs", "unknown_function", 2304);
    
    return value_create_null();
}

Value builtin_group_delete(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 3) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "group.delete() requires exactly 2 arguments (path, handler)", line, column);
        return value_create_null();
    }
    
    Value group_obj = args[0];  // Group object (ignored)
    Value path_val = args[1];   // Path
    Value handler = args[2];    // Handler
    
    if (path_val.type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "server", "unknown_function", "group.delete() path must be a string", line, column);
        return value_create_null();
    }
    
    if (handler.type != VALUE_FUNCTION) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "group.delete() handler must be a function", line, column);
        return value_create_null();
    }
    
    // Combine prefix with path
    size_t prefix_len = g_current_route_prefix ? strlen(g_current_route_prefix) : 0;
    char* full_path = shared_malloc_safe(prefix_len + strlen(path_val.data.string_value) + 1, "libs", "unknown_function", 2330);
    if (g_current_route_prefix) {
        strcpy(full_path, g_current_route_prefix);
        strcat(full_path, path_val.data.string_value);
    } else {
        strcpy(full_path, path_val.data.string_value);
    }
    
    // Create route with the full path
    Route* route = route_create("DELETE", full_path, handler);
    if (!route) {
        shared_free_safe(full_path, "libs", "unknown_function", 2337);
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "Failed to create route", line, column);
        return value_create_null();
    }
    
    // Add route to global routes
    route_add(route);
    shared_free_safe(full_path, "libs", "unknown_function", 2344);
    
    return value_create_null();
}

// Async server functions
Value builtin_server_now(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // For now, return current time as a simple number
    // In a real implementation, this would be async
    time_t current_time = time(NULL);
    return value_create_number((double)current_time);
}

Value builtin_server_sleep(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "server.sleep() requires exactly 1 argument (seconds)", line, column);
        return value_create_null();
    }
    
    Value seconds_val = args[0];
    if (seconds_val.type != VALUE_NUMBER) {
        std_error_report(ERROR_TYPE_MISMATCH, "server", "unknown_function", "server.sleep() seconds must be a number", line, column);
        return value_create_null();
    }
    
    // For now, use blocking sleep
    // In a real implementation, this would be async
    int seconds = (int)seconds_val.data.number_value;
    sleep(seconds);
    
    return value_create_null();
}

// File watching implementation
FileWatcher* file_watcher_create(const char* path, Value callback) {
    FileWatcher* watcher = (FileWatcher*)shared_malloc_safe(sizeof(FileWatcher), "libs", "unknown_function", 2379);
    if (!watcher) return NULL;
    
    watcher->watch_path = (path ? strdup(path) : NULL);
    watcher->callback = value_clone(&callback);
    watcher->active = true;
    watcher->last_check = time(NULL);
    
    // Start watching thread
    if (pthread_create(&watcher->thread, NULL, file_watcher_thread, watcher) != 0) {
        shared_free_safe(watcher->watch_path, "libs", "unknown_function", 2389);
        value_free(&watcher->callback);
        shared_free_safe(watcher, "libs", "unknown_function", 2391);
        return NULL;
    }
    
    return watcher;
}

void file_watcher_free(FileWatcher* watcher) {
    if (!watcher) return;
    
    watcher->active = false;
    
    // Join the thread
    pthread_join(watcher->thread, NULL);
    
    shared_free_safe(watcher->watch_path, "libs", "unknown_function", 2406);
    value_free(&watcher->callback);
    shared_free_safe(watcher, "libs", "unknown_function", 2408);
}

void* file_watcher_thread(void* arg) {
    FileWatcher* watcher = (FileWatcher*)arg;
    
    while (watcher->active) {
        // Sleep for 1 second between checks
        sleep(1);
        
        if (!watcher->active) break;
        
        // Check if directory exists and get its modification time
        struct stat dir_stat;
        if (stat(watcher->watch_path, &dir_stat) == 0) {
            time_t current_time = dir_stat.st_mtime;
            
            // If the directory was modified since last check
            if (current_time > watcher->last_check) {
                watcher->last_check = current_time;
                
                // Call the callback function if it's a Myco function
                if (watcher->callback.type == VALUE_FUNCTION && g_interpreter) {
                    // Create event info object
                    Value event_info = value_create_object(3);
                    value_object_set(&event_info, "path", value_create_string(watcher->watch_path));
                    value_object_set(&event_info, "filename", value_create_string("directory"));
                    value_object_set(&event_info, "mask", value_create_number(IN_MODIFY));
                    
                    // Execute callback
                    Value null_val = value_create_null();
                    Value result = execute_myco_function(g_interpreter, watcher->callback, &event_info, &null_val);
                    value_free(&result);
                    value_free(&event_info);
                    value_free(&null_val);
                }
            }
        }
    }
    
    return NULL;
}

// Register file watching
Value builtin_server_watch(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1 || arg_count > 3) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "server.watch() requires 1-2 arguments (path, [callback])", line, column);
        return value_create_null();
    }
    
    // Handle both server library calls (1-2 args) and server object calls (2-3 args)
    Value path_val = (arg_count >= 2 && args[0].type == VALUE_OBJECT) ? args[1] : args[0];
    Value callback_val = value_create_null();
    
    // Check if callback is provided
    if ((arg_count == 2 && args[0].type != VALUE_OBJECT) || (arg_count == 3 && args[0].type == VALUE_OBJECT)) {
        callback_val = (arg_count == 3) ? args[2] : args[1];
    }
    
    if (path_val.type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "server", "unknown_function", "server.watch() path must be a string", line, column);
        return value_create_null();
    }
    
    // Callback is optional - if not provided, just watch the directory
    if (callback_val.type != VALUE_NULL && callback_val.type != VALUE_FUNCTION) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "server.watch() callback must be a function", line, column);
        return value_create_null();
    }
    
    if (!g_server) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "No server created. Call server.create() first", line, column);
        return value_create_null();
    }
    
    // Free existing watcher if any
    if (g_server->file_watcher) {
        file_watcher_free(g_server->file_watcher);
    }
    
    // Create new file watcher
    g_server->file_watcher = file_watcher_create(path_val.data.string_value, callback_val);
    if (!g_server->file_watcher) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "Failed to create file watcher", line, column);
        return value_create_null();
    }
    
    printf("File watching started for: %s\n", path_val.data.string_value);
    return value_create_null();
}

// Compression and caching functions
char* compress_gzip(const char* data, size_t data_size, size_t* compressed_size) {
    if (!data || data_size == 0) return NULL;
    
    // Use our custom compression instead of zlib
    return compress_data(data, data_size, COMPRESSION_RLE, compressed_size);
}

bool should_compress_file(const char* filename) {
    if (!filename) return false;
    
    const char* ext = strrchr(filename, '.');
    if (!ext) return false;
    
    // Compress text-based files
    return (strcmp(ext, ".html") == 0 ||
            strcmp(ext, ".css") == 0 ||
            strcmp(ext, ".js") == 0 ||
            strcmp(ext, ".json") == 0 ||
            strcmp(ext, ".xml") == 0 ||
            strcmp(ext, ".txt") == 0 ||
            strcmp(ext, ".svg") == 0);
}

char* get_cache_headers(int cache_duration) {
    char* headers = (char*)shared_malloc_safe(256, "libs", "unknown_function", 2552);
    if (!headers) return NULL;
    
    snprintf(headers, 256, 
             "Cache-Control: public, max-age=%d\r\n"
             "Expires: %ld\r\n",
             cache_duration,
             time(NULL) + cache_duration);
    
    return headers;
}

// Signal handling implementation
SignalHandler* signal_handler_create(int signal, Value callback) {
    SignalHandler* handler = (SignalHandler*)shared_malloc_safe(sizeof(SignalHandler), "libs", "unknown_function", 2566);
    if (!handler) return NULL;
    
    handler->signal = signal;
    handler->callback = value_clone(&callback);
    handler->next = NULL;
    
    return handler;
}

void signal_handler_free(SignalHandler* handler) {
    if (!handler) return;
    
    value_free(&handler->callback);
    shared_free_safe(handler, "libs", "unknown_function", 2580);
}

void signal_handler_register(MycoServer* server, int signal_num, Value callback) {
    if (!server) return;
    
    SignalHandler* handler = signal_handler_create(signal_num, callback);
    if (!handler) return;
    
    // Add to the list
    handler->next = server->signal_handlers;
    server->signal_handlers = handler;
    
    // Register the signal handler
    signal(signal_num, signal_handler_execute);
}

void signal_handler_execute(int signal) {
    if (!g_server) return;
    
    // Find the handler for this signal
    SignalHandler* current = g_server->signal_handlers;
    while (current) {
        if (current->signal == signal) {
            // Execute the callback if it's a Myco function
            if (current->callback.type == VALUE_FUNCTION && g_interpreter) {
                // Create signal info object
                Value signal_info = value_create_object(2);
                value_object_set(&signal_info, "signal", value_create_number(signal));
                value_object_set(&signal_info, "name", value_create_string("SIGTERM")); // Default name
                
                // Set specific signal names
                switch (signal) {
                    case SIGTERM:
                        value_object_set(&signal_info, "name", value_create_string("SIGTERM"));
                        break;
                    case SIGINT:
                        value_object_set(&signal_info, "name", value_create_string("SIGINT"));
                        break;
                    case SIGHUP:
                        value_object_set(&signal_info, "name", value_create_string("SIGHUP"));
                        break;
                    case SIGUSR1:
                        value_object_set(&signal_info, "name", value_create_string("SIGUSR1"));
                        break;
                    case SIGUSR2:
                        value_object_set(&signal_info, "name", value_create_string("SIGUSR2"));
                        break;
                }
                
                // Execute callback with only signal_info parameter
                Value result = execute_myco_function_1(g_interpreter, current->callback, &signal_info);
                value_free(&result);
                value_free(&signal_info);
            }
            break;
        }
        current = current->next;
    }
}

// Register signal handling
Value builtin_server_onSignal(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2 && arg_count != 3) {
        std_error_report(ERROR_ARGUMENT_COUNT, "server", "unknown_function", "server.onSignal() requires exactly 2 arguments (signal, callback)", line, column);
        return value_create_null();
    }
    
    // Handle both server library calls and server object calls
    Value signal_val = (arg_count >= 3) ? args[1] : args[0];
    Value callback_val = (arg_count >= 3) ? args[2] : args[1];
    
    if (signal_val.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "server", "unknown_function", "server.onSignal() first argument must be a string (signal name)", line, column);
        return value_create_null();
    }
    
    if (callback_val.type != VALUE_FUNCTION) {
        std_error_report(ERROR_INVALID_ARGUMENT, "server", "unknown_function", "server.onSignal() second argument must be a function", line, column);
        return value_create_null();
    }
    
    if (!g_server) {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "No server created. Call server.create() first", line, column);
        return value_create_null();
    }
    
    // Parse signal name
    int signal_num = -1;
    const char* signal_name = signal_val.data.string_value;
    
    if (strcmp(signal_name, "SIGTERM") == 0) {
        signal_num = SIGTERM;
    } else if (strcmp(signal_name, "SIGINT") == 0) {
        signal_num = SIGINT;
    } else if (strcmp(signal_name, "SIGHUP") == 0) {
        signal_num = SIGHUP;
    } else if (strcmp(signal_name, "SIGUSR1") == 0) {
        signal_num = SIGUSR1;
    } else if (strcmp(signal_name, "SIGUSR2") == 0) {
        signal_num = SIGUSR2;
    } else {
        std_error_report(ERROR_INTERNAL_ERROR, "server", "unknown_function", "Unsupported signal name. Use SIGTERM, SIGINT, SIGHUP, SIGUSR1, or SIGUSR2", line, column);
        return value_create_null();
    }
    
    // Register the signal handler
    signal_handler_register(g_server, signal_num, callback_val);
    
    printf("Signal handler registered for: %s\n", signal_name);
    return value_create_null();
}
