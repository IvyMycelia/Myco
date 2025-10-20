#include "libs/http.h"
#include "libs/builtin_libs.h"
#include "core/interpreter.h"
#include "core/ast.h"
#include "../../include/core/standardized_errors.h"
#include "../../include/utils/shared_utilities.h"

// Global CURL initialization flag
static bool curl_initialized = false;

// Initialize CURL if not already done
static bool http_init_curl() {
    if (!curl_initialized) {
        CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
        if (res != CURLE_OK) {
            return false;
        }
        curl_initialized = true;
    }
    return true;
}

// Callback function to write response data
static size_t http_write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    HttpResponse* response = (HttpResponse*)userp;
    
    char* ptr = shared_realloc_safe(response->body, response->content_length + realsize + 1, "libs", "unknown_function", 28);
    if (!ptr) {
        return 0;
    }
    
    response->body = ptr;
    memcpy(&(response->body[response->content_length]), contents, realsize);
    response->content_length += realsize;
    response->body[response->content_length] = 0;
    
    return realsize;
}

// Callback function to write headers
static size_t http_header_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    HttpResponse* response = (HttpResponse*)userp;
    
    char* header = shared_malloc_safe(realsize + 1, "libs", "unknown_function", 46);
    if (!header) return 0;
    
    memcpy(header, contents, realsize);
    header[realsize] = '\0';
    
    // Remove trailing newline
    if (header[realsize - 1] == '\n') {
        header[realsize - 1] = '\0';
    }
    if (header[realsize - 2] == '\r') {
        header[realsize - 2] = '\0';
    }
    
    // Reallocate headers array
    response->headers = shared_realloc_safe(response->headers, (response->header_count + 1) * sizeof(char*), "libs", "unknown_function", 61);
    response->headers[response->header_count] = header;
    response->header_count++;
    
    return realsize;
}

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
    
    char* header = shared_malloc_safe(strlen(name) + strlen(value) + 3, "libs", "unknown_function", 89);
    if (!header) return;
    
    sprintf(header, "%s: %s", name, value);
    
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

// Perform HTTP request
HttpResponse* http_request(HttpRequest* request) {
    if (!request || !http_init_curl()) {
        HttpResponse* error_response = shared_malloc_safe(sizeof(HttpResponse), "libs", "unknown_function", 119);
        error_response->status_code = 0;
        error_response->status_text = strdup("CURL initialization failed");
        error_response->body = NULL;
        error_response->headers = NULL;
        error_response->header_count = 0;
        error_response->content_type = NULL;
        error_response->content_length = 0;
        error_response->success = false;
        error_response->error_message = strdup("Failed to initialize CURL");
        return error_response;
    }
    
    CURL* curl = curl_easy_init();
    if (!curl) {
        HttpResponse* error_response = shared_malloc_safe(sizeof(HttpResponse), "libs", "unknown_function", 134);
        error_response->status_code = 0;
        error_response->status_text = strdup("CURL initialization failed");
        error_response->body = NULL;
        error_response->headers = NULL;
        error_response->header_count = 0;
        error_response->content_type = NULL;
        error_response->content_length = 0;
        error_response->success = false;
        error_response->error_message = strdup("Failed to initialize CURL handle");
        return error_response;
    }
    
    // Initialize response
    HttpResponse* response = shared_malloc_safe(sizeof(HttpResponse), "libs", "unknown_function", 148);
    response->body = shared_malloc_safe(1, "libs", "unknown_function", 149);
    response->body[0] = '\0';
    response->content_length = 0;
    response->headers = NULL;
    response->header_count = 0;
    response->content_type = NULL;
    response->success = false;
    response->error_message = NULL;
    
    // Set CURL options
    curl_easy_setopt(curl, CURLOPT_URL, request->url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, http_header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, request->timeout_seconds);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, request->follow_redirects ? 1L : 0L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, request->user_agent);
    
    // Set HTTP method
    switch (request->method) {
        case HTTP_GET:
            curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
            break;
        case HTTP_POST:
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            if (request->body) {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request->body);
            }
            break;
        case HTTP_PUT:
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
            if (request->body) {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request->body);
            }
            break;
        case HTTP_DELETE:
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
            break;
        case HTTP_PATCH:
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
            if (request->body) {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request->body);
            }
            break;
        case HTTP_HEAD:
            curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
            break;
        case HTTP_OPTIONS:
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "OPTIONS");
            break;
    }
    
    // Add custom headers
    if (request->headers && request->header_count > 0) {
        struct curl_slist* header_list = NULL;
        for (size_t i = 0; i < request->header_count; i++) {
            header_list = curl_slist_append(header_list, request->headers[i]);
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    }
    
    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    
    if (res == CURLE_OK) {
        long status_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
        response->status_code = (int)status_code;
        response->success = true;
        
        // Get content type
        char* content_type;
        curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
        if (content_type) {
            response->content_type = (content_type ? strdup(content_type) : NULL);
        }
        
        // Set status text based on status code
        if (status_code >= 200 && status_code < 300) {
            response->status_text = strdup("OK");
        } else if (status_code >= 300 && status_code < 400) {
            response->status_text = strdup("Redirect");
        } else if (status_code >= 400 && status_code < 500) {
            response->status_text = strdup("Client Error");
        } else if (status_code >= 500 && status_code < 600) {
            response->status_text = strdup("Server Error");
        } else {
            response->status_text = strdup("Unknown");
        }
    } else {
        response->status_code = 0;
        response->status_text = strdup("Error");
        response->error_message = curl_easy_strerror(res) ? strdup(curl_easy_strerror(res)) : NULL;
    }
    
    curl_easy_cleanup(curl);
    return response;
}

