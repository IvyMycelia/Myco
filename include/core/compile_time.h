#ifndef MYCO_COMPILE_TIME_H
#define MYCO_COMPILE_TIME_H

/**
 * @file compile_time.h
 * @brief Compile-Time Evaluation System - Constant folding and compile-time computation
 * 
 * The compile-time evaluation system provides constant folding, compile-time
 * computation, and optimization capabilities. This enables evaluation of
 * constant expressions at compile time, reducing runtime overhead.
 * 
 * Key features:
 * - Constant folding for arithmetic expressions
 * - String concatenation at compile time
 * - Boolean expression evaluation
 * - Pure function evaluation
 * - Constant propagation through AST
 * - Compile-time assertions
 * 
 * When adding new compile-time features, you'll typically need to:
 * 1. Add new evaluation functions for specific node types
 * 2. Implement constant folding algorithms
 * 3. Update the compile-time evaluator logic
 * 4. Ensure proper error handling and validation
 */

#include "ast.h"
#include "interpreter.h"
#include <stddef.h>

// Forward declarations
struct Interpreter;
struct Environment;

// Compile-time value with metadata
typedef struct CompileTimeValue {
    ASTNode* expression;        // Original expression AST
    struct Value* result;       // Computed result (pointer to avoid incomplete type)
    int is_constant;            // Whether this is a compile-time constant
    int is_pure;                // Whether this has no side effects
    char** dependencies;        // Dependencies for re-evaluation
    size_t dep_count;           // Number of dependencies
    int is_evaluated;           // Whether this has been evaluated
} CompileTimeValue;

// Compile-time evaluator state
typedef struct CompileTimeEvaluator {
    CompileTimeValue** values;  // Array of compile-time values
    size_t count;               // Number of values
    size_t capacity;            // Current capacity
    struct Environment* compile_env;   // Compile-time environment
    int optimization_level;     // Optimization level (0-3)
} CompileTimeEvaluator;

// ============================================================================
// COMPILE-TIME EVALUATOR FUNCTIONS
// ============================================================================

/**
 * @brief Create a new compile-time evaluator
 * 
 * @return A pointer to the initialized evaluator, or NULL if allocation failed
 */
CompileTimeEvaluator* compile_time_evaluator_create(void);

/**
 * @brief Free a compile-time evaluator and all its resources
 * 
 * @param evaluator The evaluator to free
 */
void compile_time_evaluator_free(CompileTimeEvaluator* evaluator);

/**
 * @brief Evaluate an expression at compile time
 * 
 * @param evaluator The compile-time evaluator
 * @param interpreter The interpreter context
 * @param expression The expression to evaluate
 * @return The computed value, or NULL if evaluation failed
 */
Value compile_time_eval(CompileTimeEvaluator* evaluator, struct Interpreter* interpreter, 
                       ASTNode* expression);

/**
 * @brief Check if an expression is a compile-time constant
 * 
 * @param expression The expression to check
 * @return 1 if constant, 0 otherwise
 */
int is_compile_time_constant(ASTNode* expression);

/**
 * @brief Check if an expression is pure (no side effects)
 * 
 * @param expression The expression to check
 * @return 1 if pure, 0 otherwise
 */
int is_pure_expression(ASTNode* expression);

/**
 * @brief Perform constant folding on an expression
 * 
 * @param expression The expression to fold
 * @return Folded expression, or original if not foldable
 */
ASTNode* fold_constants(ASTNode* expression);

/**
 * @brief Evaluate a pure function at compile time
 * 
 * @param evaluator The compile-time evaluator
 * @param interpreter The interpreter context
 * @param function_name Function name
 * @param arguments Function arguments
 * @param arg_count Number of arguments
 * @return Computed result, or NULL if not evaluable
 */
Value evaluate_pure_function(CompileTimeEvaluator* evaluator, struct Interpreter* interpreter,
                            const char* function_name, ASTNode** arguments, size_t arg_count);

// ============================================================================
// CONSTANT PROPAGATION FUNCTIONS
// ============================================================================

/**
 * @brief Propagate constants through the AST
 * 
 * @param evaluator The compile-time evaluator
 * @param interpreter The interpreter context
 * @param node The AST node to process
 * @return Modified AST node with constants propagated
 */
ASTNode* propagate_constants(CompileTimeEvaluator* evaluator, struct Interpreter* interpreter, 
                            ASTNode* node);

/**
 * @brief Evaluate arithmetic expressions at compile time
 * 
 * @param evaluator The compile-time evaluator
 * @param interpreter The interpreter context
 * @param node Binary operation node
 * @return Computed result, or NULL if not evaluable
 */
Value eval_arithmetic_compile_time(CompileTimeEvaluator* evaluator, struct Interpreter* interpreter,
                                  ASTNode* node);

/**
 * @brief Evaluate string concatenation at compile time
 * 
 * @param evaluator The compile-time evaluator
 * @param interpreter The interpreter context
 * @param node Binary operation node
 * @return Computed result, or NULL if not evaluable
 */
Value eval_string_concat_compile_time(CompileTimeEvaluator* evaluator, struct Interpreter* interpreter,
                                     ASTNode* node);

/**
 * @brief Evaluate boolean expressions at compile time
 * 
 * @param evaluator The compile-time evaluator
 * @param interpreter The interpreter context
 * @param node Binary operation node
 * @return Computed result, or NULL if not evaluable
 */
Value eval_boolean_compile_time(CompileTimeEvaluator* evaluator, struct Interpreter* interpreter,
                               ASTNode* node);

// ============================================================================
// COMPILE-TIME UTILITIES
// ============================================================================

/**
 * @brief Get the size of a type at compile time
 * 
 * @param type_name Type name
 * @return Size in bytes, or -1 if unknown
 */
int comptime_sizeof(const char* type_name);

/**
 * @brief Get the type of an expression at compile time
 * 
 * @param evaluator The compile-time evaluator
 * @param interpreter The interpreter context
 * @param expression The expression
 * @return Type name, or NULL if unknown
 */
char* comptime_typeof(CompileTimeEvaluator* evaluator, struct Interpreter* interpreter,
                     ASTNode* expression);

/**
 * @brief Compile-time assertion
 * 
 * @param condition The condition to assert
 * @param message Error message if assertion fails
 * @return 1 if assertion passes, 0 otherwise
 */
int comptime_assert(int condition, const char* message);

/**
 * @brief Concatenate strings at compile time
 * 
 * @param str1 First string
 * @param str2 Second string
 * @return Concatenated string, or NULL if allocation failed
 */
char* comptime_concat(const char* str1, const char* str2);

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * @brief Free compile-time value resources
 * 
 * @param value The value to free
 */
void compile_time_value_free(CompileTimeValue* value);

/**
 * @brief Clone a compile-time value
 * 
 * @param value The value to clone
 * @return Cloned value, or NULL if allocation failed
 */
CompileTimeValue* compile_time_value_clone(CompileTimeValue* value);

/**
 * @brief Check if a value is a compile-time constant
 * 
 * @param value The value to check
 * @return 1 if constant, 0 otherwise
 */
int value_is_compile_time_constant(struct Value* value);

#endif // MYCO_COMPILE_TIME_H
