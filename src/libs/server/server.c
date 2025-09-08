#include "libs/server/server.h"

// Global server instance (for simplicity, we'll support one server at a time)
static MycoServer* g_server = NULL;
static Route* g_routes = NULL;
static StaticRoute* g_static_routes = NULL;
static Interpreter* g_interpreter = NULL; // Store interpreter reference for route handlers

// Global response body storage
static char* g_response_body = NULL;

// Global current request parameters (for access from request.param())
RouteParam* g_current_request_params = NULL;

// Create a new server instance
MycoServer* server_create(int port, Interpreter* interpreter) {
    MycoServer* server = (MycoServer*)malloc(sizeof(MycoServer));
    if (!server) return NULL;
    
    server->port = port;
    server->running = false;
    server->daemon = NULL;
    server->interpreter = interpreter;
    
    return server;
}

// Free server resources
void server_free(MycoServer* server) {
    if (!server) return;
    
    if (server->daemon) {
        MHD_stop_daemon(server->daemon);
    }
    
    free(server);
}

// Create a new route parameter
RouteParam* route_param_create(const char* name, const char* value) {
    RouteParam* param = (RouteParam*)malloc(sizeof(RouteParam));
    if (!param) return NULL;
    
    param->name = strdup(name);
    param->value = strdup(value);
    param->next = NULL;
    
    return param;
}

// Free a route parameter
void route_param_free(RouteParam* param) {
    if (!param) return;
    
    free(param->name);
    free(param->value);
    free(param);
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
    Route* route = (Route*)malloc(sizeof(Route));
    if (!route) return NULL;
    
    route->method = strdup(method);
    route->path = strdup(path);
    route->pattern = strdup(path);  // Store the original pattern
    route->params = NULL;  // Initialize params to NULL
    route->handler = value_clone(&handler);  // Clone the function value
    route->next = NULL;
    
    return route;
}

// Free route resources
void route_free(Route* route) {
    if (!route) return;
    
    free(route->method);
    free(route->path);
    free(route->pattern);
    route_params_free(route->params);  // Free all parameters
    value_free(&route->handler);  // Free the function value
    free(route);
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
    char* path_copy = strdup(path);
    if (!path_copy) {
        free(segments);
        return NULL;
    }
    
    int i = 0;
    char* token = strtok(path_copy, "/");
    while (token && i < segment_count) {
        segments[i] = strdup(token);
        token = strtok(NULL, "/");
        i++;
    }
    
    free(path_copy);
    return segments;
}

// Free path segments array
void free_path_segments(char** segments) {
    if (!segments) return;
    
    for (int i = 0; segments[i]; i++) {
        free(segments[i]);
    }
    free(segments);
}

// Static file serving functions
StaticRoute* static_route_create(const char* url_prefix, const char* file_path) {
    StaticRoute* route = (StaticRoute*)malloc(sizeof(StaticRoute));
    if (!route) return NULL;
    
    route->url_prefix = strdup(url_prefix);
    route->file_path = strdup(file_path);
    route->next = NULL;
    
    return route;
}

