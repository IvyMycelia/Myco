#include "type_checker.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../../include/utils/shared_utilities.h"

// Type checker context management
TypeCheckerContext* type_checker_create_context(void) {
    TypeCheckerContext* context = shared_malloc_safe(sizeof(TypeCheckerContext), "type_checker", "unknown_function", 9);
    if (!context) return NULL;
    
    context->current_environment = type_environment_create(NULL);
    context->function_return_types = NULL;
    context->function_names = NULL;
    context->function_count = 0;
    context->function_capacity = 0;
    context->error_count = 0;
    context->error_messages = NULL;
    context->error_capacity = 0;
    context->inference_engine = type_inference_engine_create();
    
    return context;
}

void type_checker_free_context(TypeCheckerContext* context) {
    if (!context) return;
    
    type_environment_free(context->current_environment);
    
    // Free function types
    for (size_t i = 0; i < context->function_count; i++) {
        type_free(context->function_return_types[i]);
        shared_free_safe(context->function_names[i], "core", "unknown_function", 32);
    }
    shared_free_safe(context->function_return_types, "core", "unknown_function", 34);
    shared_free_safe(context->function_names, "core", "unknown_function", 35);
    
    // Free error messages
    for (int i = 0; i < context->error_count; i++) {
        shared_free_safe(context->error_messages[i], "core", "unknown_function", 39);
    }
    shared_free_safe(context->error_messages, "core", "unknown_function", 41);
    
    // Free inference engine
    type_inference_engine_free(context->inference_engine);
    
    shared_free_safe(context, "core", "unknown_function", 43);
}

// Type creation and management
MycoType* type_create(MycoTypeKind kind, int line, int column) {
    MycoType* type = shared_malloc_safe(sizeof(MycoType), "type_checker", "unknown_function", 48);
    if (!type) return NULL;
    
    type->kind = kind;
    type->line = line;
    type->column = column;
    
    
    // Initialize union based on kind
    switch (kind) {
        case TYPE_ARRAY:
            type->data.element_type = NULL;
            break;
        case TYPE_FUNCTION:
            type->data.function_type.parameter_types = NULL;
            type->data.function_type.parameter_count = 0;
            type->data.function_type.return_type = NULL;
            break;
        case TYPE_CLASS:
            type->data.class_name = NULL;
            break;
        case TYPE_UNION:
            type->data.union_type.types = NULL;
            type->data.union_type.type_count = 0;
            break;
        case TYPE_OPTIONAL:
            type->data.optional_type = NULL;
            break;
        case TYPE_GENERIC:
            type->data.generic_type.parameter_names = NULL;
            type->data.generic_type.parameter_count = 0;
            type->data.generic_type.constraints = NULL;
            type->data.generic_type.base_type = NULL;
            break;
        case TYPE_GENERIC_PARAMETER:
            type->data.generic_parameter.parameter_name = NULL;
            type->data.generic_parameter.constraint = NULL;
            break;
        default:
            break;
    }
    
    return type;
}

MycoType* type_create_array(MycoType* element_type, int line, int column) {
    MycoType* type = type_create(TYPE_ARRAY, line, column);
    if (type) {
        type->data.element_type = element_type;
    }
    return type;
}

MycoType* type_create_function(MycoType** parameter_types, size_t parameter_count, 
                               MycoType* return_type, int line, int column) {
    MycoType* type = type_create(TYPE_FUNCTION, line, column);
    if (type) {
        type->data.function_type.parameter_types = parameter_types;
        type->data.function_type.parameter_count = parameter_count;
        type->data.function_type.return_type = return_type;
    }
    return type;
}

MycoType* type_create_class(const char* class_name, int line, int column) {
    MycoType* type = type_create(TYPE_CLASS, line, column);
    if (type) {
        type->data.class_name = (class_name ? strdup(class_name) : NULL);
    }
    return type;
}

MycoType* type_create_union(MycoType** types, size_t type_count, int line, int column) {
    MycoType* type = type_create(TYPE_UNION, line, column);
    if (type) {
        type->data.union_type.types = types;
        type->data.union_type.type_count = type_count;
    }
    return type;
}

MycoType* type_create_optional(MycoType* wrapped_type, int line, int column) {
    MycoType* type = type_create(TYPE_OPTIONAL, line, column);
    if (type) {
        type->data.optional_type = wrapped_type;
    }
    return type;
}

MycoType* type_clone(MycoType* type) {
    if (!type) return NULL;
    
    MycoType* clone = type_create(type->kind, type->line, type->column);
    if (!clone) return NULL;
    
    switch (type->kind) {
        case TYPE_ARRAY:
            clone->data.element_type = type_clone(type->data.element_type);
            break;
        case TYPE_FUNCTION:
            clone->data.function_type.parameter_count = type->data.function_type.parameter_count;
            clone->data.function_type.parameter_types = shared_malloc_safe(sizeof(MycoType*) * type->data.function_type.parameter_count, "type_checker", "unknown_function", 139);
            if (clone->data.function_type.parameter_types) {
                for (size_t i = 0; i < type->data.function_type.parameter_count; i++) {
                    clone->data.function_type.parameter_types[i] = type_clone(type->data.function_type.parameter_types[i]);
                }
            }
            clone->data.function_type.return_type = type_clone(type->data.function_type.return_type);
            break;
        case TYPE_CLASS:
            clone->data.class_name = (type->data.class_name ? strdup(type->data.class_name) : NULL);
            break;
        case TYPE_UNION:
            clone->data.union_type.type_count = type->data.union_type.type_count;
            clone->data.union_type.types = shared_malloc_safe(sizeof(MycoType*) * type->data.union_type.type_count, "type_checker", "unknown_function", 152);
            if (clone->data.union_type.types) {
                for (size_t i = 0; i < type->data.union_type.type_count; i++) {
                    clone->data.union_type.types[i] = type_clone(type->data.union_type.types[i]);
                }
            }
            break;
        case TYPE_OPTIONAL:
            clone->data.optional_type = type_clone(type->data.optional_type);
            break;
        default:
            break;
    }
    
    return clone;
}

void type_free(MycoType* type) {
    if (!type) return;
    
    switch (type->kind) {
        case TYPE_ARRAY:
            type_free(type->data.element_type);
            break;
        case TYPE_FUNCTION:
            for (size_t i = 0; i < type->data.function_type.parameter_count; i++) {
                type_free(type->data.function_type.parameter_types[i]);
            }
            shared_free_safe(type->data.function_type.parameter_types, "core", "unknown_function", 180);
            type_free(type->data.function_type.return_type);
            break;
        case TYPE_CLASS:
            shared_free_safe(type->data.class_name, "core", "unknown_function", 184);
            break;
        case TYPE_UNION:
            for (size_t i = 0; i < type->data.union_type.type_count; i++) {
                type_free(type->data.union_type.types[i]);
            }
            shared_free_safe(type->data.union_type.types, "core", "unknown_function", 190);
            break;
        case TYPE_OPTIONAL:
            type_free(type->data.optional_type);
            break;
        case TYPE_GENERIC:
            // Free parameter names
            if (type->data.generic_type.parameter_names) {
                for (size_t i = 0; i < type->data.generic_type.parameter_count; i++) {
                    shared_free_safe(type->data.generic_type.parameter_names[i], "core", "unknown_function", 204);
                }
                shared_free_safe(type->data.generic_type.parameter_names, "core", "unknown_function", 205);
            }
            // Free constraints
            if (type->data.generic_type.constraints) {
                for (size_t i = 0; i < type->data.generic_type.parameter_count; i++) {
                    type_free(type->data.generic_type.constraints[i]);
                }
                shared_free_safe(type->data.generic_type.constraints, "core", "unknown_function", 210);
            }
            // Free base type
            type_free(type->data.generic_type.base_type);
            break;
        case TYPE_GENERIC_PARAMETER:
            shared_free_safe(type->data.generic_parameter.parameter_name, "core", "unknown_function", 214);
            type_free(type->data.generic_parameter.constraint);
            break;
        default:
            break;
    }
    
    shared_free_safe(type, "core", "unknown_function", 199);
}

