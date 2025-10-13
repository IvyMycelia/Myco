/**
 * @file escape_analysis.c
 * @brief Escape analysis implementation for zero-copy optimization
 */

#include "../../include/core/optimization/escape_analysis.h"
#include "../../include/core/interpreter/value_operations.h"
#include "../../include/core/ast.h"
#include <stdlib.h>
#include <string.h>

// ============================================================================
// ESCAPE ANALYSIS CONTEXT MANAGEMENT
// ============================================================================

EscapeAnalysisContext* escape_analysis_create(ASTNode* function_node) {
    if (!function_node || function_node->type != AST_NODE_FUNCTION) {
        return NULL;
    }
    
    EscapeAnalysisContext* context = malloc(sizeof(EscapeAnalysisContext));
    if (!context) {
        return NULL;
    }
    
    context->function_node = function_node;
    context->stack_allocated_values = NULL;
    context->stack_count = 0;
    context->stack_capacity = 0;
    context->escape_map = NULL;
    context->value_count = 0;
    
    return context;
}

void escape_analysis_free(EscapeAnalysisContext* context) {
    if (!context) {
        return;
    }
    
    if (context->stack_allocated_values) {
        free(context->stack_allocated_values);
    }
    
    if (context->escape_map) {
        free(context->escape_map);
    }
    
    free(context);
}

// ============================================================================
// ESCAPE ANALYSIS CORE
// ============================================================================

static int analyze_expression(EscapeAnalysisContext* context, ASTNode* node, size_t* value_id) {
    if (!node || !value_id) {
        return 0;
    }
    
    *value_id = context->value_count++;
    
    // Resize escape map if needed
    if (context->value_count > context->stack_capacity) {
        size_t new_capacity = context->stack_capacity * 2;
        if (new_capacity < 16) new_capacity = 16;
        
        int* new_map = realloc(context->escape_map, new_capacity * sizeof(int));
        if (!new_map) {
            return 0;
        }
        
        context->escape_map = new_map;
        context->stack_capacity = new_capacity;
    }
    
    // Initialize escape status
    context->escape_map[*value_id] = ESCAPE_ANALYSIS_UNKNOWN;
    
    switch (node->type) {
        case AST_NODE_NUMBER:
        case AST_NODE_STRING:
        case AST_NODE_BOOL:
        case AST_NODE_NULL:
            // Literals don't escape
            context->escape_map[*value_id] = ESCAPE_ANALYSIS_NO_ESCAPE;
            break;
            
        case AST_NODE_IDENTIFIER:
            // Check if identifier is returned or assigned to global
            context->escape_map[*value_id] = ESCAPE_ANALYSIS_UNKNOWN;
            break;
            
        case AST_NODE_BINARY_OP:
            // Analyze left and right operands
            {
                size_t left_id, right_id;
                if (analyze_expression(context, node->data.binary.left, &left_id) &&
                    analyze_expression(context, node->data.binary.right, &right_id)) {
                    // Binary operation result escapes if either operand escapes
                    if (context->escape_map[left_id] == ESCAPE_ANALYSIS_ESCAPES ||
                        context->escape_map[right_id] == ESCAPE_ANALYSIS_ESCAPES) {
                        context->escape_map[*value_id] = ESCAPE_ANALYSIS_ESCAPES;
                    } else {
                        context->escape_map[*value_id] = ESCAPE_ANALYSIS_NO_ESCAPE;
                    }
                }
            }
            break;
            
        case AST_NODE_FUNCTION_CALL:
            // Function call results may escape
            context->escape_map[*value_id] = ESCAPE_ANALYSIS_ESCAPES;
            break;
            
        case AST_NODE_ARRAY_LITERAL:
            // Array literals may escape
            context->escape_map[*value_id] = ESCAPE_ANALYSIS_ESCAPES;
            break;
            
        case AST_NODE_HASH_MAP_LITERAL:
            // Object literals may escape
            context->escape_map[*value_id] = ESCAPE_ANALYSIS_ESCAPES;
            break;
            
        default:
            context->escape_map[*value_id] = ESCAPE_ANALYSIS_UNKNOWN;
            break;
    }
    
    return 1;
}

