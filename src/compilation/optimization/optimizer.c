#include "../../../include/compilation/optimization/optimizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Create optimization context
OptimizationContext* optimizer_create_context(ASTNode* ast, OptimizationLevel level) {
    if (!ast) return NULL;
    
    OptimizationContext* context = malloc(sizeof(OptimizationContext));
    if (!context) return NULL;
    
    context->ast = ast;
    context->level = level;
    context->debug_mode = 0;
    
    // Initialize stats
    memset(&context->stats, 0, sizeof(OptimizationStats));
    
    return context;
}

// Free optimization context
void optimizer_free_context(OptimizationContext* context) {
    if (context) {
        free(context);
    }
}

// Main optimization pipeline
int optimizer_optimize(OptimizationContext* context) {
    if (!context || !context->ast) return 0;
    
    if (context->debug_mode) {
        printf("Starting optimization pipeline (level: %d)\n", context->level);
    }
    
    // Run optimization passes based on level
    if (context->level >= OPTIMIZATION_BASIC) {
        // Dead code elimination
        if (!optimizer_dead_code_elimination(context)) {
            if (context->debug_mode) {
                printf("Warning: Dead code elimination failed\n");
            }
        }
        
        // Constant folding
        if (!optimizer_constant_folding(context)) {
            if (context->debug_mode) {
                printf("Warning: Constant folding failed\n");
            }
        }
    }
    
    if (context->level >= OPTIMIZATION_AGGRESSIVE) {
        // Function inlining
        if (!optimizer_function_inlining(context)) {
            if (context->debug_mode) {
                printf("Warning: Function inlining failed\n");
            }
        }
        
        // Variable optimization
        if (!optimizer_variable_optimization(context)) {
            if (context->debug_mode) {
                printf("Warning: Variable optimization failed\n");
            }
        }
    }
    
    if (context->debug_mode) {
        optimizer_print_stats(context);
    }
    
    return 1;
}

// Dead code elimination pass
int optimizer_dead_code_elimination(OptimizationContext* context) {
    if (!context || !context->ast) return 0;
    
    int eliminated = 0;
    
    // For now, implement basic dead code elimination
    // This is a simplified version - in a full implementation, we'd do:
    // 1. Build a use-def graph
    // 2. Identify unused variables
    // 3. Remove unused statements
    
    if (context->debug_mode) {
        printf("Running dead code elimination...\n");
    }
    
    // TODO: Implement full dead code elimination
    // For now, just count as if we did something
    context->stats.dead_code_eliminated = eliminated;
    
    return 1;
}

// Constant folding pass
int optimizer_constant_folding(OptimizationContext* context) {
    if (!context || !context->ast) return 0;
    
    int folded = 0;
    
    if (context->debug_mode) {
        printf("Running constant folding...\n");
    }
    
    // Traverse AST and fold constant expressions
    folded = optimizer_fold_constants_recursive(context->ast, context);
    
    context->stats.constants_folded = folded;
    context->stats.total_optimizations += folded;
    
    return 1;
}

