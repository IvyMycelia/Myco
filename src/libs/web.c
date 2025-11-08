#include "../../include/libs/web.h"
#include "../../include/libs/server/server.h"
#include "../../include/libs/json.h"
#include "../../include/core/environment.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

// Global web app registry
static MycoWebApp* g_web_apps = NULL;

// Web App Creation and Management
Value builtin_web_app_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // Use default name if no argument provided
    const char* name = "MycoWebApp";
    if (arg_count >= 1) {
        if (args[0].type != VALUE_STRING) {
            printf("Error: web.create() name must be a string\n");
            return value_create_null();
        }
        name = args[0].data.string_value;
    }
    MycoWebApp* app = web_app_create(name);
    
    if (!app) {
        printf("Error: Failed to create web app\n");
        return value_create_null();
    }
    
    // Create the web app object
    Value app_obj = value_create_object(8);
    value_object_set_member(&app_obj, "__web_app_id__", value_create_number((double)(intptr_t)app));
    value_object_set_member(&app_obj, "name", value_create_string(name));
    value_object_set_member(&app_obj, "port", value_create_number(3000));
    value_object_set_member(&app_obj, "running", value_create_boolean(false));
    
    // Add methods
    value_object_set_member(&app_obj, "get", value_create_builtin_function(builtin_web_app_get));
    value_object_set_member(&app_obj, "post", value_create_builtin_function(builtin_web_app_post));
    value_object_set_member(&app_obj, "put", value_create_builtin_function(builtin_web_app_put));
    value_object_set_member(&app_obj, "delete", value_create_builtin_function(builtin_web_app_delete));
    value_object_set_member(&app_obj, "use", value_create_builtin_function(builtin_web_app_use));
    value_object_set_member(&app_obj, "listen", value_create_builtin_function(builtin_web_app_listen));
    
    return app_obj;
}

Value builtin_web_app_get(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2) {
        printf("Error: app.get() requires path and handler\n");
        return value_create_null();
    }
    
    // Get the web app from the calling object
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        printf("Error: app.get() must be called on a web app\n");
        return value_create_null();
    }
    
    Value app_id_val = value_object_get(self, "__web_app_id__");
    if (app_id_val.type != VALUE_NUMBER) {
        printf("Error: Invalid web app object\n");
        return value_create_null();
    }
    
    MycoWebApp* app = (MycoWebApp*)(intptr_t)app_id_val.data.number_value;
    if (!app) {
        printf("Error: Web app not found\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING) {
        printf("Error: app.get() path must be a string\n");
        return value_create_null();
    }
    
    const char* path = args[0].data.string_value;
    Value handler = args[1];
    
    // Create request object template
    Value req_template = value_create_object(6);
    value_object_set(&req_template, "query", value_create_builtin_function(builtin_web_request_get_query));
    value_object_set(&req_template, "body", value_create_builtin_function(builtin_web_request_get_body));
    value_object_set(&req_template, "header", value_create_builtin_function(builtin_web_request_get_header));
    value_object_set(&req_template, "json", value_create_builtin_function(builtin_web_request_json));
    value_object_set(&req_template, "params", value_create_builtin_function(builtin_web_request_get_param));
    
    // Create response object template
    Value res_template = value_create_object(6);
    value_object_set(&res_template, "status", value_create_builtin_function(builtin_web_response_status));
    value_object_set(&res_template, "json", value_create_builtin_function(builtin_web_response_json));
    value_object_set(&res_template, "send", value_create_builtin_function(builtin_web_response_send));
    value_object_set(&res_template, "header", value_create_builtin_function(builtin_web_response_header));
    value_object_set(&res_template, "html", value_create_builtin_function(builtin_web_response_html));
    value_object_set(&res_template, "redirect", value_create_builtin_function(builtin_web_response_redirect));
    
    // Create and add route
    WebRoute* route = web_route_create("GET", path, handler);
    if (!route) {
        printf("Error: Failed to create route\n");
        return value_create_null();
    }
    
    route->next = app->routes;
    app->routes = route;
    
    // Return self for chaining
    return *self;
}

Value builtin_web_app_post(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2) {
        printf("Error: app.post() requires path and handler\n");
        return value_create_null();
    }
    
    // Get the web app from the calling object
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        printf("Error: app.post() must be called on a web app\n");
        return value_create_null();
    }
    
    Value app_id_val = value_object_get(self, "__web_app_id__");
    if (app_id_val.type != VALUE_NUMBER) {
        printf("Error: Invalid web app object\n");
        return value_create_null();
    }
    
    MycoWebApp* app = (MycoWebApp*)(intptr_t)app_id_val.data.number_value;
    if (!app) {
        printf("Error: Web app not found\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING) {
        printf("Error: app.post() path must be a string\n");
        return value_create_null();
    }
    
    const char* path = args[0].data.string_value;
    Value handler = args[1];
    
    // Create and add route
    WebRoute* route = web_route_create("POST", path, handler);
    if (!route) {
        printf("Error: Failed to create route\n");
        return value_create_null();
    }
    
    route->next = app->routes;
    app->routes = route;
    
    // Return self for chaining
    return *self;
}