// Type environment management
TypeEnvironment* type_environment_create(TypeEnvironment* parent) {
    TypeEnvironment* env = shared_malloc_safe(sizeof(TypeEnvironment), "type_checker", "unknown_function", 204);
    if (!env) return NULL;
    
    env->parent = parent;
    env->variable_names = NULL;
    env->variable_types = NULL;
    env->variable_count = 0;
    env->variable_capacity = 0;
    
    return env;
}

void type_environment_free(TypeEnvironment* env) {
    if (!env) return;
    
    for (size_t i = 0; i < env->variable_count; i++) {
        shared_free_safe(env->variable_names[i], "core", "unknown_function", 220);
        type_free(env->variable_types[i]);
    }
    shared_free_safe(env->variable_names, "core", "unknown_function", 223);
    shared_free_safe(env->variable_types, "core", "unknown_function", 224);
    shared_free_safe(env, "core", "unknown_function", 225);
}

int type_environment_add_variable(TypeEnvironment* env, const char* name, MycoType* type) {
    if (!env || !name || !type) return 0;
    
    // Check if variable already exists
    if (type_environment_lookup_variable(env, name)) {
        return 0; // Variable already exists
    }
    
    // Expand capacity if needed
    if (env->variable_count >= env->variable_capacity) {
        size_t new_capacity = env->variable_capacity == 0 ? 4 : env->variable_capacity * 2;
        char** new_names = shared_realloc_safe(env->variable_names, new_capacity * sizeof(char*), "type_checker", "unknown_function", 239);
        MycoType** new_types = shared_realloc_safe(env->variable_types, new_capacity * sizeof(MycoType*), "type_checker", "unknown_function", 240);
        
        if (!new_names || !new_types) {
            shared_free_safe(new_names, "core", "unknown_function", 243);
            shared_free_safe(new_types, "core", "unknown_function", 244);
            return 0;
        }
        
        env->variable_names = new_names;
        env->variable_types = new_types;
        env->variable_capacity = new_capacity;
    }
    
    // Add variable
    env->variable_names[env->variable_count] = (name ? strdup(name) : NULL);
    env->variable_types[env->variable_count] = type_clone(type);
    env->variable_count++;
    
    return 1;
}

MycoType* type_environment_lookup_variable(TypeEnvironment* env, const char* name) {
    if (!env || !name) return NULL;
    
    // Search current environment
    for (size_t i = 0; i < env->variable_count; i++) {
        if (strcmp(env->variable_names[i], name) == 0) {
            return type_clone(env->variable_types[i]);
        }
    }
    
    // Search parent environment
    if (env->parent) {
        return type_environment_lookup_variable(env->parent, name);
    }
    
    return NULL;
}

int type_environment_update_variable(TypeEnvironment* env, const char* name, MycoType* type) {
    if (!env || !name || !type) return 0;
    
    // Search current environment
    for (size_t i = 0; i < env->variable_count; i++) {
        if (strcmp(env->variable_names[i], name) == 0) {
            type_free(env->variable_types[i]);
            env->variable_types[i] = type_clone(type);
            return 1;
        }
    }
    
    // Search parent environment
    if (env->parent) {
        return type_environment_update_variable(env->parent, name, type);
    }
    
    return 0;
}

// Type inference
MycoType* type_infer_expression(TypeCheckerContext* context, ASTNode* node) {
    if (!context || !node) return NULL;
    
    switch (node->type) {
        case AST_NODE_NUMBER:
            return type_infer_literal(node);
        case AST_NODE_STRING:
            return type_infer_literal(node);
        case AST_NODE_BOOL:
            return type_infer_literal(node);
        case AST_NODE_NULL:
            return type_infer_literal(node);
        case AST_NODE_IDENTIFIER:
            return type_environment_lookup_variable(context->current_environment, node->data.identifier_value);
        case AST_NODE_BINARY_OP:
            return type_infer_binary_op(context, node);
        case AST_NODE_UNARY_OP:
            return type_infer_unary_op(context, node);
        case AST_NODE_FUNCTION_CALL:
        case AST_NODE_FUNCTION_CALL_EXPR:
            return type_infer_function_call(context, node);
        case AST_NODE_ARRAY_LITERAL:
            return type_infer_array_literal(context, node);
        case AST_NODE_MEMBER_ACCESS:
            return type_infer_member_access(context, node);
        default:
            return type_create(TYPE_UNKNOWN, node->line, node->column);
    }
}

MycoType* type_infer_literal(ASTNode* node) {
    if (!node) return NULL;
    
    switch (node->type) {
        case AST_NODE_NUMBER:
            // Check if it's an integer or float
            double value = node->data.number_value;
            if (value == (double)((int)value)) {
                return type_create(TYPE_INT, node->line, node->column);
            } else {
                return type_create(TYPE_FLOAT, node->line, node->column);
            }
        case AST_NODE_STRING:
            return type_create(TYPE_STRING, node->line, node->column);
        case AST_NODE_BOOL:
            return type_create(TYPE_BOOL, node->line, node->column);
        case AST_NODE_NULL:
            return type_create(TYPE_NULL, node->line, node->column);
        default:
            return type_create(TYPE_UNKNOWN, node->line, node->column);
    }
}

MycoType* type_infer_binary_op(TypeCheckerContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_BINARY_OP) return NULL;
    
    MycoType* left_type = type_infer_expression(context, node->data.binary.left);
    MycoType* right_type = type_infer_expression(context, node->data.binary.right);
    
    if (!left_type || !right_type) {
        type_free(left_type);
        type_free(right_type);
        return NULL;
    }
    
    MycoType* result_type = NULL;
    
    switch (node->data.binary.op) {
        case OP_ADD:
        case OP_SUBTRACT:
        case OP_MULTIPLY:
        case OP_DIVIDE:
        case OP_MODULO:
        case OP_POWER:
            // Numeric operations
            if (left_type->kind == TYPE_INT && right_type->kind == TYPE_INT) {
                result_type = type_create(TYPE_INT, node->line, node->column);
            } else if ((left_type->kind == TYPE_INT || left_type->kind == TYPE_FLOAT) &&
                      (right_type->kind == TYPE_INT || right_type->kind == TYPE_FLOAT)) {
                result_type = type_create(TYPE_FLOAT, node->line, node->column);
            } else if (left_type->kind == TYPE_STRING || right_type->kind == TYPE_STRING) {
                result_type = type_create(TYPE_STRING, node->line, node->column);
            } else {
                result_type = type_create(TYPE_ERROR, node->line, node->column);
            }
            break;
            
        case OP_EQUAL:
        case OP_NOT_EQUAL:
        case OP_LESS_THAN:
        case OP_LESS_EQUAL:
        case OP_GREATER_THAN:
        case OP_GREATER_EQUAL:
            // Comparison operations
            result_type = type_create(TYPE_BOOL, node->line, node->column);
            break;
            
        case OP_LOGICAL_AND:
        case OP_LOGICAL_OR:
        case OP_LOGICAL_XOR:
            // Logical operations
            if (left_type->kind == TYPE_BOOL && right_type->kind == TYPE_BOOL) {
                result_type = type_create(TYPE_BOOL, node->line, node->column);
            } else {
                result_type = type_create(TYPE_ERROR, node->line, node->column);
            }
            break;
            
        default:
            result_type = type_create(TYPE_UNKNOWN, node->line, node->column);
            break;
    }
    
    type_free(left_type);
    type_free(right_type);
    return result_type;
}

