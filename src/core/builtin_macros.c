#include "../../include/core/builtin_macros.h"
#include "../../include/core/interpreter.h"
#include "../../include/core/ast.h"
#include "../../include/core/macros.h"
#include "../../include/core/compile_time.h"
#include "../../include/utils/shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ============================================================================
// BUILT-IN MACRO DEFINITIONS
// ============================================================================

/**
 * @brief Register all built-in macros with the macro expander
 * 
 * @param expander The macro expander to register macros with
 * @return 1 if successful, 0 otherwise
 */
int register_builtin_macros(MacroExpander* expander) {
    if (!expander) return 0;
    
    int success = 1;
    
    // Register debug_print macro
    success &= register_debug_print_macro(expander);
    
    // Register assert_msg macro
    success &= register_assert_msg_macro(expander);
    
    // Register ensure_type macro
    success &= register_ensure_type_macro(expander);
    
    // Register generate_accessors macro
    success &= register_generate_accessors_macro(expander);
    
    // Register comptime_sizeof macro
    success &= register_comptime_sizeof_macro(expander);
    
    // Register comptime_typeof macro
    success &= register_comptime_typeof_macro(expander);
    
    return success;
}

/**
 * @brief Register debug_print macro
 * 
 * Syntax: debug_print(message)
 * Only prints in debug builds
 */
int register_debug_print_macro(MacroExpander* expander) {
    if (!expander) return 0;
    
    // Create macro body AST
    ASTNode* condition = ast_create_binary_op(
        OP_EQUAL,
        ast_create_identifier("DEBUG", 0, 0),
        ast_create_bool(1, 0, 0),
        0, 0
    );
    
    ASTNode* print_call = ast_create_function_call(
        "print",
        (ASTNode*[]){ast_create_string("DEBUG: ", 0, 0), ast_create_identifier("message", 0, 0)},
        2,
        0, 0
    );
    
    ASTNode* if_block = ast_create_if_statement(
        condition,
        print_call,
        NULL,
        NULL,
        0, 0
    );
    
    // Register macro
    char* params[] = {"message"};
    return macro_define(expander, "debug_print", params, 1, if_block, 1);
}

/**
 * @brief Register assert_msg macro
 * 
 * Syntax: assert_msg(condition, message)
 * Throws error with custom message if condition is false
 */
int register_assert_msg_macro(MacroExpander* expander) {
    if (!expander) return 0;
    
    // Create macro body AST
    ASTNode* not_condition = ast_create_unary_op(
        OP_LOGICAL_NOT,
        ast_create_identifier("condition", 0, 0),
        0, 0
    );
    
    ASTNode* error_msg = ast_create_binary_op(
        OP_ADD,
        ast_create_string("Assertion failed: ", 0, 0),
        ast_create_identifier("message", 0, 0),
        0, 0
    );
    
    ASTNode* throw_stmt = ast_create_throw(error_msg, 0, 0);
    
    ASTNode* if_block = ast_create_if_statement(
        not_condition,
        throw_stmt,
        NULL,
        NULL,
        0, 0
    );
    
    // Register macro
    char* params[] = {"condition", "message"};
    return macro_define(expander, "assert_msg", params, 2, if_block, 1);
}

/**
 * @brief Register ensure_type macro
 * 
 * Syntax: ensure_type(value, expected_type)
 * Throws error if value type doesn't match expected type
 */
int register_ensure_type_macro(MacroExpander* expander) {
    if (!expander) return 0;
    
    // Create macro body AST
    ASTNode* type_call = ast_create_function_call(
        "type",
        (ASTNode*[]){ast_create_identifier("value", 0, 0)},
        1,
        0, 0
    );
    
    ASTNode* type_check = ast_create_binary_op(
        OP_NOT_EQUAL,
        type_call,
        ast_create_identifier("expected_type", 0, 0),
        0, 0
    );
    
    ASTNode* error_msg = ast_create_binary_op(
        OP_ADD,
        ast_create_string("Type mismatch: expected ", 0, 0),
        ast_create_identifier("expected_type", 0, 0),
        0, 0
    );
    
    ASTNode* throw_stmt = ast_create_throw(error_msg, 0, 0);
    
    ASTNode* if_block = ast_create_if_statement(
        type_check,
        throw_stmt,
        NULL,
        NULL,
        0, 0
    );
    
    ASTNode* return_stmt = ast_create_return(ast_create_identifier("value", 0, 0), 0, 0);
    
    ASTNode* block = ast_create_block(
        (ASTNode*[]){if_block, return_stmt},
        2,
        0, 0
    );
    
    // Register macro
    char* params[] = {"value", "expected_type"};
    return macro_define(expander, "ensure_type", params, 2, block, 1);
}