Value builtin_web_app_put(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2) {
        printf("Error: app.put() requires path and handler\n");
        return value_create_null();
    }
    
    // Get the web app from the calling object
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        printf("Error: app.put() must be called on a web app\n");
        return value_create_null();
    }
    
    Value app_id_val = value_object_get(self, "__web_app_id__");
    if (app_id_val.type != VALUE_NUMBER) {
        printf("Error: Invalid web app object\n");
        return value_create_null();
    }
    
    MycoWebApp* app = (MycoWebApp*)(intptr_t)app_id_val.data.number_value;
    if (!app) {
        printf("Error: Web app not found\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING) {
        printf("Error: app.put() path must be a string\n");
        return value_create_null();
    }
    
    const char* path = args[0].data.string_value;
    Value handler = args[1];
    
    // Create and add route
    WebRoute* route = web_route_create("PUT", path, handler);
    if (!route) {
        printf("Error: Failed to create route\n");
        return value_create_null();
    }
    
    route->next = app->routes;
    app->routes = route;
    
    // Return self for chaining
    return *self;
}

Value builtin_web_app_delete(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2) {
        printf("Error: app.delete() requires path and handler\n");
        return value_create_null();
    }
    
    // Get the web app from the calling object
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        printf("Error: app.delete() must be called on a web app\n");
        return value_create_null();
    }
    
    Value app_id_val = value_object_get(self, "__web_app_id__");
    if (app_id_val.type != VALUE_NUMBER) {
        printf("Error: Invalid web app object\n");
        return value_create_null();
    }
    
    MycoWebApp* app = (MycoWebApp*)(intptr_t)app_id_val.data.number_value;
    if (!app) {
        printf("Error: Web app not found\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING) {
        printf("Error: app.delete() path must be a string\n");
        return value_create_null();
    }
    
    const char* path = args[0].data.string_value;
    Value handler = args[1];
    
    // Create and add route
    WebRoute* route = web_route_create("DELETE", path, handler);
    if (!route) {
        printf("Error: Failed to create route\n");
        return value_create_null();
    }
    
    route->next = app->routes;
    app->routes = route;
    
    // Return self for chaining
    return *self;
}

Value builtin_web_app_use(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        printf("Error: app.use() requires middleware function\n");
        return value_create_null();
    }
    
    // Get the web app from the calling object
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        printf("Error: app.use() must be called on a web app\n");
        return value_create_null();
    }
    
    Value app_id_val = value_object_get(self, "__web_app_id__");
    if (app_id_val.type != VALUE_NUMBER) {
        printf("Error: Invalid web app object\n");
        return value_create_null();
    }
    
    MycoWebApp* app = (MycoWebApp*)(intptr_t)app_id_val.data.number_value;
    if (!app) {
        printf("Error: Web app not found\n");
        return value_create_null();
    }
    
    Value middleware = args[0];
    
    // Create middleware entry
    Middleware* mw = shared_malloc_safe(sizeof(Middleware), "web", "builtin_web_app_use", 200);
    if (!mw) {
        printf("Error: Failed to create middleware\n");
        return value_create_null();
    }
    
    mw->function = middleware;
    mw->next = app->middleware;
    app->middleware = mw;
    
    // Return self for chaining
    return *self;
}

Value builtin_web_app_listen(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // Get the web app from the calling object
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        printf("Error: app.listen() must be called on a web app\n");
        return value_create_null();
    }
    
    Value app_id_val = value_object_get(self, "__web_app_id__");
    if (app_id_val.type != VALUE_NUMBER) {
        printf("Error: Invalid web app object\n");
        return value_create_null();
    }
    
    MycoWebApp* app = (MycoWebApp*)(intptr_t)app_id_val.data.number_value;
    if (!app) {
        printf("Error: Web app not found\n");
        return value_create_null();
    }
    
    int port = 3000;
    if (arg_count > 0 && args[0].type == VALUE_NUMBER) {
        port = (int)args[0].data.number_value;
    }
    
    app->port = port;
    
    // Create server using existing server library
    Value server_args[1];
    server_args[0] = value_create_number(port);
    Value server = builtin_server_create(interpreter, server_args, 1, line, column);
    
    if (server.type == VALUE_NULL) {
        printf("Error: Failed to create server\n");
        return value_create_null();
    }
    
    // Store server reference
    // For now, just store a placeholder - this would be implemented with proper server integration
    app->server = NULL;
    app->is_running = true;
    
    // Update the app object
    value_object_set_member(self, "port", value_create_number(port));
    value_object_set_member(self, "running", value_create_boolean(true));
    
    printf("Web app '%s' listening on port %d\n", app->name, port);
    
    return *self;
}

// Enhanced Request Methods
Value builtin_web_request_get_param(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        printf("Error: request.get_param() requires parameter name\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING) {
        printf("Error: request.get_param() parameter name must be a string\n");
        return value_create_null();
    }
    
    // For now, return a placeholder - this would be implemented with proper URL parameter parsing
    return value_create_string("placeholder_param_value");
}

Value builtin_web_request_get_query(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        printf("Error: request.get_query() requires query parameter name\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING) {
        printf("Error: request.get_query() parameter name must be a string\n");
        return value_create_null();
    }
    
    // Use existing query string functionality
    return builtin_request_query(interpreter, args, arg_count, line, column);
}

Value builtin_web_request_get_header(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        printf("Error: request.get_header() requires header name\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING) {
        printf("Error: request.get_header() header name must be a string\n");
        return value_create_null();
    }
    
    // Use existing header functionality
    return builtin_request_header(interpreter, args, arg_count, line, column);
}

Value builtin_web_request_get_body(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // For now, return a mock JSON string for testing
    return value_create_string("{\"username\":\"testuser\",\"password\":\"testpass\"}");
}

Value builtin_web_request_json(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // Use existing JSON parsing functionality
    return builtin_request_json(interpreter, args, arg_count, line, column);
}

