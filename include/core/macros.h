#ifndef MYCO_MACROS_H
#define MYCO_MACROS_H

/**
 * @file macros.h
 * @brief Macro System - Compile-time code generation and metaprogramming
 * 
 * The macro system provides compile-time code generation capabilities through
 * macro definitions, expansions, and template instantiation. This enables
 * powerful metaprogramming patterns while maintaining zero runtime overhead.
 * 
 * Key features:
 * - Macro definitions with parameter substitution
 * - Hygenic macro expansion (prevents variable capture)
 * - Recursive macro detection and prevention
 * - Template-based code generation
 * - Compile-time evaluation and constant folding
 * 
 * When adding new macro features, you'll typically need to:
 * 1. Add new macro definition structures
 * 2. Implement expansion algorithms
 * 3. Update the macro expander logic
 * 4. Ensure proper error handling and validation
 */

#include "ast.h"
#include <stddef.h>

// Forward declarations
struct Interpreter;

// Macro definition structure
typedef struct MacroDefinition {
    char* name;                    // Macro name
    char** parameters;             // Parameter names
    size_t parameter_count;        // Number of parameters
    ASTNode* body;                 // Macro body AST
    int is_hygenic;               // Prevents variable capture
    struct ExpansionRule* rules;   // Expansion rules
    size_t rule_count;            // Number of expansion rules
} MacroDefinition;

// Expansion rule for macro transformations
typedef struct ExpansionRule {
    ASTNode* pattern;             // Pattern to match
    ASTNode* replacement;         // Replacement AST
    struct Condition* conditions; // Expansion conditions
    size_t condition_count;       // Number of conditions
} ExpansionRule;

// Condition for conditional macro expansion
typedef struct Condition {
    char* variable;               // Variable to check
    char* value;                  // Expected value
    int is_type_check;           // 1 for type check, 0 for value check
} Condition;

// Macro expander state
typedef struct MacroExpander {
    MacroDefinition** macros;     // Array of macro definitions
    size_t macro_count;          // Number of macros
    size_t capacity;             // Current capacity
    int* expansion_stack;        // Stack to prevent infinite recursion
    size_t stack_depth;          // Current stack depth
    size_t max_depth;            // Maximum allowed depth
} MacroExpander;

// ============================================================================
// MACRO EXPANDER FUNCTIONS
// ============================================================================

/**
 * @brief Create a new macro expander
 * 
 * @return A pointer to the initialized macro expander, or NULL if allocation failed
 */
MacroExpander* macro_expander_create(void);

/**
 * @brief Free a macro expander and all its resources
 * 
 * @param expander The macro expander to free
 */
void macro_expander_free(MacroExpander* expander);

/**
 * @brief Define a new macro
 * 
 * @param expander The macro expander
 * @param name Macro name
 * @param parameters Parameter names
 * @param param_count Number of parameters
 * @param body Macro body AST
 * @param is_hygenic Whether to use hygenic expansion
 * @return 1 if successful, 0 otherwise
 */
int macro_define(MacroExpander* expander, const char* name, char** parameters, 
                size_t param_count, ASTNode* body, int is_hygenic);

/**
 * @brief Expand a macro call
 * 
 * @param expander The macro expander
 * @param macro_name Name of macro to expand
 * @param arguments Arguments to substitute
 * @param arg_count Number of arguments
 * @return Expanded AST node, or NULL if expansion failed
 */
ASTNode* macro_expand(MacroExpander* expander, const char* macro_name, 
                     ASTNode** arguments, size_t arg_count);

/**
 * @brief Check if a macro is defined
 * 
 * @param expander The macro expander
 * @param name Macro name to check
 * @return 1 if defined, 0 otherwise
 */
int macro_is_defined(MacroExpander* expander, const char* name);

/**
 * @brief Get a macro definition by name
 * 
 * @param expander The macro expander
 * @param name Macro name
 * @return Macro definition, or NULL if not found
 */
MacroDefinition* macro_get_definition(MacroExpander* expander, const char* name);

/**
 * @brief Validate a macro definition
 * 
 * @param macro The macro definition to validate
 * @return 1 if valid, 0 otherwise
 */
int macro_validate(MacroDefinition* macro);

/**
 * @brief Check if macro expansion would be recursive
 * 
 * @param expander The macro expander
 * @param macro_name Name of macro to check
 * @param call_stack Current call stack
 * @param stack_depth Current stack depth
 * @return 1 if recursive, 0 otherwise
 */
int macro_is_recursive(MacroExpander* expander, const char* macro_name, 
                      const char** call_stack, size_t stack_depth);

// ============================================================================
// MACRO EXPANSION ALGORITHMS
// ============================================================================

/**
 * @brief Perform parameter substitution in macro body
 * 
 * @param body Original macro body
 * @param parameters Parameter names
 * @param param_count Number of parameters
 * @param arguments Argument ASTs
 * @param arg_count Number of arguments
 * @return New AST with parameters substituted
 */
ASTNode* macro_substitute_parameters(ASTNode* body, char** parameters, 
                                    size_t param_count, ASTNode** arguments, 
                                    size_t arg_count);

/**
 * @brief Perform hygenic expansion (rename variables to prevent capture)
 * 
 * @param body Macro body to expand
 * @param macro_name Name of the macro (for generating unique names)
 * @return New AST with variables renamed
 */
ASTNode* macro_hygenic_expand(ASTNode* body, const char* macro_name);

/**
 * @brief Clone an AST node for macro expansion
 * 
 * @param node Node to clone
 * @return Cloned node
 */
ASTNode* macro_clone_ast(ASTNode* node);

/**
 * @brief Free macro definition resources
 * 
 * @param macro Macro definition to free
 */
void macro_definition_free(MacroDefinition* macro);

/**
 * @brief Free expansion rule resources
 * 
 * @param rule Expansion rule to free
 */
void expansion_rule_free(ExpansionRule* rule);

#endif // MYCO_MACROS_H