void static_route_free(StaticRoute* route) {
    if (!route) return;
    
    free(route->url_prefix);
    free(route->file_path);
    free(route);
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
    char* content = (char*)malloc(*size + 1);
    if (!content) {
        fclose(file);
        return NULL;
    }
    
    // Read content
    size_t bytes_read = fread(content, 1, *size, file);
    fclose(file);
    
    if (bytes_read != *size) {
        free(content);
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
    
    // For now, return a simple object representation
    // TODO: Implement proper JSON parsing
    Value obj = value_create_object(4);
    value_object_set(&obj, "__class_name__", value_create_string("JSON"));
    value_object_set(&obj, "raw", value_create_string(body));
    
    return obj;
}

Value parse_form_body(const char* body) {
    if (!body || strlen(body) == 0) {
        return value_create_null();
    }
    
    // Parse form data (key=value&key2=value2)
    Value obj = value_create_object(4);
    value_object_set(&obj, "__class_name__", value_create_string("FormData"));
    
    char* body_copy = strdup(body);
    char* token = strtok(body_copy, "&");
    
    while (token) {
        char* equal_pos = strchr(token, '=');
        if (equal_pos) {
            *equal_pos = '\0';
            char* key = url_decode(token);
            char* value = url_decode(equal_pos + 1);
            
            value_object_set(&obj, key, value_create_string(value));
            
            free(key);
            free(value);
        }
        token = strtok(NULL, "&");
    }
    
    free(body_copy);
    return obj;
}

Value parse_query_string(const char* query_string) {
    if (!query_string || strlen(query_string) == 0) {
        return value_create_null();
    }
    
    // Parse query string (?key=value&key2=value2)
    Value obj = value_create_object(4);
    value_object_set(&obj, "__class_name__", value_create_string("QueryParams"));
    
    char* query_copy = strdup(query_string);
    char* token = strtok(query_copy, "&");
    
    while (token) {
        char* equal_pos = strchr(token, '=');
        if (equal_pos) {
            *equal_pos = '\0';
            char* key = url_decode(token);
            char* value = url_decode(equal_pos + 1);
            
            value_object_set(&obj, key, value_create_string(value));
            
            free(key);
            free(value);
        }
        token = strtok(NULL, "&");
    }
    
    free(query_copy);
    return obj;
}

char* url_decode(const char* str) {
    if (!str) return NULL;
    
    size_t len = strlen(str);
    char* decoded = (char*)malloc(len + 1);
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
enum MHD_Result server_handle_request(void* cls, struct MHD_Connection* connection, 
                                     const char* url, const char* method, const char* version,
                                     const char* upload_data, size_t* upload_data_size, 
                                     void** con_cls) {
    
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
                    
                    // Create response with proper MIME type
                    struct MHD_Response* mhd_response = MHD_create_response_from_buffer(
                        file_size, (void*)file_content, MHD_RESPMEM_MUST_FREE);
                    
                    MHD_add_response_header(mhd_response, "Content-Type", mime_type);
                    
                    enum MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_OK, mhd_response);
                    MHD_destroy_response(mhd_response);
                    return ret;
                }
            }
        }
    }
    
    // Find matching route
    Route* route = route_match(g_routes, method, url);
    if (!route) {
        // Return 404 for unmatched routes
        const char* response = "404 Not Found";
        struct MHD_Response* mhd_response = MHD_create_response_from_buffer(
            strlen(response), (void*)response, MHD_RESPMEM_PERSISTENT);
        
        enum MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, mhd_response);
        MHD_destroy_response(mhd_response);
        return ret;
    }
    
    // Store the matched route's parameters for this request
    RouteParam* request_params = route->params;
    g_current_request_params = request_params;  // Set global for request.param() access
    
    // Parse the HTTP request
    MycoRequest* request = parse_http_request(connection, url, method);
    if (!request) {
        const char* response = "500 Internal Server Error";
        struct MHD_Response* mhd_response = MHD_create_response_from_buffer(
            strlen(response), (void*)response, MHD_RESPMEM_PERSISTENT);
        
        enum MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, mhd_response);
        MHD_destroy_response(mhd_response);
        return ret;
    }
    
    // For now, skip query string parsing - it's complex with libmicrohttpd
    // TODO: Implement proper query string extraction
    
    // Create HTTP response
    MycoResponse* response = create_http_response();
    if (!response) {
        free_request_object(request);
        const char* error_response = "500 Internal Server Error";
        struct MHD_Response* mhd_response = MHD_create_response_from_buffer(
            strlen(error_response), (void*)error_response, MHD_RESPMEM_PERSISTENT);
        
        enum MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, mhd_response);
        MHD_destroy_response(mhd_response);
        return ret;
    }
    
    
    // Create Myco Request and Response objects
    Value req_obj = create_request_object(request);
    Value res_obj = create_response_object(response);
    
    // Call the route handler if it exists
    if (route->handler.type == VALUE_FUNCTION && g_interpreter) {
        // Execute the Myco route handler function
        Value handler_result = execute_myco_function(g_interpreter, route->handler, &req_obj, &res_obj);
        value_free(&handler_result);
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
    
    Value status_val = value_object_get(&res_obj, "status_code");
    int status_code = status_val.type == VALUE_NUMBER ? (int)status_val.data.number_value : 200;
    
    
    // Create libmicrohttpd response
    struct MHD_Response* mhd_response = MHD_create_response_from_buffer(
        strlen(response_body), (void*)response_body, MHD_RESPMEM_PERSISTENT);
    
    enum MHD_Result ret = MHD_queue_response(connection, status_code, mhd_response);
    MHD_destroy_response(mhd_response);
    
    // Clean up
    value_free(&status_val);
    value_free(&req_obj);
    value_free(&res_obj);
    free_request_object(request);
    free_response_object(response);
    
    // Clear global request parameters
    g_current_request_params = NULL;
    
    return ret;
}

