/**
 * @file type_predictor.h
 * @brief Predictive type specialization
 * 
 * Machine learning-based type prediction, anticipates argument types before
 * execution, pre-compiles specialized versions.
 */

#ifndef TYPE_PREDICTOR_H
#define TYPE_PREDICTOR_H

#include "trace_recorder.h"
#include "register_vm.h"
#include "../interpreter/interpreter_core.h"
#include <stdint.h>
#include <stddef.h>

// ============================================================================
// TYPE PREDICTION DATA STRUCTURES
// ============================================================================

/**
 * @brief Prediction model type
 * 
 * Defines the type of machine learning model used for type prediction.
 */
typedef enum {
    PREDICTOR_MODEL_NAIVE_BAYES = 0,    // Naive Bayes classifier
    PREDICTOR_MODEL_DECISION_TREE = 1,  // Decision tree classifier
    PREDICTOR_MODEL_NEURAL_NETWORK = 2, // Neural network classifier
    PREDICTOR_MODEL_ENSEMBLE = 3,       // Ensemble of models
    PREDICTOR_MODEL_ADAPTIVE = 4        // Adaptive model selection
} PredictorModelType;

/**
 * @brief Type prediction confidence
 * 
 * Represents the confidence level of a type prediction.
 */
typedef enum {
    PREDICTION_CONFIDENCE_LOW = 0,      // Low confidence (0.0-0.5)
    PREDICTION_CONFIDENCE_MEDIUM = 1,   // Medium confidence (0.5-0.8)
    PREDICTION_CONFIDENCE_HIGH = 2,     // High confidence (0.8-0.95)
    PREDICTION_CONFIDENCE_VERY_HIGH = 3 // Very high confidence (0.95-1.0)
} PredictionConfidence;

/**
 * @brief Type pattern
 * 
 * Represents a pattern of types observed at a call site.
 */
typedef struct {
    uint32_t pattern_id;                // Unique pattern identifier
    uint8_t* argument_types;            // Array of argument types
    uint32_t argument_count;            // Number of arguments
    uint8_t return_type;                // Return type
    uint64_t observation_count;         // Number of times observed
    double frequency;                   // Frequency of this pattern (0.0-1.0)
    double confidence;                  // Confidence in this pattern (0.0-1.0)
    uint64_t last_observed;             // Timestamp of last observation
    uint64_t first_observed;            // Timestamp of first observation
    int is_stable;                      // Is this pattern stable?
    int is_hot;                         // Is this pattern hot?
    double hotness_score;               // Hotness score (0.0-1.0)
} TypePattern;

/**
 * @brief Call site information
 * 
 * Contains information about a specific call site for type prediction.
 */
typedef struct {
    uint32_t call_site_id;              // Unique call site identifier
    ASTNode* function_node;             // Function AST node
    char* function_name;                // Function name
    uint32_t parameter_count;           // Number of parameters
    TypePattern* patterns;              // Array of observed patterns
    uint32_t pattern_count;             // Number of patterns
    uint32_t pattern_capacity;          // Capacity of patterns array
    
    // Prediction state
    TypePattern* predicted_pattern;     // Currently predicted pattern
    double prediction_confidence;       // Confidence in current prediction
    uint64_t prediction_count;          // Number of predictions made
    uint64_t correct_predictions;       // Number of correct predictions
    double accuracy;                    // Prediction accuracy (0.0-1.0)
    
    // Specialization state
    int has_specialized_version;        // Has specialized version been compiled?
    uint32_t specialized_version_id;    // ID of specialized version
    double specialization_benefit;      // Benefit of specialization (0.0-1.0)
    
    // Statistics
    uint64_t total_calls;               // Total number of calls
    uint64_t specialized_calls;         // Number of specialized calls
    double specialization_ratio;        // Ratio of specialized calls
    double average_execution_time;      // Average execution time
    double specialized_execution_time;  // Average specialized execution time
    double speedup;                     // Speedup from specialization
} CallSite;

/**
 * @brief Type prediction result
 * 
 * Contains the result of a type prediction operation.
 */
typedef struct {
    int success;                        // Prediction success flag
    TypePattern* predicted_pattern;     // Predicted type pattern
    double confidence;                  // Prediction confidence (0.0-1.0)
    PredictionConfidence confidence_level; // Confidence level
    uint32_t prediction_time_ns;        // Time taken for prediction (nanoseconds)
    char* error_message;                // Error message (if prediction failed)
} TypePredictionResult;