MycoType* type_infer_unary_op(TypeCheckerContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_UNARY_OP) return NULL;
    
    MycoType* operand_type = type_infer_expression(context, node->data.unary.operand);
    if (!operand_type) return NULL;
    
    MycoType* result_type = NULL;
    
    switch (node->data.unary.op) {
        case OP_POSITIVE:
        case OP_NEGATIVE:
            if (operand_type->kind == TYPE_INT || operand_type->kind == TYPE_FLOAT) {
                result_type = type_clone(operand_type);
            } else {
                result_type = type_create(TYPE_ERROR, node->line, node->column);
            }
            break;
            
        case OP_LOGICAL_NOT:
            if (operand_type->kind == TYPE_BOOL) {
                result_type = type_create(TYPE_BOOL, node->line, node->column);
            } else {
                result_type = type_create(TYPE_ERROR, node->line, node->column);
            }
            break;
            
        default:
            result_type = type_create(TYPE_UNKNOWN, node->line, node->column);
            break;
    }
    
    type_free(operand_type);
    return result_type;
}

MycoType* type_infer_function_call(TypeCheckerContext* context, ASTNode* node) {
    if (!context || !node) return NULL;
    
    // Check if this is a class constructor call
    if (node->data.function_call.function_name) {
        const char* func_name = node->data.function_call.function_name;
        
        // If the function name starts with uppercase, it's likely a class constructor
        if (func_name[0] >= 'A' && func_name[0] <= 'Z') {
            return type_create_class(func_name, node->line, node->column);
        }
        
        // Check if this is a method call (contains a dot)
        char* dot_pos = strchr(func_name, '.');
        if (dot_pos) {
            const char* method_name = dot_pos + 1;
            
            // Enhanced method type inference
            if (strcmp(method_name, "speak") == 0 || strcmp(method_name, "name") == 0 || 
                strcmp(method_name, "type") == 0 || strcmp(method_name, "toString") == 0) {
                return type_create(TYPE_STRING, node->line, node->column);
            } else if (strcmp(method_name, "age") == 0 || strcmp(method_name, "length") == 0 ||
                       strcmp(method_name, "size") == 0 || strcmp(method_name, "count") == 0) {
                return type_create(TYPE_INT, node->line, node->column);
            } else if (strcmp(method_name, "active") == 0 || strcmp(method_name, "enabled") == 0 ||
                       strcmp(method_name, "valid") == 0 || strcmp(method_name, "empty") == 0) {
                return type_create(TYPE_BOOL, node->line, node->column);
            } else if (strcmp(method_name, "join") == 0 || strcmp(method_name, "concat") == 0 ||
                       strcmp(method_name, "toString") == 0) {
                return type_create(TYPE_STRING, node->line, node->column);
            } else if (strcmp(method_name, "push") == 0 || strcmp(method_name, "pop") == 0 ||
                       strcmp(method_name, "shift") == 0 || strcmp(method_name, "unshift") == 0) {
                // Array methods that return the element type
                return type_create(TYPE_ANY, node->line, node->column);
            } else if (strcmp(method_name, "slice") == 0 || strcmp(method_name, "filter") == 0 ||
                       strcmp(method_name, "map") == 0 || strcmp(method_name, "unique") == 0) {
                // Array methods that return arrays
                return type_create_array(NULL, node->line, node->column);
            }
        }
        
        // Built-in function type inference
        if (strcmp(func_name, "print") == 0 || strcmp(func_name, "println") == 0) {
            return type_create(TYPE_NULL, node->line, node->column);
        } else if (strcmp(func_name, "len") == 0 || strcmp(func_name, "length") == 0) {
            return type_create(TYPE_INT, node->line, node->column);
        } else if (strcmp(func_name, "str") == 0 || strcmp(func_name, "string") == 0) {
            return type_create(TYPE_STRING, node->line, node->column);
        } else if (strcmp(func_name, "int") == 0) {
            return type_create(TYPE_INT, node->line, node->column);
        } else if (strcmp(func_name, "float") == 0) {
            return type_create(TYPE_FLOAT, node->line, node->column);
        } else if (strcmp(func_name, "bool") == 0) {
            return type_create(TYPE_BOOL, node->line, node->column);
        }
    }
    
    // Check if this is a function call expression (function is a member access)
    if (node->data.function_call_expr.function) {
        // If the function is a member access, handle it specially
        if (node->data.function_call_expr.function->type == AST_NODE_MEMBER_ACCESS) {
            const char* member_name = node->data.function_call_expr.function->data.member_access.member_name;
            
            // Enhanced method type inference for member access
            if (strcmp(member_name, "speak") == 0 || strcmp(member_name, "name") == 0 || 
                strcmp(member_name, "type") == 0 || strcmp(member_name, "toString") == 0) {
                return type_create(TYPE_STRING, node->line, node->column);
            } else if (strcmp(member_name, "age") == 0 || strcmp(member_name, "length") == 0 ||
                       strcmp(member_name, "size") == 0 || strcmp(member_name, "count") == 0) {
                return type_create(TYPE_INT, node->line, node->column);
            } else if (strcmp(member_name, "active") == 0 || strcmp(member_name, "enabled") == 0 ||
                       strcmp(member_name, "valid") == 0 || strcmp(member_name, "empty") == 0) {
                return type_create(TYPE_BOOL, node->line, node->column);
            } else if (strcmp(member_name, "join") == 0 || strcmp(member_name, "concat") == 0) {
                return type_create(TYPE_STRING, node->line, node->column);
            } else if (strcmp(member_name, "push") == 0 || strcmp(member_name, "pop") == 0 ||
                       strcmp(member_name, "shift") == 0 || strcmp(member_name, "unshift") == 0) {
                return type_create(TYPE_ANY, node->line, node->column);
            } else if (strcmp(member_name, "slice") == 0 || strcmp(member_name, "filter") == 0 ||
                       strcmp(member_name, "map") == 0 || strcmp(member_name, "unique") == 0) {
                return type_create_array(NULL, node->line, node->column);
            }
        }
    }
    
    // For other function calls, try to infer from context
    // This could be expanded to look up function signatures in the future
    return type_create(TYPE_ANY, node->line, node->column);
}

// Type compatibility
int type_is_compatible(MycoType* expected, MycoType* actual) {
    if (!expected || !actual) return 0;
    
    // Any type is compatible with TYPE_ANY
    if (expected->kind == TYPE_ANY) return 1;
    
    // Exact match
    if (type_is_equal(expected, actual)) return 1;
    
    // Array compatibility: Array is compatible with any Array[T]
    if (expected->kind == TYPE_ARRAY && actual->kind == TYPE_ARRAY) {
        // If expected has no element type (generic Array), it's compatible with any array
        if (!expected->data.element_type) return 1;
        // If both have element types, check compatibility
        if (actual->data.element_type) {
            return type_is_compatible(expected->data.element_type, actual->data.element_type);
        }
        return 1;
    }
    
    // Numeric compatibility
    if ((expected->kind == TYPE_FLOAT && actual->kind == TYPE_INT) ||
        (expected->kind == TYPE_INT && actual->kind == TYPE_FLOAT)) {
        return 1;
    }
    
    // Union type compatibility: actual type is compatible with union if it matches any member
    if (expected->kind == TYPE_UNION) {
        for (size_t i = 0; i < expected->data.union_type.type_count; i++) {
            if (type_is_compatible(expected->data.union_type.types[i], actual)) {
                return 1;
            }
        }
    }
    
    // Null compatibility: Null is compatible with any optional type
    if (actual->kind == TYPE_NULL && expected->kind == TYPE_OPTIONAL) {
        return 1;
    }
    
    // Optional type compatibility: T is compatible with T?
    if (expected->kind == TYPE_OPTIONAL) {
        return type_is_compatible(expected->data.optional_type, actual);
    }
    
    return 0;
}

