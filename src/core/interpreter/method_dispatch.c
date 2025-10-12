#include "interpreter/value_operations.h"
#include "../../include/core/interpreter.h"
#include "../../include/utils/shared_utilities.h"
#include <stdlib.h>
#include <string.h>

// Forward declarations for library method handlers
Value handle_tree_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_graph_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_heap_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_queue_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_stack_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_server_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_request_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_response_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_route_group_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_server_library_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);


// ============================================================================
// METHOD DISPATCH FUNCTIONS
// ============================================================================

Value handle_super_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name) {
    if (!interpreter->self_context) {
        shared_error_report("interpreter", "handle_super_method_call", 
                        "super is not available outside of method calls", call_node->line, call_node->column);
        return value_create_null();
    }
    
    // Get the class name from self context
    Value class_name = value_object_get(interpreter->self_context, "__class_name__");
    if (class_name.type != VALUE_STRING) {
        shared_error_report("interpreter", "handle_super_method_call", 
                        "self context does not have a class name", call_node->line, call_node->column);
        value_free(&class_name);
        return value_create_null();
    }
    
    // Look up the parent class
    Value parent_class = environment_get(interpreter->global_environment, class_name.data.string_value);
    if (parent_class.type != VALUE_CLASS) {
        shared_error_report("interpreter", "handle_super_method_call", 
                        "parent class not found", call_node->line, call_node->column);
        value_free(&class_name);
        value_free(&parent_class);
        return value_create_null();
    }
    
    // Find the method in the parent class
    Value method = find_method_in_inheritance_chain(interpreter, &parent_class, method_name);
    if (method.type == VALUE_NULL) {
        shared_error_report("interpreter", "handle_super_method_call", 
                        "method not found in parent class", call_node->line, call_node->column);
        value_free(&class_name);
        value_free(&parent_class);
        return value_create_null();
    }
    
    // Evaluate arguments
    size_t arg_count = call_node->data.function_call_expr.argument_count;
    Value* args = (Value*)shared_malloc_safe(arg_count * sizeof(Value), "interpreter", "handle_super_method_call", 0);
    if (!args) {
        value_free(&class_name);
        value_free(&parent_class);
        value_free(&method);
        return value_create_null();
    }
    
    for (size_t i = 0; i < arg_count; i++) {
        args[i] = interpreter_execute(interpreter, call_node->data.function_call_expr.arguments[i]);
    }
    
    // Call the method with self context
    Value result = value_function_call_with_self(&method, args, arg_count, interpreter, interpreter->self_context, call_node->line, call_node->column);
    
    // Clean up
    for (size_t i = 0; i < arg_count; i++) {
        value_free(&args[i]);
    }
    shared_free_safe(args, "interpreter", "handle_super_method_call", 0);
    value_free(&class_name);
    value_free(&parent_class);
    value_free(&method);
    
    return result;
}