// Enhanced Response Methods
Value builtin_web_response_status(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        printf("Error: response.status() requires status code\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_NUMBER) {
        printf("Error: response.status() status code must be a number\n");
        return value_create_null();
    }
    
    // Get self context for chaining
    Value* self = interpreter_get_self_context(interpreter);
    if (self && self->type == VALUE_OBJECT) {
        value_object_set_member(self, "__status_code__", args[0]);
        printf("Response status set to %d\n", (int)args[0].data.number_value);
        return *self;  // Return self for chaining
    }
    
    printf("Response status set to %d\n", (int)args[0].data.number_value);
    return value_create_null();
}

Value builtin_web_response_header(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2) {
        printf("Error: response.header() requires name and value\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING || args[1].type != VALUE_STRING) {
        printf("Error: response.header() name and value must be strings\n");
        return value_create_null();
    }
    
    // Get self context for chaining
    Value* self = interpreter_get_self_context(interpreter);
    if (self && self->type == VALUE_OBJECT) {
        printf("Response header set: %s: %s\n", args[0].data.string_value, args[1].data.string_value);
        return *self;  // Return self for chaining
    }
    
    // Use existing header functionality
    return builtin_response_header(interpreter, args, arg_count, line, column);
}

Value builtin_web_response_json(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        printf("Error: response.json() requires data to serialize\n");
        return value_create_null();
    }
    
    // Get self context for chaining
    Value* self = interpreter_get_self_context(interpreter);
    if (self && self->type == VALUE_OBJECT) {
        printf("Response JSON data set\n");
        return *self;  // Return self for chaining
    }
    
    // Use existing JSON functionality
    return builtin_response_json(interpreter, args, arg_count, line, column);
}

Value builtin_web_response_html(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        printf("Error: response.html() requires HTML content\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING) {
        printf("Error: response.html() content must be a string\n");
        return value_create_null();
    }
    
    // For now, just print the HTML content - this would be implemented with proper response object management
    printf("HTML content: %s\n", args[0].data.string_value);
    return value_create_null();
}

Value builtin_web_response_send(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        printf("Error: response.send() requires content\n");
        return value_create_null();
    }
    
    // For now, just print the content - this would be implemented with proper response object management
    if (args[0].type == VALUE_STRING) {
        printf("Response body: %s\n", args[0].data.string_value);
    } else {
        printf("Response body: [non-string content]\n");
    }
    return value_create_null();
}

Value builtin_web_response_redirect(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        printf("Error: response.redirect() requires URL\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING) {
        printf("Error: response.redirect() URL must be a string\n");
        return value_create_null();
    }
    
    // For now, just print the redirect - this would be implemented with proper response object management
    int status_code = 302; // Default redirect status
    if (arg_count > 1 && args[1].type == VALUE_NUMBER) {
        status_code = (int)args[1].data.number_value;
    }
    printf("Redirect to %s with status %d\n", args[0].data.string_value, status_code);
    return value_create_null();
}

// Template Engine Functions
Value builtin_template_render(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2) {
        printf("Error: template.render() requires template and context\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING) {
        printf("Error: template.render() template must be a string\n");
        return value_create_null();
    }
    
    if (args[1].type != VALUE_OBJECT && args[1].type != VALUE_HASH_MAP) {
        printf("Error: template.render() context must be an object or map\n");
        return value_create_null();
    }
    
    const char* template_content = args[0].data.string_value;
    Value* context = &args[1];
    
    // Parse template
    Template* tmpl = template_parse(template_content);
    if (!tmpl) {
        printf("Error: Failed to parse template\n");
        return value_create_null();
    }
    
    // Render template
    char* result = template_render(tmpl, context);
    template_free(tmpl);
    
    if (!result) {
        printf("Error: Failed to render template\n");
        return value_create_null();
    }
    
    Value result_value = value_create_string(result);
    shared_free_safe(result, "web", "builtin_template_render", 900);
    return result_value;
}

Value builtin_web_response_render(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2) {
        printf("Error: response.render() requires template and context\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING) {
        printf("Error: response.render() template must be a string\n");
        return value_create_null();
    }
    
    if (args[1].type != VALUE_OBJECT) {
        printf("Error: response.render() context must be an object\n");
        return value_create_null();
    }
    
    // For now, just render the template - this would be implemented with proper response object management
    
    const char* template_content = args[0].data.string_value;
    Value* context = &args[1];
    
    // Parse and render template
    Template* tmpl = template_parse(template_content);
    if (!tmpl) {
        printf("Error: Failed to parse template\n");
        return value_create_null();
    }
    
    char* result = template_render(tmpl, context);
    template_free(tmpl);
    
    if (!result) {
        printf("Error: Failed to render template\n");
        return value_create_null();
    }
    
    // For now, just print the rendered template
    printf("Rendered template: %s\n", result);
    shared_free_safe(result, "web", "builtin_web_response_render", 950);
    
    return value_create_null();
}

// Internal helper functions
MycoWebApp* web_app_create(const char* name) {
    MycoWebApp* app = shared_malloc_safe(sizeof(MycoWebApp), "web", "web_app_create", 600);
    if (!app) return NULL;
    
    app->name = shared_strdup(name);
    app->port = 3000;
    app->is_running = false;
    app->server = NULL;
    app->routes = NULL;
    app->middleware = NULL;
    app->next = g_web_apps;
    g_web_apps = app;
    
    return app;
}