/**
 * @brief Register generate_accessors macro
 * 
 * Syntax: generate_accessors(class_name, fields)
 * Generates getter/setter methods for class fields
 */
int register_generate_accessors_macro(MacroExpander* expander) {
    if (!expander) return 0;
    
    // This is a placeholder - in a full implementation, this would
    // generate actual getter/setter methods based on the fields array
    
    ASTNode* comment = ast_create_string("// Generated accessors would go here", 0, 0);
    
    // Register macro
    char* params[] = {"class_name", "fields"};
    return macro_define(expander, "generate_accessors", params, 2, comment, 1);
}

/**
 * @brief Register comptime_sizeof macro
 * 
 * Syntax: comptime_sizeof(type_name)
 * Returns size of type at compile time
 */
int register_comptime_sizeof_macro(MacroExpander* expander) {
    if (!expander) return 0;
    
    // Create macro body AST
    ASTNode* sizeof_call = ast_create_function_call(
        "comptime_sizeof",
        (ASTNode*[]){ast_create_identifier("type_name", 0, 0)},
        1,
        0, 0
    );
    
    // Register macro
    char* params[] = {"type_name"};
    return macro_define(expander, "comptime_sizeof", params, 1, sizeof_call, 1);
}

/**
 * @brief Register comptime_typeof macro
 * 
 * Syntax: comptime_typeof(expression)
 * Returns type of expression at compile time
 */
int register_comptime_typeof_macro(MacroExpander* expander) {
    if (!expander) return 0;
    
    // Create macro body AST
    ASTNode* typeof_call = ast_create_function_call(
        "comptime_typeof",
        (ASTNode*[]){ast_create_identifier("expression", 0, 0)},
        1,
        0, 0
    );
    
    // Register macro
    char* params[] = {"expression"};
    return macro_define(expander, "comptime_typeof", params, 1, typeof_call, 1);
}

// ============================================================================
// COMPILE-TIME UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Built-in comptime_sizeof function
 */
Value builtin_comptime_sizeof(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) {
        interpreter_set_error(interpreter, "comptime_sizeof expects one string argument", line, column);
        return value_create_null();
    }
    
    int size = comptime_sizeof(args[0].data.string_value);
    if (size < 0) {
        interpreter_set_error(interpreter, "Unknown type for sizeof", line, column);
        return value_create_null();
    }
    
    return value_create_number(size);
}

/**
 * @brief Built-in comptime_typeof function
 */
Value builtin_comptime_typeof(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "comptime_typeof expects one argument", line, column);
        return value_create_null();
    }
    
    // Get the type of the value
    const char* type_name = value_type_to_string(args[0].type);
    return value_create_string(type_name);
}

/**
 * @brief Built-in comptime_concat function
 */
Value builtin_comptime_concat(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2 || args[0].type != VALUE_STRING || args[1].type != VALUE_STRING) {
        interpreter_set_error(interpreter, "comptime_concat expects two string arguments", line, column);
        return value_create_null();
    }
    
    char* result = comptime_concat(args[0].data.string_value, args[1].data.string_value);
    if (!result) {
        interpreter_set_error(interpreter, "Failed to concatenate strings", line, column);
        return value_create_null();
    }
    
    Value value = value_create_string(result);
    shared_free_safe(result, "builtin_macros", "builtin_comptime_concat", 0);
    return value;
}

/**
 * @brief Built-in comptime_assert function
 */
Value builtin_comptime_assert(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        interpreter_set_error(interpreter, "comptime_assert expects at least one argument", line, column);
        return value_create_null();
    }
    
    if (args[0].type != VALUE_BOOLEAN) {
        interpreter_set_error(interpreter, "comptime_assert first argument must be boolean", line, column);
        return value_create_null();
    }
    
    const char* message = "Compile-time assertion failed";
    if (arg_count > 1 && args[1].type == VALUE_STRING) {
        message = args[1].data.string_value;
    }
    
    if (!comptime_assert(args[0].data.boolean_value, message)) {
        interpreter_set_error(interpreter, message, line, column);
        return value_create_null();
    }
    
    return value_create_null();
}