// Create a new server
Value builtin_server_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "server.create() requires exactly 1 argument (port)", line, column);
        return value_create_null();
    }
    
    Value port_val = args[0];
    if (port_val.type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "server.create() port must be a number", line, column);
        return value_create_null();
    }
    
    int port = (int)port_val.data.number_value;
    
    // Free existing server if any
    if (g_server) {
        server_free(g_server);
    }
    
    // Create new server
    g_server = server_create(port, interpreter);
    if (!g_server) {
        interpreter_set_error(interpreter, "Failed to create server", line, column);
        return value_create_null();
    }
    
    // Store interpreter reference for route handlers
    g_interpreter = interpreter;
    
    // Create server object for Myco
    Value server_obj = value_create_object(6);
    value_object_set(&server_obj, "port", value_create_number(port));
    value_object_set(&server_obj, "running", value_create_boolean(false));
    value_object_set(&server_obj, "__class_name__", value_create_string("Server"));
    
    // Add methods to the server object
    value_object_set(&server_obj, "listen", value_create_builtin_function(builtin_server_listen));
    value_object_set(&server_obj, "stop", value_create_builtin_function(builtin_server_stop));
    
    return server_obj;
}

// Start the server
Value builtin_server_listen(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "server.listen() requires server object", line, column);
        return value_create_null();
    }
    
    Value server_obj = args[0];
    
    if (!g_server) {
        interpreter_set_error(interpreter, "No server created. Call server.create() first", line, column);
        return value_create_null();
    }
    
    if (g_server->running) {
        interpreter_set_error(interpreter, "Server is already running", line, column);
        return value_create_null();
    }
    
    // Start the HTTP daemon
    g_server->daemon = MHD_start_daemon(
        MHD_USE_SELECT_INTERNALLY,
        g_server->port,
        NULL, NULL,
        &server_handle_request,
        NULL,
        MHD_OPTION_END
    );
    
    if (!g_server->daemon) {
        interpreter_set_error(interpreter, "Failed to start server", line, column);
        return value_create_null();
    }
    
    g_server->running = true;
    printf("Server started on port %d\n", g_server->port);
    
    // Update the server object's running property
    value_object_set(&server_obj, "running", value_create_boolean(true));
    
    return value_create_null();
}

// Stop the server
Value builtin_server_stop(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "server.stop() requires server object", line, column);
        return value_create_null();
    }
    
    Value server_obj = args[0];
    
    if (!g_server || !g_server->running) {
        interpreter_set_error(interpreter, "Server is not running", line, column);
        return value_create_null();
    }
    
    MHD_stop_daemon(g_server->daemon);
    g_server->daemon = NULL;
    g_server->running = false;
    
    // Update the server object's running property
    value_object_set(&server_obj, "running", value_create_boolean(false));
    
    printf("Server stopped\n");
    return value_create_null();
}

