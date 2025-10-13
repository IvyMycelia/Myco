/**
 * @file vectorizer.h
 * @brief SIMD vectorization for loop optimization
 * 
 * Detects vectorizable patterns and generates SIMD code
 * for maximum performance on modern CPUs.
 */

#ifndef MYCO_VECTORIZER_H
#define MYCO_VECTORIZER_H

#include "../ast.h"
#include "loop_analyzer.h"

/**
 * @brief SIMD instruction set support
 */
typedef enum {
    SIMD_NONE = 0,                 // No SIMD support
    SIMD_SSE2 = 1,                 // SSE2 support
    SIMD_SSE4_2 = 2,               // SSE4.2 support
    SIMD_AVX = 3,                  // AVX support
    SIMD_AVX2 = 4,                 // AVX2 support
    SIMD_AVX512 = 5,               // AVX-512 support
    SIMD_NEON = 6                  // ARM NEON support
} SIMDInstructionSet;

/**
 * @brief Vectorization strategy
 */
typedef enum {
    VECTORIZE_NONE = 0,            // No vectorization
    VECTORIZE_2X = 2,              // 2x vectorization
    VECTORIZE_4X = 4,              // 4x vectorization
    VECTORIZE_8X = 8,              // 8x vectorization
    VECTORIZE_16X = 16             // 16x vectorization
} VectorizationStrategy;

/**
 * @brief Vectorization result
 */
typedef struct {
    ASTNode* vectorized_loop;      // Vectorized loop AST
    int vectorization_applied;     // 1 if vectorization was applied
    int vectorization_factor;      // Vectorization factor used
    int performance_gain;          // Estimated performance gain percentage
    SIMDInstructionSet simd_used;  // SIMD instruction set used
    char* vectorization_type;      // Type of vectorization applied
} VectorizationResult;

/**
 * @brief Vectorization pattern
 */
typedef struct {
    char* pattern_name;            // Name of the pattern
    int is_arithmetic;             // 1 if arithmetic pattern
    int is_memory_bound;           // 1 if memory-bound pattern
    int is_compute_bound;          // 1 if compute-bound pattern
    int vectorization_factor;      // Optimal vectorization factor
    SIMDInstructionSet required_simd; // Required SIMD instruction set
} VectorizationPattern;

/**
 * @brief Vectorizer context
 */
typedef struct {
    SIMDInstructionSet available_simd; // Available SIMD instruction sets
    int enable_auto_vectorization; // Enable automatic vectorization
    int max_vectorization_factor;  // Maximum vectorization factor
    VectorizationPattern* patterns; // Available vectorization patterns
    size_t pattern_count;          // Number of patterns
    size_t vectorization_count;    // Number of vectorizations applied
    size_t total_gain;             // Total performance gain
} Vectorizer;

/**
 * @brief Create vectorizer
 * 
 * @return Vectorizer* New vectorizer or NULL on failure
 */
Vectorizer* vectorizer_create(void);

/**
 * @brief Free vectorizer
 * 
 * @param vectorizer Vectorizer to free
 */
void vectorizer_free(Vectorizer* vectorizer);

/**
 * @brief Detect CPU SIMD capabilities
 * 
 * @param vectorizer Vectorizer context
 * @return SIMDInstructionSet Detected SIMD instruction set
 */
SIMDInstructionSet vectorizer_detect_simd(Vectorizer* vectorizer);

/**
 * @brief Vectorize loop
 * 
 * @param vectorizer Vectorizer context
 * @param analysis Loop analysis result
 * @return VectorizationResult* Vectorization result or NULL on failure
 */
VectorizationResult* vectorizer_vectorize_loop(Vectorizer* vectorizer,
                                               LoopAnalysis* analysis);

/**
 * @brief Check if loop can be vectorized
 * 
 * @param vectorizer Vectorizer context
 * @param analysis Loop analysis result
 * @return int 1 if can be vectorized, 0 otherwise
 */
int vectorizer_can_vectorize(Vectorizer* vectorizer, LoopAnalysis* analysis);

/**
 * @brief Determine optimal vectorization strategy
 * 
 * @param vectorizer Vectorizer context
 * @param analysis Loop analysis result
 * @return VectorizationStrategy Optimal strategy
 */
VectorizationStrategy vectorizer_determine_strategy(Vectorizer* vectorizer,
                                                    LoopAnalysis* analysis);

/**
 * @brief Generate vectorized code
 * 
 * @param vectorizer Vectorizer context
 * @param analysis Loop analysis result
 * @param strategy Vectorization strategy
 * @return VectorizationResult* Vectorization result or NULL on failure
 */
VectorizationResult* vectorizer_generate_code(Vectorizer* vectorizer,
                                              LoopAnalysis* analysis,
                                              VectorizationStrategy strategy);

/**
 * @brief Detect vectorization patterns
 * 
 * @param vectorizer Vectorizer context
 * @param analysis Loop analysis result
 * @param patterns Output array for patterns
 * @param max_patterns Maximum number of patterns
 * @return size_t Number of patterns detected
 */
size_t vectorizer_detect_patterns(Vectorizer* vectorizer,
                                  LoopAnalysis* analysis,
                                  VectorizationPattern* patterns,
                                  size_t max_patterns);

/**
 * @brief Optimize vectorized code
 * 
 * @param vectorizer Vectorizer context
 * @param result Vectorization result
 * @return int 1 on success, 0 on failure
 */
int vectorizer_optimize_code(Vectorizer* vectorizer, VectorizationResult* result);

/**
 * @brief Get vectorization statistics
 * 
 * @param vectorizer Vectorizer context
 * @param vectorization_count Number of vectorizations applied
 * @param total_gain Total performance gain
 * @param simd_usage SIMD instruction set usage
 */
void vectorizer_get_stats(Vectorizer* vectorizer,
                          size_t* vectorization_count,
                          size_t* total_gain,
                          SIMDInstructionSet* simd_usage);

/**
 * @brief Set vectorization preferences
 * 
 * @param vectorizer Vectorizer context
 * @param enable_auto Enable automatic vectorization
 * @param max_factor Maximum vectorization factor
 * @param preferred_simd Preferred SIMD instruction set
 */
void vectorizer_set_preferences(Vectorizer* vectorizer,
                                int enable_auto,
                                int max_factor,
                                SIMDInstructionSet preferred_simd);

/**
 * @brief Estimate vectorization gain
 * 
 * @param vectorizer Vectorizer context
 * @param analysis Loop analysis result
 * @param strategy Vectorization strategy
 * @return int Estimated performance gain percentage
 */
int vectorizer_estimate_gain(Vectorizer* vectorizer,
                             LoopAnalysis* analysis,
                             VectorizationStrategy strategy);

#endif // MYCO_VECTORIZER_H
