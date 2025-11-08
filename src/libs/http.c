#include "libs/http.h"
#include "libs/builtin_libs.h"
#include "core/interpreter.h"
#include "core/ast.h"
#include "../../include/core/standardized_errors.h"
#include "../../include/utils/shared_utilities.h"
#include "../../include/libs/http_client.h"

// Global HTTP client initialization flag
static bool http_client_initialized = false;

// Initialize HTTP client if not already done
static bool http_init_client() {
    if (!http_client_initialized) {
        // Our custom HTTP client doesn't need initialization
        http_client_initialized = true;
    }
    return true;
}

// Note: Callback functions removed as we're using our custom HTTP client

// Create HTTP request
HttpRequest* http_create_request(HttpMethod method, const char* url) {
    HttpRequest* request = shared_malloc_safe(sizeof(HttpRequest), "libs", "unknown_function", 70);
    if (!request) return NULL;
    
    request->method = method;
    request->url = (url ? strdup(url) : NULL);
    request->headers = NULL;
    request->header_count = 0;
    request->body = NULL;
    request->timeout_seconds = 30;
    request->follow_redirects = true;
    request->user_agent = strdup("Myco-HTTP/1.0");
    
    return request;
}

// Add header to request
void http_add_header(HttpRequest* request, const char* name, const char* value) {
    if (!request || !name || !value) return;
    
    size_t header_len = strlen(name) + strlen(value) + 3; // +3 for ": " and null terminator
    char* header = shared_malloc_safe(header_len, "libs", "unknown_function", 89);
    if (!header) return;
    
    snprintf(header, header_len, "%s: %s", name, value);
    
    request->headers = shared_realloc_safe(request->headers, (request->header_count + 1) * sizeof(char*), "libs", "unknown_function", 94);
    request->headers[request->header_count] = header;
    request->header_count++;
}

// Set request body
void http_set_body(HttpRequest* request, const char* body) {
    if (!request) return;
    
    if (request->body) {
        shared_free_safe(request->body, "libs", "unknown_function", 104);
    }
    
    request->body = body ? strdup(body) : NULL;
}

// Set request timeout
void http_set_timeout(HttpRequest* request, int seconds) {
    if (!request) return;
    request->timeout_seconds = seconds;
}

// Perform HTTP request using our custom HTTP client
HttpResponse* http_request(HttpRequest* request) {
    if (!request || !http_init_client()) {
        HttpResponse* error_response = shared_malloc_safe(sizeof(HttpResponse), "libs", "unknown_function", 119);
        error_response->status_code = 0;
        error_response->body = NULL;
        error_response->headers = NULL;
        error_response->success = false;
        return error_response;
    }
    
    // Convert HttpMethod to string
    const char* method_str = "GET";
    switch (request->method) {
        case HTTP_GET: method_str = "GET"; break;
        case HTTP_POST: method_str = "POST"; break;
        case HTTP_PUT: method_str = "PUT"; break;
        case HTTP_DELETE: method_str = "DELETE"; break;
        case HTTP_PATCH: method_str = "PATCH"; break;
        case HTTP_HEAD: method_str = "HEAD"; break;
        case HTTP_OPTIONS: method_str = "OPTIONS"; break;
    }
    
    // Convert headers array to string
    char* headers_str = NULL;
    if (request->headers && request->header_count > 0) {
        size_t total_len = 0;
        for (size_t i = 0; i < request->header_count; i++) {
            total_len += strlen(request->headers[i]) + 2; // +2 for \r\n
        }
        headers_str = shared_malloc_safe(total_len + 1, "libs", "unknown_function", 150);
        if (headers_str) {
            headers_str[0] = '\0';
            size_t current_pos = 0;
            for (size_t i = 0; i < request->header_count; i++) {
                size_t header_len = strlen(request->headers[i]);
                size_t remaining = total_len - current_pos;
                if (header_len < remaining) {
                    strncat(headers_str, request->headers[i], remaining - 1);
                    current_pos += header_len;
                }
                remaining = total_len - current_pos;
                if (remaining >= 3) {
                    strncat(headers_str, "\r\n", remaining - 1);
                    current_pos += 2;
                }
            }
        }
    }
    
    // Use our custom HTTP client
    HttpResponse* response = NULL;
    if (strcmp(method_str, "GET") == 0) {
        response = http_get(request->url, headers_str, request->timeout_seconds);
    } else if (strcmp(method_str, "POST") == 0) {
        response = http_post(request->url, request->body, headers_str, request->timeout_seconds);
    } else if (strcmp(method_str, "PUT") == 0) {
        response = http_put(request->url, request->body, headers_str, request->timeout_seconds);
    } else if (strcmp(method_str, "DELETE") == 0) {
        response = http_delete(request->url, headers_str, request->timeout_seconds);
    } else {
        // For other methods, use GET as fallback
        response = http_get(request->url, headers_str, request->timeout_seconds);
    }
    
    if (headers_str) {
        shared_free_safe(headers_str, "libs", "unknown_function", 150);
    }
    
    return response;
}