// Register a GET route
Value builtin_server_get(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "server.get() requires exactly 2 arguments (path, handler)", line, column);
        return value_create_null();
    }
    
    Value path_val = args[0];
    Value handler_val = args[1];
    
    if (path_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "server.get() path must be a string", line, column);
        return value_create_null();
    }
    
    if (handler_val.type != VALUE_FUNCTION) {
        interpreter_set_error(interpreter, "server.get() handler must be a function", line, column);
        return value_create_null();
    }
    
    // Create and add route with handler
    Route* route = route_create("GET", path_val.data.string_value, handler_val);
    if (route) {
        route_add(route);
    }
    
    return value_create_null();
}

// Register a POST route
Value builtin_server_post(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "server.post() requires exactly 2 arguments (path, handler)", line, column);
        return value_create_null();
    }
    
    Value path_val = args[0];
    Value handler_val = args[1];
    
    if (path_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "server.post() path must be a string", line, column);
        return value_create_null();
    }
    
    if (handler_val.type != VALUE_FUNCTION) {
        interpreter_set_error(interpreter, "server.post() handler must be a function", line, column);
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
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "server.put() requires exactly 2 arguments (path, handler)", line, column);
        return value_create_null();
    }
    
    Value path_val = args[0];
    Value handler_val = args[1];
    
    if (path_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "server.put() path must be a string", line, column);
        return value_create_null();
    }
    
    if (handler_val.type != VALUE_FUNCTION) {
        interpreter_set_error(interpreter, "server.put() handler must be a function", line, column);
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
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "server.delete() requires exactly 2 arguments (path, handler)", line, column);
        return value_create_null();
    }
    
    Value path_val = args[0];
    Value handler_val = args[1];
    
    if (path_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "server.delete() path must be a string", line, column);
        return value_create_null();
    }
    
    if (handler_val.type != VALUE_FUNCTION) {
        interpreter_set_error(interpreter, "server.delete() handler must be a function", line, column);
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
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "server.static() requires exactly 2 arguments (url_prefix, file_path)", line, column);
        return value_create_null();
    }
    
    Value url_prefix_val = args[0];
    Value file_path_val = args[1];
    
    if (url_prefix_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "server.static() first argument must be a string (URL prefix)", line, column);
        return value_create_null();
    }
    
    if (file_path_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "server.static() second argument must be a string (file path)", line, column);
        return value_create_null();
    }
    
    // Create and add static route
    StaticRoute* static_route = static_route_create(url_prefix_val.data.string_value, file_path_val.data.string_value);
    if (static_route) {
        static_route_add(static_route);
    }
    
    return value_create_null();
}

// Request method implementations
Value builtin_request_method(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "request.method() takes no arguments", line, column);
        return value_create_null();
    }
    
    Value request_obj = args[0];
    if (request_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "request.method() must be called on a request object", line, column);
        return value_create_null();
    }
    
    Value method_val = value_object_get(&request_obj, "method");
    Value result = value_clone(&method_val);
    value_free(&method_val);
    return result;
}

Value builtin_request_url(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "request.url() takes no arguments", line, column);
        return value_create_null();
    }
    
    Value request_obj = args[0];
    if (request_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "request.url() must be called on a request object", line, column);
        return value_create_null();
    }
    
    Value url_val = value_object_get(&request_obj, "url");
    Value result = value_clone(&url_val);
    value_free(&url_val);
    return result;
}

Value builtin_request_path(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "request.path() takes no arguments", line, column);
        return value_create_null();
    }
    
    Value request_obj = args[0];
    if (request_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "request.path() must be called on a request object", line, column);
        return value_create_null();
    }
    
    Value path_val = value_object_get(&request_obj, "path");
    Value result = value_clone(&path_val);
    value_free(&path_val);
    return result;
}

Value builtin_request_body(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "request.body() takes no arguments", line, column);
        return value_create_null();
    }
    
    Value request_obj = args[0];
    if (request_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "request.body() must be called on a request object", line, column);
        return value_create_null();
    }
    
    Value body_val = value_object_get(&request_obj, "body");
    Value result = value_clone(&body_val);
    value_free(&body_val);
    return result;
}

