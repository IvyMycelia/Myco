/**
 * @file loop_analyzer.c
 * @brief Loop analysis implementation for advanced optimization
 */

#include "../../include/core/optimization/loop_analyzer.h"
#include "../../include/core/ast.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ============================================================================
// LOOP ANALYZER CONTEXT MANAGEMENT
// ============================================================================

LoopAnalyzer* loop_analyzer_create(void) {
    LoopAnalyzer* analyzer = malloc(sizeof(LoopAnalyzer));
    if (!analyzer) {
        return NULL;
    }
    
    analyzer->loops = NULL;
    analyzer->loop_count = 0;
    analyzer->loop_capacity = 0;
    analyzer->is_analyzing = 0;
    analyzer->current_nesting = 0;
    
    return analyzer;
}

void loop_analyzer_free(LoopAnalyzer* analyzer) {
    if (!analyzer) {
        return;
    }
    
    if (analyzer->loops) {
        for (size_t i = 0; i < analyzer->loop_count; i++) {
            if (analyzer->loops[i].induction_vars) {
                free(analyzer->loops[i].induction_vars);
            }
        }
        free(analyzer->loops);
    }
    
    free(analyzer);
}

// ============================================================================
// LOOP DETECTION AND ANALYSIS
// ============================================================================

static int analyze_statement_for_loops(LoopAnalyzer* analyzer, ASTNode* node, size_t nesting_level);

int loop_analyzer_analyze_function(LoopAnalyzer* analyzer, ASTNode* function_node) {
    if (!analyzer || !function_node || function_node->type != AST_NODE_FUNCTION) {
        return 0;
    }
    
    analyzer->is_analyzing = 1;
    analyzer->current_nesting = 0;
    
    // Analyze function body for loops
    if (function_node->data.function_definition.body) {
        if (!analyze_statement_for_loops(analyzer, function_node->data.function_definition.body, 0)) {
            analyzer->is_analyzing = 0;
            return 0;
        }
    }
    
    analyzer->is_analyzing = 0;
    return 1;
}

static int analyze_statement_for_loops(LoopAnalyzer* analyzer, ASTNode* node, size_t nesting_level) {
    if (!node || !analyzer) {
        return 1;
    }
    
    switch (node->type) {
        case AST_NODE_WHILE_LOOP:
        case AST_NODE_FOR_LOOP:
            // Analyze this loop
            if (!loop_analyzer_analyze_loop(analyzer, node, nesting_level)) {
                return 0;
            }
            break;
            
        case AST_NODE_IF_STATEMENT:
            // Analyze if statement branches
            if (node->data.if_statement.then_block) {
                analyze_statement_for_loops(analyzer, node->data.if_statement.then_block, nesting_level);
            }
            if (node->data.if_statement.else_block) {
                analyze_statement_for_loops(analyzer, node->data.if_statement.else_block, nesting_level);
            }
            break;
            
        case AST_NODE_BLOCK:
            // Analyze block statements
            for (size_t i = 0; i < node->data.block.statement_count; i++) {
                analyze_statement_for_loops(analyzer, node->data.block.statements[i], nesting_level);
            }
            break;
            
        default:
            break;
    }
    
    return 1;
}