// Note: Convenience functions are defined in http_client.c

// Free response memory
void http_free_response(HttpResponse* response) {
    if (!response) return;
    
    // Use our custom HTTP client's free function
    http_response_free(response);
}

// Free request memory
void http_free_request(HttpRequest* request) {
    if (!request) return;
    
    if (request->url) shared_free_safe(request->url, "libs", "unknown_function", 329);
    if (request->body) shared_free_safe(request->body, "libs", "unknown_function", 330);
    if (request->user_agent) shared_free_safe(request->user_agent, "libs", "unknown_function", 331);
    
    if (request->headers) {
        for (size_t i = 0; i < request->header_count; i++) {
            shared_free_safe(request->headers[i], "libs", "unknown_function", 335);
        }
        shared_free_safe(request->headers, "libs", "unknown_function", 337);
    }
    
    shared_free_safe(request, "libs", "unknown_function", 340);
}

// Response utility functions
char* http_get_header(HttpResponse* response, const char* name) {
    if (!response || !name || !response->headers) return NULL;
    
    // Simple header parsing from the headers string
    char* header_start = response->headers;
    while (*header_start) {
        char* line_end = strstr(header_start, "\r\n");
        if (!line_end) break;
        
        size_t line_len = line_end - header_start;
        char* line = shared_malloc_safe(line_len + 1, "libs", "unknown_function", 150);
        if (line) {
            strncpy(line, header_start, line_len);
            line[line_len] = '\0';
            
            if (strncasecmp(line, name, strlen(name)) == 0) {
                char* colon = strchr(line, ':');
                if (colon) {
                    char* result = strdup(colon + 1);
                    shared_free_safe(line, "libs", "unknown_function", 150);
                    return result;
                }
            }
            shared_free_safe(line, "libs", "unknown_function", 150);
        }
        
        header_start = line_end + 2; // Skip \r\n
    }
    return NULL;
}

bool http_is_success(HttpResponse* response) {
    return response && response->status_code >= 200 && response->status_code < 300;
}

bool http_is_client_error(HttpResponse* response) {
    return response && response->status_code >= 400 && response->status_code < 500;
}

bool http_is_server_error(HttpResponse* response) {
    return response && response->status_code >= 500 && response->status_code < 600;
}

char* http_get_json(HttpResponse* response) {
    if (!response || !response->body) return NULL;
    
    // For now, just return the body as JSON
    // In a real implementation, we'd check the content-type header
    return (response->body ? strdup(response->body) : NULL);
}

// Myco library functions
Value builtin_http_get(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "http", "unknown_function", "http.get() requires at least 1 argument (url)", line, column);
        return value_create_null();
    }
    
    Value url_value = args[0];
    if (url_value.type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "http", "unknown_function", "http.get() URL must be a string", line, column);
        return value_create_null();
    }
    
    HttpResponse* response = http_get(url_value.data.string_value, NULL, 30);
    if (!response) {
        return value_create_null();
    }
    
    // Create Myco response object
    Value response_obj = value_create_object(8);
    value_object_set(&response_obj, "type", value_create_string("Object"));
    value_object_set(&response_obj, "status_code", value_create_number(response->status_code));
    value_object_set(&response_obj, "status_text", value_create_string("OK")); // Simplified
    value_object_set(&response_obj, "body", value_create_string(response->body ? response->body : ""));
    value_object_set(&response_obj, "success", value_create_boolean(response->success));
    value_object_set(&response_obj, "content_type", value_create_string("text/plain")); // Simplified
    value_object_set(&response_obj, "content_length", value_create_number(response->body ? strlen(response->body) : 0));
    
    // No error message in simplified structure
    
    http_free_response(response);
    return response_obj;
}

