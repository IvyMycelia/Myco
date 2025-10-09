#include "macros.h"
#include "interpreter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
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
        printf("DEBUG: Performing hygenic expansion for macro %s\n", macro_name);
        ASTNode* hygenic = macro_hygenic_expand(expanded, macro_name);
        if (hygenic) {
            ast_free(expanded);
            expanded = hygenic;
        }
    } else {
        printf("DEBUG: Skipping hygenic expansion for macro %s (is_hygenic=%d)\n", macro_name, macro->is_hygenic);
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
    
    // Perform deep substitution
    result = macro_substitute_parameters_recursive(result, parameters, param_count, arguments, arg_count);
    
    return result;
}

/**
 * @brief Recursively substitute parameters in AST nodes
 */
ASTNode* macro_substitute_parameters_recursive(ASTNode* node, char** parameters, 
                                              size_t param_count, ASTNode** arguments, 
                                              size_t arg_count) {
    if (!node) return NULL;
    
    // If it's an identifier, check if it's a parameter
    if (node->type == AST_NODE_IDENTIFIER) {
        for (size_t i = 0; i < param_count; i++) {
            if (strcmp(node->data.identifier_value, parameters[i]) == 0) {
                // Found a parameter, substitute with the corresponding argument
                ASTNode* substituted = macro_clone_ast(arguments[i]);
                if (substituted) {
                    // Preserve line/column information
                    substituted->line = node->line;
                    substituted->column = node->column;
                    ast_free(node);
                    return substituted;
                }
            }
        }
        return node; // Not a parameter, return as-is
    }
    
    // Recursively substitute in child nodes
    switch (node->type) {
        case AST_NODE_BINARY_OP:
            node->data.binary.left = macro_substitute_parameters_recursive(
                node->data.binary.left, parameters, param_count, arguments, arg_count);
            node->data.binary.right = macro_substitute_parameters_recursive(
                node->data.binary.right, parameters, param_count, arguments, arg_count);
            break;
            
        case AST_NODE_UNARY_OP:
            node->data.unary.operand = macro_substitute_parameters_recursive(
                node->data.unary.operand, parameters, param_count, arguments, arg_count);
            break;
            
        case AST_NODE_FUNCTION_CALL:
            for (size_t i = 0; i < node->data.function_call.argument_count; i++) {
                node->data.function_call.arguments[i] = macro_substitute_parameters_recursive(
                    node->data.function_call.arguments[i], parameters, param_count, arguments, arg_count);
            }
            break;
            
        case AST_NODE_FUNCTION_CALL_EXPR:
            node->data.function_call_expr.function = macro_substitute_parameters_recursive(
                node->data.function_call_expr.function, parameters, param_count, arguments, arg_count);
            for (size_t i = 0; i < node->data.function_call_expr.argument_count; i++) {
                node->data.function_call_expr.arguments[i] = macro_substitute_parameters_recursive(
                    node->data.function_call_expr.arguments[i], parameters, param_count, arguments, arg_count);
            }
            break;
            
        case AST_NODE_ARRAY_LITERAL:
            for (size_t i = 0; i < node->data.array_literal.element_count; i++) {
                node->data.array_literal.elements[i] = macro_substitute_parameters_recursive(
                    node->data.array_literal.elements[i], parameters, param_count, arguments, arg_count);
            }
            break;
            
        case AST_NODE_HASH_MAP_LITERAL:
            for (size_t i = 0; i < node->data.hash_map_literal.pair_count; i++) {
                node->data.hash_map_literal.keys[i] = macro_substitute_parameters_recursive(
                    node->data.hash_map_literal.keys[i], parameters, param_count, arguments, arg_count);
                node->data.hash_map_literal.values[i] = macro_substitute_parameters_recursive(
                    node->data.hash_map_literal.values[i], parameters, param_count, arguments, arg_count);
            }
            break;
            
        case AST_NODE_SET_LITERAL:
            for (size_t i = 0; i < node->data.set_literal.element_count; i++) {
                node->data.set_literal.elements[i] = macro_substitute_parameters_recursive(
                    node->data.set_literal.elements[i], parameters, param_count, arguments, arg_count);
            }
            break;
            
        case AST_NODE_ARRAY_ACCESS:
            node->data.array_access.array = macro_substitute_parameters_recursive(
                node->data.array_access.array, parameters, param_count, arguments, arg_count);
            node->data.array_access.index = macro_substitute_parameters_recursive(
                node->data.array_access.index, parameters, param_count, arguments, arg_count);
            break;
            
        case AST_NODE_MEMBER_ACCESS:
            node->data.member_access.object = macro_substitute_parameters_recursive(
                node->data.member_access.object, parameters, param_count, arguments, arg_count);
            break;
            
        case AST_NODE_IF_STATEMENT:
            node->data.if_statement.condition = macro_substitute_parameters_recursive(
                node->data.if_statement.condition, parameters, param_count, arguments, arg_count);
            node->data.if_statement.then_block = macro_substitute_parameters_recursive(
                node->data.if_statement.then_block, parameters, param_count, arguments, arg_count);
            node->data.if_statement.else_block = macro_substitute_parameters_recursive(
                node->data.if_statement.else_block, parameters, param_count, arguments, arg_count);
            node->data.if_statement.else_if_chain = macro_substitute_parameters_recursive(
                node->data.if_statement.else_if_chain, parameters, param_count, arguments, arg_count);
            break;
            
        case AST_NODE_WHILE_LOOP:
            node->data.while_loop.condition = macro_substitute_parameters_recursive(
                node->data.while_loop.condition, parameters, param_count, arguments, arg_count);
            node->data.while_loop.body = macro_substitute_parameters_recursive(
                node->data.while_loop.body, parameters, param_count, arguments, arg_count);
            break;
            
        case AST_NODE_FOR_LOOP:
            node->data.for_loop.collection = macro_substitute_parameters_recursive(
                node->data.for_loop.collection, parameters, param_count, arguments, arg_count);
            node->data.for_loop.body = macro_substitute_parameters_recursive(
                node->data.for_loop.body, parameters, param_count, arguments, arg_count);
            break;
            
        case AST_NODE_BLOCK:
            for (size_t i = 0; i < node->data.block.statement_count; i++) {
                node->data.block.statements[i] = macro_substitute_parameters_recursive(
                    node->data.block.statements[i], parameters, param_count, arguments, arg_count);
            }
            break;
            
        case AST_NODE_RETURN:
            if (node->data.return_statement.value) {
                node->data.return_statement.value = macro_substitute_parameters_recursive(
                    node->data.return_statement.value, parameters, param_count, arguments, arg_count);
            }
            break;
            
        case AST_NODE_THROW:
            if (node->data.throw_statement.value) {
                node->data.throw_statement.value = macro_substitute_parameters_recursive(
                    node->data.throw_statement.value, parameters, param_count, arguments, arg_count);
            }
            break;
            
        case AST_NODE_TRY_CATCH:
            node->data.try_catch.try_block = macro_substitute_parameters_recursive(
                node->data.try_catch.try_block, parameters, param_count, arguments, arg_count);
            node->data.try_catch.catch_block = macro_substitute_parameters_recursive(
                node->data.try_catch.catch_block, parameters, param_count, arguments, arg_count);
            node->data.try_catch.finally_block = macro_substitute_parameters_recursive(
                node->data.try_catch.finally_block, parameters, param_count, arguments, arg_count);
            break;
            
        case AST_NODE_ASYNC_FUNCTION:
            if (node->data.async_function_definition.body) {
                node->data.async_function_definition.body = macro_substitute_parameters_recursive(
                    node->data.async_function_definition.body, parameters, param_count, arguments, arg_count);
            }
            break;
            
        case AST_NODE_AWAIT:
            node->data.await_expression.expression = macro_substitute_parameters_recursive(
                node->data.await_expression.expression, parameters, param_count, arguments, arg_count);
            break;
            
        case AST_NODE_PROMISE:
            node->data.promise_creation.expression = macro_substitute_parameters_recursive(
                node->data.promise_creation.expression, parameters, param_count, arguments, arg_count);
            break;
            
        case AST_NODE_COMPTIME_EVAL:
            node->data.comptime_eval.expression = macro_substitute_parameters_recursive(
                node->data.comptime_eval.expression, parameters, param_count, arguments, arg_count);
            break;
            
        default:
            // No child nodes to substitute
            break;
    }
    
    return node;
}