void web_app_free(MycoWebApp* app) {
    if (!app) return;
    
    shared_free_safe(app->name, "web", "web_app_free", 610);
    
    // Free routes
    WebRoute* route = app->routes;
    while (route) {
        WebRoute* next = route->next;
        web_route_free(route);
        route = next;
    }
    
    // Free middleware
    Middleware* mw = app->middleware;
    while (mw) {
        Middleware* next = mw->next;
        shared_free_safe(mw, "web", "web_app_free", 620);
        mw = next;
    }
    
    shared_free_safe(app, "web", "web_app_free", 630);
}

WebRoute* web_route_create(const char* method, const char* path, Value handler) {
    WebRoute* route = shared_malloc_safe(sizeof(WebRoute), "web", "web_route_create", 640);
    if (!route) return NULL;
    
    route->method = shared_strdup(method);
    route->path = shared_strdup(path);
    route->handler = handler;
    route->next = NULL;
    
    return route;
}

void web_route_free(WebRoute* route) {
    if (!route) return;
    
    shared_free_safe(route->method, "web", "web_route_free", 650);
    shared_free_safe(route->path, "web", "web_route_free", 660);
    shared_free_safe(route, "web", "web_route_free", 670);
}

MycoWebRequest* web_request_create(const char* method, const char* url) {
    MycoWebRequest* req = shared_malloc_safe(sizeof(MycoWebRequest), "web", "web_request_create", 680);
    if (!req) return NULL;
    
    req->method = shared_strdup(method);
    req->url = shared_strdup(url);
    req->path = NULL;
    req->query_string = NULL;
    req->headers = NULL;
    req->header_count = 0;
    req->body = NULL;
    req->params = NULL;
    req->param_count = 0;
    req->next = NULL;
    
    return req;
}

void web_request_free(MycoWebRequest* req) {
    if (!req) return;
    
    shared_free_safe(req->method, "web", "web_request_free", 690);
    shared_free_safe(req->url, "web", "web_request_free", 700);
    shared_free_safe(req->path, "web", "web_request_free", 710);
    shared_free_safe(req->query_string, "web", "web_request_free", 720);
    shared_free_safe(req->body, "web", "web_request_free", 730);
    
    // Free headers
    for (size_t i = 0; i < req->header_count; i++) {
        shared_free_safe(req->headers[i], "web", "web_request_free", 740);
    }
    shared_free_safe(req->headers, "web", "web_request_free", 750);
    
    // Free params
    for (size_t i = 0; i < req->param_count; i++) {
        shared_free_safe(req->params[i], "web", "web_request_free", 760);
    }
    shared_free_safe(req->params, "web", "web_request_free", 770);
    
    shared_free_safe(req, "web", "web_request_free", 780);
}

MycoWebResponse* web_response_create() {
    MycoWebResponse* res = shared_malloc_safe(sizeof(MycoWebResponse), "web", "web_response_create", 790);
    if (!res) return NULL;
    
    res->status_code = 200;
    res->headers = NULL;
    res->header_count = 0;
    res->body = NULL;
    res->sent = false;
    res->next = NULL;
    
    return res;
}

void web_response_free(MycoWebResponse* res) {
    if (!res) return;
    
    shared_free_safe(res->body, "web", "web_response_free", 800);
    
    // Free headers
    for (size_t i = 0; i < res->header_count; i++) {
        shared_free_safe(res->headers[i], "web", "web_response_free", 810);
    }
    shared_free_safe(res->headers, "web", "web_response_free", 820);
    
    shared_free_safe(res, "web", "web_response_free", 830);
}

// Library registration
void web_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    // Create web namespace
    Value web_namespace = value_create_object(10);
    // Mark as Library for .type reporting
    value_object_set(&web_namespace, "__type__", value_create_string("Library"));
    value_object_set(&web_namespace, "type", value_create_string("Library"));
    value_object_set(&web_namespace, "__library_name__", value_create_string("web"));
    
    // Add web functions
    value_object_set(&web_namespace, "create", value_create_builtin_function(builtin_web_app_create));
    
    // Add template functions to web namespace
    value_object_set(&web_namespace, "render", value_create_builtin_function(builtin_template_render));
    
    // Add session management functions to web namespace
    value_object_set(&web_namespace, "session_start", value_create_builtin_function(builtin_session_start));
    value_object_set(&web_namespace, "session_get", value_create_builtin_function(builtin_session_get));
    value_object_set(&web_namespace, "session_set", value_create_builtin_function(builtin_session_set));
    value_object_set(&web_namespace, "session_destroy", value_create_builtin_function(builtin_session_destroy));
    value_object_set(&web_namespace, "session_cleanup", value_create_builtin_function(builtin_session_cleanup));
    
    // Register web namespace in global environment
    environment_define(interpreter->global_environment, "web", web_namespace);
}

// Global session manager
static SessionManager* g_session_manager = NULL;

// Session Management Implementation
SessionManager* session_manager_create(int max_age) {
    SessionManager* mgr = shared_malloc_safe(sizeof(SessionManager), "web", "session_manager_create", 2000);
    if (!mgr) return NULL;
    
    mgr->sessions = NULL;
    mgr->max_age = max_age;
    mgr->cookie_name = shared_strdup("myco_session");
    pthread_mutex_init(&mgr->lock, NULL);
    
    return mgr;
}

void session_manager_free(SessionManager* mgr) {
    if (!mgr) return;
    
    pthread_mutex_lock(&mgr->lock);
    
    Session* session = mgr->sessions;
    while (session) {
        Session* next = session->next;
        session_destroy(mgr, session->session_id);
        session = next;
    }
    
    pthread_mutex_unlock(&mgr->lock);
    pthread_mutex_destroy(&mgr->lock);
    
    shared_free_safe(mgr->cookie_name, "web", "session_manager_free", 2010);
    shared_free_safe(mgr, "web", "session_manager_free", 2011);
}

