/**
 * @file loop_analyzer.h
 * @brief Loop analysis for advanced optimization
 * 
 * Detects loop structures, induction variables, and optimization opportunities
 * for aggressive loop optimization including vectorization and fusion.
 */

#ifndef MYCO_LOOP_ANALYZER_H
#define MYCO_LOOP_ANALYZER_H

#include "../ast.h"
#include <stddef.h>

/**
 * @brief Loop type classification
 */
typedef enum {
    LOOP_TYPE_FOR = 0,            // for (init; condition; update)
    LOOP_TYPE_WHILE = 1,          // while (condition)
    LOOP_TYPE_DO_WHILE = 2,       // do { ... } while (condition)
    LOOP_TYPE_FOR_EACH = 3,       // for (item in collection)
    LOOP_TYPE_UNKNOWN = 4         // Unknown or complex loop
} LoopType;

/**
 * @brief Loop optimization opportunities
 */
typedef enum {
    LOOP_OPT_UNROLL = 1 << 0,     // Can be unrolled
    LOOP_OPT_VECTORIZE = 1 << 1,  // Can be vectorized
    LOOP_OPT_FUSE = 1 << 2,       // Can be fused with adjacent loops
    LOOP_OPT_INTERCHANGE = 1 << 3, // Can interchange loop order
    LOOP_OPT_STRENGTH_REDUCE = 1 << 4, // Can reduce operation strength
    LOOP_OPT_PARALLEL = 1 << 5    // Can be parallelized
} LoopOptimizationFlags;

/**
 * @brief Induction variable information
 */
typedef struct {
    char* variable_name;           // Name of the induction variable
    ASTNode* initial_value;        // Initial value expression
    ASTNode* update_expression;    // Update expression
    int is_increasing;             // 1 if increasing, 0 if decreasing
    int is_constant_step;          // 1 if step is constant
    double step_value;             // Step value if constant
    ASTNode* bound_expression;     // Bound expression
    int is_linear;                 // 1 if linear induction
} InductionVariable;

/**
 * @brief Loop analysis result
 */
typedef struct {
    ASTNode* loop_node;            // Original loop AST node
    LoopType loop_type;            // Type of loop
    int is_simple;                 // 1 if simple loop structure
    int is_countable;              // 1 if trip count can be computed
    int trip_count;                // Computed trip count (-1 if unknown)
    int is_innermost;              // 1 if innermost loop
    int is_outermost;              // 1 if outermost loop
    int nesting_level;             // Loop nesting level
    InductionVariable* induction_vars; // Array of induction variables
    size_t induction_var_count;    // Number of induction variables
    int optimization_flags;        // Available optimizations
    int has_loop_carried_deps;     // 1 if has loop-carried dependencies
    int has_early_exit;            // 1 if has break/continue statements
    int has_function_calls;        // 1 if contains function calls
    int has_side_effects;          // 1 if has side effects
} LoopAnalysis;

/**
 * @brief Loop analyzer context
 */
typedef struct {
    LoopAnalysis* loops;           // Array of analyzed loops
    size_t loop_count;             // Number of loops found
    size_t loop_capacity;          // Capacity of loops array
    int is_analyzing;              // 1 if currently analyzing
    size_t current_nesting;        // Current nesting level
} LoopAnalyzer;

/**
 * @brief Create loop analyzer
 * 
 * @return LoopAnalyzer* New analyzer or NULL on failure
 */
LoopAnalyzer* loop_analyzer_create(void);

/**
 * @brief Free loop analyzer
 * 
 * @param analyzer Analyzer to free
 */
void loop_analyzer_free(LoopAnalyzer* analyzer);

/**
 * @brief Analyze function for loops
 * 
 * @param analyzer Loop analyzer
 * @param function_node Function AST node to analyze
 * @return int 1 on success, 0 on failure
 */
int loop_analyzer_analyze_function(LoopAnalyzer* analyzer, ASTNode* function_node);

/**
 * @brief Analyze loop node
 * 
 * @param analyzer Loop analyzer
 * @param loop_node Loop AST node
 * @param nesting_level Current nesting level
 * @return LoopAnalysis* Analysis result or NULL on failure
 */
LoopAnalysis* loop_analyzer_analyze_loop(LoopAnalyzer* analyzer, 
                                         ASTNode* loop_node, 
                                         size_t nesting_level);

/**
 * @brief Detect induction variables in loop
 * 
 * @param analyzer Loop analyzer
 * @param loop_node Loop AST node
 * @param induction_vars Output array for induction variables
 * @param max_count Maximum number of induction variables
 * @return size_t Number of induction variables found
 */
size_t loop_analyzer_detect_induction_variables(LoopAnalyzer* analyzer,
                                                ASTNode* loop_node,
                                                InductionVariable* induction_vars,
                                                size_t max_count);

/**
 * @brief Compute loop trip count
 * 
 * @param analyzer Loop analyzer
 * @param loop_node Loop AST node
 * @param induction_vars Array of induction variables
 * @param var_count Number of induction variables
 * @return int Trip count (-1 if unknown)
 */
int loop_analyzer_compute_trip_count(LoopAnalyzer* analyzer,
                                     ASTNode* loop_node,
                                     InductionVariable* induction_vars,
                                     size_t var_count);

/**
 * @brief Check for loop-carried dependencies
 * 
 * @param analyzer Loop analyzer
 * @param loop_node Loop AST node
 * @return int 1 if has dependencies, 0 otherwise
 */
int loop_analyzer_has_loop_carried_dependencies(LoopAnalyzer* analyzer,
                                                ASTNode* loop_node);

/**
 * @brief Determine optimization opportunities
 * 
 * @param analyzer Loop analyzer
 * @param analysis Loop analysis result
 * @return int Optimization flags
 */
int loop_analyzer_determine_optimizations(LoopAnalyzer* analyzer,
                                          LoopAnalysis* analysis);

/**
 * @brief Check if loop can be vectorized
 * 
 * @param analyzer Loop analyzer
 * @param analysis Loop analysis result
 * @return int 1 if can be vectorized, 0 otherwise
 */
int loop_analyzer_can_vectorize(LoopAnalyzer* analyzer, LoopAnalysis* analysis);

/**
 * @brief Check if loop can be unrolled
 * 
 * @param analyzer Loop analyzer
 * @param analysis Loop analysis result
 * @return int 1 if can be unrolled, 0 otherwise
 */
int loop_analyzer_can_unroll(LoopAnalyzer* analyzer, LoopAnalysis* analysis);

/**
 * @brief Check if loop can be fused
 * 
 * @param analyzer Loop analyzer
 * @param analysis1 First loop analysis
 * @param analysis2 Second loop analysis
 * @return int 1 if can be fused, 0 otherwise
 */
int loop_analyzer_can_fuse(LoopAnalyzer* analyzer,
                           LoopAnalysis* analysis1,
                           LoopAnalysis* analysis2);

/**
 * @brief Get loop statistics
 * 
 * @param analyzer Loop analyzer
 * @param total_loops Total number of loops found
 * @param vectorizable_loops Number of vectorizable loops
 * @param unrollable_loops Number of unrollable loops
 * @param fusible_loops Number of fusible loops
 */
void loop_analyzer_get_stats(LoopAnalyzer* analyzer,
                             size_t* total_loops,
                             size_t* vectorizable_loops,
                             size_t* unrollable_loops,
                             size_t* fusible_loops);

#endif // MYCO_LOOP_ANALYZER_H