/**
 * @brief Perform hygenic expansion (rename variables to prevent capture)
 */
ASTNode* macro_hygenic_expand(ASTNode* body, const char* macro_name) {
    if (!body || !macro_name) return NULL;
    
    // Clone the body
    ASTNode* result = macro_clone_ast(body);
    if (!result) return NULL;
    
    // Create a variable mapping for hygenic expansion
    VariableMapping* mapping = variable_mapping_create();
    if (!mapping) {
        ast_free(result);
        return NULL;
    }
    
    // Generate unique prefix for this macro
    char prefix[64];
    snprintf(prefix, sizeof(prefix), "__%s_%zu_", macro_name, (size_t)time(NULL));
    
    // Perform hygenic expansion
    result = macro_hygenic_expand_recursive(result, mapping, prefix);
    
    // Free mapping
    variable_mapping_free(mapping);
    
    return result;
}


/**
 * @brief Create a variable mapping
 */
VariableMapping* variable_mapping_create(void) {
    VariableMapping* mapping = shared_malloc_safe(sizeof(VariableMapping), 
        "macros", "variable_mapping_create", 0);
    if (!mapping) return NULL;
    
    mapping->original_names = NULL;
    mapping->new_names = NULL;
    mapping->count = 0;
    mapping->capacity = 0;
    
    return mapping;
}