int type_is_assignable(MycoType* target, MycoType* source) {
    return type_is_compatible(target, source);
}

// Strict type compatibility (for explicit typing like Rust)
int type_is_strictly_compatible(MycoType* expected, MycoType* actual) {
    if (!expected || !actual) return 0;
    
    // Exact match required for strict typing
    if (type_is_equal(expected, actual)) return 1;
    
    // Null is compatible with optional types
    if (actual->kind == TYPE_NULL && expected->kind == TYPE_OPTIONAL) return 1;
    
    // Union type compatibility: actual type must match exactly one of the union types
    if (expected->kind == TYPE_UNION) {
        for (size_t i = 0; i < expected->data.union_type.type_count; i++) {
            if (type_is_strictly_compatible(expected->data.union_type.types[i], actual)) {
                return 1;
            }
        }
    }
    
    // Optional type compatibility: T is compatible with T?
    if (expected->kind == TYPE_OPTIONAL) {
        return type_is_strictly_compatible(expected->data.optional_type, actual);
    }
    
    // Array compatibility: arrays must have compatible element types
    if (expected->kind == TYPE_ARRAY && actual->kind == TYPE_ARRAY) {
        // If expected has no element type (generic Array), it's compatible with any array
        if (!expected->data.element_type) return 1;
        // If both have element types, check compatibility
        if (!actual->data.element_type) return 0;
        return type_is_strictly_compatible(expected->data.element_type, actual->data.element_type);
    }
    
    // Int and Float are NOT strictly compatible (unlike loose compatibility)
    // This enforces strict typing like Rust
    
    return 0;
}

int type_is_equal(MycoType* type1, MycoType* type2) {
    if (!type1 || !type2) return type1 == type2;
    
    if (type1->kind != type2->kind) return 0;
    
    switch (type1->kind) {
        case TYPE_INT:
        case TYPE_FLOAT:
        case TYPE_STRING:
        case TYPE_BOOL:
        case TYPE_NULL:
        case TYPE_ANY:
        case TYPE_UNKNOWN:
        case TYPE_ERROR:
            return 1;
            
        case TYPE_ARRAY:
            return type_is_equal(type1->data.element_type, type2->data.element_type);
            
        case TYPE_FUNCTION:
            if (type1->data.function_type.parameter_count != type2->data.function_type.parameter_count) {
                return 0;
            }
            for (size_t i = 0; i < type1->data.function_type.parameter_count; i++) {
                if (!type_is_equal(type1->data.function_type.parameter_types[i], 
                                  type2->data.function_type.parameter_types[i])) {
                    return 0;
                }
            }
            return type_is_equal(type1->data.function_type.return_type, 
                                type2->data.function_type.return_type);
            
        case TYPE_CLASS:
            return strcmp(type1->data.class_name, type2->data.class_name) == 0;
            
        case TYPE_UNION:
            if (type1->data.union_type.type_count != type2->data.union_type.type_count) {
                return 0;
            }
            for (size_t i = 0; i < type1->data.union_type.type_count; i++) {
                if (!type_is_equal(type1->data.union_type.types[i], type2->data.union_type.types[i])) {
                    return 0;
                }
            }
            return 1;
            
        case TYPE_OPTIONAL:
            return type_is_equal(type1->data.optional_type, type2->data.optional_type);
            
        case TYPE_GENERIC:
            if (type1->data.generic_type.parameter_count != type2->data.generic_type.parameter_count) {
                return 0;
            }
            // Check parameter names
            for (size_t i = 0; i < type1->data.generic_type.parameter_count; i++) {
                if (strcmp(type1->data.generic_type.parameter_names[i], 
                         type2->data.generic_type.parameter_names[i]) != 0) {
                    return 0;
                }
            }
            // Check constraints
            for (size_t i = 0; i < type1->data.generic_type.parameter_count; i++) {
                if (!type_is_equal(type1->data.generic_type.constraints[i], 
                                  type2->data.generic_type.constraints[i])) {
                    return 0;
                }
            }
            // Check base type
            return type_is_equal(type1->data.generic_type.base_type, type2->data.generic_type.base_type);
            
        case TYPE_GENERIC_PARAMETER:
            return strcmp(type1->data.generic_parameter.parameter_name, 
                         type2->data.generic_parameter.parameter_name) == 0 &&
                   type_is_equal(type1->data.generic_parameter.constraint, 
                                type2->data.generic_parameter.constraint);
            
        default:
            return 0;
    }
}

// Error reporting
void type_checker_add_error(TypeCheckerContext* context, const char* message, int line, int column) {
    if (!context || !message) return;
    
    // Expand error capacity if needed
    if (context->error_count >= context->error_capacity) {
        size_t new_capacity = context->error_capacity == 0 ? 4 : context->error_capacity * 2;
        char** new_messages = shared_realloc_safe(context->error_messages, new_capacity * sizeof(char*), "type_checker", "unknown_function", 614);
        
        if (!new_messages) return;
        
        context->error_messages = new_messages;
        context->error_capacity = new_capacity;
    }
    
    // Format error message with location
    char* error_msg = shared_malloc_safe(strlen(message) + 50, "type_checker", "unknown_function", 623);
    if (error_msg) {
        snprintf(error_msg, strlen(message) + 50, "Type error at line %d, column %d: %s", line, column, message);
        context->error_messages[context->error_count] = error_msg;
        context->error_count++;
    }
}

void type_checker_print_errors(TypeCheckerContext* context) {
    if (!context || context->error_count == 0) return;
    
    printf("Type checking errors:\n");
    for (int i = 0; i < context->error_count; i++) {
        printf("  %s\n", context->error_messages[i]);
    }
}

// Utility functions
const char* type_kind_to_string(MycoTypeKind kind) {
    switch (kind) {
        case TYPE_INT: return "Int";
        case TYPE_FLOAT: return "Float";
        case TYPE_STRING: return "String";
        case TYPE_BOOL: return "Bool";
        case TYPE_NULL: return "Null";
        case TYPE_ARRAY: return "Array";
        case TYPE_HASH_MAP: return "HashMap";
        case TYPE_SET: return "Set";
        case TYPE_FUNCTION: return "Function";
        case TYPE_CLASS: return "Class";
        case TYPE_UNION: return "Union";
        case TYPE_OPTIONAL: return "Optional";
        case TYPE_GENERIC: return "Generic";
        case TYPE_GENERIC_PARAMETER: return "GenericParameter";
        case TYPE_ANY: return "Any";
        case TYPE_UNKNOWN: return "Unknown";
        case TYPE_ERROR: return "Error";
        default: return "Unknown";
    }
}

// Helper function to get type kind string without recursion issues
static const char* type_kind_to_string_simple(MycoType* type) {
    if (!type) return "Unknown";
    
    switch (type->kind) {
        case TYPE_INT: return "Int";
        case TYPE_FLOAT: return "Float";
        case TYPE_STRING: return "String";
        case TYPE_BOOL: return "Bool";
        case TYPE_NULL: return "Null";
        case TYPE_ANY: return "Any";
        case TYPE_UNKNOWN: return "Unknown";
        case TYPE_ERROR: return "Error";
        default: return "Unknown";
    }
}