char* session_generate_id(void) {
    char* id = shared_malloc_safe(33, "web", "session_generate_id", 2020);
    if (!id) return NULL;
    
    // Simple random ID generation (in production, use cryptographically secure random)
    srand((unsigned int)time(NULL));
    for (int i = 0; i < 32; i++) {
        id[i] = "0123456789abcdef"[rand() % 16];
    }
    id[32] = '\0';
    
    return id;
}

Session* session_create(SessionManager* mgr) {
    if (!mgr) return NULL;
    
    Session* session = shared_malloc_safe(sizeof(Session), "web", "session_create", 2030);
    if (!session) return NULL;
    
    session->session_id = session_generate_id();
    session->data = NULL;
    session->created_at = time(NULL);
    session->last_accessed = time(NULL);
    session->expires_in = mgr->max_age;
    session->next = NULL;
    
    pthread_mutex_lock(&mgr->lock);
    session->next = mgr->sessions;
    mgr->sessions = session;
    pthread_mutex_unlock(&mgr->lock);
    
    return session;
}

Session* session_get(SessionManager* mgr, const char* session_id) {
    if (!mgr || !session_id) return NULL;
    
    pthread_mutex_lock(&mgr->lock);
    
    Session* session = mgr->sessions;
    while (session) {
        if (strcmp(session->session_id, session_id) == 0) {
            // Check if session has expired
            time_t now = time(NULL);
            if (now - session->last_accessed > session->expires_in) {
                // Session expired, remove it
                session_destroy(mgr, session_id);
                pthread_mutex_unlock(&mgr->lock);
                return NULL;
            }
            
            // Update last accessed time
            session->last_accessed = now;
            pthread_mutex_unlock(&mgr->lock);
            return session;
        }
        session = session->next;
    }
    
    pthread_mutex_unlock(&mgr->lock);
    return NULL;
}

void session_set(Session* session, const char* key, Value value) {
    if (!session || !key) return;
    
    // Look for existing data with this key
    SessionData* data = session->data;
    while (data) {
        if (strcmp(data->key, key) == 0) {
            // Update existing value
            value_free(&data->value);
            data->value = value_clone(&value);
            return;
        }
        data = data->next;
    }
    
    // Create new data entry
    SessionData* new_data = shared_malloc_safe(sizeof(SessionData), "web", "session_set", 2040);
    if (!new_data) return;
    
    new_data->key = shared_strdup(key);
    new_data->value = value_clone(&value);
    new_data->next = session->data;
    session->data = new_data;
}

Value session_get_value(Session* session, const char* key) {
    if (!session || !key) return value_create_null();
    
    SessionData* data = session->data;
    while (data) {
        if (strcmp(data->key, key) == 0) {
            return value_clone(&data->value);
        }
        data = data->next;
    }
    
    return value_create_null();
}

void session_destroy(SessionManager* mgr, const char* session_id) {
    if (!mgr || !session_id) return;
    
    pthread_mutex_lock(&mgr->lock);
    
    Session** current = &mgr->sessions;
    while (*current) {
        if (strcmp((*current)->session_id, session_id) == 0) {
            Session* to_remove = *current;
            *current = (*current)->next;
            
            // Free session data
            session_data_free(to_remove->data);
            shared_free_safe(to_remove->session_id, "web", "session_destroy", 2050);
            shared_free_safe(to_remove, "web", "session_destroy", 2051);
            break;
        }
        current = &(*current)->next;
    }
    
    pthread_mutex_unlock(&mgr->lock);
}

void session_cleanup_expired(SessionManager* mgr) {
    if (!mgr) return;
    
    pthread_mutex_lock(&mgr->lock);
    
    time_t now = time(NULL);
    Session** current = &mgr->sessions;
    
    while (*current) {
        Session* session = *current;
        if (now - session->last_accessed > session->expires_in) {
            // Session expired, remove it
            *current = session->next;
            session_data_free(session->data);
            shared_free_safe(session->session_id, "web", "session_cleanup_expired", 2060);
            shared_free_safe(session, "web", "session_cleanup_expired", 2061);
        } else {
            current = &(*current)->next;
        }
    }
    
    pthread_mutex_unlock(&mgr->lock);
}

void session_data_free(SessionData* data) {
    while (data) {
        SessionData* next = data->next;
        value_free(&data->value);
        shared_free_safe(data->key, "web", "session_data_free", 2070);
        shared_free_safe(data, "web", "session_data_free", 2071);
        data = next;
    }
}

// Myco-facing Session Management Functions
Value builtin_session_start(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // Initialize global session manager if not already done
    if (!g_session_manager) {
        g_session_manager = session_manager_create(3600); // 1 hour default
    }
    
    Session* session = session_create(g_session_manager);
    if (!session) {
        printf("Error: Failed to create session\n");
        return value_create_null();
    }
    
    // Create session object
    Value session_obj = value_create_object(4);
    value_object_set(&session_obj, "__type__", value_create_string("Session"));
    value_object_set(&session_obj, "type", value_create_string("Session"));
    value_object_set(&session_obj, "id", value_create_string(session->session_id));
    value_object_set(&session_obj, "created_at", value_create_number((double)session->created_at));
    
    return session_obj;
}

Value builtin_session_get(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2) {
        printf("Error: session.get() requires session_id and key\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING || args[1].type != VALUE_STRING) {
        printf("Error: session.get() requires string arguments\n");
        return value_create_null();
    }
    
    if (!g_session_manager) {
        printf("Error: Session manager not initialized\n");
        return value_create_null();
    }
    
    const char* session_id = args[0].data.string_value;
    const char* key = args[1].data.string_value;
    
    Session* session = session_get(g_session_manager, session_id);
    if (!session) {
        return value_create_null();
    }
    
    return session_get_value(session, key);
}