Value builtin_request_header(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "request.header() requires exactly 2 arguments (request, header_name)", line, column);
        return value_create_null();
    }
    
    Value request_obj = args[0];
    Value header_name_val = args[1];
    
    if (request_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "request.header() first argument must be a request object", line, column);
        return value_create_null();
    }
    
    if (header_name_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "request.header() second argument must be a string (header name)", line, column);
        return value_create_null();
    }
    
    // For now, return empty string - TODO: implement header parsing
    return value_create_string("");
}

Value builtin_request_param(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "request.param() requires exactly 2 arguments (request, param_name)", line, column);
        return value_create_null();
    }
    
    Value request_obj = args[0];
    Value param_name_val = args[1];
    
    if (request_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "request.param() first argument must be a request object", line, column);
        return value_create_null();
    }
    
    if (param_name_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "request.param() second argument must be a string (parameter name)", line, column);
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
        interpreter_set_error(interpreter, "request.json() takes no arguments", line, column);
        return value_create_null();
    }
    
    Value request_obj = args[0];
    
    if (request_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "request.json() first argument must be a request object", line, column);
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
        interpreter_set_error(interpreter, "request.form() takes no arguments", line, column);
        return value_create_null();
    }
    
    Value request_obj = args[0];
    
    if (request_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "request.form() first argument must be a request object", line, column);
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
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "request.query() takes no arguments", line, column);
        return value_create_null();
    }
    
    Value request_obj = args[0];
    
    if (request_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "request.query() first argument must be a request object", line, column);
        return value_create_null();
    }
    
    // Get the query string from the request object
    Value query_val = value_object_get(&request_obj, "query_string");
    if (query_val.type == VALUE_STRING) {
        return parse_query_string(query_val.data.string_value);
    }
    
    return value_create_null();
}

// Response method implementations
Value builtin_response_send(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "response.send() requires exactly 2 arguments (response, data)", line, column);
        return value_create_null();
    }
    
    Value response_obj = args[0];
    Value data_val = args[1];
    
    if (response_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "response.send() first argument must be a response object", line, column);
        return value_create_null();
    }
    
    // Set the response body using global storage
    if (data_val.type == VALUE_STRING) {
        // Free existing global response body
        if (g_response_body) {
            free(g_response_body);
        }
        
        // Store the response body globally
        g_response_body = strdup(data_val.data.string_value);
        
        // Also set it in the object (for compatibility)
        Value body_value = value_create_string(data_val.data.string_value);
        value_object_set(&response_obj, "body", body_value);
    } else {
        // Convert to string
        Value str_val = value_to_string(&data_val);
        
        // Free existing global response body
        if (g_response_body) {
            free(g_response_body);
        }
        
        // Store the response body globally
        g_response_body = strdup(str_val.data.string_value);
        
        value_object_set(&response_obj, "body", str_val);
    }
    
    return value_create_null();
}

Value builtin_response_json(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "response.json() requires exactly 2 arguments (response, data)", line, column);
        return value_create_null();
    }
    
    Value response_obj = args[0];
    Value data_val = args[1];
    
    if (response_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "response.json() first argument must be a response object", line, column);
        return value_create_null();
    }
    
    // Set content type to JSON
    value_object_set(&response_obj, "content_type", value_create_string("application/json"));
    
    // Set the response body (for now, just convert to string - TODO: implement JSON serialization)
    Value str_val = value_to_string(&data_val);
    value_object_set(&response_obj, "body", str_val);
    
    // Also set the global response body for the final response
    if (g_response_body) {
        free(g_response_body);
    }
    g_response_body = strdup(str_val.data.string_value);
    
    return value_create_null();
}

Value builtin_response_status(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "response.status() requires exactly 2 arguments (response, code)", line, column);
        return value_create_null();
    }
    
    Value response_obj = args[0];
    Value code_val = args[1];
    
    if (response_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "response.status() first argument must be a response object", line, column);
        return value_create_null();
    }
    
    if (code_val.type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "response.status() second argument must be a number (status code)", line, column);
        return value_create_null();
    }
    
    // Set the status code
    value_object_set(&response_obj, "status_code", value_clone(&code_val));
    
    return value_create_null();
}