const char* type_to_string(MycoType* type) {
    if (!type) return "Unknown";
    
    static char buffer[256];
    switch (type->kind) {
        case TYPE_ARRAY:
            if (type->data.element_type) {
                // For arrays, use the simple type name to avoid recursion issues
                const char* element_str = type_kind_to_string_simple(type->data.element_type);
                snprintf(buffer, sizeof(buffer), "[%s]", element_str);
            } else {
                snprintf(buffer, sizeof(buffer), "Array");
            }
            break;
        case TYPE_FUNCTION:
            snprintf(buffer, sizeof(buffer), "Function");
            break;
        case TYPE_CLASS:
            snprintf(buffer, sizeof(buffer), "%s", type->data.class_name);
            break;
        case TYPE_UNION:
            {
                char union_str[512] = "";
                for (size_t i = 0; i < type->data.union_type.type_count; i++) {
                    if (i > 0) strcat(union_str, " | ");
                    strcat(union_str, type_to_string(type->data.union_type.types[i]));
                }
                snprintf(buffer, sizeof(buffer), "%s", union_str);
            }
            break;
        case TYPE_OPTIONAL:
            snprintf(buffer, sizeof(buffer), "%s?", type_to_string(type->data.optional_type));
            break;
        case TYPE_GENERIC:
            {
                char generic_str[512] = {0};
                strcat(generic_str, "<");
                for (size_t i = 0; i < type->data.generic_type.parameter_count; i++) {
                    if (i > 0) strcat(generic_str, ", ");
                    strcat(generic_str, type->data.generic_type.parameter_names[i]);
                }
                strcat(generic_str, ">");
                snprintf(buffer, sizeof(buffer), "Generic%s", generic_str);
            }
            break;
        case TYPE_GENERIC_PARAMETER:
            snprintf(buffer, sizeof(buffer), "%s", type->data.generic_parameter.parameter_name);
            break;
        default:
            snprintf(buffer, sizeof(buffer), "%s", type_kind_to_string(type->kind));
            break;
    }
    
    return buffer;
}

MycoType* type_parse_string(const char* type_string, int line, int column) {
    if (!type_string) return NULL;
    
    // Check for union types (e.g., "String | Int")
    char* pipe_pos = strstr(type_string, " | ");
    if (pipe_pos) {
        // Split the string at the pipe
        size_t left_len = pipe_pos - type_string;
        char* left_type = (char*)shared_malloc_safe(left_len + 1, "core", "unknown_function", 728);
        strncpy(left_type, type_string, left_len);
        left_type[left_len] = '\0';
        
        char* right_type = (pipe_pos + 3 ? strdup(pipe_pos + 3) : NULL); // Skip " | "
        
        // Parse both types
        MycoType* left = type_parse_string(left_type, line, column);
        MycoType* right = type_parse_string(right_type, line, column);
        
        shared_free_safe(left_type, "core", "unknown_function", 738);
        shared_free_safe(right_type, "core", "unknown_function", 739);
        
        if (left && right) {
            // Create union type
            MycoType** types = (MycoType**)shared_malloc_safe(2 * sizeof(MycoType*), "type_checker", "unknown_function", 743);
            types[0] = left;
            types[1] = right;
            return type_create_union(types, 2, line, column);
        } else {
            if (left) type_free(left);
            if (right) type_free(right);
            return NULL;
        }
    }
    
    // Check for array types (e.g., "[Int]", "[String]")
    size_t len = strlen(type_string);
    if (len >= 3 && type_string[0] == '[' && type_string[len - 1] == ']') {
        // Extract the element type from [ElementType]
        char* element_type_string = (char*)shared_malloc_safe(len - 1, "core", "unknown_function", 758);
        strncpy(element_type_string, type_string + 1, len - 2);
        element_type_string[len - 2] = '\0';
        
        MycoType* element_type = type_parse_string(element_type_string, line, column);
        shared_free_safe(element_type_string, "core", "unknown_function", 763);
        
        if (element_type) {
            return type_create_array(element_type, line, column);
        } else {
            return NULL;
        }
    }
    
    // Check for optional types (e.g., "String?")
    if (len > 0 && type_string[len - 1] == '?') {
        // Remove the '?' and parse the wrapped type
        char* wrapped_type_string = (char*)shared_malloc_safe(len, "core", "unknown_function", 775);
        strncpy(wrapped_type_string, type_string, len - 1);
        wrapped_type_string[len - 1] = '\0';
        
        MycoType* wrapped_type = type_parse_string(wrapped_type_string, line, column);
        shared_free_safe(wrapped_type_string, "core", "unknown_function", 780);
        
        if (wrapped_type) {
            return type_create_optional(wrapped_type, line, column);
        } else {
            return NULL;
        }
    }
    
    // Handle single types
    if (strcmp(type_string, "Int") == 0) {
        return type_create(TYPE_INT, line, column);
    } else if (strcmp(type_string, "Float") == 0) {
        return type_create(TYPE_FLOAT, line, column);
    } else if (strcmp(type_string, "String") == 0) {
        return type_create(TYPE_STRING, line, column);
    } else if (strcmp(type_string, "Bool") == 0) {
        return type_create(TYPE_BOOL, line, column);
    } else if (strcmp(type_string, "Null") == 0) {
        return type_create(TYPE_NULL, line, column);
    } else if (strcmp(type_string, "Array") == 0) {
        return type_create(TYPE_ARRAY, line, column);
    } else if (strcmp(type_string, "Any") == 0) {
        return type_create(TYPE_ANY, line, column);
    } else {
        // Assume it's a class type
        return type_create_class(type_string, line, column);
    }
}

// Main type checking functions
int type_check_ast(TypeCheckerContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    // Type check the main program
    return type_check_statement(context, node);
}

int type_check_statement(TypeCheckerContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    switch (node->type) {
        case AST_NODE_VARIABLE_DECLARATION:
            return type_check_variable_declaration(context, node);
        case AST_NODE_ASSIGNMENT:
            return type_check_assignment(context, node);
        case AST_NODE_FUNCTION:
            return type_check_function(context, node);
        case AST_NODE_CLASS:
            return type_check_class(context, node);
        case AST_NODE_BLOCK:
            return type_check_block(context, node);
        default:
            return 1; // Skip unsupported statement types for now
    }
}

int type_check_variable_declaration(TypeCheckerContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_VARIABLE_DECLARATION) return 0;
    
    const char* var_name = node->data.variable_declaration.variable_name;
    const char* declared_type = node->data.variable_declaration.type_name;
    ASTNode* initial_value = node->data.variable_declaration.initial_value;
    
    MycoType* var_type = NULL;
    
    if (declared_type) {
        // Use declared type
        var_type = type_parse_string(declared_type, node->line, node->column);
    } else if (initial_value) {
        // Infer type from initial value
        var_type = type_infer_expression(context, initial_value);
    } else {
        // No type information available - use Any type
        var_type = type_create(TYPE_ANY, node->line, node->column);
    }
    
    if (!var_type) {
        type_checker_add_error(context, "Failed to determine variable type", node->line, node->column);
        return 0;
    }
    
    // Add variable to environment
    if (!type_environment_add_variable(context->current_environment, var_name, var_type)) {
        type_checker_add_error(context, "Variable already declared in this scope", node->line, node->column);
        type_free(var_type);
        return 0;
    }
    
    // If there's an initial value, check type compatibility
    if (initial_value && declared_type) {
        MycoType* inferred_type = type_infer_expression(context, initial_value);
        if (inferred_type) {
            // For explicitly typed variables, use strict type checking
            if (!type_is_strictly_compatible(var_type, inferred_type)) {
                char error_msg[256];
                const char* expected = type_to_string(var_type);
                const char* actual = type_to_string(inferred_type);
                snprintf(error_msg, sizeof(error_msg), 
                    "Type mismatch: expected '%s', got '%s'", expected, actual);
                type_checker_add_error(context, error_msg, node->line, node->column);
            type_free(inferred_type);
            type_free(var_type);
            return 0;
        }
        type_free(inferred_type);
        } else {
            type_checker_add_error(context, "Failed to infer type of initial value", node->line, node->column);
            type_free(var_type);
            return 0;
        }
    }
    
    type_free(var_type);
    return 1;
}

