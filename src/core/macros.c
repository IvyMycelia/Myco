#include "macros.h"
#include "interpreter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../../include/utils/shared_utilities.h"

// ============================================================================
// MACRO EXPANDER FUNCTIONS
// ============================================================================

/**
 * @brief Create a new macro expander
 */
MacroExpander* macro_expander_create(void) {
    MacroExpander* expander = shared_malloc_safe(sizeof(MacroExpander), "macros", "unknown_function", 0);
    if (!expander) return NULL;
    
    expander->macros = NULL;
    expander->macro_count = 0;
    expander->capacity = 0;
    expander->expansion_stack = NULL;
    expander->stack_depth = 0;
    expander->max_depth = 100;  // Prevent infinite recursion
    
    return expander;
}

/**
 * @brief Free a macro expander and all its resources
 */
void macro_expander_free(MacroExpander* expander) {
    if (!expander) return;
    
    // Free all macro definitions
    for (size_t i = 0; i < expander->macro_count; i++) {
        macro_definition_free(expander->macros[i]);
    }
    
    // Free macro array
    shared_free_safe(expander->macros, "macros", "unknown_function", 0);
    
    // Free expansion stack
    shared_free_safe(expander->expansion_stack, "macros", "unknown_function", 0);
    
    // Free expander
    shared_free_safe(expander, "macros", "unknown_function", 0);
}

/**
 * @brief Define a new macro
 */
int macro_define(MacroExpander* expander, const char* name, char** parameters, 
                size_t param_count, ASTNode* body, int is_hygenic) {
    if (!expander || !name || !body) return 0;
    
    // Check if macro already exists
    if (macro_is_defined(expander, name)) {
        // Replace existing macro
        MacroDefinition* existing = macro_get_definition(expander, name);
        if (existing) {
            macro_definition_free(existing);
        }
    } else {
        // Expand macro array if needed
        if (expander->macro_count >= expander->capacity) {
            size_t new_capacity = expander->capacity == 0 ? 4 : expander->capacity * 2;
            MacroDefinition** new_macros = shared_realloc_safe(expander->macros, 
                new_capacity * sizeof(MacroDefinition*), "macros", "unknown_function", 0);
            if (!new_macros) return 0;
            
            expander->macros = new_macros;
            expander->capacity = new_capacity;
        }
    }
    
    // Create new macro definition
    MacroDefinition* macro = shared_malloc_safe(sizeof(MacroDefinition), "macros", "unknown_function", 0);
    if (!macro) return 0;
    
    macro->name = strdup(name);
    macro->parameters = NULL;
    macro->parameter_count = param_count;
    macro->body = body;
    macro->is_hygenic = is_hygenic;
    macro->rules = NULL;
    macro->rule_count = 0;
    
    // Copy parameters
    if (param_count > 0 && parameters) {
        macro->parameters = shared_malloc_safe(param_count * sizeof(char*), "macros", "unknown_function", 0);
        if (!macro->parameters) {
            shared_free_safe(macro->name, "macros", "unknown_function", 0);
            shared_free_safe(macro, "macros", "unknown_function", 0);
            return 0;
        }
        
        for (size_t i = 0; i < param_count; i++) {
            macro->parameters[i] = strdup(parameters[i]);
        }
    }
    
    // Add to expander
    if (macro_is_defined(expander, name)) {
        // Replace existing
        for (size_t i = 0; i < expander->macro_count; i++) {
            if (strcmp(expander->macros[i]->name, name) == 0) {
                macro_definition_free(expander->macros[i]);
                expander->macros[i] = macro;
                break;
            }
        }
    } else {
        // Add new
        expander->macros[expander->macro_count++] = macro;
    }
    
    return 1;
}

/**
 * @brief Expand a macro call
 */
ASTNode* macro_expand(MacroExpander* expander, const char* macro_name, 
                     ASTNode** arguments, size_t arg_count) {
    if (!expander || !macro_name) return NULL;
    
    // Check for recursion
    if (macro_is_recursive(expander, macro_name, NULL, 0)) {
        fprintf(stderr, "Error: Recursive macro expansion detected for '%s'\n", macro_name);
        return NULL;
    }
    
    // Get macro definition
    MacroDefinition* macro = macro_get_definition(expander, macro_name);
    if (!macro) {
        fprintf(stderr, "Error: Macro '%s' is not defined\n", macro_name);
        return NULL;
    }
    
    // Check argument count
    if (arg_count != macro->parameter_count) {
        fprintf(stderr, "Error: Macro '%s' expects %zu arguments, got %zu\n", 
                macro_name, macro->parameter_count, arg_count);
        return NULL;
    }
    
    // Clone the macro body
    ASTNode* expanded = macro_clone_ast(macro->body);
    if (!expanded) return NULL;
    
    // Perform parameter substitution
    if (macro->parameter_count > 0 && arguments) {
        ASTNode* substituted = macro_substitute_parameters(expanded, macro->parameters, 
                                                          macro->parameter_count, 
                                                          arguments, arg_count);
        if (substituted) {
            ast_free(expanded);
            expanded = substituted;
        }
    }
    
    // Perform hygenic expansion if needed
    if (macro->is_hygenic) {
        ASTNode* hygenic = macro_hygenic_expand(expanded, macro_name);
        if (hygenic) {
            ast_free(expanded);
            expanded = hygenic;
        }
    }
    
    return expanded;
}

