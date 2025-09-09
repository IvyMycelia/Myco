#include "type_checker.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Type checker context management
TypeCheckerContext* type_checker_create_context(void) {
    TypeCheckerContext* context = malloc(sizeof(TypeCheckerContext));
    if (!context) return NULL;
    
    context->current_environment = type_environment_create(NULL);
    context->function_return_types = NULL;
    context->function_names = NULL;
    context->function_count = 0;
    context->function_capacity = 0;
    context->error_count = 0;
    context->error_messages = NULL;
    context->error_capacity = 0;
    
    return context;
}

void type_checker_free_context(TypeCheckerContext* context) {
    if (!context) return;
    
    type_environment_free(context->current_environment);
    
    // Free function types
    for (size_t i = 0; i < context->function_count; i++) {
        type_free(context->function_return_types[i]);
        free(context->function_names[i]);
    }
    free(context->function_return_types);
    free(context->function_names);
    
    // Free error messages
    for (int i = 0; i < context->error_count; i++) {
        free(context->error_messages[i]);
    }
    free(context->error_messages);
    
    free(context);
}

// Type creation and management
MycoType* type_create(MycoTypeKind kind, int line, int column) {
    MycoType* type = malloc(sizeof(MycoType));
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
        type->data.class_name = strdup(class_name);
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
            clone->data.function_type.parameter_types = malloc(sizeof(MycoType*) * type->data.function_type.parameter_count);
            if (clone->data.function_type.parameter_types) {
                for (size_t i = 0; i < type->data.function_type.parameter_count; i++) {
                    clone->data.function_type.parameter_types[i] = type_clone(type->data.function_type.parameter_types[i]);
                }
            }
            clone->data.function_type.return_type = type_clone(type->data.function_type.return_type);
            break;
        case TYPE_CLASS:
            clone->data.class_name = strdup(type->data.class_name);
            break;
        case TYPE_UNION:
            clone->data.union_type.type_count = type->data.union_type.type_count;
            clone->data.union_type.types = malloc(sizeof(MycoType*) * type->data.union_type.type_count);
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
            free(type->data.function_type.parameter_types);
            type_free(type->data.function_type.return_type);
            break;
        case TYPE_CLASS:
            free(type->data.class_name);
            break;
        case TYPE_UNION:
            for (size_t i = 0; i < type->data.union_type.type_count; i++) {
                type_free(type->data.union_type.types[i]);
            }
            free(type->data.union_type.types);
            break;
        case TYPE_OPTIONAL:
            type_free(type->data.optional_type);
            break;
        default:
            break;
    }
    
    free(type);
}

// Type environment management
TypeEnvironment* type_environment_create(TypeEnvironment* parent) {
    TypeEnvironment* env = malloc(sizeof(TypeEnvironment));
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
        free(env->variable_names[i]);
        type_free(env->variable_types[i]);
    }
    free(env->variable_names);
    free(env->variable_types);
    free(env);
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
        char** new_names = realloc(env->variable_names, new_capacity * sizeof(char*));
        MycoType** new_types = realloc(env->variable_types, new_capacity * sizeof(MycoType*));
        
        if (!new_names || !new_types) {
            free(new_names);
            free(new_types);
            return 0;
        }
        
        env->variable_names = new_names;
        env->variable_types = new_types;
        env->variable_capacity = new_capacity;
    }
    
    // Add variable
    env->variable_names[env->variable_count] = strdup(name);
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
    
    // For now, return TYPE_UNKNOWN for function calls
    // This would need to be expanded to look up function signatures
    return type_create(TYPE_UNKNOWN, node->line, node->column);
}

// Type compatibility
int type_is_compatible(MycoType* expected, MycoType* actual) {
    if (!expected || !actual) return 0;
    
    // Any type is compatible with TYPE_ANY
    if (expected->kind == TYPE_ANY) return 1;
    
    // Exact match
    if (type_is_equal(expected, actual)) return 1;
    
    // Numeric compatibility
    if ((expected->kind == TYPE_FLOAT && actual->kind == TYPE_INT) ||
        (expected->kind == TYPE_INT && actual->kind == TYPE_INT)) {
        return 1;
    }
    
    return 0;
}

int type_is_assignable(MycoType* target, MycoType* source) {
    return type_is_compatible(target, source);
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
        char** new_messages = realloc(context->error_messages, new_capacity * sizeof(char*));
        
        if (!new_messages) return;
        
        context->error_messages = new_messages;
        context->error_capacity = new_capacity;
    }
    
    // Format error message with location
    char* error_msg = malloc(strlen(message) + 50);
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
            snprintf(buffer, sizeof(buffer), "[%s]", type_to_string(type->data.element_type));
            break;
        case TYPE_FUNCTION:
            snprintf(buffer, sizeof(buffer), "Function");
            break;
        case TYPE_CLASS:
            snprintf(buffer, sizeof(buffer), "%s", type->data.class_name);
            break;
        default:
            snprintf(buffer, sizeof(buffer), "%s", type_kind_to_string(type->kind));
            break;
    }
    
    return buffer;
}

MycoType* type_parse_string(const char* type_string, int line, int column) {
    if (!type_string) return NULL;
    
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
        if (inferred_type && !type_is_compatible(var_type, inferred_type)) {
            type_checker_add_error(context, "Type mismatch in variable initialization", node->line, node->column);
            type_free(inferred_type);
            type_free(var_type);
            return 0;
        }
        type_free(inferred_type);
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
    
    // For now, just type check the class body
    return type_check_statement(context, node->data.class_definition.body);
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
        return type_create_array(type_create(TYPE_ANY, node->line, node->column), node->line, node->column);
    }
    
    // Infer type from first element
    MycoType* element_type = type_infer_expression(context, node->data.array_literal.elements[0]);
    if (!element_type) return NULL;
    
    return type_create_array(element_type, node->line, node->column);
}

MycoType* type_infer_member_access(TypeCheckerContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_MEMBER_ACCESS) return NULL;
    
    // For now, return TYPE_UNKNOWN for member access
    // This would need to be expanded to look up class definitions
    return type_create(TYPE_UNKNOWN, node->line, node->column);
}
