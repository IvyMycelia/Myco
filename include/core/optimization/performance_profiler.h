/**
 * @file performance_profiler.h
 * @brief Real-time performance monitoring and optimization guidance
 * 
 * Tracks tier effectiveness, identifies bottlenecks, and provides
 * optimization suggestions for the adaptive execution system.
 */

#ifndef MYCO_PERFORMANCE_PROFILER_H
#define MYCO_PERFORMANCE_PROFILER_H

#include "../ast.h"
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Performance tier enumeration
 */
typedef enum {
    TIER_AST = 0,                  // AST interpreter
    TIER_BYTECODE = 1,             // Register bytecode
    TIER_TRACE_RECORDING = 2,      // Trace recording
    TIER_TRACE_COMPILED = 3,       // Trace compilation
    TIER_SPECIALIZED = 4,          // Predictive specialization
    TIER_VECTORIZED = 5,           // Vectorized code
    TIER_COUNT = 6                 // Total number of tiers
} PerformanceTier;

/**
 * @brief Performance metrics
 */
typedef struct {
    uint64_t execution_time_ns;    // Execution time in nanoseconds
    uint64_t instruction_count;    // Number of instructions executed
    uint64_t memory_allocations;   // Number of memory allocations
    uint64_t memory_bytes;         // Bytes allocated
    uint64_t cache_hits;           // Cache hits
    uint64_t cache_misses;         // Cache misses
    uint64_t branch_predictions;   // Branch predictions
    uint64_t branch_mispredictions; // Branch mispredictions
} PerformanceMetrics;

/**
 * @brief Tier effectiveness data
 */
typedef struct {
    PerformanceTier tier;          // Tier identifier
    PerformanceMetrics metrics;    // Performance metrics
    double speedup_factor;         // Speedup relative to AST
    double efficiency_score;       // Efficiency score (0.0-1.0)
    int is_optimal;                // 1 if this is the optimal tier
    uint64_t sample_count;         // Number of samples
} TierEffectiveness;

/**
 * @brief Bottleneck analysis
 */
typedef struct {
    char* bottleneck_type;         // Type of bottleneck
    char* description;             // Description of the bottleneck
    double impact_score;           // Impact score (0.0-1.0)
    char* suggestion;              // Optimization suggestion
    PerformanceTier affected_tier; // Tier affected by bottleneck
} BottleneckAnalysis;

/**
 * @brief Optimization suggestion
 */
typedef struct {
    char* suggestion_type;         // Type of suggestion
    char* description;             // Description of the suggestion
    double expected_gain;          // Expected performance gain
    int difficulty;                // Implementation difficulty (1-5)
    char* implementation_hint;     // Implementation hint
} OptimizationSuggestion;

/**
 * @brief Performance profiler context
 */
typedef struct {
    TierEffectiveness* tier_data;  // Effectiveness data for each tier
    BottleneckAnalysis* bottlenecks; // Detected bottlenecks
    size_t bottleneck_count;       // Number of bottlenecks
    OptimizationSuggestion* suggestions; // Optimization suggestions
    size_t suggestion_count;       // Number of suggestions
    uint64_t total_samples;        // Total number of samples
    uint64_t profiling_start_time; // Profiling start time
    int is_profiling;              // 1 if currently profiling
    double overall_efficiency;     // Overall system efficiency
} PerformanceProfiler;

/**
 * @brief Create performance profiler
 * 
 * @return PerformanceProfiler* New profiler or NULL on failure
 */
PerformanceProfiler* performance_profiler_create(void);

/**
 * @brief Free performance profiler
 * 
 * @param profiler Profiler to free
 */
void performance_profiler_free(PerformanceProfiler* profiler);

/**
 * @brief Start profiling
 * 
 * @param profiler Performance profiler
 * @return int 1 on success, 0 on failure
 */
int performance_profiler_start(PerformanceProfiler* profiler);

/**
 * @brief Stop profiling
 * 
 * @param profiler Performance profiler
 * @return int 1 on success, 0 on failure
 */
int performance_profiler_stop(PerformanceProfiler* profiler);