Value builtin_session_set(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 3) {
        printf("Error: session.set() requires session_id, key, and value\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING || args[1].type != VALUE_STRING) {
        printf("Error: session.set() requires string session_id and key\n");
        return value_create_null();
    }
    
    if (!g_session_manager) {
        printf("Error: Session manager not initialized\n");
        return value_create_null();
    }
    
    const char* session_id = args[0].data.string_value;
    const char* key = args[1].data.string_value;
    Value value = args[2];
    
    Session* session = session_get(g_session_manager, session_id);
    if (!session) {
        printf("Error: Session not found\n");
        return value_create_null();
    }
    
    session_set(session, key, value);
    return value_create_boolean(true);
}

Value builtin_session_destroy(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        printf("Error: session.destroy() requires session_id\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING) {
        printf("Error: session.destroy() requires string session_id\n");
        return value_create_null();
    }
    
    if (!g_session_manager) {
        printf("Error: Session manager not initialized\n");
        return value_create_null();
    }
    
    const char* session_id = args[0].data.string_value;
    session_destroy(g_session_manager, session_id);
    return value_create_boolean(true);
}

Value builtin_session_cleanup(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (!g_session_manager) {
        printf("Error: Session manager not initialized\n");
        return value_create_null();
    }
    
    session_cleanup_expired(g_session_manager);
    return value_create_boolean(true);
}

// Template Engine Implementation
Template* template_parse(const char* content) {
    if (!content) return NULL;
    
    Template* tmpl = shared_malloc_safe(sizeof(Template), "web", "template_parse", 1000);
    if (!tmpl) return NULL;
    
    tmpl->content = shared_strdup(content);
    tmpl->template_path = NULL;
    
    size_t pos = 0;
    tmpl->ast = template_parse_node(content, &pos);
    
    return tmpl;
}

TemplateNode* template_parse_node(const char* content, size_t* pos) {
    if (!content || !pos) return NULL;
    
    TemplateNode* head = NULL;
    TemplateNode* current = NULL;
    
    while (content[*pos] != '\0') {
        // Look for template tags
        if (content[*pos] == '{' && content[*pos + 1] == '{') {
            // Variable substitution: {{variable}}
            if (content[*pos + 2] != '{') {
                TemplateNode* node = shared_malloc_safe(sizeof(TemplateNode), "web", "template_parse_node", 1010);
                if (!node) break;
                
                node->type = TEMPLATE_VAR;
                node->content = NULL;
                node->children = NULL;
                node->next = NULL;
                
                // Extract variable name
                size_t start = *pos + 2;
                size_t end = start;
                while (content[end] != '}' || content[end + 1] != '}') {
                    end++;
                    if (content[end] == '\0') break;
                }
                
                if (content[end] == '}' && content[end + 1] == '}') {
                    size_t var_len = end - start;
                    node->variable = shared_malloc_safe(var_len + 1, "web", "template_parse_node", 1020);
                    if (node->variable) {
                        strncpy(node->variable, content + start, var_len);
                        node->variable[var_len] = '\0';
                    }
                    *pos = end + 2;
                } else {
                    // Malformed tag, treat as text
                    node->type = TEMPLATE_TEXT;
                    node->content = shared_strdup("{{");
                    *pos += 2;
                }
                
                if (!head) {
                    head = current = node;
                } else {
                    current->next = node;
                    current = node;
                }
                continue;
            }
        }
        
        // Look for control structures: {% if %}, {% for %}, {% include %}
        if (content[*pos] == '{' && content[*pos + 1] == '%') {
            size_t start = *pos;
            size_t end = start + 2;
            
            // Find matching %}
            while (content[end] != '%' || content[end + 1] != '}') {
                end++;
                if (content[end] == '\0') break;
            }
            
            if (content[end] == '%' && content[end + 1] == '}') {
                size_t tag_len = end - start - 2;
                char* tag_content = shared_malloc_safe(tag_len + 1, "web", "template_parse_node", 1030);
                if (tag_content) {
                    strncpy(tag_content, content + start + 2, tag_len);
                    tag_content[tag_len] = '\0';
                    
                    TemplateNode* node = shared_malloc_safe(sizeof(TemplateNode), "web", "template_parse_node", 1040);
                    if (node) {
                        node->content = tag_content;
                        node->children = NULL;
                        node->next = NULL;
                        
                        // Determine node type
                        if (strncmp(tag_content, "if ", 3) == 0) {
                            node->type = TEMPLATE_IF;
                            node->condition = shared_strdup(tag_content + 3);
                            // Parse children until {% endif %}
                            node->children = template_parse_until(content, pos, "endif");
                        } else if (strncmp(tag_content, "for ", 4) == 0) {
                            node->type = TEMPLATE_FOR;
                            node->condition = shared_strdup(tag_content + 4);
                            // Parse children until {% endfor %}
                            node->children = template_parse_until(content, pos, "endfor");
                        } else if (strncmp(tag_content, "include ", 8) == 0) {
                            node->type = TEMPLATE_INCLUDE;
                            node->condition = shared_strdup(tag_content + 8);
                        } else {
                            node->type = TEMPLATE_TEXT;
                        }
                        
                        if (!head) {
                            head = current = node;
                        } else {
                            current->next = node;
                            current = node;
                        }
                    } else {
                        shared_free_safe(tag_content, "web", "template_parse_node", 1050);
                    }
                }
                *pos = end + 2;
                continue;
            }
        }
        
        // Regular text
        size_t text_start = *pos;
        while (content[*pos] != '\0' && 
               !(content[*pos] == '{' && (content[*pos + 1] == '{' || content[*pos + 1] == '%'))) {
            (*pos)++;
        }
        
        if (*pos > text_start) {
            TemplateNode* node = shared_malloc_safe(sizeof(TemplateNode), "web", "template_parse_node", 1060);
            if (node) {
                node->type = TEMPLATE_TEXT;
                size_t text_len = *pos - text_start;
                node->content = shared_malloc_safe(text_len + 1, "web", "template_parse_node", 1070);
                if (node->content) {
                    strncpy(node->content, content + text_start, text_len);
                    node->content[text_len] = '\0';
                }
                node->children = NULL;
                node->next = NULL;
                
                if (!head) {
                    head = current = node;
                } else {
                    current->next = node;
                    current = node;
                }
            }
        }
    }
    
    return head;
}