Value builtin_http_post(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2) {
        std_error_report(ERROR_INTERNAL_ERROR, "http", "unknown_function", "http.post() requires at least 2 arguments (url, data)", line, column);
        return value_create_null();
    }
    
    Value url_value = args[0];
    Value data_value = args[1];
    
    if (url_value.type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "http", "unknown_function", "http.post() URL must be a string", line, column);
        return value_create_null();
    }
    
    if (data_value.type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "http", "unknown_function", "http.post() data must be a string", line, column);
        return value_create_null();
    }
    
    HttpResponse* response = http_post(url_value.data.string_value, data_value.data.string_value, NULL, 30);
    if (!response) {
        return value_create_null();
    }
    
    // Create Myco response object
    Value response_obj = value_create_object(8);
    value_object_set(&response_obj, "type", value_create_string("Object"));
    value_object_set(&response_obj, "status_code", value_create_number(response->status_code));
    value_object_set(&response_obj, "status_text", value_create_string("OK")); // Simplified
    value_object_set(&response_obj, "body", value_create_string(response->body ? response->body : ""));
    value_object_set(&response_obj, "success", value_create_boolean(response->success));
    value_object_set(&response_obj, "content_type", value_create_string("text/plain")); // Simplified
    value_object_set(&response_obj, "content_length", value_create_number(response->body ? strlen(response->body) : 0));
    
    // No error message in simplified structure
    
    http_free_response(response);
    return response_obj;
}

Value builtin_http_put(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2) {
        std_error_report(ERROR_INTERNAL_ERROR, "http", "unknown_function", "http.put() requires at least 2 arguments (url, data)", line, column);
        return value_create_null();
    }
    
    Value url_value = args[0];
    Value data_value = args[1];
    
    if (url_value.type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "http", "unknown_function", "http.put() URL must be a string", line, column);
        return value_create_null();
    }
    
    if (data_value.type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "http", "unknown_function", "http.put() data must be a string", line, column);
        return value_create_null();
    }
    
    HttpResponse* response = http_put(url_value.data.string_value, data_value.data.string_value, NULL, 30);
    if (!response) {
        return value_create_null();
    }
    
    // Create Myco response object
    Value response_obj = value_create_object(8);
    value_object_set(&response_obj, "type", value_create_string("Object"));
    value_object_set(&response_obj, "status_code", value_create_number(response->status_code));
    value_object_set(&response_obj, "status_text", value_create_string("OK")); // Simplified
    value_object_set(&response_obj, "body", value_create_string(response->body ? response->body : ""));
    value_object_set(&response_obj, "success", value_create_boolean(response->success));
    value_object_set(&response_obj, "content_type", value_create_string("text/plain")); // Simplified
    value_object_set(&response_obj, "content_length", value_create_number(response->body ? strlen(response->body) : 0));
    
    // No error message in simplified structure
    
    http_free_response(response);
    return response_obj;
}

Value builtin_http_delete(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "http", "unknown_function", "http.delete() requires at least 1 argument (url)", line, column);
        return value_create_null();
    }
    
    Value url_value = args[0];
    if (url_value.type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "http", "unknown_function", "http.delete() URL must be a string", line, column);
        return value_create_null();
    }
    
    HttpResponse* response = http_delete(url_value.data.string_value, NULL, 30);
    if (!response) {
        return value_create_null();
    }
    
    // Create Myco response object
    Value response_obj = value_create_object(8);
    value_object_set(&response_obj, "type", value_create_string("Object"));
    value_object_set(&response_obj, "status_code", value_create_number(response->status_code));
    value_object_set(&response_obj, "status_text", value_create_string("OK")); // Simplified
    value_object_set(&response_obj, "body", value_create_string(response->body ? response->body : ""));
    value_object_set(&response_obj, "success", value_create_boolean(response->success));
    value_object_set(&response_obj, "content_type", value_create_string("text/plain")); // Simplified
    value_object_set(&response_obj, "content_length", value_create_number(response->body ? strlen(response->body) : 0));
    
    // No error message in simplified structure
    
    http_free_response(response);
    return response_obj;
}

// Placeholder implementations for other functions
Value builtin_http_request(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // For now, just return a simple object
    Value result = value_create_object(4);
    value_object_set(&result, "status_code", value_create_number(200));
    value_object_set(&result, "body", value_create_string("HTTP request placeholder"));
    return result;
}