Value builtin_response_header(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 3) {
        interpreter_set_error(interpreter, "response.header() requires exactly 3 arguments (response, name, value)", line, column);
        return value_create_null();
    }
    
    Value response_obj = args[0];
    Value name_val = args[1];
    Value value_val = args[2];
    
    if (response_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "response.header() first argument must be a response object", line, column);
        return value_create_null();
    }
    
    if (name_val.type != VALUE_STRING || value_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "response.header() name and value must be strings", line, column);
        return value_create_null();
    }
    
    // For now, just return - TODO: implement header storage
    return value_create_null();
}

Value builtin_response_send_file(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "response.sendFile() requires exactly 2 arguments (response, file_path)", line, column);
        return value_create_null();
    }
    
    Value response_obj = args[0];
    Value file_path_val = args[1];
    
    if (response_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "response.sendFile() first argument must be a response object", line, column);
        return value_create_null();
    }
    
    if (file_path_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "response.sendFile() second argument must be a string (file path)", line, column);
        return value_create_null();
    }
    
    // Check if file exists
    if (!file_exists(file_path_val.data.string_value)) {
        interpreter_set_error(interpreter, "File not found", line, column);
        return value_create_null();
    }
    
    // Read file content
    size_t file_size;
    char* file_content = read_file_content(file_path_val.data.string_value, &file_size);
    if (!file_content) {
        interpreter_set_error(interpreter, "Failed to read file", line, column);
        return value_create_null();
    }
    
    // Set content type based on file extension
    char* mime_type = get_mime_type(file_path_val.data.string_value);
    value_object_set(&response_obj, "content_type", value_create_string(mime_type));
    
    // Set the file content as response body
    if (g_response_body) {
        free(g_response_body);
    }
    g_response_body = file_content;  // File content will be freed by libmicrohttpd
    
    return value_create_null();
}

Value builtin_response_set_header(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 3) {
        interpreter_set_error(interpreter, "response.setHeader() requires exactly 3 arguments (response, name, value)", line, column);
        return value_create_null();
    }
    
    Value response_obj = args[0];
    Value name_val = args[1];
    Value value_val = args[2];
    
    if (response_obj.type != VALUE_OBJECT) {
        interpreter_set_error(interpreter, "response.setHeader() first argument must be a response object", line, column);
        return value_create_null();
    }
    
    if (name_val.type != VALUE_STRING || value_val.type != VALUE_STRING) {
        interpreter_set_error(interpreter, "response.setHeader() name and value must be strings", line, column);
        return value_create_null();
    }
    
    // Store header in response object (for now, just store as properties)
    char header_key[256];
    snprintf(header_key, sizeof(header_key), "header_%s", name_val.data.string_value);
    value_object_set(&response_obj, header_key, value_create_string(value_val.data.string_value));
    
    return value_create_null();
}

// Register the server library
void server_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    // Create server library object
    Value server_lib = value_create_object(8);
    value_object_set(&server_lib, "create", value_create_builtin_function(builtin_server_create));
    value_object_set(&server_lib, "listen", value_create_builtin_function(builtin_server_listen));
    value_object_set(&server_lib, "stop", value_create_builtin_function(builtin_server_stop));
    value_object_set(&server_lib, "get", value_create_builtin_function(builtin_server_get));
    value_object_set(&server_lib, "post", value_create_builtin_function(builtin_server_post));
    value_object_set(&server_lib, "put", value_create_builtin_function(builtin_server_put));
    value_object_set(&server_lib, "delete", value_create_builtin_function(builtin_server_delete));
    value_object_set(&server_lib, "static", value_create_builtin_function(builtin_server_static));
    
    // Register the library in global environment
    environment_define(interpreter->global_environment, "server", server_lib);
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
        value_object_set(&req_obj, "query_string", value_create_string(request->query_string));
    } else {
        value_object_set(&req_obj, "query_string", value_create_string(""));
    }
    
    if (request->body) {
        value_object_set(&req_obj, "body", value_create_string(request->body));
    } else {
        value_object_set(&req_obj, "body", value_create_string(""));
    }
    
    // Add request methods
    value_object_set(&req_obj, "header", value_create_builtin_function(builtin_request_header));
    value_object_set(&req_obj, "param", value_create_builtin_function(builtin_request_param));
    value_object_set(&req_obj, "json", value_create_builtin_function(builtin_request_json));
    value_object_set(&req_obj, "form", value_create_builtin_function(builtin_request_form));
    value_object_set(&req_obj, "query", value_create_builtin_function(builtin_request_query));
    
    return req_obj;
}