int type_check_assignment(TypeCheckerContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_ASSIGNMENT) return 0;
    
    const char* var_name = node->data.assignment.variable_name;
    ASTNode* value = node->data.assignment.value;
    
    // Look up variable type
    MycoType* var_type = type_environment_lookup_variable(context->current_environment, var_name);
    if (!var_type) {
        type_checker_add_error(context, "Undefined variable", node->line, node->column);
        return 0;
    }
    
    // Infer value type
    MycoType* value_type = type_infer_expression(context, value);
    if (!value_type) {
        type_checker_add_error(context, "Failed to infer value type", node->line, node->column);
        type_free(var_type);
        return 0;
    }
    
    // Check compatibility
    if (!type_is_assignable(var_type, value_type)) {
        type_checker_add_error(context, "Type mismatch in assignment", node->line, node->column);
        type_free(var_type);
        type_free(value_type);
        return 0;
    }
    
    type_free(var_type);
    type_free(value_type);
    return 1;
}

int type_check_function(TypeCheckerContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_FUNCTION) return 0;
    
    // Create new environment for function parameters
    TypeEnvironment* old_env = context->current_environment;
    context->current_environment = type_environment_create(old_env);
    
    // Add parameters to environment
    for (size_t i = 0; i < node->data.function_definition.parameter_count; i++) {
        ASTNode* param = node->data.function_definition.parameters[i];
        if (param->type == AST_NODE_TYPED_PARAMETER) {
            MycoType* param_type = type_parse_string(param->data.typed_parameter.parameter_type, 
                                                    param->line, param->column);
            if (param_type) {
                type_environment_add_variable(context->current_environment, 
                                            param->data.typed_parameter.parameter_name, 
                                            param_type);
                type_free(param_type);
            }
        }
    }
    
    // Type check function body
    int result = type_check_statement(context, node->data.function_definition.body);
    
    // Restore environment
    type_environment_free(context->current_environment);
    context->current_environment = old_env;
    
    return result;
}

int type_check_class(TypeCheckerContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_CLASS) return 0;
    
    // Create a new environment scope for the class
    TypeEnvironment* class_env = type_environment_create(context->current_environment);
    if (!class_env) return 0;
    
    // Set the current environment to the class environment
    TypeEnvironment* old_env = context->current_environment;
    context->current_environment = class_env;
    
    // Type check the class body
    int result = 1;
    if (node->data.class_definition.body) {
        if (node->data.class_definition.body->type == AST_NODE_BLOCK) {
            // If it's a block, type check each statement in the block
            for (size_t i = 0; i < node->data.class_definition.body->data.block.statement_count; i++) {
                ASTNode* statement = node->data.class_definition.body->data.block.statements[i];
                
                // Handle field declarations specially - don't add them to environment
                if (statement->type == AST_NODE_VARIABLE_DECLARATION) {
                    // Type check the field declaration but don't add to environment
                    const char* var_name = statement->data.variable_declaration.variable_name;
                    const char* declared_type = statement->data.variable_declaration.type_name;
                    ASTNode* initial_value = statement->data.variable_declaration.initial_value;
                    
                    MycoType* var_type = NULL;
                    if (declared_type) {
                        var_type = type_parse_string(declared_type, statement->line, statement->column);
                    } else if (initial_value) {
                        var_type = type_infer_expression(context, initial_value);
                    } else {
                        var_type = type_create(TYPE_ANY, statement->line, statement->column);
                    }
                    
                    if (!var_type) {
                        result = 0;
                        break;
                    }
                    
                    // Don't add to environment - just validate the type
                    type_free(var_type);
                } else {
                    // For non-field declarations, use normal type checking
                    if (!type_check_statement(context, statement)) {
                        result = 0;
                        break;
                    }
                }
            }
        } else {
            // If it's a single statement, type check it directly
            result = type_check_statement(context, node->data.class_definition.body);
        }
    }
    
    // Restore the old environment
    context->current_environment = old_env;
    
    // Free the class environment
    type_environment_free(class_env);
    
    return result;
}

int type_check_block(TypeCheckerContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_BLOCK) return 0;
    
    // Type check each statement in the block
    for (size_t i = 0; i < node->data.block.statement_count; i++) {
        if (!type_check_statement(context, node->data.block.statements[i])) {
            return 0;
        }
    }
    
    return 1;
}

// Additional helper functions that were referenced but not implemented
MycoType* type_infer_array_literal(TypeCheckerContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_ARRAY_LITERAL) return NULL;
    
    if (node->data.array_literal.element_count == 0) {
        // Empty array - return generic Array type
        return type_create_array(NULL, node->line, node->column);
    }
    
    // Infer element type from first element
    MycoType* element_type = type_infer_expression(context, node->data.array_literal.elements[0]);
    if (!element_type) {
        return type_create_array(NULL, node->line, node->column);
    }
    
    // Check if all elements have compatible types
    for (size_t i = 1; i < node->data.array_literal.element_count; i++) {
        MycoType* current_type = type_infer_expression(context, node->data.array_literal.elements[i]);
        if (!current_type || !type_is_compatible(element_type, current_type)) {
            // Mixed types - use generic Array
            type_free(element_type);
            type_free(current_type);
            return type_create_array(NULL, node->line, node->column);
        }
        type_free(current_type);
    }
    
    return type_create_array(element_type, node->line, node->column);
}

MycoType* type_infer_member_access(TypeCheckerContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_MEMBER_ACCESS) return NULL;
    
    // Get the type of the object being accessed
    MycoType* object_type = type_infer_expression(context, node->data.member_access.object);
    if (!object_type) {
    return type_create(TYPE_UNKNOWN, node->line, node->column);
    }
    
    const char* member_name = node->data.member_access.member_name;
    if (!member_name) {
        type_free(object_type);
        return type_create(TYPE_UNKNOWN, node->line, node->column);
    }
    
    // Enhanced member access type inference based on object type
    if (object_type->kind == TYPE_ARRAY) {
        if (strcmp(member_name, "length") == 0 || strcmp(member_name, "size") == 0) {
            type_free(object_type);
            return type_create(TYPE_INT, node->line, node->column);
        } else if (strcmp(member_name, "join") == 0 || strcmp(member_name, "toString") == 0) {
            type_free(object_type);
            return type_create(TYPE_STRING, node->line, node->column);
        } else if (strcmp(member_name, "push") == 0 || strcmp(member_name, "pop") == 0 ||
                   strcmp(member_name, "shift") == 0 || strcmp(member_name, "unshift") == 0) {
            type_free(object_type);
            return type_create(TYPE_ANY, node->line, node->column);
        } else if (strcmp(member_name, "slice") == 0 || strcmp(member_name, "filter") == 0 ||
                   strcmp(member_name, "map") == 0 || strcmp(member_name, "unique") == 0) {
            type_free(object_type);
            return type_create_array(NULL, node->line, node->column);
        }
    } else if (object_type->kind == TYPE_STRING) {
        if (strcmp(member_name, "length") == 0 || strcmp(member_name, "size") == 0) {
            type_free(object_type);
            return type_create(TYPE_INT, node->line, node->column);
        } else if (strcmp(member_name, "toUpperCase") == 0 || strcmp(member_name, "toLowerCase") == 0 ||
                   strcmp(member_name, "trim") == 0 || strcmp(member_name, "toString") == 0) {
            type_free(object_type);
            return type_create(TYPE_STRING, node->line, node->column);
        }
    } else if (object_type->kind == TYPE_CLASS) {
        // For class objects, try to infer based on common property names
        if (strcmp(member_name, "name") == 0 || strcmp(member_name, "toString") == 0) {
            type_free(object_type);
            return type_create(TYPE_STRING, node->line, node->column);
        } else if (strcmp(member_name, "age") == 0 || strcmp(member_name, "id") == 0) {
            type_free(object_type);
            return type_create(TYPE_INT, node->line, node->column);
        } else if (strcmp(member_name, "active") == 0 || strcmp(member_name, "enabled") == 0) {
            type_free(object_type);
            return type_create(TYPE_BOOL, node->line, node->column);
        }
    }
    
    // For other member access, return TYPE_ANY for better inference
    type_free(object_type);
    return type_create(TYPE_ANY, node->line, node->column);
}