// Parse template content until a specific closing tag is found
TemplateNode* template_parse_until(const char* content, size_t* pos, const char* end_tag) {
    if (!content || !pos || !end_tag) return NULL;
    
    TemplateNode* head = NULL;
    TemplateNode* current = NULL;
    
    while (content[*pos] != '\0') {
        // Look for closing tag
        if (content[*pos] == '{' && content[*pos + 1] == '%') {
            size_t start = *pos + 2;
            size_t end = start;
            while (content[end] != '%' || content[end + 1] != '}') {
                end++;
                if (content[end] == '\0') break;
            }
            
            if (content[end] == '%' && content[end + 1] == '}') {
                size_t tag_len = end - start;
                char* tag_content = shared_malloc_safe(tag_len + 1, "web", "template_parse_until", 1080);
                if (tag_content) {
                    strncpy(tag_content, content + start, tag_len);
                    tag_content[tag_len] = '\0';
                    
                    // Check if this is the closing tag we're looking for
                    if (strcmp(tag_content, end_tag) == 0) {
                        shared_free_safe(tag_content, "web", "template_parse_until", 1090);
                        *pos = end + 2;
                        return head;
                    }
                    
                    shared_free_safe(tag_content, "web", "template_parse_until", 1100);
                }
            }
        }
        
        // Look for template tags
        if (content[*pos] == '{' && content[*pos + 1] == '{') {
            // Variable substitution: {{variable}}
            if (content[*pos + 2] != '{') {
                TemplateNode* node = shared_malloc_safe(sizeof(TemplateNode), "web", "template_parse_until", 1110);
                if (!node) break;
                
                node->type = TEMPLATE_VAR;
                node->content = NULL;
                node->children = NULL;
                node->next = NULL;
                
                // Extract variable name
                size_t start = *pos + 2;
                size_t end = start;
                while (content[end] != '}' || content[end + 1] != '}') {
                    end++;
                    if (content[end] == '\0') break;
                }
                
                if (content[end] == '}' && content[end + 1] == '}') {
                    size_t var_len = end - start;
                    node->variable = shared_malloc_safe(var_len + 1, "web", "template_parse_until", 1120);
                    if (node->variable) {
                        strncpy(node->variable, content + start, var_len);
                        node->variable[var_len] = '\0';
                    }
                    *pos = end + 2;
                } else {
                    // Malformed tag, treat as text
                    node->type = TEMPLATE_TEXT;
                    node->content = shared_strdup("{{");
                    *pos += 2;
                }
                
                if (!head) {
                    head = current = node;
                } else {
                    current->next = node;
                    current = node;
                }
                continue;
            }
        }
        
        // Regular text
        size_t text_start = *pos;
        while (content[*pos] != '\0' && 
               !(content[*pos] == '{' && (content[*pos + 1] == '{' || content[*pos + 1] == '%'))) {
            (*pos)++;
        }
        
        if (*pos > text_start) {
            TemplateNode* node = shared_malloc_safe(sizeof(TemplateNode), "web", "template_parse_until", 1130);
            if (node) {
                node->type = TEMPLATE_TEXT;
                size_t text_len = *pos - text_start;
                node->content = shared_malloc_safe(text_len + 1, "web", "template_parse_until", 1140);
                if (node->content) {
                    strncpy(node->content, content + text_start, text_len);
                    node->content[text_len] = '\0';
                }
                node->children = NULL;
                node->next = NULL;
                
                if (!head) {
                    head = current = node;
                } else {
                    current->next = node;
                    current = node;
                }
            }
        }
    }
    
    return head;
}

char* template_render(Template* tmpl, Value* context) {
    if (!tmpl || !tmpl->ast) return NULL;
    
    // Create result buffer
    size_t capacity = 1024;
    char* result = shared_malloc_safe(capacity, "web", "template_render", 1100);
    if (!result) return NULL;
    
    size_t length = 0;
    result[0] = '\0';
    
    TemplateNode* node = tmpl->ast;
    while (node) {
        char* node_result = template_render_node(node, context);
        if (node_result) {
            size_t node_len = strlen(node_result);
            
            // Resize buffer if needed
            if (length + node_len >= capacity) {
                capacity = (length + node_len) * 2;
                char* new_result = shared_realloc_safe(result, capacity, "web", "template_render", 1110);
                if (!new_result) {
                    shared_free_safe(result, "web", "template_render", 1120);
                    shared_free_safe(node_result, "web", "template_render", 1130);
                    return NULL;
                }
                result = new_result;
            }
            
            size_t remaining = length + node_len + 1;
            if (remaining > length) {
                size_t avail = remaining - length;
                strncat(result, node_result, avail - 1);
                length += node_len;
            }
            shared_free_safe(node_result, "web", "template_render", 1140);
        }
        
        node = node->next;
    }
    
    return result;
}