/**
 * @brief Type predictor context
 * 
 * Contains the state and configuration for type prediction.
 */
typedef struct {
    // Configuration
    PredictorModelType model_type;      // Type of prediction model
    double confidence_threshold;        // Minimum confidence for specialization
    double stability_threshold;         // Minimum stability for pattern recognition
    uint32_t max_patterns_per_site;     // Maximum patterns per call site
    uint32_t min_observations;          // Minimum observations for pattern recognition
    double learning_rate;               // Learning rate for model updates
    int enable_adaptive_learning;       // Enable adaptive learning
    
    // Call sites
    CallSite* call_sites;               // Array of call sites
    uint32_t call_site_count;           // Number of call sites
    uint32_t max_call_sites;            // Maximum number of call sites
    
    // Type patterns
    TypePattern* global_patterns;       // Global type patterns
    uint32_t global_pattern_count;      // Number of global patterns
    uint32_t max_global_patterns;       // Maximum number of global patterns
    
    // Prediction model
    void* model_data;                   // Model-specific data
    size_t model_size;                  // Size of model data
    int model_trained;                  // Is the model trained?
    double model_accuracy;              // Model accuracy (0.0-1.0)
    
    // Statistics
    uint64_t total_predictions;         // Total number of predictions
    uint64_t correct_predictions;       // Number of correct predictions
    uint64_t specialized_predictions;   // Number of specialized predictions
    double overall_accuracy;            // Overall prediction accuracy
    double specialization_rate;         // Rate of specialization
    double average_prediction_time;     // Average prediction time (nanoseconds)
    double average_speedup;             // Average speedup from specialization
    
    // Performance tracking
    uint64_t prediction_start_time;     // Prediction start time
    uint64_t prediction_end_time;       // Prediction end time
    double total_prediction_time_ms;    // Total prediction time (milliseconds)
    double prediction_overhead;         // Prediction overhead percentage
} TypePredictorContext;

// ============================================================================
// TYPE PREDICTION FUNCTIONS
// ============================================================================

/**
 * @brief Core prediction functions
 * 
 * These functions handle the main type prediction process,
 * from initialization to prediction execution.
 */

/**
 * @brief Create a new type predictor context
 * @param model_type Type of prediction model to use
 * @return New TypePredictorContext, or NULL on failure
 * @note The context is initialized with default settings
 */
TypePredictorContext* type_predictor_create(PredictorModelType model_type);

/**
 * @brief Free a type predictor context
 * @param context Context to free (can be NULL)
 * @note This function is safe to call with NULL
 */
void type_predictor_free(TypePredictorContext* context);

/**
 * @brief Predict argument types for a function call
 * @param context Predictor context
 * @param call_site_id Call site identifier
 * @param function_name Function name
 * @param parameter_count Number of parameters
 * @return Type prediction result
 * @note This is the main entry point for type prediction
 */
TypePredictionResult type_predictor_predict_types(TypePredictorContext* context, 
                                                 uint32_t call_site_id,
                                                 const char* function_name,
                                                 uint32_t parameter_count);

/**
 * @brief Record type observation for learning
 * @param context Predictor context
 * @param call_site_id Call site identifier
 * @param argument_types Array of argument types
 * @param argument_count Number of arguments
 * @param return_type Return type
 * @return 1 on success, 0 on failure
 * @note This function records type observations for model learning
 */
int type_predictor_record_observation(TypePredictorContext* context,
                                     uint32_t call_site_id,
                                     const uint8_t* argument_types,
                                     uint32_t argument_count,
                                     uint8_t return_type);

/**
 * @brief Update prediction model with new data
 * @param context Predictor context
 * @return 1 on success, 0 on failure
 * @note This function updates the prediction model with new observations
 */
int type_predictor_update_model(TypePredictorContext* context);

/**
 * @brief Call site management functions
 * 
 * These functions manage call sites and their type patterns.
 */

/**
 * @brief Register a new call site
 * @param context Predictor context
 * @param function_name Function name
 * @param function_node Function AST node
 * @param parameter_count Number of parameters
 * @return Call site ID, or 0 on failure
 * @note This function registers a new call site for type prediction
 */
uint32_t type_predictor_register_call_site(TypePredictorContext* context,
                                          const char* function_name,
                                          ASTNode* function_node,
                                          uint32_t parameter_count);