/**
 * @brief Free a variable mapping
 */
void variable_mapping_free(VariableMapping* mapping) {
    if (!mapping) return;
    
    if (mapping->original_names) {
        for (size_t i = 0; i < mapping->count; i++) {
            shared_free_safe(mapping->original_names[i], "macros", "variable_mapping_free", 0);
        }
        shared_free_safe(mapping->original_names, "macros", "variable_mapping_free", 0);
    }
    
    if (mapping->new_names) {
        for (size_t i = 0; i < mapping->count; i++) {
            shared_free_safe(mapping->new_names[i], "macros", "variable_mapping_free", 0);
        }
        shared_free_safe(mapping->new_names, "macros", "variable_mapping_free", 0);
    }
    
    shared_free_safe(mapping, "macros", "variable_mapping_free", 0);
}

/**
 * @brief Get or create a mapping for a variable
 */
char* variable_mapping_get_or_create(VariableMapping* mapping, const char* original_name, const char* prefix) {
    if (!mapping || !original_name || !prefix) return NULL;
    
    // Check if we already have a mapping for this variable
    for (size_t i = 0; i < mapping->count; i++) {
        if (strcmp(mapping->original_names[i], original_name) == 0) {
            return mapping->new_names[i];
        }
    }
    
    // Create new mapping
    if (mapping->count >= mapping->capacity) {
        size_t new_capacity = mapping->capacity == 0 ? 4 : mapping->capacity * 2;
        char** new_original = shared_realloc_safe(mapping->original_names, 
            new_capacity * sizeof(char*), "macros", "variable_mapping_get_or_create", 0);
        char** new_new = shared_realloc_safe(mapping->new_names, 
            new_capacity * sizeof(char*), "macros", "variable_mapping_get_or_create", 0);
        
        if (!new_original || !new_new) return NULL;
        
        mapping->original_names = new_original;
        mapping->new_names = new_new;
        mapping->capacity = new_capacity;
    }
    
    // Generate unique name
    char new_name[128];
    snprintf(new_name, sizeof(new_name), "%s%s_%zu", prefix, original_name, mapping->count);
    
    // Store mapping
    mapping->original_names[mapping->count] = strdup(original_name);
    mapping->new_names[mapping->count] = strdup(new_name);
    mapping->count++;
    
    return mapping->new_names[mapping->count - 1];
}

/**
 * @brief Recursively perform hygenic expansion
 */
