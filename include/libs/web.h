#ifndef MYCO_WEB_H
#define MYCO_WEB_H

#include "../core/core.h"
#include "../core/interpreter/value_operations.h"
#include <time.h>
#include <pthread.h>

// Web Framework Structures
typedef struct MycoWebApp {
    char* name;
    int port;
    bool is_running;
    struct MycoServer* server;
    struct WebRoute* routes;
    struct Middleware* middleware;
    struct MycoWebApp* next;
} MycoWebApp;

typedef struct WebRoute {
    char* method;
    char* path;
    Value handler;
    struct WebRoute* next;
} WebRoute;

// Enhanced Request/Response structures (extending existing server structures)
typedef struct MycoWebRequest {
    char* method;
    char* url;
    char* path;
    char* query_string;
    char** headers;
    size_t header_count;
    char* body;
    char** params;
    size_t param_count;
    struct MycoWebRequest* next;
} MycoWebRequest;

typedef struct MycoWebResponse {
    int status_code;
    char** headers;
    size_t header_count;
    char* body;
    bool sent;
    struct MycoWebResponse* next;
} MycoWebResponse;

// Session Management Structures
typedef struct SessionData {
    char* key;
    Value value;
    struct SessionData* next;
} SessionData;

typedef struct Session {
    char* session_id;
    SessionData* data;
    time_t created_at;
    time_t last_accessed;
    int expires_in; // seconds
    struct Session* next;
} Session;

typedef struct SessionManager {
    Session* sessions;
    int max_age;
    char* cookie_name;
    pthread_mutex_t lock;
} SessionManager;

// Template Engine Structures
typedef enum {
    TEMPLATE_TEXT,
    TEMPLATE_VAR,
    TEMPLATE_IF,
    TEMPLATE_FOR,
    TEMPLATE_INCLUDE
} TemplateNodeType;

typedef struct TemplateNode {
    TemplateNodeType type;
    char* content;
    char* condition;  // for if/for nodes
    char* variable;   // for var nodes
    struct TemplateNode* children;
    struct TemplateNode* next;
} TemplateNode;

typedef struct Template {
    char* content;
    TemplateNode* ast;
    char* template_path;
} Template;

// Web Framework Functions
Value builtin_web_app_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_web_app_get(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_web_app_post(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_web_app_put(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_web_app_delete(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_web_app_use(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_web_app_listen(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Enhanced Request Methods
Value builtin_web_request_get_param(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_web_request_get_query(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_web_request_get_header(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_web_request_get_body(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_web_request_json(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Enhanced Response Methods
Value builtin_web_response_status(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_web_response_header(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_web_response_json(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_web_response_html(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_web_response_send(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_web_response_redirect(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Template Engine Functions
Value builtin_template_render(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_web_response_render(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Session Management Functions
Value builtin_session_start(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_session_get(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_session_set(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_session_destroy(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_session_cleanup(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Internal helper functions
MycoWebApp* web_app_create(const char* name);
void web_app_free(MycoWebApp* app);
WebRoute* web_route_create(const char* method, const char* path, Value handler);
void web_route_free(WebRoute* route);
MycoWebRequest* web_request_create(const char* method, const char* url);
void web_request_free(MycoWebRequest* req);
MycoWebResponse* web_response_create(void);
void web_response_free(MycoWebResponse* res);

// Template Engine Internal Functions
Template* template_parse(const char* content);
char* template_render(Template* tmpl, Value* context);
void template_free(Template* tmpl);
TemplateNode* template_parse_node(const char* content, size_t* pos);
TemplateNode* template_parse_until(const char* content, size_t* pos, const char* end_tag);
char* template_eval_expression(const char* expr, Value* context);
Value template_get_variable(const char* var_name, Value* context);
char* template_render_node(TemplateNode* node, Value* context);
void template_free_node(TemplateNode* node);

// Session Management Internal Functions
SessionManager* session_manager_create(int max_age);
void session_manager_free(SessionManager* mgr);
Session* session_create(SessionManager* mgr);
Session* session_get(SessionManager* mgr, const char* session_id);
void session_set(Session* session, const char* key, Value value);
Value session_get_value(Session* session, const char* key);
void session_destroy(SessionManager* mgr, const char* session_id);
void session_cleanup_expired(SessionManager* mgr);
char* session_generate_id(void);
void session_data_free(SessionData* data);

// Library registration
void web_library_register(Interpreter* interpreter);

#endif // MYCO_WEB_H