// Convenience functions
HttpResponse* http_get(const char* url, char** headers, size_t header_count) {
    HttpRequest* request = http_create_request(HTTP_GET, url);
    if (!request) return NULL;
    
    for (size_t i = 0; i < header_count; i++) {
        http_add_header(request, "Custom-Header", headers[i]);
    }
    
    HttpResponse* response = http_request(request);
    http_free_request(request);
    return response;
}

HttpResponse* http_post(const char* url, const char* data, char** headers, size_t header_count) {
    HttpRequest* request = http_create_request(HTTP_POST, url);
    if (!request) return NULL;
    
    http_set_body(request, data);
    
    for (size_t i = 0; i < header_count; i++) {
        http_add_header(request, "Custom-Header", headers[i]);
    }
    
    HttpResponse* response = http_request(request);
    http_free_request(request);
    return response;
}

HttpResponse* http_put(const char* url, const char* data, char** headers, size_t header_count) {
    HttpRequest* request = http_create_request(HTTP_PUT, url);
    if (!request) return NULL;
    
    http_set_body(request, data);
    
    for (size_t i = 0; i < header_count; i++) {
        http_add_header(request, "Custom-Header", headers[i]);
    }
    
    HttpResponse* response = http_request(request);
    http_free_request(request);
    return response;
}

HttpResponse* http_delete(const char* url, char** headers, size_t header_count) {
    HttpRequest* request = http_create_request(HTTP_DELETE, url);
    if (!request) return NULL;
    
    for (size_t i = 0; i < header_count; i++) {
        http_add_header(request, "Custom-Header", headers[i]);
    }
    
    HttpResponse* response = http_request(request);
    http_free_request(request);
    return response;
}