// Helper function to handle method calls
Value handle_method_call(Interpreter* interpreter, ASTNode* call_node, Value object) {
    // Get the object and method name from the member access
    ASTNode* member_access = call_node->data.function_call_expr.function;
    const char* method_name = member_access->data.member_access.member_name;
    
    // Handle namespace marker method calls (e.g., math.abs(-5))
    if (object.type == VALUE_STRING && strcmp(object.data.string_value, "namespace_marker") == 0) {
        // This is a namespace marker, try to look up the prefixed function
        // Get the alias name from the member access object
        const char* alias_name = NULL;
        if (member_access->data.member_access.object->type == AST_NODE_IDENTIFIER) {
            alias_name = member_access->data.member_access.object->data.identifier_value;
        }
        
        if (alias_name) {
            // Look up the function in the global environment
            Value func = environment_get(interpreter->global_environment, alias_name);
            if (func.type == VALUE_FUNCTION) {
                // Evaluate arguments
                size_t arg_count = call_node->data.function_call_expr.argument_count;
                Value* args = (Value*)shared_malloc_safe(arg_count * sizeof(Value), "interpreter", "handle_method_call", 0);
                if (!args) {
                    value_free(&func);
                    return value_create_null();
                }
                
                for (size_t i = 0; i < arg_count; i++) {
                    args[i] = interpreter_execute(interpreter, call_node->data.function_call_expr.arguments[i]);
                }
                
                // Call the function
                Value result = value_function_call(&func, args, arg_count, interpreter, call_node->line, call_node->column);
                
                // Clean up
                for (size_t i = 0; i < arg_count; i++) {
                    value_free(&args[i]);
                }
                shared_free_safe(args, "interpreter", "handle_method_call", 0);
                value_free(&func);
                
                return result;
            }
            value_free(&func);
        }
        
        // If we get here, the function wasn't found
        interpreter_set_error(interpreter, "Function not found in namespace", call_node->line, call_node->column);
        return value_create_null();
    }
    
    // Handle super method calls
    if (strcmp(method_name, "super") == 0) {
        // Get the method name from the first argument
        if (call_node->data.function_call_expr.argument_count > 0) {
            Value method_name_val = interpreter_execute(interpreter, call_node->data.function_call_expr.arguments[0]);
            if (method_name_val.type == VALUE_STRING) {
                Value result = handle_super_method_call(interpreter, call_node, method_name_val.data.string_value);
                value_free(&method_name_val);
                return result;
            }
            value_free(&method_name_val);
        }
        interpreter_set_error(interpreter, "super() requires a method name", call_node->line, call_node->column);
        return value_create_null();
    }
    
    // Check if this is a custom object method call (Tree, Graph, Heap, Queue, Stack)
    if (object.type == VALUE_OBJECT) {
        Value class_name = value_object_get(&object, "__class_name__");
        if (class_name.type == VALUE_STRING) {
            if (strcmp(class_name.data.string_value, "Tree") == 0) {
                // Handle tree method calls
                value_free(&class_name);
                return handle_tree_method_call(interpreter, call_node, method_name, object);
            } else if (strcmp(class_name.data.string_value, "Graph") == 0) {
                // Handle graph method calls
                value_free(&class_name);
                return handle_graph_method_call(interpreter, call_node, method_name, object);
            } else if (strcmp(class_name.data.string_value, "Heap") == 0) {
                // Handle heap method calls
                value_free(&class_name);
                return handle_heap_method_call(interpreter, call_node, method_name, object);
            } else if (strcmp(class_name.data.string_value, "Queue") == 0) {
                // Handle queue method calls
                value_free(&class_name);
                return handle_queue_method_call(interpreter, call_node, method_name, object);
            } else if (strcmp(class_name.data.string_value, "Stack") == 0) {
                // Handle stack method calls
                value_free(&class_name);
                return handle_stack_method_call(interpreter, call_node, method_name, object);
            } else if (strcmp(class_name.data.string_value, "Server") == 0) {
                // Handle server method calls
                value_free(&class_name);
                return handle_server_method_call(interpreter, call_node, method_name, object);
            } else if (strcmp(class_name.data.string_value, "Request") == 0) {
                // Handle request method calls
                value_free(&class_name);
                return handle_request_method_call(interpreter, call_node, method_name, object);
            } else if (strcmp(class_name.data.string_value, "Response") == 0) {
                // Handle response method calls
                value_free(&class_name);
                return handle_response_method_call(interpreter, call_node, method_name, object);
            } else if (strcmp(class_name.data.string_value, "RouteGroup") == 0) {
                // Handle route group method calls
                value_free(&class_name);
                return handle_route_group_method_call(interpreter, call_node, method_name, object);
            } else if (strcmp(class_name.data.string_value, "ServerLibrary") == 0) {
                // Handle server library method calls
                value_free(&class_name);
                return handle_server_library_method_call(interpreter, call_node, method_name, object);
            }
        }
        value_free(&class_name);
    }
    
    // If we get here, the method wasn't found
    interpreter_set_error(interpreter, "Method not found", call_node->line, call_node->column);
    return value_create_null();
}