static int analyze_statement(EscapeAnalysisContext* context, ASTNode* node) {
    if (!node) {
        return 1;
    }
    
    switch (node->type) {
        case AST_NODE_RETURN:
            // Return statement causes all values to escape
            if (node->data.return_statement.value) {
                size_t value_id;
                if (analyze_expression(context, node->data.return_statement.value, &value_id)) {
                    context->escape_map[value_id] = ESCAPE_ANALYSIS_ESCAPES;
                }
            }
            break;
            
        case AST_NODE_VARIABLE_DECLARATION:
            // Variable declaration
            if (node->data.variable_declaration.initial_value) {
                size_t value_id;
                analyze_expression(context, node->data.variable_declaration.initial_value, &value_id);
            }
            break;
            
        case AST_NODE_ASSIGNMENT:
            // Assignment may cause escape
            {
                size_t value_id;
                if (analyze_expression(context, node->data.assignment.value, &value_id)) {
                    context->escape_map[value_id] = ESCAPE_ANALYSIS_ESCAPES;
                }
            }
            break;
            
        case AST_NODE_IF_STATEMENT:
            // Analyze if statement
            if (node->data.if_statement.condition) {
                size_t value_id;
                analyze_expression(context, node->data.if_statement.condition, &value_id);
            }
            if (node->data.if_statement.then_block) {
                analyze_statement(context, node->data.if_statement.then_block);
            }
            if (node->data.if_statement.else_block) {
                analyze_statement(context, node->data.if_statement.else_block);
            }
            break;
            
        case AST_NODE_WHILE_LOOP:
            // Analyze while loop
            if (node->data.while_loop.condition) {
                size_t value_id;
                analyze_expression(context, node->data.while_loop.condition, &value_id);
            }
            if (node->data.while_loop.body) {
                analyze_statement(context, node->data.while_loop.body);
            }
            break;
            
        case AST_NODE_BLOCK:
            // Analyze block statements
            for (size_t i = 0; i < node->data.block.statement_count; i++) {
                analyze_statement(context, node->data.block.statements[i]);
            }
            break;
            
        default:
            break;
    }
    
    return 1;
}

int escape_analysis_analyze_function(EscapeAnalysisContext* context) {
    if (!context || !context->function_node) {
        return 0;
    }
    
    // Analyze function body
    if (context->function_node->data.function_definition.body) {
        if (!analyze_statement(context, context->function_node->data.function_definition.body)) {
            return 0;
        }
    }
    
    // Collect stack-allocatable values
    context->stack_count = 0;
    for (size_t i = 0; i < context->value_count; i++) {
        if (context->escape_map[i] == ESCAPE_ANALYSIS_NO_ESCAPE) {
            context->stack_count++;
        }
    }
    
    // Allocate stack values array
    if (context->stack_count > 0) {
        context->stack_allocated_values = malloc(context->stack_count * sizeof(Value*));
        if (!context->stack_allocated_values) {
            return 0;
        }
    }
    
    return 1;
}

// ============================================================================
// ESCAPE ANALYSIS QUERIES
// ============================================================================

EscapeAnalysisResult escape_analysis_check_escape(EscapeAnalysisContext* context, 
                                                  size_t value_id) {
    if (!context || value_id >= context->value_count) {
        return ESCAPE_ANALYSIS_UNKNOWN;
    }
    
    return (EscapeAnalysisResult)context->escape_map[value_id];
}

int escape_analysis_get_stack_values(EscapeAnalysisContext* context,
                                     Value*** values,
                                     size_t* count) {
    if (!context || !values || !count) {
        return 0;
    }
    
    *values = context->stack_allocated_values;
    *count = context->stack_count;
    return 1;
}

int escape_analysis_apply_sroa(EscapeAnalysisContext* context, ASTNode* ast_node) {
    if (!context || !ast_node) {
        return 0;
    }
    
    // TODO: Implement SROA optimization
    // This would replace aggregate values with individual scalar values
    // when possible to enable better optimization
    
    return 1;
}

int escape_analysis_can_eliminate(EscapeAnalysisContext* context, size_t value_id) {
    if (!context || value_id >= context->value_count) {
        return 0;
    }
    
    // Values that don't escape and are only used once can be eliminated
    return context->escape_map[value_id] == ESCAPE_ANALYSIS_NO_ESCAPE;
}

void escape_analysis_get_stats(EscapeAnalysisContext* context,
                               size_t* total_values,
                               size_t* escaped_values,
                               size_t* stack_values,
                               size_t* eliminated_values) {
    if (!context || !total_values || !escaped_values || !stack_values || !eliminated_values) {
        return;
    }
    
    *total_values = context->value_count;
    *escaped_values = 0;
    *stack_values = 0;
    *eliminated_values = 0;
    
    for (size_t i = 0; i < context->value_count; i++) {
        switch (context->escape_map[i]) {
            case ESCAPE_ANALYSIS_ESCAPES:
                (*escaped_values)++;
                break;
            case ESCAPE_ANALYSIS_NO_ESCAPE:
                (*stack_values)++;
                if (escape_analysis_can_eliminate(context, i)) {
                    (*eliminated_values)++;
                }
                break;
            case ESCAPE_ANALYSIS_UNKNOWN:
                break;
        }
    }
}
