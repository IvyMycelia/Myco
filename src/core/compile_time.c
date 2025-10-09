#include "compile_time.h"
#include "interpreter.h"
#include "environment.h"
#include "shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * @brief Create a new compile-time evaluator
 */
CompileTimeEvaluator* compile_time_evaluator_create(void) {
    CompileTimeEvaluator* evaluator = shared_malloc_safe(sizeof(CompileTimeEvaluator), 
        "compile_time", "compile_time_evaluator_create", 0);
    if (!evaluator) return NULL;
    
    evaluator->values = NULL;
    evaluator->count = 0;
    evaluator->capacity = 0;
    evaluator->compile_env = environment_create(NULL);
    evaluator->optimization_level = 2; // Default optimization level
    
    return evaluator;
}

/**
 * @brief Free a compile-time evaluator
 */
void compile_time_evaluator_free(CompileTimeEvaluator* evaluator) {
    if (!evaluator) return;
    
    // Free all compile-time values
    for (size_t i = 0; i < evaluator->count; i++) {
        compile_time_value_free(evaluator->values[i]);
    }
    
    if (evaluator->values) {
        shared_free_safe(evaluator->values, "compile_time", "compile_time_evaluator_free", 0);
    }
    
    if (evaluator->compile_env) {
        environment_free(evaluator->compile_env);
    }
    
    shared_free_safe(evaluator, "compile_time", "compile_time_evaluator_free", 0);
}

/**
 * @brief Evaluate an expression at compile time
 */
Value compile_time_eval(CompileTimeEvaluator* evaluator, struct Interpreter* interpreter, 
                       ASTNode* expression) {
    // For now, just return null - this will be implemented later
    // when the type system issues are resolved
    return value_create_null();
}

/**
 * @brief Check if an expression is a compile-time constant
 */
int is_compile_time_constant(ASTNode* expression) {
    if (!expression) return 0;
    
    switch (expression->type) {
        case AST_NODE_NUMBER:
        case AST_NODE_STRING:
        case AST_NODE_BOOL:
            return 1;
        default:
            return 0;
    }
}

/**
 * @brief Check if an expression is pure (no side effects)
 */
int is_pure_expression(ASTNode* expression) {
    if (!expression) return 0;
    
    switch (expression->type) {
        case AST_NODE_NUMBER:
        case AST_NODE_STRING:
        case AST_NODE_BOOL:
        case AST_NODE_BINARY_OP:
        case AST_NODE_UNARY_OP:
            return 1;
        default:
            return 0;
    }
}

/**
 * @brief Evaluate a pure function at compile time
 */
Value evaluate_pure_function(CompileTimeEvaluator* evaluator, struct Interpreter* interpreter, 
                            const char* function_name, ASTNode** arguments, size_t arg_count) {
    // For now, just return null - this will be implemented later
    return value_create_null();
}

/**
 * @brief Evaluate arithmetic expressions at compile time
 */
Value eval_arithmetic_compile_time(CompileTimeEvaluator* evaluator, struct Interpreter* interpreter,
                                ASTNode* expression) {
    // For now, just return null - this will be implemented later
    return value_create_null();
}

/**
 * @brief Evaluate string concatenation at compile time
 */
Value eval_string_concat_compile_time(CompileTimeEvaluator* evaluator, struct Interpreter* interpreter,
                                     ASTNode* expression) {
    // For now, just return null - this will be implemented later
    return value_create_null();
}

/**
 * @brief Evaluate boolean expressions at compile time
 */
Value eval_boolean_compile_time(CompileTimeEvaluator* evaluator, struct Interpreter* interpreter,
                               ASTNode* expression) {
    // For now, just return null - this will be implemented later
    return value_create_null();
}

/**
 * @brief Check if a value is a compile-time constant
 */
int value_is_compile_time_constant(struct Value* value) {
    if (!value) return 0;
    
    // For now, just return 0 - this will be implemented later
    return 0;
}

/**
 * @brief Free a compile-time value
 */
void compile_time_value_free(CompileTimeValue* value) {
    if (!value) return;
    
    if (value->dependencies) {
        for (size_t i = 0; i < value->dep_count; i++) {
            shared_free_safe(value->dependencies[i], "compile_time", "compile_time_value_free", 0);
        }
        shared_free_safe(value->dependencies, "compile_time", "compile_time_value_free", 0);
    }
    
    if (value->result) {
        value_free((Value*)value->result);
        shared_free_safe(value->result, "compile_time", "compile_time_value_free", 0);
    }
    shared_free_safe(value, "compile_time", "compile_time_value_free", 0);
}

/**
 * @brief Clone a compile-time value
 */
CompileTimeValue* compile_time_value_clone(CompileTimeValue* value) {
    if (!value) return NULL;
    
    CompileTimeValue* clone = shared_malloc_safe(sizeof(CompileTimeValue), 
        "compile_time", "compile_time_value_clone", 0);
    if (!clone) return NULL;
    
    clone->expression = value->expression; // Don't clone the expression
    clone->result = NULL; // For now, just set to NULL
    clone->is_constant = value->is_constant;
    clone->is_pure = value->is_pure;
    clone->is_evaluated = value->is_evaluated;
    clone->dep_count = value->dep_count;
    clone->dependencies = NULL; // For now, just set to NULL
    
    return clone;
}

// ============================================================================
// COMPILE-TIME UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Get the size of a type at compile time
 */
int comptime_sizeof(const char* type_name) {
    if (!type_name) return -1;
    
    // Basic type sizes (simplified)
    if (strcmp(type_name, "int") == 0 || strcmp(type_name, "Int") == 0) return 4;
    if (strcmp(type_name, "float") == 0 || strcmp(type_name, "Float") == 0) return 4;
    if (strcmp(type_name, "double") == 0 || strcmp(type_name, "Double") == 0) return 8;
    if (strcmp(type_name, "bool") == 0 || strcmp(type_name, "Bool") == 0) return 1;
    if (strcmp(type_name, "char") == 0 || strcmp(type_name, "Char") == 0) return 1;
    if (strcmp(type_name, "string") == 0 || strcmp(type_name, "String") == 0) return 8; // pointer size
    
    return -1; // Unknown type
}

/**
 * @brief Get the type of an expression at compile time
 */
char* comptime_typeof(CompileTimeEvaluator* evaluator, struct Interpreter* interpreter,
                     ASTNode* expression) {
    if (!expression) return NULL;
    
    // For now, return a placeholder
    return shared_strdup("unknown");
}

/**
 * @brief Compile-time assertion
 */
int comptime_assert(int condition, const char* message) {
    if (!condition) {
        if (message) {
            fprintf(stderr, "Compile-time assertion failed: %s\n", message);
        } else {
            fprintf(stderr, "Compile-time assertion failed\n");
        }
        return 0;
    }
    return 1;
}

/**
 * @brief Concatenate strings at compile time
 */
char* comptime_concat(const char* str1, const char* str2) {
    if (!str1 || !str2) return NULL;
    
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    char* result = shared_malloc_safe(len1 + len2 + 1, "compile_time", "comptime_concat", 0);
    if (!result) return NULL;
    
    strcpy(result, str1);
    strcat(result, str2);
    
    return result;
}