Value builtin_http_status_ok(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "http", "unknown_function", "http.status_ok() requires exactly 1 argument (response)", line, column);
        return value_create_null();
    }
    
    Value response = args[0];
    if (response.type != VALUE_OBJECT) {
        std_error_report(ERROR_INVALID_ARGUMENT, "http", "unknown_function", "http.statusOk() argument must be a response object", line, column);
        return value_create_null();
    }
    
    // Get the status_code from the response object
    Value status_code_val = value_object_get(&response, "statusCode");
    if (status_code_val.type != VALUE_NUMBER) {
        value_free(&status_code_val);
        return value_create_boolean(false);
    }
    
    int status_code = (int)status_code_val.data.number_value;
    value_free(&status_code_val);
    
    // Check if status code is in 200-299 range (success)
    return value_create_boolean(status_code >= 200 && status_code < 300);
}

Value builtin_http_get_header(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_ARGUMENT_COUNT, "http", "unknown_function", "http.get_header() requires exactly 2 arguments (response, header_name)", line, column);
        return value_create_null();
    }
    
    Value response_val = args[0];
    Value header_name_val = args[1];
    
    if (response_val.type != VALUE_OBJECT) {
        std_error_report(ERROR_INVALID_ARGUMENT, "http", "unknown_function", "http.get_header() first argument must be a response object", line, column);
        return value_create_null();
    }
    
    if (header_name_val.type != VALUE_STRING) {
        std_error_report(ERROR_INVALID_ARGUMENT, "http", "unknown_function", "http.get_header() second argument must be a string (header name)", line, column);
        return value_create_null();
    }
    
    const char* header_name = header_name_val.data.string_value;
    
    // Check for common headers that are stored in the response object
    if (strcasecmp(header_name, "content-type") == 0) {
        Value content_type_val = value_object_get(&response_val, "content_type");
        if (content_type_val.type == VALUE_STRING) {
            Value result = value_clone(&content_type_val);
            value_free(&content_type_val);
            return result;
        }
        value_free(&content_type_val);
    }
    
    // For other headers, return null (headers array not currently stored in response object)
    return value_create_null();
}

Value builtin_http_get_json(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_ARGUMENT_COUNT, "http", "unknown_function", "http.get_json() requires exactly 1 argument (response)", line, column);
        return value_create_null();
    }
    
    Value response = args[0];
    if (response.type != VALUE_OBJECT) {
        std_error_report(ERROR_INVALID_ARGUMENT, "http", "unknown_function", "http.get_json() argument must be a response object", line, column);
        return value_create_null();
    }
    
    // Get the response body
    Value body_val = value_object_get(&response, "body");
    if (body_val.type != VALUE_STRING) {
        value_free(&body_val);
        return value_create_null();
    }
    
    // For now, return the body as a string (JSON parsing would require a JSON library)
    Value result = value_clone(&body_val);
    value_free(&body_val);
    return result;
}

// Additional HTTP methods
Value builtin_http_head(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "http", "unknown_function", "http.head() requires at least 1 argument (url)", line, column);
        return value_create_null();
    }
    
    Value url_value = args[0];
    if (url_value.type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "http", "unknown_function", "http.head() URL must be a string", line, column);
        return value_create_null();
    }
    
    HttpRequest* request = http_create_request(HTTP_HEAD, url_value.data.string_value);
    if (!request) return value_create_null();
    
    HttpResponse* response = http_request(request);
    http_free_request(request);
    
    if (!response) {
        return value_create_null();
    }
    
    // Create Myco response object
    Value response_obj = value_create_object(8);
    value_object_set(&response_obj, "type", value_create_string("Object"));
    value_object_set(&response_obj, "status_code", value_create_number(response->status_code));
    value_object_set(&response_obj, "status_text", value_create_string("OK")); // Simplified
    value_object_set(&response_obj, "body", value_create_string(response->body ? response->body : ""));
    value_object_set(&response_obj, "success", value_create_boolean(response->success));
    value_object_set(&response_obj, "content_type", value_create_string("text/plain")); // Simplified
    value_object_set(&response_obj, "content_length", value_create_number(response->body ? strlen(response->body) : 0));
    
    // No error message in simplified structure
    
    http_free_response(response);
    return response_obj;
}