// Create a Myco Response object from MycoResponse structure
Value create_response_object(MycoResponse* response) {
    if (!response) return value_create_null();
    
    Value res_obj = value_create_object(8);
    value_object_set(&res_obj, "__class_name__", value_create_string("Response"));
    
    // Set response properties
    value_object_set(&res_obj, "status_code", value_create_number(response->status_code));
    
    if (response->content_type) {
        value_object_set(&res_obj, "content_type", value_create_string(response->content_type));
    } else {
        value_object_set(&res_obj, "content_type", value_create_string("text/plain"));
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

// Parse HTTP request from libmicrohttpd connection
MycoRequest* parse_http_request(struct MHD_Connection* connection, const char* url, const char* method) {
    MycoRequest* request = (MycoRequest*)malloc(sizeof(MycoRequest));
    if (!request) return NULL;
    
    // Initialize request
    request->method = method ? strdup(method) : NULL;
    request->url = url ? strdup(url) : NULL;
    
    // Extract path and query string from URL
    if (url) {
        char* query_pos = strchr(url, '?');
        if (query_pos) {
            // Split URL into path and query string
            size_t path_len = query_pos - url;
            request->path = (char*)malloc(path_len + 1);
            strncpy(request->path, url, path_len);
            request->path[path_len] = '\0';
            
            request->query_string = strdup(query_pos + 1);
        } else {
            request->path = strdup(url);
            request->query_string = NULL;
        }
    } else {
        request->path = NULL;
        request->query_string = NULL;
    }
    request->body = NULL;
    request->headers = NULL;
    request->header_count = 0;
    request->params = NULL;
    request->param_count = 0;
    
    // TODO: Parse query string, headers, body, etc.
    
    return request;
}

// Create a new HTTP response
MycoResponse* create_http_response(void) {
    MycoResponse* response = (MycoResponse*)malloc(sizeof(MycoResponse));
    if (!response) return NULL;
    
    // Initialize response
    response->status_code = 200;
    response->content_type = strdup("text/plain");
    response->body = NULL;
    response->headers = NULL;
    response->header_count = 0;
    response->headers_sent = false;
    
    return response;
}

// Free request object
void free_request_object(MycoRequest* request) {
    if (!request) return;
    
    free(request->method);
    free(request->url);
    free(request->path);
    free(request->query_string);
    free(request->body);
    
    // Free headers
    for (size_t i = 0; i < request->header_count; i++) {
        free(request->headers[i]);
    }
    free(request->headers);
    
    // Free params
    for (size_t i = 0; i < request->param_count; i++) {
        free(request->params[i]);
    }
    free(request->params);
    
    free(request);
}

// Free response object
void free_response_object(MycoResponse* response) {
    if (!response) return;
    
    free(response->content_type);
    free(response->body);
    
    // Free headers
    for (size_t i = 0; i < response->header_count; i++) {
        free(response->headers[i]);
    }
    free(response->headers);
    
    free(response);
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
        if (function.data.function_value.parameter_count >= 2) {
            // Set up the first parameter (req)
            if (function.data.function_value.parameters[0]) {
                environment_define(func_env, function.data.function_value.parameters[0]->data.identifier_value, *arg1);
            }
            // Set up the second parameter (res)
            if (function.data.function_value.parameters[1]) {
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