ASTNode* macro_hygenic_expand_recursive(ASTNode* node, VariableMapping* mapping, const char* prefix) {
    if (!node) return NULL;
    
    // If it's an identifier, check if it needs renaming
    if (node->type == AST_NODE_IDENTIFIER) {
        char* new_name = variable_mapping_get_or_create(mapping, node->data.identifier_value, prefix);
        if (new_name) {
            shared_free_safe(node->data.identifier_value, "macros", "macro_hygenic_expand_recursive", 0);
            node->data.identifier_value = strdup(new_name);
        }
        return node;
    }
    
    // Recursively expand child nodes
    switch (node->type) {
        case AST_NODE_BINARY_OP:
            node->data.binary.left = macro_hygenic_expand_recursive(
                node->data.binary.left, mapping, prefix);
            node->data.binary.right = macro_hygenic_expand_recursive(
                node->data.binary.right, mapping, prefix);
            break;
            
        case AST_NODE_UNARY_OP:
            node->data.unary.operand = macro_hygenic_expand_recursive(
                node->data.unary.operand, mapping, prefix);
            break;
            
        case AST_NODE_FUNCTION_CALL:
            for (size_t i = 0; i < node->data.function_call.argument_count; i++) {
                node->data.function_call.arguments[i] = macro_hygenic_expand_recursive(
                    node->data.function_call.arguments[i], mapping, prefix);
            }
            break;
            
        case AST_NODE_FUNCTION_CALL_EXPR:
            node->data.function_call_expr.function = macro_hygenic_expand_recursive(
                node->data.function_call_expr.function, mapping, prefix);
            for (size_t i = 0; i < node->data.function_call_expr.argument_count; i++) {
                node->data.function_call_expr.arguments[i] = macro_hygenic_expand_recursive(
                    node->data.function_call_expr.arguments[i], mapping, prefix);
            }
            break;
            
        case AST_NODE_ARRAY_LITERAL:
            for (size_t i = 0; i < node->data.array_literal.element_count; i++) {
                node->data.array_literal.elements[i] = macro_hygenic_expand_recursive(
                    node->data.array_literal.elements[i], mapping, prefix);
            }
            break;
            
        case AST_NODE_HASH_MAP_LITERAL:
            for (size_t i = 0; i < node->data.hash_map_literal.pair_count; i++) {
                node->data.hash_map_literal.keys[i] = macro_hygenic_expand_recursive(
                    node->data.hash_map_literal.keys[i], mapping, prefix);
                node->data.hash_map_literal.values[i] = macro_hygenic_expand_recursive(
                    node->data.hash_map_literal.values[i], mapping, prefix);
            }
            break;
            
        case AST_NODE_SET_LITERAL:
            for (size_t i = 0; i < node->data.set_literal.element_count; i++) {
                node->data.set_literal.elements[i] = macro_hygenic_expand_recursive(
                    node->data.set_literal.elements[i], mapping, prefix);
            }
            break;
            
        case AST_NODE_ARRAY_ACCESS:
            node->data.array_access.array = macro_hygenic_expand_recursive(
                node->data.array_access.array, mapping, prefix);
            node->data.array_access.index = macro_hygenic_expand_recursive(
                node->data.array_access.index, mapping, prefix);
            break;
            
        case AST_NODE_MEMBER_ACCESS:
            node->data.member_access.object = macro_hygenic_expand_recursive(
                node->data.member_access.object, mapping, prefix);
            break;
            
        case AST_NODE_IF_STATEMENT:
            node->data.if_statement.condition = macro_hygenic_expand_recursive(
                node->data.if_statement.condition, mapping, prefix);
            node->data.if_statement.then_block = macro_hygenic_expand_recursive(
                node->data.if_statement.then_block, mapping, prefix);
            node->data.if_statement.else_block = macro_hygenic_expand_recursive(
                node->data.if_statement.else_block, mapping, prefix);
            node->data.if_statement.else_if_chain = macro_hygenic_expand_recursive(
                node->data.if_statement.else_if_chain, mapping, prefix);
            break;
            
        case AST_NODE_WHILE_LOOP:
            node->data.while_loop.condition = macro_hygenic_expand_recursive(
                node->data.while_loop.condition, mapping, prefix);
            node->data.while_loop.body = macro_hygenic_expand_recursive(
                node->data.while_loop.body, mapping, prefix);
            break;
            
        case AST_NODE_FOR_LOOP:
            node->data.for_loop.collection = macro_hygenic_expand_recursive(
                node->data.for_loop.collection, mapping, prefix);
            node->data.for_loop.body = macro_hygenic_expand_recursive(
                node->data.for_loop.body, mapping, prefix);
            break;
            
        case AST_NODE_BLOCK:
            for (size_t i = 0; i < node->data.block.statement_count; i++) {
                node->data.block.statements[i] = macro_hygenic_expand_recursive(
                    node->data.block.statements[i], mapping, prefix);
            }
            break;
            
        case AST_NODE_RETURN:
            if (node->data.return_statement.value) {
                node->data.return_statement.value = macro_hygenic_expand_recursive(
                    node->data.return_statement.value, mapping, prefix);
            }
            break;
            
        case AST_NODE_THROW:
            if (node->data.throw_statement.value) {
                node->data.throw_statement.value = macro_hygenic_expand_recursive(
                    node->data.throw_statement.value, mapping, prefix);
            }
            break;
            
        case AST_NODE_TRY_CATCH:
            node->data.try_catch.try_block = macro_hygenic_expand_recursive(
                node->data.try_catch.try_block, mapping, prefix);
            node->data.try_catch.catch_block = macro_hygenic_expand_recursive(
                node->data.try_catch.catch_block, mapping, prefix);
            node->data.try_catch.finally_block = macro_hygenic_expand_recursive(
                node->data.try_catch.finally_block, mapping, prefix);
            break;
            
        case AST_NODE_ASYNC_FUNCTION:
            if (node->data.async_function_definition.body) {
                node->data.async_function_definition.body = macro_hygenic_expand_recursive(
                    node->data.async_function_definition.body, mapping, prefix);
            }
            break;
            
        case AST_NODE_AWAIT:
            node->data.await_expression.expression = macro_hygenic_expand_recursive(
                node->data.await_expression.expression, mapping, prefix);
            break;
            
        case AST_NODE_PROMISE:
            node->data.promise_creation.expression = macro_hygenic_expand_recursive(
                node->data.promise_creation.expression, mapping, prefix);
            break;
            
        case AST_NODE_COMPTIME_EVAL:
            node->data.comptime_eval.expression = macro_hygenic_expand_recursive(
                node->data.comptime_eval.expression, mapping, prefix);
            break;
            
        default:
            // No child nodes to expand
            break;
    }
    
    return node;
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