// Recursively fold constants in AST
int optimizer_fold_constants_recursive(ASTNode* node, OptimizationContext* context) {
    if (!node) return 0;
    
    int folded = 0;
    
    switch (node->type) {
        case AST_NODE_BINARY_OP:
            // Check if both operands are constants
            if (optimizer_is_constant_expression(node->data.binary.left) &&
                optimizer_is_constant_expression(node->data.binary.right)) {
                
                ASTNode* folded_node = optimizer_fold_constant(node);
                if (folded_node) {
                    // Replace the binary operation with the folded constant
                    *node = *folded_node;
                    free(folded_node);
                    folded++;
                    
                    if (context->debug_mode) {
                        printf("Folded constant expression\n");
                    }
                }
            }
            break;
            
        case AST_NODE_BLOCK:
            // Process all statements in the block
            for (int i = 0; i < node->data.block.statement_count; i++) {
                folded += optimizer_fold_constants_recursive(node->data.block.statements[i], context);
            }
            break;
            
        case AST_NODE_IF_STATEMENT:
            // Fold condition if it's constant
            if (optimizer_is_constant_expression(node->data.if_statement.condition)) {
                ASTNode* folded_condition = optimizer_fold_constant(node->data.if_statement.condition);
                if (folded_condition) {
                    *node->data.if_statement.condition = *folded_condition;
                    free(folded_condition);
                    folded++;
                }
            }
            // Process then and else blocks
            folded += optimizer_fold_constants_recursive(node->data.if_statement.then_block, context);
            if (node->data.if_statement.else_block) {
                folded += optimizer_fold_constants_recursive(node->data.if_statement.else_block, context);
            }
            break;
            
        case AST_NODE_WHILE_LOOP:
            // Fold condition if it's constant
            if (optimizer_is_constant_expression(node->data.while_loop.condition)) {
                ASTNode* folded_condition = optimizer_fold_constant(node->data.while_loop.condition);
                if (folded_condition) {
                    *node->data.while_loop.condition = *folded_condition;
                    free(folded_condition);
                    folded++;
                }
            }
            // Process body
            folded += optimizer_fold_constants_recursive(node->data.while_loop.body, context);
            break;
            
        case AST_NODE_FOR_LOOP:
            // Process collection and body
            folded += optimizer_fold_constants_recursive(node->data.for_loop.collection, context);
            folded += optimizer_fold_constants_recursive(node->data.for_loop.body, context);
            break;
            
        case AST_NODE_FUNCTION:
            // Process function body
            if (node->data.function_definition.body) {
                folded += optimizer_fold_constants_recursive(node->data.function_definition.body, context);
            }
            break;
            
        case AST_NODE_VARIABLE_DECLARATION:
            // Fold initial value if it's a constant expression
            if (node->data.variable_declaration.initial_value) {
                folded += optimizer_fold_constants_recursive(node->data.variable_declaration.initial_value, context);
            }
            break;
            
        case AST_NODE_ASSIGNMENT:
            // Fold assigned value if it's a constant expression
            folded += optimizer_fold_constants_recursive(node->data.assignment.value, context);
            break;
            
        case AST_NODE_FUNCTION_CALL:
            // Fold arguments if they're constant expressions
            for (int i = 0; i < node->data.function_call.argument_count; i++) {
                folded += optimizer_fold_constants_recursive(node->data.function_call.arguments[i], context);
            }
            break;
            
        default:
            // For other node types, no constant folding needed
            break;
    }
    
    return folded;
}

// Check if a node is a constant expression
int optimizer_is_constant_expression(ASTNode* node) {
    if (!node) return 0;
    
    switch (node->type) {
        case AST_NODE_NUMBER:
        case AST_NODE_STRING:
        case AST_NODE_BOOL:
            return 1;
            
        case AST_NODE_BINARY_OP:
            // Both operands must be constant
            return optimizer_is_constant_expression(node->data.binary.left) &&
                   optimizer_is_constant_expression(node->data.binary.right);
                   
        case AST_NODE_UNARY_OP:
            // Operand must be constant
            return optimizer_is_constant_expression(node->data.unary.operand);
            
        default:
            return 0;
    }
}

// Fold a constant expression to its result
ASTNode* optimizer_fold_constant(ASTNode* node) {
    if (!node) return NULL;
    
    switch (node->type) {
        case AST_NODE_NUMBER:
        case AST_NODE_STRING:
        case AST_NODE_BOOL:
            // Already a constant, return a copy
            // Create a copy of the constant node
            switch (node->type) {
                case AST_NODE_NUMBER:
                    return ast_create_number(node->data.number_value, 0, 0);
                case AST_NODE_STRING:
                    return ast_create_string(node->data.string_value, 0, 0);
                case AST_NODE_BOOL:
                    return ast_create_bool(node->data.bool_value, 0, 0);
                default:
                    return NULL;
            }
            
        case AST_NODE_BINARY_OP:
            return optimizer_fold_binary_operation(node);
            
        case AST_NODE_UNARY_OP:
            return optimizer_fold_unary_operation(node);
            
        default:
            return NULL;
    }
}