// Generic type support functions

/**
 * @brief Create a generic type with type parameters
 */
MycoType* type_create_generic(const char** parameter_names, size_t parameter_count,
                              MycoType** constraints, MycoType* base_type, int line, int column) {
    MycoType* type = type_create(TYPE_GENERIC, line, column);
    if (!type) return NULL;
    
    type->data.generic_type.parameter_count = parameter_count;
    type->data.generic_type.base_type = base_type;
    
    if (parameter_count > 0) {
        // Allocate parameter names
        type->data.generic_type.parameter_names = shared_malloc_safe(parameter_count * sizeof(char*), "type_checker", "unknown_function", 1225);
        if (!type->data.generic_type.parameter_names) {
            type_free(type);
            return NULL;
        }
        
        // Copy parameter names
        for (size_t i = 0; i < parameter_count; i++) {
            type->data.generic_type.parameter_names[i] = (parameter_names[i] ? strdup(parameter_names[i]) : NULL);
        }
        
        // Allocate constraints if provided
        if (constraints) {
            type->data.generic_type.constraints = shared_malloc_safe(parameter_count * sizeof(MycoType*), "type_checker", "unknown_function", 1235);
            if (!type->data.generic_type.constraints) {
                type_free(type);
                return NULL;
            }
            
            // Copy constraints
            for (size_t i = 0; i < parameter_count; i++) {
                type->data.generic_type.constraints[i] = constraints[i] ? type_clone(constraints[i]) : NULL;
            }
        } else {
            type->data.generic_type.constraints = NULL;
        }
    } else {
        type->data.generic_type.parameter_names = NULL;
        type->data.generic_type.constraints = NULL;
    }
    
    return type;
}

/**
 * @brief Create a generic parameter type
 */
MycoType* type_create_generic_parameter(const char* parameter_name, MycoType* constraint, int line, int column) {
    MycoType* type = type_create(TYPE_GENERIC_PARAMETER, line, column);
    if (!type) return NULL;
    
    type->data.generic_parameter.parameter_name = (parameter_name ? strdup(parameter_name) : NULL);
    type->data.generic_parameter.constraint = constraint ? type_clone(constraint) : NULL;
    
    return type;
}

/**
 * @brief Instantiate a generic type with concrete type arguments
 */
MycoType* type_instantiate_generic(MycoType* generic_type, MycoType** type_arguments, int line, int column) {
    if (!generic_type || generic_type->kind != TYPE_GENERIC) return NULL;
    
    // Create a copy of the base type
    MycoType* instantiated = type_clone(generic_type->data.generic_type.base_type);
    if (!instantiated) return NULL;
    
    // Replace generic parameters with concrete types
    // This is a simplified implementation - in a full implementation,
    // you would need to traverse the type tree and replace all generic parameters
    return instantiated;
}

/**
 * @brief Check if a type satisfies generic constraints
 */
int type_satisfies_constraints(MycoType* type, MycoType* constraint) {
    if (!type || !constraint) return 0;
    
    // If constraint is TYPE_ANY, any type satisfies it
    if (constraint->kind == TYPE_ANY) return 1;
    
    // Check compatibility
    return type_is_compatible(constraint, type);
}

/**
 * @brief Find common type between two types (for generic inference)
 */
MycoType* type_find_common_type(MycoType* type1, MycoType* type2) {
    if (!type1 || !type2) return NULL;
    
    // If types are equal, return a copy
    if (type_is_equal(type1, type2)) {
        return type_clone(type1);
    }
    
    // If one is TYPE_ANY, return the other
    if (type1->kind == TYPE_ANY) {
        return type_clone(type2);
    }
    if (type2->kind == TYPE_ANY) {
        return type_clone(type1);
    }
    
    // For numeric types, promote to float
    if ((type1->kind == TYPE_INT || type1->kind == TYPE_FLOAT) &&
        (type2->kind == TYPE_INT || type2->kind == TYPE_FLOAT)) {
        return type_create(TYPE_FLOAT, type1->line, type1->column);
    }
    
    // For arrays, try to find common element type
    if (type1->kind == TYPE_ARRAY && type2->kind == TYPE_ARRAY) {
        MycoType* common_element = type_find_common_type(type1->data.element_type, type2->data.element_type);
        if (common_element) {
            return type_create_array(common_element, type1->line, type1->column);
        }
    }
    
    // If no common type found, return TYPE_ANY
    return type_create(TYPE_ANY, type1->line, type1->column);
}

// Enhanced Type Inference Engine Implementation

TypeInferenceEngine* type_inference_engine_create(void) {
    TypeInferenceEngine* engine = shared_malloc_safe(sizeof(TypeInferenceEngine), "type_checker", "unknown_function", 1416);
    if (!engine) return NULL;
    
    engine->inferences = NULL;
    engine->inference_count = 0;
    engine->inference_capacity = 0;
    engine->enable_confidence_scoring = 1;
    engine->enable_constraint_solving = 1;
    engine->enable_ambiguity_detection = 1;
    
    return engine;
}

void type_inference_engine_free(TypeInferenceEngine* engine) {
    if (!engine) return;
    
    for (size_t i = 0; i < engine->inference_count; i++) {
        type_inference_free(engine->inferences[i]);
    }
    shared_free_safe(engine->inferences, "type_checker", "unknown_function", 1430);
    shared_free_safe(engine, "type_checker", "unknown_function", 1431);
}

TypeInference* type_inference_create(MycoType* type, int confidence, int is_ambiguous) {
    TypeInference* inference = shared_malloc_safe(sizeof(TypeInference), "type_checker", "unknown_function", 1435);
    if (!inference) return NULL;
    
    inference->type = type;
    inference->confidence = confidence;
    inference->is_ambiguous = is_ambiguous;
    inference->constraints = NULL;
    inference->constraint_count = 0;
    inference->is_inferred = 1;
    
    return inference;
}

void type_inference_free(TypeInference* inference) {
    if (!inference) return;
    
    // Free constraints
    for (size_t i = 0; i < inference->constraint_count; i++) {
        shared_free_safe(inference->constraints[i].constraint_name, "type_checker", "unknown_function", 1450);
        type_free(inference->constraints[i].constraint_type);
    }
    shared_free_safe(inference->constraints, "type_checker", "unknown_function", 1453);
    
    // Free the type (but don't free if it's a reference)
    if (inference->is_inferred) {
        type_free(inference->type);
    }
    
    shared_free_safe(inference, "type_checker", "unknown_function", 1458);
}