/**
 * @brief Get call site by ID
 * @param context Predictor context
 * @param call_site_id Call site ID
 * @return Call site, or NULL if not found
 * @note This function retrieves a call site by its ID
 */
CallSite* type_predictor_get_call_site(TypePredictorContext* context, uint32_t call_site_id);

/**
 * @brief Get all call sites
 * @param context Predictor context
 * @param call_sites Array to store call sites
 * @param max_sites Maximum number of call sites to return
 * @return Number of call sites returned
 * @note This function retrieves all call sites
 */
uint32_t type_predictor_get_all_call_sites(TypePredictorContext* context, 
                                          CallSite** call_sites, 
                                          uint32_t max_sites);

/**
 * @brief Remove a call site
 * @param context Predictor context
 * @param call_site_id Call site ID to remove
 * @return 1 on success, 0 on failure
 * @note This function removes a call site and its patterns
 */
int type_predictor_remove_call_site(TypePredictorContext* context, uint32_t call_site_id);

/**
 * @brief Clear all call sites
 * @param context Predictor context
 * @note This function removes all call sites and patterns
 */
void type_predictor_clear_call_sites(TypePredictorContext* context);

/**
 * @brief Pattern analysis functions
 * 
 * These functions analyze type patterns for optimization opportunities.
 */

/**
 * @brief Analyze type patterns for a call site
 * @param context Predictor context
 * @param call_site_id Call site ID
 * @return 1 on success, 0 on failure
 * @note This function analyzes type patterns for optimization opportunities
 */
int type_predictor_analyze_patterns(TypePredictorContext* context, uint32_t call_site_id);

/**
 * @brief Find the most common pattern for a call site
 * @param context Predictor context
 * @param call_site_id Call site ID
 * @return Most common pattern, or NULL if not found
 * @note This function finds the most frequently observed pattern
 */
TypePattern* type_predictor_find_most_common_pattern(TypePredictorContext* context, uint32_t call_site_id);

/**
 * @brief Check if a call site is monomorphic
 * @param context Predictor context
 * @param call_site_id Call site ID
 * @return 1 if monomorphic, 0 otherwise
 * @note A monomorphic call site has only one type pattern
 */
int type_predictor_is_monomorphic(TypePredictorContext* context, uint32_t call_site_id);

/**
 * @brief Check if a call site is polymorphic
 * @param context Predictor context
 * @param call_site_id Call site ID
 * @return 1 if polymorphic, 0 otherwise
 * @note A polymorphic call site has 2-4 type patterns
 */
int type_predictor_is_polymorphic(TypePredictorContext* context, uint32_t call_site_id);

/**
 * @brief Check if a call site is megamorphic
 * @param context Predictor context
 * @param call_site_id Call site ID
 * @return 1 if megamorphic, 0 otherwise
 * @note A megamorphic call site has 5+ type patterns
 */
int type_predictor_is_megamorphic(TypePredictorContext* context, uint32_t call_site_id);

/**
 * @brief Calculate pattern stability
 * @param context Predictor context
 * @param call_site_id Call site ID
 * @return Pattern stability (0.0-1.0)
 * @note Higher stability indicates more consistent type patterns
 */
double type_predictor_calculate_stability(TypePredictorContext* context, uint32_t call_site_id);

/**
 * @brief Calculate pattern hotness
 * @param context Predictor context
 * @param call_site_id Call site ID
 * @return Pattern hotness (0.0-1.0)
 * @note Higher hotness indicates more frequently executed patterns
 */
double type_predictor_calculate_hotness(TypePredictorContext* context, uint32_t call_site_id);

/**
 * @brief Specialization functions
 * 
 * These functions handle the creation and management of specialized versions.
 */

/**
 * @brief Create a specialized version of a function
 * @param context Predictor context
 * @param call_site_id Call site ID
 * @param pattern Type pattern to specialize for
 * @return Specialized version ID, or 0 on failure
 * @note This function creates a specialized version optimized for the pattern
 */
uint32_t type_predictor_create_specialized_version(TypePredictorContext* context,
                                                  uint32_t call_site_id,
                                                  TypePattern* pattern);

/**
 * @brief Check if a specialized version exists
 * @param context Predictor context
 * @param call_site_id Call site ID
 * @param pattern Type pattern
 * @return 1 if specialized version exists, 0 otherwise
 * @note This function checks if a specialized version exists for a pattern
 */