LoopAnalysis* loop_analyzer_analyze_loop(LoopAnalyzer* analyzer, 
                                         ASTNode* loop_node, 
                                         size_t nesting_level) {
    if (!analyzer || !loop_node) {
        return NULL;
    }
    
    // Resize loops array if needed
    if (analyzer->loop_count >= analyzer->loop_capacity) {
        size_t new_capacity = analyzer->loop_capacity * 2;
        if (new_capacity < 8) new_capacity = 8;
        
        LoopAnalysis* new_loops = realloc(analyzer->loops, new_capacity * sizeof(LoopAnalysis));
        if (!new_loops) {
            return NULL;
        }
        
        analyzer->loops = new_loops;
        analyzer->loop_capacity = new_capacity;
    }
    
    LoopAnalysis* analysis = &analyzer->loops[analyzer->loop_count];
    analyzer->loop_count++;
    
    // Initialize analysis
    analysis->loop_node = loop_node;
    analysis->is_simple = 1;
    analysis->is_countable = 0;
    analysis->trip_count = -1;
    analysis->is_innermost = 1; // Will be updated later
    analysis->is_outermost = (nesting_level == 0);
    analysis->nesting_level = nesting_level;
    analysis->induction_vars = NULL;
    analysis->induction_var_count = 0;
    analysis->optimization_flags = 0;
    analysis->has_loop_carried_deps = 0;
    analysis->has_early_exit = 0;
    analysis->has_function_calls = 0;
    analysis->has_side_effects = 0;
    
    // Determine loop type
    switch (loop_node->type) {
        case AST_NODE_WHILE_LOOP:
            analysis->loop_type = LOOP_TYPE_WHILE;
            break;
        case AST_NODE_FOR_LOOP:
            analysis->loop_type = LOOP_TYPE_FOR;
            break;
        default:
            analysis->loop_type = LOOP_TYPE_UNKNOWN;
            break;
    }
    
    // Detect induction variables
    analysis->induction_vars = malloc(8 * sizeof(InductionVariable));
    if (analysis->induction_vars) {
        analysis->induction_var_count = loop_analyzer_detect_induction_variables(
            analyzer, loop_node, analysis->induction_vars, 8);
    }
    
    // Compute trip count if possible
    if (analysis->induction_var_count > 0) {
        analysis->trip_count = loop_analyzer_compute_trip_count(
            analyzer, loop_node, analysis->induction_vars, analysis->induction_var_count);
        analysis->is_countable = (analysis->trip_count > 0);
    }
    
    // Check for dependencies
    analysis->has_loop_carried_deps = loop_analyzer_has_loop_carried_dependencies(
        analyzer, loop_node);
    
    // Determine optimization opportunities
    analysis->optimization_flags = loop_analyzer_determine_optimizations(analyzer, analysis);
    
    return analysis;
}

// ============================================================================
// INDUCTION VARIABLE DETECTION
// ============================================================================

static int is_induction_variable_update(ASTNode* node, const char* var_name);

size_t loop_analyzer_detect_induction_variables(LoopAnalyzer* analyzer,
                                                ASTNode* loop_node,
                                                InductionVariable* induction_vars,
                                                size_t max_count) {
    if (!analyzer || !loop_node || !induction_vars || max_count == 0) {
        return 0;
    }
    
    size_t count = 0;
    
    // For while loops, look for variable updates in the body
    if (loop_node->type == AST_NODE_WHILE_LOOP) {
        // Simple heuristic: look for assignment patterns
        // This is a simplified implementation
        if (count < max_count) {
            induction_vars[count].variable_name = NULL; // Would need to extract from AST
            induction_vars[count].initial_value = NULL;
            induction_vars[count].update_expression = NULL;
            induction_vars[count].is_increasing = 1;
            induction_vars[count].is_constant_step = 0;
            induction_vars[count].step_value = 0.0;
            induction_vars[count].bound_expression = loop_node->data.while_loop.condition;
            induction_vars[count].is_linear = 0;
            count++;
        }
    }
    // For for loops, extract from the loop structure
    else if (loop_node->type == AST_NODE_FOR_LOOP) {
        // This would need to be implemented based on the actual for loop structure
        // For now, create a placeholder
        if (count < max_count) {
            induction_vars[count].variable_name = NULL;
            induction_vars[count].initial_value = NULL;
            induction_vars[count].update_expression = NULL;
            induction_vars[count].is_increasing = 1;
            induction_vars[count].is_constant_step = 1;
            induction_vars[count].step_value = 1.0;
            induction_vars[count].bound_expression = NULL;
            induction_vars[count].is_linear = 1;
            count++;
        }
    }
    
    return count;
}

// ============================================================================
// TRIP COUNT COMPUTATION
// ============================================================================

int loop_analyzer_compute_trip_count(LoopAnalyzer* analyzer,
                                     ASTNode* loop_node,
                                     InductionVariable* induction_vars,
                                     size_t var_count) {
    if (!analyzer || !loop_node || !induction_vars || var_count == 0) {
        return -1;
    }
    
    // Simple trip count computation for linear induction variables
    for (size_t i = 0; i < var_count; i++) {
        if (induction_vars[i].is_linear && induction_vars[i].is_constant_step) {
            // For now, return a placeholder trip count
            // In a real implementation, this would analyze the loop bounds
            return 100; // Placeholder
        }
    }
    
    return -1;
}