// Free response memory
void http_free_response(HttpResponse* response) {
    if (!response) return;
    
    if (response->body) shared_free_safe(response->body, "libs", "unknown_function", 310);
    if (response->status_text) shared_free_safe(response->status_text, "libs", "unknown_function", 311);
    if (response->content_type) shared_free_safe(response->content_type, "libs", "unknown_function", 312);
    if (response->error_message) shared_free_safe(response->error_message, "libs", "unknown_function", 313);
    
    if (response->headers) {
        for (size_t i = 0; i < response->header_count; i++) {
            shared_free_safe(response->headers[i], "libs", "unknown_function", 317);
        }
        shared_free_safe(response->headers, "libs", "unknown_function", 319);
    }
    
    shared_free_safe(response, "libs", "unknown_function", 322);
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
    if (!response || !name) return NULL;
    
    for (size_t i = 0; i < response->header_count; i++) {
        if (strncasecmp(response->headers[i], name, strlen(name)) == 0) {
            char* colon = strchr(response->headers[i], ':');
            if (colon) {
                return colon + 1; // Skip the colon and space
            }
        }
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
    
    // Check if content type is JSON
    if (response->content_type && strstr(response->content_type, "application/json")) {
        return (response->body ? strdup(response->body) : NULL);
    }
    
    return NULL;
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
    
    HttpResponse* response = http_get(url_value.data.string_value, NULL, 0);
    if (!response) {
        return value_create_null();
    }
    
    // Create Myco response object
    Value response_obj = value_create_object(8);
    value_object_set(&response_obj, "status_code", value_create_number(response->status_code));
    value_object_set(&response_obj, "status_text", value_create_string(response->status_text ? response->status_text : ""));
    value_object_set(&response_obj, "body", value_create_string(response->body ? response->body : ""));
    value_object_set(&response_obj, "success", value_create_boolean(response->success));
    value_object_set(&response_obj, "content_type", value_create_string(response->content_type ? response->content_type : ""));
    value_object_set(&response_obj, "content_length", value_create_number(response->content_length));
    
    if (response->error_message) {
        value_object_set(&response_obj, "error", value_create_string(response->error_message));
    }
    
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
    
    HttpResponse* response = http_post(url_value.data.string_value, data_value.data.string_value, NULL, 0);
    if (!response) {
        return value_create_null();
    }
    
    // Create Myco response object
    Value response_obj = value_create_object(8);
    value_object_set(&response_obj, "status_code", value_create_number(response->status_code));
    value_object_set(&response_obj, "status_text", value_create_string(response->status_text ? response->status_text : ""));
    value_object_set(&response_obj, "body", value_create_string(response->body ? response->body : ""));
    value_object_set(&response_obj, "success", value_create_boolean(response->success));
    value_object_set(&response_obj, "content_type", value_create_string(response->content_type ? response->content_type : ""));
    value_object_set(&response_obj, "content_length", value_create_number(response->content_length));
    
    if (response->error_message) {
        value_object_set(&response_obj, "error", value_create_string(response->error_message));
    }
    
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
    
    HttpResponse* response = http_put(url_value.data.string_value, data_value.data.string_value, NULL, 0);
    if (!response) {
        return value_create_null();
    }
    
    // Create Myco response object
    Value response_obj = value_create_object(8);
    value_object_set(&response_obj, "status_code", value_create_number(response->status_code));
    value_object_set(&response_obj, "status_text", value_create_string(response->status_text ? response->status_text : ""));
    value_object_set(&response_obj, "body", value_create_string(response->body ? response->body : ""));
    value_object_set(&response_obj, "success", value_create_boolean(response->success));
    value_object_set(&response_obj, "content_type", value_create_string(response->content_type ? response->content_type : ""));
    value_object_set(&response_obj, "content_length", value_create_number(response->content_length));
    
    if (response->error_message) {
        value_object_set(&response_obj, "error", value_create_string(response->error_message));
    }
    
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
    
    HttpResponse* response = http_delete(url_value.data.string_value, NULL, 0);
    if (!response) {
        return value_create_null();
    }
    
    // Create Myco response object
    Value response_obj = value_create_object(8);
    value_object_set(&response_obj, "status_code", value_create_number(response->status_code));
    value_object_set(&response_obj, "status_text", value_create_string(response->status_text ? response->status_text : ""));
    value_object_set(&response_obj, "body", value_create_string(response->body ? response->body : ""));
    value_object_set(&response_obj, "success", value_create_boolean(response->success));
    value_object_set(&response_obj, "content_type", value_create_string(response->content_type ? response->content_type : ""));
    value_object_set(&response_obj, "content_length", value_create_number(response->content_length));
    
    if (response->error_message) {
        value_object_set(&response_obj, "error", value_create_string(response->error_message));
    }
    
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
    value_object_set(&response_obj, "status_code", value_create_number(response->status_code));
    value_object_set(&response_obj, "status_text", value_create_string(response->status_text ? response->status_text : ""));
    value_object_set(&response_obj, "body", value_create_string(response->body ? response->body : ""));
    value_object_set(&response_obj, "success", value_create_boolean(response->success));
    value_object_set(&response_obj, "content_type", value_create_string(response->content_type ? response->content_type : ""));
    value_object_set(&response_obj, "content_length", value_create_number(response->content_length));
    
    if (response->error_message) {
        value_object_set(&response_obj, "error", value_create_string(response->error_message));
    }
    
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
    value_object_set(&response_obj, "status_code", value_create_number(response->status_code));
    value_object_set(&response_obj, "status_text", value_create_string(response->status_text ? response->status_text : ""));
    value_object_set(&response_obj, "body", value_create_string(response->body ? response->body : ""));
    value_object_set(&response_obj, "success", value_create_boolean(response->success));
    value_object_set(&response_obj, "content_type", value_create_string(response->content_type ? response->content_type : ""));
    value_object_set(&response_obj, "content_length", value_create_number(response->content_length));
    
    if (response->error_message) {
        value_object_set(&response_obj, "error", value_create_string(response->error_message));
    }
    
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
    value_object_set(&response_obj, "status_code", value_create_number(response->status_code));
    value_object_set(&response_obj, "status_text", value_create_string(response->status_text ? response->status_text : ""));
    value_object_set(&response_obj, "body", value_create_string(response->body ? response->body : ""));
    value_object_set(&response_obj, "success", value_create_boolean(response->success));
    value_object_set(&response_obj, "content_type", value_create_string(response->content_type ? response->content_type : ""));
    value_object_set(&response_obj, "content_length", value_create_number(response->content_length));
    
    if (response->error_message) {
        value_object_set(&response_obj, "error", value_create_string(response->error_message));
    }
    
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

    // Register the library in global environment
    environment_define(interpreter->global_environment, "http", http_lib);
}