/**
 * @brief Check if a macro is defined
 */
int macro_is_defined(MacroExpander* expander, const char* name) {
    if (!expander || !name) return 0;
    
    for (size_t i = 0; i < expander->macro_count; i++) {
        if (strcmp(expander->macros[i]->name, name) == 0) {
            return 1;
        }
    }
    
    return 0;
}

/**
 * @brief Get a macro definition by name
 */
MacroDefinition* macro_get_definition(MacroExpander* expander, const char* name) {
    if (!expander || !name) return NULL;
    
    for (size_t i = 0; i < expander->macro_count; i++) {
        if (strcmp(expander->macros[i]->name, name) == 0) {
            return expander->macros[i];
        }
    }
    
    return NULL;
}

/**
 * @brief Validate a macro definition
 */
int macro_validate(MacroDefinition* macro) {
    if (!macro) return 0;
    
    // Check basic fields
    if (!macro->name || !macro->body) return 0;
    
    // Check parameter consistency
    if (macro->parameter_count > 0 && !macro->parameters) return 0;
    
    // Validate parameter names
    for (size_t i = 0; i < macro->parameter_count; i++) {
        if (!macro->parameters[i] || strlen(macro->parameters[i]) == 0) {
            return 0;
        }
    }
    
    return 1;
}

/**
 * @brief Check if macro expansion would be recursive
 */
int macro_is_recursive(MacroExpander* expander, const char* macro_name, 
                      const char** call_stack, size_t stack_depth) {
    if (!expander || !macro_name) return 0;
    
    // Simple check: if we're already expanding this macro, it's recursive
    // In a full implementation, we'd maintain a proper call stack
    // For now, we'll use a simple depth limit
    return (stack_depth > expander->max_depth);
}

// ============================================================================
// MACRO EXPANSION ALGORITHMS
// ============================================================================

/**
 * @brief Perform parameter substitution in macro body
 */
ASTNode* macro_substitute_parameters(ASTNode* body, char** parameters, 
                                    size_t param_count, ASTNode** arguments, 
                                    size_t arg_count) {
    if (!body || param_count != arg_count) return NULL;
    
    // Clone the body first
    ASTNode* result = macro_clone_ast(body);
    if (!result) return NULL;
    
    // TODO: Implement parameter substitution
    // This is a simplified version - in a full implementation,
    // we'd traverse the AST and replace parameter references with arguments
    
    return result;
}

/**
 * @brief Perform hygenic expansion (rename variables to prevent capture)
 */
ASTNode* macro_hygenic_expand(ASTNode* body, const char* macro_name) {
    if (!body || !macro_name) return NULL;
    
    // Clone the body
    ASTNode* result = macro_clone_ast(body);
    if (!result) return NULL;
    
    // TODO: Implement hygenic expansion
    // This would rename all variables in the macro body to unique names
    // to prevent variable capture during expansion
    
    return result;
}

/**
 * @brief Clone an AST node for macro expansion
 */
ASTNode* macro_clone_ast(ASTNode* node) {
    if (!node) return NULL;
    
    // Use the existing AST clone function
    return ast_clone(node);
}

/**
 * @brief Free macro definition resources
 */
void macro_definition_free(MacroDefinition* macro) {
    if (!macro) return;
    
    shared_free_safe(macro->name, "macros", "unknown_function", 0);
    
    if (macro->parameters) {
        for (size_t i = 0; i < macro->parameter_count; i++) {
            shared_free_safe(macro->parameters[i], "macros", "unknown_function", 0);
        }
        shared_free_safe(macro->parameters, "macros", "unknown_function", 0);
    }
    
    if (macro->body) {
        ast_free(macro->body);
    }
    
    if (macro->rules) {
        for (size_t i = 0; i < macro->rule_count; i++) {
            expansion_rule_free(&macro->rules[i]);
        }
        shared_free_safe(macro->rules, "macros", "unknown_function", 0);
    }
    
    shared_free_safe(macro, "macros", "unknown_function", 0);
}

/**
 * @brief Free expansion rule resources
 */
void expansion_rule_free(ExpansionRule* rule) {
    if (!rule) return;
    
    if (rule->pattern) {
        ast_free(rule->pattern);
    }
    
    if (rule->replacement) {
        ast_free(rule->replacement);
    }
    
    if (rule->conditions) {
        for (size_t i = 0; i < rule->condition_count; i++) {
            shared_free_safe(rule->conditions[i].variable, "macros", "unknown_function", 0);
            shared_free_safe(rule->conditions[i].value, "macros", "unknown_function", 0);
        }
        shared_free_safe(rule->conditions, "macros", "unknown_function", 0);
    }
}
