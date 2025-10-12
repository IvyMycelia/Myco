#include "interpreter/value_operations.h"
#include "../../include/core/interpreter.h"
#include "../../include/utils/shared_utilities.h"
#include "../../include/libs/trees.h"
#include "../../include/libs/graphs.h"
#include "../../include/libs/heaps.h"
#include "../../include/libs/queues.h"
#include "../../include/libs/stacks.h"
#include "../../include/libs/server/server.h"
#include <stdlib.h>
#include <string.h>


// ============================================================================
// LIBRARY METHOD HANDLERS
// ============================================================================

Value handle_tree_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object) {
    // Pass the object directly to the builtin functions
    
    // Evaluate all arguments and add object as first argument
    size_t arg_count = call_node->data.function_call_expr.argument_count;
    Value* args = (Value*)calloc(arg_count + 1, sizeof(Value));
    if (!args) {
        interpreter_set_error(interpreter, "Memory allocation failed", call_node->line, call_node->column);
        return value_create_null();
    }
    
    // Add object as first argument
    args[0] = value_clone(&object);
    
    for (size_t i = 0; i < arg_count; i++) {
        args[i + 1] = interpreter_execute(interpreter, call_node->data.function_call_expr.arguments[i]);
    }
    
    Value result = value_create_null();
    
    // Call the appropriate tree method
    if (strcmp(method_name, "insert") == 0) {
        result = builtin_tree_insert(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "search") == 0) {
        result = builtin_tree_search(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "size") == 0) {
        result = builtin_tree_size(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "isEmpty") == 0) {
        result = builtin_tree_is_empty(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "clear") == 0) {
        result = builtin_tree_clear(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else {
        interpreter_set_error(interpreter, "Unknown tree method", call_node->line, call_node->column);
        result = value_create_null();
    }
    
    // Clean up arguments
    for (size_t i = 0; i < arg_count + 1; i++) {
        value_free(&args[i]);
    }
    shared_free_safe(args, "interpreter", "handle_tree_method_call", 0);
    
    return result;
}

Value handle_graph_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object) {
    // Pass the object directly to the builtin functions
    
    // Evaluate all arguments and add object as first argument
    size_t arg_count = call_node->data.function_call_expr.argument_count;
    Value* args = (Value*)calloc(arg_count + 1, sizeof(Value));
    if (!args) {
        interpreter_set_error(interpreter, "Memory allocation failed", call_node->line, call_node->column);
        return value_create_null();
    }
    
    // Add object as first argument
    args[0] = value_clone(&object);
    
    for (size_t i = 0; i < arg_count; i++) {
        args[i + 1] = interpreter_execute(interpreter, call_node->data.function_call_expr.arguments[i]);
    }
    
    Value result = value_create_null();
    
    // Call the appropriate graph method
    if (strcmp(method_name, "add_node") == 0) {
        result = builtin_graph_add_node(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "add_edge") == 0) {
        result = builtin_graph_add_edge(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "size") == 0) {
        result = builtin_graph_size(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "is_empty") == 0 || strcmp(method_name, "isEmpty") == 0) {
        result = builtin_graph_is_empty(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "clear") == 0) {
        result = builtin_graph_clear(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else {
        interpreter_set_error(interpreter, "Unknown graph method", call_node->line, call_node->column);
        result = value_create_null();
    }
    
    // Clean up arguments
    for (size_t i = 0; i < arg_count + 1; i++) {
        value_free(&args[i]);
    }
    shared_free_safe(args, "interpreter", "handle_graph_method_call", 0);
    
    return result;
}

Value handle_heap_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object) {
    // Pass the object directly to the builtin functions
    
    // Evaluate all arguments and add object as first argument
    size_t arg_count = call_node->data.function_call_expr.argument_count;
    Value* args = (Value*)calloc(arg_count + 1, sizeof(Value));
    if (!args) {
        interpreter_set_error(interpreter, "Memory allocation failed", call_node->line, call_node->column);
        return value_create_null();
    }
    
    // Add object as first argument
    args[0] = value_clone(&object);
    
    for (size_t i = 0; i < arg_count; i++) {
        args[i + 1] = interpreter_execute(interpreter, call_node->data.function_call_expr.arguments[i]);
    }
    
    Value result = value_create_null();
    
    // Call the appropriate heap method
    if (strcmp(method_name, "insert") == 0) {
        result = builtin_heap_insert(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "extract") == 0) {
        result = builtin_heap_extract(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "peek") == 0) {
        result = builtin_heap_peek(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "size") == 0) {
        result = builtin_heap_size(interpreter, args, arg_count + 1, call_node->line, call_node->column);
        } else if (strcmp(method_name, "isEmpty") == 0) {
            result = builtin_heap_isEmpty(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "clear") == 0) {
        result = builtin_heap_clear(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else {
        interpreter_set_error(interpreter, "Unknown heap method", call_node->line, call_node->column);
        result = value_create_null();
    }
    
    // Clean up arguments
    for (size_t i = 0; i < arg_count + 1; i++) {
        value_free(&args[i]);
    }
    shared_free_safe(args, "interpreter", "handle_heap_method_call", 0);
    
    return result;
}

Value handle_queue_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object) {
    // Pass the object directly to the builtin functions
    
    // Evaluate all arguments and add object as first argument
    size_t arg_count = call_node->data.function_call_expr.argument_count;
    Value* args = (Value*)calloc(arg_count + 1, sizeof(Value));
    if (!args) {
        interpreter_set_error(interpreter, "Memory allocation failed", call_node->line, call_node->column);
        return value_create_null();
    }
    
    // Add object as first argument
    args[0] = value_clone(&object);
    
    for (size_t i = 0; i < arg_count; i++) {
        args[i + 1] = interpreter_execute(interpreter, call_node->data.function_call_expr.arguments[i]);
    }
    
    Value result = value_create_null();
    
    // Call the appropriate queue method
    if (strcmp(method_name, "enqueue") == 0) {
        result = builtin_queue_enqueue(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "dequeue") == 0) {
        result = builtin_queue_dequeue(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "front") == 0) {
        result = builtin_queue_front(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "back") == 0) {
        result = builtin_queue_back(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "size") == 0) {
        result = builtin_queue_size(interpreter, args, arg_count + 1, call_node->line, call_node->column);
        } else if (strcmp(method_name, "isEmpty") == 0) {
            result = builtin_queue_isEmpty(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "clear") == 0) {
        result = builtin_queue_clear(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else {
        interpreter_set_error(interpreter, "Unknown queue method", call_node->line, call_node->column);
        result = value_create_null();
    }
    
    // Clean up arguments
    for (size_t i = 0; i < arg_count + 1; i++) {
        value_free(&args[i]);
    }
    shared_free_safe(args, "interpreter", "handle_queue_method_call", 0);
    
    return result;
}

Value handle_stack_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object) {
    // Pass the object directly to the builtin functions
    
    // Evaluate all arguments and add object as first argument
    size_t arg_count = call_node->data.function_call_expr.argument_count;
    Value* args = (Value*)calloc(arg_count + 1, sizeof(Value));
    if (!args) {
        interpreter_set_error(interpreter, "Memory allocation failed", call_node->line, call_node->column);
        return value_create_null();
    }
    
    // Add object as first argument
    args[0] = value_clone(&object);
    
    for (size_t i = 0; i < arg_count; i++) {
        args[i + 1] = interpreter_execute(interpreter, call_node->data.function_call_expr.arguments[i]);
    }
    
    Value result = value_create_null();
    
    // Call the appropriate stack method
    if (strcmp(method_name, "push") == 0) {
        result = builtin_stack_push(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "pop") == 0) {
        result = builtin_stack_pop(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "top") == 0) {
        result = builtin_stack_top(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "size") == 0) {
        result = builtin_stack_size(interpreter, args, arg_count + 1, call_node->line, call_node->column);
        } else if (strcmp(method_name, "isEmpty") == 0) {
            result = builtin_stack_isEmpty(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "clear") == 0) {
        result = builtin_stack_clear(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else {
        interpreter_set_error(interpreter, "Unknown stack method", call_node->line, call_node->column);
        result = value_create_null();
    }
    
    // Clean up arguments
    for (size_t i = 0; i < arg_count + 1; i++) {
        value_free(&args[i]);
    }
    shared_free_safe(args, "interpreter", "handle_stack_method_call", 0);
    
    return result;
}

Value handle_server_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object) {
    size_t arg_count = call_node->data.function_call_expr.argument_count;
    Value* args = (Value*)calloc(arg_count + 1, sizeof(Value));
    if (!args) {
        interpreter_set_error(interpreter, "Out of memory", call_node->line, call_node->column);
        return value_create_null();
    }
    
    // Add object as first argument
    args[0] = value_clone(&object);
    
    for (size_t i = 0; i < arg_count; i++) {
        args[i + 1] = interpreter_execute(interpreter, call_node->data.function_call_expr.arguments[i]);
    }
    
    Value result = value_create_null();
    
    // Call the appropriate server method
    if (strcmp(method_name, "start") == 0) {
        result = builtin_server_listen(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "stop") == 0) {
        result = builtin_server_stop(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "get") == 0) {
        result = builtin_server_get(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "post") == 0) {
        result = builtin_server_post(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "put") == 0) {
        result = builtin_server_put(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "delete") == 0) {
        result = builtin_server_delete(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "listen") == 0) {
        result = builtin_server_listen(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else {
        interpreter_set_error(interpreter, "Unknown server method", call_node->line, call_node->column);
        result = value_create_null();
    }
    
    // Clean up arguments
    for (size_t i = 0; i < arg_count + 1; i++) {
        value_free(&args[i]);
    }
    shared_free_safe(args, "interpreter", "handle_server_method_call", 0);
    
    return result;
}

Value handle_request_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object) {
    size_t arg_count = call_node->data.function_call_expr.argument_count;
    Value* args = (Value*)calloc(arg_count + 1, sizeof(Value));
    if (!args) {
        interpreter_set_error(interpreter, "Out of memory", call_node->line, call_node->column);
        return value_create_null();
    }
    
    // Add object as first argument
    args[0] = value_clone(&object);
    
    for (size_t i = 0; i < arg_count; i++) {
        args[i + 1] = interpreter_execute(interpreter, call_node->data.function_call_expr.arguments[i]);
    }
    
    Value result = value_create_null();
    
    // Call the appropriate request method
    if (strcmp(method_name, "get_header") == 0) {
        result = builtin_request_header(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "get_body") == 0) {
        result = builtin_request_body(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "get_method") == 0) {
        result = builtin_request_method(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "get_url") == 0) {
        result = builtin_request_url(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "get_query") == 0) {
        result = builtin_request_query(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else {
        interpreter_set_error(interpreter, "Unknown request method", call_node->line, call_node->column);
        result = value_create_null();
    }
    
    // Clean up arguments
    for (size_t i = 0; i < arg_count + 1; i++) {
        value_free(&args[i]);
    }
    shared_free_safe(args, "interpreter", "handle_request_method_call", 0);
    
    return result;
}

Value handle_response_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object) {
    size_t arg_count = call_node->data.function_call_expr.argument_count;
    Value* args = (Value*)calloc(arg_count + 1, sizeof(Value));
    if (!args) {
        interpreter_set_error(interpreter, "Out of memory", call_node->line, call_node->column);
        return value_create_null();
    }
    
    // Add object as first argument
    args[0] = value_clone(&object);
    
    for (size_t i = 0; i < arg_count; i++) {
        args[i + 1] = interpreter_execute(interpreter, call_node->data.function_call_expr.arguments[i]);
    }
    
    Value result = value_create_null();
    
    // Call the appropriate response method
    if (strcmp(method_name, "set_status") == 0) {
        result = builtin_response_status(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "set_header") == 0) {
        result = builtin_response_set_header(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "write") == 0) {
        result = builtin_response_send(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "end") == 0) {
        result = builtin_response_send(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "json") == 0) {
        result = builtin_response_json(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else {
        interpreter_set_error(interpreter, "Unknown response method", call_node->line, call_node->column);
        result = value_create_null();
    }
    
    // Clean up arguments
    for (size_t i = 0; i < arg_count + 1; i++) {
        value_free(&args[i]);
    }
    shared_free_safe(args, "interpreter", "handle_response_method_call", 0);
    
    return result;
}

Value handle_route_group_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object) {
    size_t arg_count = call_node->data.function_call_expr.argument_count;
    Value* args = (Value*)calloc(arg_count + 1, sizeof(Value));
    if (!args) {
        interpreter_set_error(interpreter, "Out of memory", call_node->line, call_node->column);
        return value_create_null();
    }
    
    // Add object as first argument
    args[0] = value_clone(&object);
    
    for (size_t i = 0; i < arg_count; i++) {
        args[i + 1] = interpreter_execute(interpreter, call_node->data.function_call_expr.arguments[i]);
    }
    
    Value result = value_create_null();
    
    // Call the appropriate route group method
    if (strcmp(method_name, "get") == 0) {
        result = builtin_group_get(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "post") == 0) {
        result = builtin_group_post(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "put") == 0) {
        result = builtin_group_put(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "delete") == 0) {
        result = builtin_group_delete(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else if (strcmp(method_name, "use") == 0) {
        result = builtin_server_use(interpreter, args, arg_count + 1, call_node->line, call_node->column);
    } else {
        interpreter_set_error(interpreter, "Unknown route group method", call_node->line, call_node->column);
        result = value_create_null();
    }
    
    // Clean up arguments
    for (size_t i = 0; i < arg_count + 1; i++) {
        value_free(&args[i]);
    }
    shared_free_safe(args, "interpreter", "handle_route_group_method_call", 0);
    
    return result;
}

Value handle_server_library_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object) {
    size_t arg_count = call_node->data.function_call_expr.argument_count;
    Value* args = (Value*)calloc(arg_count, sizeof(Value));
    if (!args) {
        interpreter_set_error(interpreter, "Out of memory", call_node->line, call_node->column);
        return value_create_null();
    }
    
    for (size_t i = 0; i < arg_count; i++) {
        args[i] = interpreter_execute(interpreter, call_node->data.function_call_expr.arguments[i]);
    }
    
    Value result = value_create_null();
    
    // Call the appropriate server library method
    if (strcmp(method_name, "create") == 0) {
        result = builtin_server_create(interpreter, args, arg_count, call_node->line, call_node->column);
    } else if (strcmp(method_name, "start") == 0) {
        result = builtin_server_listen(interpreter, args, arg_count, call_node->line, call_node->column);
    } else if (strcmp(method_name, "stop") == 0) {
        result = builtin_server_stop(interpreter, args, arg_count, call_node->line, call_node->column);
    } else if (strcmp(method_name, "get") == 0) {
        result = builtin_server_get(interpreter, args, arg_count, call_node->line, call_node->column);
    } else if (strcmp(method_name, "post") == 0) {
        result = builtin_server_post(interpreter, args, arg_count, call_node->line, call_node->column);
    } else if (strcmp(method_name, "put") == 0) {
        result = builtin_server_put(interpreter, args, arg_count, call_node->line, call_node->column);
    } else if (strcmp(method_name, "delete") == 0) {
        result = builtin_server_delete(interpreter, args, arg_count, call_node->line, call_node->column);
    } else if (strcmp(method_name, "listen") == 0) {
        result = builtin_server_listen(interpreter, args, arg_count, call_node->line, call_node->column);
    } else {
        interpreter_set_error(interpreter, "Unknown server library method", call_node->line, call_node->column);
        result = value_create_null();
    }
    
    // Clean up arguments
    for (size_t i = 0; i < arg_count; i++) {
        value_free(&args[i]);
    }
    shared_free_safe(args, "interpreter", "handle_server_library_method_call", 0);
    
    return result;
}