TypeInference* type_infer_expression_enhanced(TypeCheckerContext* context, ASTNode* node) {
    if (!context || !node) return NULL;
    
    // Get basic type inference
    MycoType* basic_type = type_infer_expression(context, node);
    if (!basic_type) return NULL;
    
    // Calculate confidence based on context
    int confidence = 100; // Start with high confidence
    
    // Reduce confidence for ambiguous cases
    if (node->type == AST_NODE_BINARY_OP) {
        // Binary operations might be ambiguous
        if (node->data.binary.op == OP_ADD) {
            confidence = 85; // String concatenation vs arithmetic
        } else if (node->data.binary.op == OP_EQUAL) {
            confidence = 95; // Equality is usually clear
        }
    } else if (node->type == AST_NODE_FUNCTION_CALL) {
        confidence = 90; // Function calls have some uncertainty
    } else if (node->type == AST_NODE_NUMBER || node->type == AST_NODE_STRING || 
               node->type == AST_NODE_BOOL || node->type == AST_NODE_NULL) {
        confidence = 100; // Literals are always certain
    }
    
    // Check for ambiguity
    int is_ambiguous = 0;
    if (node->type == AST_NODE_BINARY_OP && node->data.binary.op == OP_ADD) {
        // Check if both operands could be strings or numbers
        MycoType* left_type = type_infer_expression(context, node->data.binary.left);
        MycoType* right_type = type_infer_expression(context, node->data.binary.right);
        
        if ((left_type && (left_type->kind == TYPE_STRING || left_type->kind == TYPE_ANY)) &&
            (right_type && (right_type->kind == TYPE_STRING || right_type->kind == TYPE_ANY))) {
            is_ambiguous = 1;
            confidence = 70; // Lower confidence for ambiguous cases
        }
    }
    
    return type_inference_create(basic_type, confidence, is_ambiguous);
}

int type_infer_with_confidence(TypeCheckerContext* context, ASTNode* node, int* confidence) {
    if (!context || !node || !confidence) return 0;
    
    TypeInference* inference = type_infer_expression_enhanced(context, node);
    if (!inference) return 0;
    
    *confidence = inference->confidence;
    type_inference_free(inference);
    return 1;
}

int type_infer_with_constraints(TypeCheckerContext* context, ASTNode* node, TypeConstraint* constraints, size_t constraint_count) {
    if (!context || !node) return 0;
    
    // For now, basic implementation - just check if the inferred type matches constraints
    MycoType* inferred_type = type_infer_expression(context, node);
    if (!inferred_type) return 0;
    
    // Check constraints
    for (size_t i = 0; i < constraint_count; i++) {
        if (constraints[i].is_required && !type_is_compatible(constraints[i].constraint_type, inferred_type)) {
            type_free(inferred_type);
            return 0;
        }
    }
    
    type_free(inferred_type);
    return 1;
}

int type_infer_ambiguous_types(TypeCheckerContext* context, ASTNode* node, MycoType** possible_types, size_t* type_count) {
    if (!context || !node || !possible_types || !type_count) return 0;
    
    *type_count = 0;
    
    // For binary operations, check both possible interpretations
    if (node->type == AST_NODE_BINARY_OP && node->data.binary.op == OP_ADD) {
        MycoType* left_type = type_infer_expression(context, node->data.binary.left);
        MycoType* right_type = type_infer_expression(context, node->data.binary.right);
        
        if (left_type && right_type) {
            // String concatenation
            if (left_type->kind == TYPE_STRING || right_type->kind == TYPE_STRING) {
                possible_types[0] = type_create(TYPE_STRING, node->line, node->column);
                *type_count = 1;
            }
            
            // Arithmetic addition
            if ((left_type->kind == TYPE_INT || left_type->kind == TYPE_FLOAT) &&
                (right_type->kind == TYPE_INT || right_type->kind == TYPE_FLOAT)) {
                MycoType* arithmetic_type = type_find_common_type(left_type, right_type);
                if (arithmetic_type) {
                    if (*type_count == 0) {
                        possible_types[0] = arithmetic_type;
                        *type_count = 1;
                    } else {
                        possible_types[1] = arithmetic_type;
                        *type_count = 2;
                    }
                }
            }
        }
        
        type_free(left_type);
        type_free(right_type);
    }
    
    return *type_count > 0;
}

// Generic Type System Implementation

MycoType* generic_instantiate(MycoType* generic_type, MycoType** type_args, size_t arg_count) {
    if (!generic_type || generic_type->kind != TYPE_GENERIC) return NULL;
    
    // Create a new type instance by substituting type parameters
    MycoType* instance = type_clone(generic_type->data.generic_type.base_type);
    if (!instance) return NULL;
    
    // For now, basic implementation - in a full implementation, we would
    // recursively substitute type parameters throughout the type structure
    return instance;
}

int generic_constraint_check(MycoType* type, TypeConstraint* constraint) {
    if (!type || !constraint) return 0;
    
    // Check if the type satisfies the constraint
    if (constraint->constraint_type) {
        return type_is_compatible(constraint->constraint_type, type);
    }
    
    // If no specific constraint type, check by name
    if (constraint->constraint_name) {
        // For now, basic name matching - in a full implementation,
        // we would check against a constraint registry
        return 1; // Placeholder
    }
    
    return 1;
}

MycoType* generic_create_parameter(const char* name, MycoType* constraint) {
    if (!name) return NULL;
    
    MycoType* param = type_create(TYPE_GENERIC_PARAMETER, 0, 0);
    if (!param) return NULL;
    
    param->data.generic_parameter.parameter_name = (name ? strdup(name) : NULL);
    param->data.generic_parameter.constraint = constraint;
    
    return param;
}

MycoType* generic_create_instance(const char* name, MycoType** parameters, size_t param_count) {
    if (!name) return NULL;
    
    MycoType* instance = type_create(TYPE_GENERIC, 0, 0);
    if (!instance) return NULL;
    
    // Store the generic instance information
    instance->data.generic_type.base_type = type_create(TYPE_ANY, 0, 0);
    instance->data.generic_type.parameter_count = param_count;
    instance->data.generic_type.parameter_names = NULL;
    instance->data.generic_type.constraints = NULL;
    
    if (param_count > 0 && parameters) {
        instance->data.generic_type.parameter_names = shared_malloc_safe(sizeof(char*) * param_count, "type_checker", "unknown_function", 1600);
        instance->data.generic_type.constraints = shared_malloc_safe(sizeof(MycoType*) * param_count, "type_checker", "unknown_function", 1601);
        
        if (instance->data.generic_type.parameter_names && instance->data.generic_type.constraints) {
            for (size_t i = 0; i < param_count; i++) {
                instance->data.generic_type.parameter_names[i] = ("T" ? strdup("T") : NULL); // Placeholder
                instance->data.generic_type.constraints[i] = parameters[i];
            }
        }
    }
    
    return instance;
}

int generic_is_assignable(MycoType* generic_type, MycoType* concrete_type) {
    if (!generic_type || !concrete_type) return 0;
    
    // Check if the concrete type can be assigned to the generic type
    if (generic_type->kind == TYPE_GENERIC) {
        // For now, basic compatibility check
        return type_is_compatible(generic_type->data.generic_type.base_type, concrete_type);
    }
    
    return type_is_compatible(generic_type, concrete_type);
}

int generic_constraint_satisfies(MycoType* type, MycoType* constraint) {
    if (!type || !constraint) return 0;
    
    // Check if the type satisfies the constraint
    if (constraint->kind == TYPE_GENERIC_PARAMETER) {
        // Check against parameter constraints
        if (constraint->data.generic_parameter.constraint) {
            return type_is_compatible(constraint->data.generic_parameter.constraint, type);
        }
    }
    
    return type_is_compatible(constraint, type);
}