/**
 * @brief Record tier execution
 * 
 * @param profiler Performance profiler
 * @param tier Tier that was executed
 * @param metrics Performance metrics for this execution
 * @return int 1 on success, 0 on failure
 */
int performance_profiler_record_tier_execution(PerformanceProfiler* profiler,
                                               PerformanceTier tier,
                                               PerformanceMetrics metrics);

/**
 * @brief Analyze tier effectiveness
 * 
 * @param profiler Performance profiler
 * @return int 1 on success, 0 on failure
 */
int performance_profiler_analyze_effectiveness(PerformanceProfiler* profiler);

/**
 * @brief Detect performance bottlenecks
 * 
 * @param profiler Performance profiler
 * @return int 1 on success, 0 on failure
 */
int performance_profiler_detect_bottlenecks(PerformanceProfiler* profiler);

/**
 * @brief Generate optimization suggestions
 * 
 * @param profiler Performance profiler
 * @return int 1 on success, 0 on failure
 */
int performance_profiler_generate_suggestions(PerformanceProfiler* profiler);

/**
 * @brief Get optimal tier for function
 * 
 * @param profiler Performance profiler
 * @param function_node Function AST node
 * @return PerformanceTier Optimal tier for this function
 */
PerformanceTier performance_profiler_get_optimal_tier(PerformanceProfiler* profiler,
                                                      ASTNode* function_node);

/**
 * @brief Get tier effectiveness data
 * 
 * @param profiler Performance profiler
 * @param tier Tier to get data for
 * @return TierEffectiveness* Effectiveness data or NULL if not found
 */
TierEffectiveness* performance_profiler_get_tier_data(PerformanceProfiler* profiler,
                                                      PerformanceTier tier);

/**
 * @brief Get bottleneck analysis
 * 
 * @param profiler Performance profiler
 * @param bottleneck_index Index of the bottleneck
 * @return BottleneckAnalysis* Bottleneck analysis or NULL if not found
 */
BottleneckAnalysis* performance_profiler_get_bottleneck(PerformanceProfiler* profiler,
                                                        size_t bottleneck_index);

/**
 * @brief Get optimization suggestion
 * 
 * @param profiler Performance profiler
 * @param suggestion_index Index of the suggestion
 * @return OptimizationSuggestion* Optimization suggestion or NULL if not found
 */
OptimizationSuggestion* performance_profiler_get_suggestion(PerformanceProfiler* profiler,
                                                            size_t suggestion_index);

/**
 * @brief Get profiling statistics
 * 
 * @param profiler Performance profiler
 * @param total_samples Total number of samples
 * @param overall_efficiency Overall system efficiency
 * @param bottleneck_count Number of bottlenecks detected
 * @param suggestion_count Number of suggestions generated
 */
void performance_profiler_get_stats(PerformanceProfiler* profiler,
                                    uint64_t* total_samples,
                                    double* overall_efficiency,
                                    size_t* bottleneck_count,
                                    size_t* suggestion_count);

/**
 * @brief Reset profiler data
 * 
 * @param profiler Performance profiler
 */
void performance_profiler_reset(PerformanceProfiler* profiler);

/**
 * @brief Export profiling report
 * 
 * @param profiler Performance profiler
 * @param filename Output filename
 * @return int 1 on success, 0 on failure
 */
int performance_profiler_export_report(PerformanceProfiler* profiler,
                                       const char* filename);

/**
 * @brief Get current time in nanoseconds
 * 
 * @return uint64_t Current time in nanoseconds
 */
uint64_t performance_profiler_get_time_ns(void);

/**
 * @brief Calculate speedup factor
 * 
 * @param baseline_time Baseline execution time
 * @param optimized_time Optimized execution time
 * @return double Speedup factor
 */
double performance_profiler_calculate_speedup(uint64_t baseline_time,
                                              uint64_t optimized_time);

/**
 * @brief Calculate efficiency score
 * 
 * @param metrics Performance metrics
 * @return double Efficiency score (0.0-1.0)
 */
double performance_profiler_calculate_efficiency(PerformanceMetrics metrics);

#endif // MYCO_PERFORMANCE_PROFILER_H
