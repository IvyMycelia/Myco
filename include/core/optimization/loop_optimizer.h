/**
 * @file loop_optimizer.h
 * @brief Loop optimization for performance enhancement
 * 
 * Implements loop unrolling, fusion, interchange, and strength reduction
 * to maximize performance of loop-heavy code.
 */

#ifndef MYCO_LOOP_OPTIMIZER_H
#define MYCO_LOOP_OPTIMIZER_H

#include "../ast.h"
#include "loop_analyzer.h"

/**
 * @brief Loop unrolling strategy
 */
typedef enum {
    LOOP_UNROLL_NONE = 0,         // No unrolling
    LOOP_UNROLL_2X = 2,           // 2x unrolling
    LOOP_UNROLL_4X = 4,           // 4x unrolling
    LOOP_UNROLL_8X = 8,           // 8x unrolling
    LOOP_UNROLL_FULL = -1         // Full unrolling
} LoopUnrollStrategy;

/**
 * @brief Loop optimization result
 */
typedef struct {
    ASTNode* optimized_loop;       // Optimized loop AST
    int optimization_applied;      // 1 if optimization was applied
    int performance_gain;          // Estimated performance gain percentage
    char* optimization_type;       // Type of optimization applied
} LoopOptimizationResult;

/**
 * @brief Loop optimizer context
 */
typedef struct {
    LoopAnalyzer* analyzer;        // Loop analyzer
    int max_unroll_factor;         // Maximum unroll factor
    int enable_fusion;             // Enable loop fusion
    int enable_interchange;        // Enable loop interchange
    int enable_strength_reduction; // Enable strength reduction
    size_t optimization_count;     // Number of optimizations applied
    size_t total_gain;             // Total performance gain
} LoopOptimizer;

/**
 * @brief Create loop optimizer
 * 
 * @param analyzer Loop analyzer to use
 * @return LoopOptimizer* New optimizer or NULL on failure
 */
LoopOptimizer* loop_optimizer_create(LoopAnalyzer* analyzer);

/**
 * @brief Free loop optimizer
 * 
 * @param optimizer Optimizer to free
 */
void loop_optimizer_free(LoopOptimizer* optimizer);

/**
 * @brief Optimize loop
 * 
 * @param optimizer Loop optimizer
 * @param analysis Loop analysis result
 * @return LoopOptimizationResult* Optimization result or NULL on failure
 */
LoopOptimizationResult* loop_optimizer_optimize_loop(LoopOptimizer* optimizer,
                                                     LoopAnalysis* analysis);

/**
 * @brief Unroll loop
 * 
 * @param optimizer Loop optimizer
 * @param analysis Loop analysis result
 * @param strategy Unrolling strategy
 * @return LoopOptimizationResult* Optimization result or NULL on failure
 */
LoopOptimizationResult* loop_optimizer_unroll_loop(LoopOptimizer* optimizer,
                                                   LoopAnalysis* analysis,
                                                   LoopUnrollStrategy strategy);

/**
 * @brief Fuse two loops
 * 
 * @param optimizer Loop optimizer
 * @param analysis1 First loop analysis
 * @param analysis2 Second loop analysis
 * @return LoopOptimizationResult* Optimization result or NULL on failure
 */
LoopOptimizationResult* loop_optimizer_fuse_loops(LoopOptimizer* optimizer,
                                                  LoopAnalysis* analysis1,
                                                  LoopAnalysis* analysis2);

/**
 * @brief Interchange loop order
 * 
 * @param optimizer Loop optimizer
 * @param analysis Loop analysis result
 * @return LoopOptimizationResult* Optimization result or NULL on failure
 */
LoopOptimizationResult* loop_optimizer_interchange_loops(LoopOptimizer* optimizer,
                                                         LoopAnalysis* analysis);

/**
 * @brief Apply strength reduction
 * 
 * @param optimizer Loop optimizer
 * @param analysis Loop analysis result
 * @return LoopOptimizationResult* Optimization result or NULL on failure
 */
LoopOptimizationResult* loop_optimizer_strength_reduce(LoopOptimizer* optimizer,
                                                       LoopAnalysis* analysis);

/**
 * @brief Optimize all loops in function
 * 
 * @param optimizer Loop optimizer
 * @param function_node Function AST node
 * @return int 1 on success, 0 on failure
 */
int loop_optimizer_optimize_function(LoopOptimizer* optimizer, ASTNode* function_node);

/**
 * @brief Get optimization statistics
 * 
 * @param optimizer Loop optimizer
 * @param optimization_count Number of optimizations applied
 * @param total_gain Total performance gain percentage
 * @param unroll_count Number of loops unrolled
 * @param fusion_count Number of loop fusions
 */
void loop_optimizer_get_stats(LoopOptimizer* optimizer,
                              size_t* optimization_count,
                              size_t* total_gain,
                              size_t* unroll_count,
                              size_t* fusion_count);

/**
 * @brief Set optimization preferences
 * 
 * @param optimizer Loop optimizer
 * @param max_unroll Maximum unroll factor
 * @param enable_fusion Enable loop fusion
 * @param enable_interchange Enable loop interchange
 * @param enable_strength_reduction Enable strength reduction
 */
void loop_optimizer_set_preferences(LoopOptimizer* optimizer,
                                    int max_unroll,
                                    int enable_fusion,
                                    int enable_interchange,
                                    int enable_strength_reduction);

/**
 * @brief Estimate performance gain
 * 
 * @param optimizer Loop optimizer
 * @param analysis Loop analysis result
 * @param optimization_type Type of optimization
 * @return int Estimated performance gain percentage
 */
int loop_optimizer_estimate_gain(LoopOptimizer* optimizer,
                                 LoopAnalysis* analysis,
                                 const char* optimization_type);

#endif // MYCO_LOOP_OPTIMIZER_H