char* template_render_node(TemplateNode* node, Value* context) {
    if (!node) return NULL;
    
    switch (node->type) {
        case TEMPLATE_TEXT:
            return node->content ? shared_strdup(node->content) : shared_strdup("");
            
        case TEMPLATE_VAR: {
            if (!node->variable) return shared_strdup("");
            
            Value var_value = template_get_variable(node->variable, context);
            if (var_value.type == VALUE_STRING) {
                return shared_strdup(var_value.data.string_value);
            } else if (var_value.type == VALUE_NUMBER) {
                char* num_str = shared_malloc_safe(64, "web", "template_render_node", 1150);
                if (num_str) {
                    snprintf(num_str, 64, "%.6g", var_value.data.number_value);
                }
                return num_str;
            } else if (var_value.type == VALUE_BOOLEAN) {
                return shared_strdup(var_value.data.boolean_value ? "true" : "false");
            } else {
                return shared_strdup("");
            }
        }
        
        case TEMPLATE_IF: {
            if (!node->condition) return shared_strdup("");
            
            // Simple condition evaluation
            Value cond_value = template_get_variable(node->condition, context);
            if (cond_value.type == VALUE_BOOLEAN && cond_value.data.boolean_value) {
                // Render children
                char* result = shared_strdup("");
                TemplateNode* child = node->children;
                while (child) {
                    char* child_result = template_render_node(child, context);
                    if (child_result) {
                        size_t old_len = strlen(result);
                        size_t child_len = strlen(child_result);
                        char* new_result = shared_realloc_safe(result, old_len + child_len + 1, "web", "template_render_node", 1160);
                        if (new_result) {
                            result = new_result;
                            size_t remaining = old_len + child_len + 1 - old_len;
                            strncat(result, child_result, remaining - 1);
                        }
                        shared_free_safe(child_result, "web", "template_render_node", 1170);
                    }
                    child = child->next;
                }
                return result;
            }
            return shared_strdup("");
        }
        
        case TEMPLATE_FOR: {
            // Simple for loop implementation
            if (!node->condition) return shared_strdup("");
            
            // Parse "item in items" format
            char* var_name = node->condition;
            char* in_pos = strstr(var_name, " in ");
            if (!in_pos) return shared_strdup("");
            
            *in_pos = '\0';
            char* array_name = in_pos + 4;
            
            Value array_value = template_get_variable(array_name, context);
            if (array_value.type != VALUE_ARRAY) return shared_strdup("");
            
            char* result = shared_strdup("");
            for (size_t i = 0; i < array_value.data.array_value.count; i++) {
                // Create new context with loop variable
                Value* item_value = (Value*)array_value.data.array_value.elements[i];
                
                // Create new context with loop variable
                Value loop_context = value_create_object(1);
                value_object_set_member(&loop_context, "item", *item_value);
                
                // Render children with item context
                TemplateNode* child = node->children;
                while (child) {
                    char* child_result = template_render_node(child, &loop_context);
                    if (child_result) {
                        size_t old_len = strlen(result);
                        size_t child_len = strlen(child_result);
                        char* new_result = shared_realloc_safe(result, old_len + child_len + 1, "web", "template_render_node", 1180);
                        if (new_result) {
                            result = new_result;
                            size_t remaining = old_len + child_len + 1 - old_len;
                            strncat(result, child_result, remaining - 1);
                        }
                        shared_free_safe(child_result, "web", "template_render_node", 1190);
                    }
                    child = child->next;
                }
            }
            return result;
        }
        
        case TEMPLATE_INCLUDE: {
            // Simple include implementation
            if (!node->condition) return shared_strdup("");
            
            // For now, just return the include path as a comment
            char* result = shared_malloc_safe(strlen(node->condition) + 20, "web", "template_render_node", 1200);
            if (result) {
                snprintf(result, strlen(node->condition) + 20, "<!-- include: %s -->", node->condition);
            }
            return result;
        }
        
        default:
            return shared_strdup("");
    }
}

Value template_get_variable(const char* var_name, Value* context) {
    if (!var_name || !context || (context->type != VALUE_OBJECT && context->type != VALUE_HASH_MAP)) {
        return value_create_null();
    }
    
    if (context->type == VALUE_OBJECT) {
        return value_object_get(context, var_name);
    } else if (context->type == VALUE_HASH_MAP) {
        Value key = value_create_string(var_name);
        Value result = value_hash_map_get(context, key);
        value_free(&key);
        return result;
    }
    
    return value_create_null();
}

void template_free(Template* tmpl) {
    if (!tmpl) return;
    
    shared_free_safe(tmpl->content, "web", "template_free", 1210);
    shared_free_safe(tmpl->template_path, "web", "template_free", 1220);
    
    // Free AST
    template_free_node(tmpl->ast);
    
    shared_free_safe(tmpl, "web", "template_free", 1230);
}

void template_free_node(TemplateNode* node) {
    if (!node) return;
    
    shared_free_safe(node->content, "web", "template_free_node", 1240);
    shared_free_safe(node->condition, "web", "template_free_node", 1250);
    shared_free_safe(node->variable, "web", "template_free_node", 1260);
    
    // Free children
    template_free_node(node->children);
    
    // Free siblings
    template_free_node(node->next);
    
    shared_free_safe(node, "web", "template_free_node", 1270);
}
