#ifndef MYCO_BUILTIN_MACROS_H
#define MYCO_BUILTIN_MACROS_H

/**
 * @file builtin_macros.h
 * @brief Built-in Macros and Compile-Time Functions
 * 
 * This file provides built-in macros and compile-time functions that are
 * available by default in Myco. These macros provide common functionality
 * like debug printing, assertions, type checking, and compile-time utilities.
 * 
 * Key features:
 * - Debug printing macros (removed in release builds)
 * - Assertion macros with custom messages
 * - Type checking and validation macros
 * - Compile-time utility functions
 * - Code generation macros
 * 
 * When adding new built-in macros, you'll typically need to:
 * 1. Add the macro definition function
 * 2. Register it in register_builtin_macros()
 * 3. Add any supporting built-in functions
 * 4. Update the documentation
 */

#include "macros.h"
#include "interpreter.h"
#include <stddef.h>

// ============================================================================
// BUILT-IN MACRO REGISTRATION
// ============================================================================

/**
 * @brief Register all built-in macros with the macro expander
 * 
 * @param expander The macro expander to register macros with
 * @return 1 if successful, 0 otherwise
 */
int register_builtin_macros(MacroExpander* expander);

// ============================================================================
// INDIVIDUAL MACRO REGISTRATION FUNCTIONS
// ============================================================================

/**
 * @brief Register debug_print macro
 * 
 * Syntax: debug_print(message)
 * Only prints in debug builds
 * 
 * @param expander The macro expander
 * @return 1 if successful, 0 otherwise
 */
int register_debug_print_macro(MacroExpander* expander);

/**
 * @brief Register assert_msg macro
 * 
 * Syntax: assert_msg(condition, message)
 * Throws error with custom message if condition is false
 * 
 * @param expander The macro expander
 * @return 1 if successful, 0 otherwise
 */
int register_assert_msg_macro(MacroExpander* expander);

/**
 * @brief Register ensure_type macro
 * 
 * Syntax: ensure_type(value, expected_type)
 * Throws error if value type doesn't match expected type
 * 
 * @param expander The macro expander
 * @return 1 if successful, 0 otherwise
 */
int register_ensure_type_macro(MacroExpander* expander);

/**
 * @brief Register generate_accessors macro
 * 
 * Syntax: generate_accessors(class_name, fields)
 * Generates getter/setter methods for class fields
 * 
 * @param expander The macro expander
 * @return 1 if successful, 0 otherwise
 */
int register_generate_accessors_macro(MacroExpander* expander);

/**
 * @brief Register comptime_sizeof macro
 * 
 * Syntax: comptime_sizeof(type_name)
 * Returns size of type at compile time
 * 
 * @param expander The macro expander
 * @return 1 if successful, 0 otherwise
 */
int register_comptime_sizeof_macro(MacroExpander* expander);

/**
 * @brief Register comptime_typeof macro
 * 
 * Syntax: comptime_typeof(expression)
 * Returns type of expression at compile time
 * 
 * @param expander The macro expander
 * @return 1 if successful, 0 otherwise
 */
int register_comptime_typeof_macro(MacroExpander* expander);

// ============================================================================
// COMPILE-TIME BUILT-IN FUNCTIONS
// ============================================================================

/**
 * @brief Built-in comptime_sizeof function
 * 
 * @param interpreter The interpreter context
 * @param args Function arguments
 * @param arg_count Number of arguments
 * @param line Line number for error reporting
 * @param column Column number for error reporting
 * @return The size of the type, or null if error
 */
Value builtin_comptime_sizeof(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

/**
 * @brief Built-in comptime_typeof function
 * 
 * @param interpreter The interpreter context
 * @param args Function arguments
 * @param arg_count Number of arguments
 * @param line Line number for error reporting
 * @param column Column number for error reporting
 * @return The type of the expression, or null if error
 */
Value builtin_comptime_typeof(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

/**
 * @brief Built-in comptime_concat function
 * 
 * @param interpreter The interpreter context
 * @param args Function arguments
 * @param arg_count Number of arguments
 * @param line Line number for error reporting
 * @param column Column number for error reporting
 * @return The concatenated string, or null if error
 */
Value builtin_comptime_concat(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

/**
 * @brief Built-in comptime_assert function
 * 
 * @param interpreter The interpreter context
 * @param args Function arguments
 * @param arg_count Number of arguments
 * @param line Line number for error reporting
 * @param column Column number for error reporting
 * @return Null if assertion passes, error if fails
 */
Value builtin_comptime_assert(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

#endif // MYCO_BUILTIN_MACROS_H