Value builtin_http_patch(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2) {
        std_error_report(ERROR_INTERNAL_ERROR, "http", "unknown_function", "http.patch() requires at least 2 arguments (url, data)", line, column);
        return value_create_null();
    }
    
    Value url_value = args[0];
    Value data_value = args[1];
    
    if (url_value.type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "http", "unknown_function", "http.patch() URL must be a string", line, column);
        return value_create_null();
    }
    
    if (data_value.type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "http", "unknown_function", "http.patch() data must be a string", line, column);
        return value_create_null();
    }
    
    HttpRequest* request = http_create_request(HTTP_PATCH, url_value.data.string_value);
    if (!request) return value_create_null();
    
    http_set_body(request, data_value.data.string_value);
    
    HttpResponse* response = http_request(request);
    http_free_request(request);
    
    if (!response) {
        return value_create_null();
    }
    
    // Create Myco response object
    Value response_obj = value_create_object(8);
    value_object_set(&response_obj, "type", value_create_string("Object"));
    value_object_set(&response_obj, "status_code", value_create_number(response->status_code));
    value_object_set(&response_obj, "status_text", value_create_string("OK")); // Simplified
    value_object_set(&response_obj, "body", value_create_string(response->body ? response->body : ""));
    value_object_set(&response_obj, "success", value_create_boolean(response->success));
    value_object_set(&response_obj, "content_type", value_create_string("text/plain")); // Simplified
    value_object_set(&response_obj, "content_length", value_create_number(response->body ? strlen(response->body) : 0));
    
    // No error message in simplified structure
    
    http_free_response(response);
    return response_obj;
}

Value builtin_http_options(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "http", "unknown_function", "http.options() requires at least 1 argument (url)", line, column);
        return value_create_null();
    }
    
    Value url_value = args[0];
    if (url_value.type != VALUE_STRING) {
        std_error_report(ERROR_TYPE_MISMATCH, "http", "unknown_function", "http.options() URL must be a string", line, column);
        return value_create_null();
    }
    
    HttpRequest* request = http_create_request(HTTP_OPTIONS, url_value.data.string_value);
    if (!request) return value_create_null();
    
    HttpResponse* response = http_request(request);
    http_free_request(request);
    
    if (!response) {
        return value_create_null();
    }
    
    // Create Myco response object
    Value response_obj = value_create_object(8);
    value_object_set(&response_obj, "type", value_create_string("Object"));
    value_object_set(&response_obj, "status_code", value_create_number(response->status_code));
    value_object_set(&response_obj, "status_text", value_create_string("OK")); // Simplified
    value_object_set(&response_obj, "body", value_create_string(response->body ? response->body : ""));
    value_object_set(&response_obj, "success", value_create_boolean(response->success));
    value_object_set(&response_obj, "content_type", value_create_string("text/plain")); // Simplified
    value_object_set(&response_obj, "content_length", value_create_number(response->body ? strlen(response->body) : 0));
    
    // No error message in simplified structure
    
    http_free_response(response);
    return response_obj;
}

// Register HTTP library with interpreter
void http_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;

    // Create HTTP library object
    Value http_lib = value_create_object(16);

    // Register core functions
    value_object_set(&http_lib, "get", value_create_builtin_function(builtin_http_get));
    value_object_set(&http_lib, "post", value_create_builtin_function(builtin_http_post));
    value_object_set(&http_lib, "put", value_create_builtin_function(builtin_http_put));
    value_object_set(&http_lib, "delete", value_create_builtin_function(builtin_http_delete));
    value_object_set(&http_lib, "head", value_create_builtin_function(builtin_http_head));
    value_object_set(&http_lib, "patch", value_create_builtin_function(builtin_http_patch));
    value_object_set(&http_lib, "options", value_create_builtin_function(builtin_http_options));
    value_object_set(&http_lib, "request", value_create_builtin_function(builtin_http_request));
    value_object_set(&http_lib, "statusOk", value_create_builtin_function(builtin_http_status_ok));
    value_object_set(&http_lib, "getHeader", value_create_builtin_function(builtin_http_get_header));
    value_object_set(&http_lib, "getJson", value_create_builtin_function(builtin_http_get_json));

    // Mark as Library for .type reporting
    value_object_set(&http_lib, "__type__", value_create_string("Library"));
    value_object_set(&http_lib, "type", value_create_string("Library"));
    value_object_set(&http_lib, "__library_name__", value_create_string("http"));

    // Register the library in global environment
    environment_define(interpreter->global_environment, "http", http_lib);
}
