/**
 * @file escape_analysis.h
 * @brief Escape analysis for zero-copy optimization
 * 
 * Determines which values escape function scope to enable stack allocation
 * and eliminate unnecessary heap allocations in hot paths.
 */

#ifndef MYCO_ESCAPE_ANALYSIS_H
#define MYCO_ESCAPE_ANALYSIS_H

#include "../interpreter/value_operations.h"
#include "../ast.h"

/**
 * @brief Escape analysis result for a value
 */
typedef enum {
    ESCAPE_ANALYSIS_NO_ESCAPE = 0,    // Value does not escape function
    ESCAPE_ANALYSIS_ESCAPES = 1,      // Value escapes function scope
    ESCAPE_ANALYSIS_UNKNOWN = 2       // Cannot determine escape status
} EscapeAnalysisResult;

/**
 * @brief Escape analysis context
 */
typedef struct {
    ASTNode* function_node;           // Function being analyzed
    Value** stack_allocated_values;   // Values that can be stack-allocated
    size_t stack_count;               // Number of stack-allocated values
    size_t stack_capacity;            // Capacity of stack array
    int* escape_map;                  // Map from value ID to escape status
    size_t value_count;               // Total number of values analyzed
} EscapeAnalysisContext;

/**
 * @brief Create escape analysis context
 * 
 * @param function_node Function to analyze
 * @return EscapeAnalysisContext* New context or NULL on failure
 */
EscapeAnalysisContext* escape_analysis_create(ASTNode* function_node);

/**
 * @brief Free escape analysis context
 * 
 * @param context Context to free
 */
void escape_analysis_free(EscapeAnalysisContext* context);

/**
 * @brief Analyze function for escape patterns
 * 
 * @param context Analysis context
 * @return int 1 on success, 0 on failure
 */
int escape_analysis_analyze_function(EscapeAnalysisContext* context);

/**
 * @brief Check if value escapes function scope
 * 
 * @param context Analysis context
 * @param value_id Unique identifier for value
 * @return EscapeAnalysisResult Escape analysis result
 */
EscapeAnalysisResult escape_analysis_check_escape(EscapeAnalysisContext* context, 
                                                  size_t value_id);

/**
 * @brief Get values that can be stack-allocated
 * 
 * @param context Analysis context
 * @param values Output array for stack-allocated values
 * @param count Output count of stack-allocated values
 * @return int 1 on success, 0 on failure
 */
int escape_analysis_get_stack_values(EscapeAnalysisContext* context,
                                     Value*** values,
                                     size_t* count);

/**
 * @brief Apply SROA (Scalar Replacement of Aggregates)
 * 
 * @param context Analysis context
 * @param ast_node AST node to optimize
 * @return int 1 on success, 0 on failure
 */
int escape_analysis_apply_sroa(EscapeAnalysisContext* context, ASTNode* ast_node);

/**
 * @brief Check if value can be eliminated
 * 
 * @param context Analysis context
 * @param value_id Value identifier
 * @return int 1 if can be eliminated, 0 otherwise
 */
int escape_analysis_can_eliminate(EscapeAnalysisContext* context, size_t value_id);

/**
 * @brief Get escape analysis statistics
 * 
 * @param context Analysis context
 * @param total_values Total number of values analyzed
 * @param escaped_values Number of values that escape
 * @param stack_values Number of values that can be stack-allocated
 * @param eliminated_values Number of values that can be eliminated
 */
void escape_analysis_get_stats(EscapeAnalysisContext* context,
                               size_t* total_values,
                               size_t* escaped_values,
                               size_t* stack_values,
                               size_t* eliminated_values);

#endif // MYCO_ESCAPE_ANALYSIS_H