// ============================================================================
// DEPENDENCY ANALYSIS
// ============================================================================

int loop_analyzer_has_loop_carried_dependencies(LoopAnalyzer* analyzer,
                                                ASTNode* loop_node) {
    if (!analyzer || !loop_node) {
        return 0;
    }
    
    // Simplified dependency analysis
    // In a real implementation, this would perform data flow analysis
    // to detect if values from one iteration are used in subsequent iterations
    
    return 0; // Placeholder: assume no dependencies for now
}

// ============================================================================
// OPTIMIZATION OPPORTUNITY DETECTION
// ============================================================================

int loop_analyzer_determine_optimizations(LoopAnalyzer* analyzer,
                                          LoopAnalysis* analysis) {
    if (!analyzer || !analysis) {
        return 0;
    }
    
    int flags = 0;
    
    // Check if loop can be unrolled
    if (loop_analyzer_can_unroll(analyzer, analysis)) {
        flags |= LOOP_OPT_UNROLL;
    }
    
    // Check if loop can be vectorized
    if (loop_analyzer_can_vectorize(analyzer, analysis)) {
        flags |= LOOP_OPT_VECTORIZE;
    }
    
    // Check if loop can be fused (simplified)
    if (analysis->is_simple && !analysis->has_loop_carried_deps) {
        flags |= LOOP_OPT_FUSE;
    }
    
    // Check if strength reduction is possible
    if (analysis->induction_var_count > 0) {
        flags |= LOOP_OPT_STRENGTH_REDUCE;
    }
    
    return flags;
}

int loop_analyzer_can_vectorize(LoopAnalyzer* analyzer, LoopAnalysis* analysis) {
    if (!analyzer || !analysis) {
        return 0;
    }
    
    // Simplified vectorization check
    // In a real implementation, this would check for:
    // - Simple loop structure
    // - No loop-carried dependencies
    // - Vectorizable operations
    // - Suitable data types
    
    return analysis->is_simple && 
           !analysis->has_loop_carried_deps && 
           analysis->is_countable;
}

int loop_analyzer_can_unroll(LoopAnalyzer* analyzer, LoopAnalysis* analysis) {
    if (!analyzer || !analysis) {
        return 0;
    }
    
    // Check if loop is suitable for unrolling
    return analysis->is_countable && 
           analysis->trip_count > 0 && 
           analysis->trip_count <= 100; // Reasonable unroll limit
}

int loop_analyzer_can_fuse(LoopAnalyzer* analyzer,
                           LoopAnalysis* analysis1,
                           LoopAnalysis* analysis2) {
    if (!analyzer || !analysis1 || !analysis2) {
        return 0;
    }
    
    // Check if two loops can be fused
    // In a real implementation, this would check for:
    // - Same nesting level
    // - Compatible loop bounds
    // - No dependencies between loops
    // - Similar loop structures
    
    return analysis1->nesting_level == analysis2->nesting_level &&
           analysis1->is_simple && analysis2->is_simple &&
           !analysis1->has_loop_carried_deps && !analysis2->has_loop_carried_deps;
}

// ============================================================================
// STATISTICS
// ============================================================================

void loop_analyzer_get_stats(LoopAnalyzer* analyzer,
                             size_t* total_loops,
                             size_t* vectorizable_loops,
                             size_t* unrollable_loops,
                             size_t* fusible_loops) {
    if (!analyzer || !total_loops || !vectorizable_loops || !unrollable_loops || !fusible_loops) {
        return;
    }
    
    *total_loops = analyzer->loop_count;
    *vectorizable_loops = 0;
    *unrollable_loops = 0;
    *fusible_loops = 0;
    
    for (size_t i = 0; i < analyzer->loop_count; i++) {
        LoopAnalysis* analysis = &analyzer->loops[i];
        
        if (analysis->optimization_flags & LOOP_OPT_VECTORIZE) {
            (*vectorizable_loops)++;
        }
        if (analysis->optimization_flags & LOOP_OPT_UNROLL) {
            (*unrollable_loops)++;
        }
        if (analysis->optimization_flags & LOOP_OPT_FUSE) {
            (*fusible_loops)++;
        }
    }
}