// Fold binary operations
ASTNode* optimizer_fold_binary_operation(ASTNode* node) {
    if (!node || node->type != AST_NODE_BINARY_OP) return NULL;
    
    ASTNode* left = node->data.binary.left;
    ASTNode* right = node->data.binary.right;
    BinaryOperator op = node->data.binary.op;
    
    // Only fold numeric operations for now
    if (left->type != AST_NODE_NUMBER || right->type != AST_NODE_NUMBER) {
        return NULL;
    }
    
    double left_val = left->data.number_value;
    double right_val = right->data.number_value;
    double result = 0;
    
    switch (op) {
        case OP_ADD:
            result = left_val + right_val;
            break;
        case OP_SUBTRACT:
            result = left_val - right_val;
            break;
        case OP_MULTIPLY:
            result = left_val * right_val;
            break;
        case OP_DIVIDE:
            if (right_val == 0) return NULL; // Division by zero
            result = left_val / right_val;
            break;
        case OP_MODULO:
            if (right_val == 0) return NULL; // Modulo by zero
            result = (double)((int)left_val % (int)right_val);
            break;
        case OP_POWER:
            result = 1;
            for (int i = 0; i < (int)right_val; i++) {
                result *= left_val;
            }
            break;
        case OP_EQUAL:
            return ast_create_bool(left_val == right_val, 0, 0);
        case OP_NOT_EQUAL:
            return ast_create_bool(left_val != right_val, 0, 0);
        case OP_LESS_THAN:
            return ast_create_bool(left_val < right_val, 0, 0);
        case OP_GREATER_THAN:
            return ast_create_bool(left_val > right_val, 0, 0);
        case OP_LESS_EQUAL:
            return ast_create_bool(left_val <= right_val, 0, 0);
        case OP_GREATER_EQUAL:
            return ast_create_bool(left_val >= right_val, 0, 0);
        case OP_LOGICAL_AND:
            return ast_create_bool(left_val != 0 && right_val != 0, 0, 0);
        case OP_LOGICAL_OR:
            return ast_create_bool(left_val != 0 || right_val != 0, 0, 0);
        default:
            return NULL; // Can't fold this operation
    }
    
    return ast_create_number(result, 0, 0);
}

// Fold unary operations
ASTNode* optimizer_fold_unary_operation(ASTNode* node) {
    if (!node || node->type != AST_NODE_UNARY_OP) return NULL;
    
    ASTNode* operand = node->data.unary.operand;
    UnaryOperator op = node->data.unary.op;
    
    if (operand->type != AST_NODE_NUMBER) {
        return NULL;
    }
    
    double val = operand->data.number_value;
    
    switch (op) {
        case OP_NEGATIVE:
            return ast_create_number(-val, 0, 0);
        case OP_LOGICAL_NOT:
            return ast_create_bool(val == 0, 0, 0);
        default:
            return NULL;
    }
}

// Function inlining pass (placeholder)
int optimizer_function_inlining(OptimizationContext* context) {
    if (!context) return 0;
    
    if (context->debug_mode) {
        printf("Running function inlining...\n");
    }
    
    // TODO: Implement function inlining
    context->stats.functions_inlined = 0;
    
    return 1;
}

// Variable optimization pass (placeholder)
int optimizer_variable_optimization(OptimizationContext* context) {
    if (!context) return 0;
    
    if (context->debug_mode) {
        printf("Running variable optimization...\n");
    }
    
    // TODO: Implement variable optimization
    context->stats.variables_optimized = 0;
    
    return 1;
}

// Check if code is dead (placeholder)
int optimizer_is_dead_code(ASTNode* node, OptimizationContext* context) {
    if (!node || !context) return 0;
    
    // TODO: Implement dead code detection
    return 0;
}

// Print optimization statistics
void optimizer_print_stats(OptimizationContext* context) {
    if (!context) return;
    
    printf("\n=== Optimization Statistics ===\n");
    printf("Dead code eliminated: %d\n", context->stats.dead_code_eliminated);
    printf("Constants folded: %d\n", context->stats.constants_folded);
    printf("Functions inlined: %d\n", context->stats.functions_inlined);
    printf("Variables optimized: %d\n", context->stats.variables_optimized);
    printf("Total optimizations: %d\n", context->stats.total_optimizations);
    printf("===============================\n\n");
}

// Print debug information
void optimizer_print_debug_info(OptimizationContext* context) {
    if (!context) return;
    
    printf("Optimization Context:\n");
    printf("  Level: %d\n", context->level);
    printf("  Debug mode: %s\n", context->debug_mode ? "enabled" : "disabled");
    printf("  AST root type: %d\n", context->ast ? context->ast->type : -1);
}
