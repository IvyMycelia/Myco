#include "../../../include/compilation/optimization/optimizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
    
    if (context->debug_mode) {
        printf("Running dead code elimination...\n");
    }
    
    // Implement aggressive dead code elimination
    eliminated = optimizer_eliminate_dead_code_recursive(context->ast, context);
    
    context->stats.dead_code_eliminated = eliminated;
    context->stats.total_optimizations += eliminated;
    
    return 1;
}

// Recursively eliminate dead code
int optimizer_eliminate_dead_code_recursive(ASTNode* node, OptimizationContext* context) {
    if (!node) return 0;
    
    int eliminated = 0;
    
    switch (node->type) {
        case AST_NODE_BLOCK:
            // Remove unreachable code after return/break/continue
            for (size_t i = 0; i < node->data.block.statement_count; i++) {
                ASTNode* stmt = node->data.block.statements[i];
                if (stmt && (stmt->type == AST_NODE_RETURN || 
                            stmt->type == AST_NODE_BREAK || 
                            stmt->type == AST_NODE_CONTINUE)) {
                    // Remove all statements after this one
                    for (size_t j = i + 1; j < node->data.block.statement_count; j++) {
                        if (node->data.block.statements[j]) {
                            eliminated++;
                        }
                    }
                    node->data.block.statement_count = i + 1;
                    break;
                }
                eliminated += optimizer_eliminate_dead_code_recursive(stmt, context);
            }
            break;
            
        case AST_NODE_IF_STATEMENT:
            // If condition is always false, remove then block
            if (optimizer_is_constant_expression(node->data.if_statement.condition)) {
                ASTNode* folded = optimizer_fold_constant(node->data.if_statement.condition);
                if (folded && folded->type == AST_NODE_BOOL && !folded->data.bool_value) {
                    // Condition is always false, remove then block
                    eliminated++;
                }
                if (folded) free(folded);
            }
            eliminated += optimizer_eliminate_dead_code_recursive(node->data.if_statement.then_block, context);
            if (node->data.if_statement.else_block) {
                eliminated += optimizer_eliminate_dead_code_recursive(node->data.if_statement.else_block, context);
            }
            break;
            
        case AST_NODE_WHILE_LOOP:
            // If condition is always false, remove loop body
            if (optimizer_is_constant_expression(node->data.while_loop.condition)) {
                ASTNode* folded = optimizer_fold_constant(node->data.while_loop.condition);
                if (folded && folded->type == AST_NODE_BOOL && !folded->data.bool_value) {
                    // Condition is always false, remove body
                    eliminated++;
                }
                if (folded) free(folded);
            }
            eliminated += optimizer_eliminate_dead_code_recursive(node->data.while_loop.body, context);
            break;
            
        case AST_NODE_FOR_LOOP:
            eliminated += optimizer_eliminate_dead_code_recursive(node->data.for_loop.collection, context);
            eliminated += optimizer_eliminate_dead_code_recursive(node->data.for_loop.body, context);
            break;
            
        case AST_NODE_FUNCTION:
            if (node->data.function_definition.body) {
                eliminated += optimizer_eliminate_dead_code_recursive(node->data.function_definition.body, context);
            }
            break;
            
        default:
            // For other node types, no dead code elimination needed
            break;
    }
    
    return eliminated;
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
            for (size_t i = 0; i < node->data.block.statement_count; i++) {
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
            for (size_t i = 0; i < node->data.function_call.argument_count; i++) {
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

// Function inlining pass
int optimizer_function_inlining(OptimizationContext* context) {
    if (!context) return 0;
    
    if (context->debug_mode) {
        printf("Running function inlining...\n");
    }
    
    int inlined = 0;
    inlined = optimizer_inline_functions_recursive(context->ast, context);
    
    context->stats.functions_inlined = inlined;
    context->stats.total_optimizations += inlined;
    
    return 1;
}

// Recursively inline small functions
int optimizer_inline_functions_recursive(ASTNode* node, OptimizationContext* context) {
    if (!node) return 0;
    
    int inlined = 0;
    
    switch (node->type) {
        case AST_NODE_FUNCTION_CALL:
            // Check if this is a simple function that can be inlined
            if (optimizer_can_inline_function(node)) {
                ASTNode* inlined_body = optimizer_inline_function_call(node, context);
                if (inlined_body) {
                    // Replace function call with inlined body
                    *node = *inlined_body;
                    free(inlined_body);
                    inlined++;
                }
            }
            break;
            
        case AST_NODE_BLOCK:
            for (size_t i = 0; i < node->data.block.statement_count; i++) {
                inlined += optimizer_inline_functions_recursive(node->data.block.statements[i], context);
            }
            break;
            
        case AST_NODE_IF_STATEMENT:
            inlined += optimizer_inline_functions_recursive(node->data.if_statement.then_block, context);
            if (node->data.if_statement.else_block) {
                inlined += optimizer_inline_functions_recursive(node->data.if_statement.else_block, context);
            }
            break;
            
        case AST_NODE_WHILE_LOOP:
            inlined += optimizer_inline_functions_recursive(node->data.while_loop.body, context);
            break;
            
        case AST_NODE_FOR_LOOP:
            inlined += optimizer_inline_functions_recursive(node->data.for_loop.body, context);
            break;
            
        default:
            break;
    }
    
    return inlined;
}

// Check if a function can be inlined
int optimizer_can_inline_function(ASTNode* call_node) {
    if (!call_node || call_node->type != AST_NODE_FUNCTION_CALL) return 0;
    
    // Only inline simple functions
    const char* func_name = call_node->data.function_call.function_name;
    if (!func_name) return 0;
    
    // Inline simple math functions
    if (strcmp(func_name, "sqrt") == 0 || strcmp(func_name, "abs") == 0 || 
        strcmp(func_name, "sin") == 0 || strcmp(func_name, "cos") == 0) {
        return 1;
    }
    
    // Inline simple string functions
    if (strcmp(func_name, "length") == 0 || strcmp(func_name, "upper") == 0 || 
        strcmp(func_name, "lower") == 0) {
        return 1;
    }
    
    return 0;
}

// Inline a function call
ASTNode* optimizer_inline_function_call(ASTNode* call_node, OptimizationContext* context) {
    if (!call_node || call_node->type != AST_NODE_FUNCTION_CALL) return NULL;
    
    const char* func_name = call_node->data.function_call.function_name;
    if (!func_name) return NULL;
    
    // For now, create optimized inline versions of common functions
    if (strcmp(func_name, "sqrt") == 0 && call_node->data.function_call.argument_count == 1) {
        // Inline sqrt(x) as direct C sqrt() call
        ASTNode* arg = call_node->data.function_call.arguments[0];
        if (arg && arg->type == AST_NODE_NUMBER) {
            double val = arg->data.number_value;
            return ast_create_number(sqrt(val), 0, 0);
        }
    }
    
    if (strcmp(func_name, "abs") == 0 && call_node->data.function_call.argument_count == 1) {
        // Inline abs(x) as direct C fabs() call
        ASTNode* arg = call_node->data.function_call.arguments[0];
        if (arg && arg->type == AST_NODE_NUMBER) {
            double val = arg->data.number_value;
            return ast_create_number(fabs(val), 0, 0);
        }
    }
    
    return NULL;
}

// Variable optimization pass
int optimizer_variable_optimization(OptimizationContext* context) {
    if (!context) return 0;
    
    if (context->debug_mode) {
        printf("Running variable optimization...\n");
    }
    
    int optimized = 0;
    optimized = optimizer_optimize_variables_recursive(context->ast, context);
    
    context->stats.variables_optimized = optimized;
    context->stats.total_optimizations += optimized;
    
    return 1;
}

// Recursively optimize variables
int optimizer_optimize_variables_recursive(ASTNode* node, OptimizationContext* context) {
    if (!node) return 0;
    
    int optimized = 0;
    
    switch (node->type) {
        case AST_NODE_VARIABLE_DECLARATION:
            // Optimize variable declarations
            if (node->data.variable_declaration.initial_value) {
                // If initial value is constant, optimize it
                if (optimizer_is_constant_expression(node->data.variable_declaration.initial_value)) {
                    ASTNode* folded = optimizer_fold_constant(node->data.variable_declaration.initial_value);
                    if (folded) {
                        *node->data.variable_declaration.initial_value = *folded;
                        free(folded);
                        optimized++;
                    }
                }
            }
            break;
            
        case AST_NODE_ASSIGNMENT:
            // Optimize assignments
            if (node->data.assignment.value) {
                if (optimizer_is_constant_expression(node->data.assignment.value)) {
                    ASTNode* folded = optimizer_fold_constant(node->data.assignment.value);
                    if (folded) {
                        *node->data.assignment.value = *folded;
                        free(folded);
                        optimized++;
                    }
                }
            }
            break;
            
        case AST_NODE_BLOCK:
            for (size_t i = 0; i < node->data.block.statement_count; i++) {
                optimized += optimizer_optimize_variables_recursive(node->data.block.statements[i], context);
            }
            break;
            
        case AST_NODE_IF_STATEMENT:
            optimized += optimizer_optimize_variables_recursive(node->data.if_statement.then_block, context);
            if (node->data.if_statement.else_block) {
                optimized += optimizer_optimize_variables_recursive(node->data.if_statement.else_block, context);
            }
            break;
            
        case AST_NODE_WHILE_LOOP:
            optimized += optimizer_optimize_variables_recursive(node->data.while_loop.body, context);
            break;
            
        case AST_NODE_FOR_LOOP:
            optimized += optimizer_optimize_variables_recursive(node->data.for_loop.body, context);
            break;
            
        case AST_NODE_FUNCTION:
            if (node->data.function_definition.body) {
                optimized += optimizer_optimize_variables_recursive(node->data.function_definition.body, context);
            }
            break;
            
        default:
            break;
    }
    
    return optimized;
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
