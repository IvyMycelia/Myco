#ifndef MYCO_OPTIMIZER_H
#define MYCO_OPTIMIZER_H

#include "../compiler.h"
#include "../../core/ast.h"

// Use the OptimizationLevel from compiler.h

// Optimization statistics
typedef struct {
    int dead_code_eliminated;
    int constants_folded;
    int functions_inlined;
    int variables_optimized;
    int total_optimizations;
} OptimizationStats;

// Optimization context
typedef struct {
    ASTNode* ast;
    OptimizationLevel level;
    OptimizationStats stats;
    int debug_mode;
} OptimizationContext;

// Optimization functions
OptimizationContext* optimizer_create_context(ASTNode* ast, OptimizationLevel level);
void optimizer_free_context(OptimizationContext* context);

// Main optimization pipeline
int optimizer_optimize(OptimizationContext* context);

// Individual optimization passes
int optimizer_dead_code_elimination(OptimizationContext* context);
int optimizer_constant_folding(OptimizationContext* context);
int optimizer_function_inlining(OptimizationContext* context);
int optimizer_variable_optimization(OptimizationContext* context);

// Utility functions
int optimizer_is_dead_code(ASTNode* node, OptimizationContext* context);
int optimizer_can_fold_constant(ASTNode* node);
ASTNode* optimizer_fold_constant(ASTNode* node);
int optimizer_is_constant_expression(ASTNode* node);
int optimizer_fold_constants_recursive(ASTNode* node, OptimizationContext* context);
ASTNode* optimizer_fold_binary_operation(ASTNode* node);
ASTNode* optimizer_fold_unary_operation(ASTNode* node);

// Statistics and debugging
void optimizer_print_stats(OptimizationContext* context);
void optimizer_print_debug_info(OptimizationContext* context);

#endif // MYCO_OPTIMIZER_H