int type_predictor_has_specialized_version(TypePredictorContext* context,
                                          uint32_t call_site_id,
                                          TypePattern* pattern);

/**
 * @brief Get specialized version for a pattern
 * @param context Predictor context
 * @param call_site_id Call site ID
 * @param pattern Type pattern
 * @return Specialized version ID, or 0 if not found
 * @note This function retrieves the specialized version for a pattern
 */
uint32_t type_predictor_get_specialized_version(TypePredictorContext* context,
                                               uint32_t call_site_id,
                                               TypePattern* pattern);

/**
 * @brief Remove a specialized version
 * @param context Predictor context
 * @param call_site_id Call site ID
 * @param specialized_version_id Specialized version ID
 * @return 1 on success, 0 on failure
 * @note This function removes a specialized version
 */
int type_predictor_remove_specialized_version(TypePredictorContext* context,
                                             uint32_t call_site_id,
                                             uint32_t specialized_version_id);

/**
 * @brief Configuration functions
 * 
 * These functions configure the type predictor behavior.
 */

/**
 * @brief Set prediction model type
 * @param context Predictor context
 * @param model_type New model type
 * @note This function changes the prediction model type
 */
void type_predictor_set_model_type(TypePredictorContext* context, PredictorModelType model_type);

/**
 * @brief Set confidence threshold
 * @param context Predictor context
 * @param threshold Confidence threshold (0.0-1.0)
 * @note This function sets the minimum confidence for specialization
 */
void type_predictor_set_confidence_threshold(TypePredictorContext* context, double threshold);

/**
 * @brief Set stability threshold
 * @param context Predictor context
 * @param threshold Stability threshold (0.0-1.0)
 * @note This function sets the minimum stability for pattern recognition
 */
void type_predictor_set_stability_threshold(TypePredictorContext* context, double threshold);

/**
 * @brief Set learning rate
 * @param context Predictor context
 * @param rate Learning rate (0.0-1.0)
 * @note This function sets the learning rate for model updates
 */
void type_predictor_set_learning_rate(TypePredictorContext* context, double rate);

/**
 * @brief Enable or disable adaptive learning
 * @param context Predictor context
 * @param enable 1 to enable, 0 to disable
 * @note This function enables or disables adaptive learning
 */
void type_predictor_set_adaptive_learning(TypePredictorContext* context, int enable);

/**
 * @brief Utility functions
 * 
 * These functions provide utility functionality for working with
 * type predictor contexts and predictions.
 */

/**
 * @brief Get prediction statistics
 * @param context Predictor context
 * @return Statistics string
 * @note Returns a formatted string with prediction statistics
 */
char* type_predictor_get_statistics(TypePredictorContext* context);

/**
 * @brief Print call site information
 * @param context Predictor context
 * @param call_site_id Call site ID to print
 * @note Prints human-readable representation of a call site
 */
void type_predictor_print_call_site(TypePredictorContext* context, uint32_t call_site_id);

/**
 * @brief Print all call sites
 * @param context Predictor context
 * @note Prints human-readable representation of all call sites
 */
void type_predictor_print_all_call_sites(TypePredictorContext* context);

/**
 * @brief Print type pattern information
 * @param context Predictor context
 * @param pattern Pattern to print
 * @note Prints human-readable representation of a type pattern
 */
void type_predictor_print_pattern(TypePredictorContext* context, TypePattern* pattern);

/**
 * @brief Validate type prediction
 * @param context Predictor context
 * @param call_site_id Call site ID to validate
 * @return 1 if valid, 0 if invalid
 * @note Validates that type predictions are correct
 */
int type_predictor_validate_prediction(TypePredictorContext* context, uint32_t call_site_id);

/**
 * @brief Export prediction data to file
 * @param context Predictor context
 * @param filename Output filename
 * @return 1 on success, 0 on failure
 * @note Exports prediction data to a file for analysis
 */
int type_predictor_export_data(TypePredictorContext* context, const char* filename);

/**
 * @brief Import prediction data from file
 * @param context Predictor context
 * @param filename Input filename
 * @return 1 on success, 0 on failure
 * @note Imports prediction data from a file
 */
int type_predictor_import_data(TypePredictorContext* context, const char* filename);

#endif // TYPE_PREDICTOR_H
