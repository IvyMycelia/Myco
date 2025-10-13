/**
 * @file hidden_classes.h
 * @brief V8-style hidden classes
 * 
 * Fast object property access, inline caching, transition chains,
 * O(1) property lookup.
 */

#ifndef HIDDEN_CLASSES_H
#define HIDDEN_CLASSES_H

#include "type_predictor.h"
#include "register_vm.h"
#include "../interpreter/interpreter_core.h"
#include <stdint.h>
#include <stddef.h>

// ============================================================================
// HIDDEN CLASSES DATA STRUCTURES
// ============================================================================

/**
 * @brief Property descriptor
 * 
 * Describes a property in a hidden class.
 */
typedef struct {
    char* name;                         // Property name
    uint32_t name_hash;                 // Hash of property name
    uint8_t type;                       // Property type
    uint32_t offset;                    // Offset in object layout
    uint32_t index;                     // Index in property array
    int is_writable;                    // Is property writable?
    int is_enumerable;                  // Is property enumerable?
    int is_configurable;                // Is property configurable?
    int is_inline;                      // Is property stored inline?
    int is_constant;                    // Is property constant?
    uint64_t access_count;              // Number of times accessed
    double hotness_score;               // Hotness score (0.0-1.0)
} PropertyDescriptor;

/**
 * @brief Hidden class
 * 
 * Represents a hidden class for object shape tracking.
 */
typedef struct {
    uint32_t class_id;                  // Unique class identifier
    char* class_name;                   // Class name (if any)
    uint32_t property_count;            // Number of properties
    PropertyDescriptor* properties;     // Array of property descriptors
    uint32_t property_capacity;         // Capacity of properties array
    uint32_t instance_size;             // Size of instances of this class
    uint32_t in_object_properties;      // Number of in-object properties
    uint32_t out_of_object_properties;  // Number of out-of-object properties
    
    // Transition information
    uint32_t parent_class_id;           // Parent class ID (for transitions)
    uint32_t transition_count;          // Number of transitions from this class
    uint32_t* transition_targets;       // Array of target class IDs
    char** transition_properties;       // Array of transition property names
    uint32_t* transition_property_types; // Array of transition property types
    
    // Optimization metadata
    int is_optimized;                   // Has this class been optimized?
    int is_hot;                         // Is this class hot?
    double hotness_score;               // Hotness score (0.0-1.0)
    uint64_t instantiation_count;       // Number of times instantiated
    uint64_t access_count;              // Total number of property accesses
    double average_access_time;         // Average access time (nanoseconds)
    
    // Memory layout
    uint32_t* property_offsets;         // Property offsets in object layout
    uint8_t* property_types;            // Property types in object layout
    uint32_t layout_size;               // Total layout size
    int is_layout_optimized;            // Is layout optimized?
} HiddenClass;

/**
 * @brief Object instance
 * 
 * Represents an object instance with hidden class information.
 */
typedef struct {
    uint32_t instance_id;               // Unique instance identifier
    uint32_t class_id;                  // Hidden class ID
    HiddenClass* hidden_class;          // Pointer to hidden class
    void* properties;                   // Property storage
    size_t property_size;               // Size of property storage
    uint32_t property_count;            // Number of properties
    uint64_t access_count;              // Number of property accesses
    double hotness_score;               // Hotness score (0.0-1.0)
    int is_optimized;                   // Has this instance been optimized?
    int is_hot;                         // Is this instance hot?
} ObjectInstance;

/**
 * @brief Transition
 * 
 * Represents a transition between hidden classes.
 */
typedef struct {
    uint32_t transition_id;             // Unique transition identifier
    uint32_t source_class_id;           // Source class ID
    uint32_t target_class_id;           // Target class ID
    char* property_name;                // Property name that caused transition
    uint8_t property_type;              // Type of property that caused transition
    uint32_t transition_count;          // Number of times this transition occurred
    double transition_frequency;        // Frequency of this transition (0.0-1.0)
    int is_hot;                         // Is this transition hot?
    double hotness_score;               // Hotness score (0.0-1.0)
    uint64_t last_occurrence;           // Timestamp of last occurrence
} Transition;

/**
 * @brief Hidden classes context
 * 
 * Contains the state and configuration for hidden classes.
 */
typedef struct {
    // Configuration
    uint32_t max_classes;               // Maximum number of hidden classes
    uint32_t max_properties_per_class;  // Maximum properties per class
    uint32_t max_transitions;           // Maximum number of transitions
    double hotness_threshold;           // Hotness threshold for optimization
    int enable_inline_caching;          // Enable inline caching
    int enable_property_inlining;       // Enable property inlining
    int enable_layout_optimization;     // Enable layout optimization
    
    // Hidden classes
    HiddenClass* classes;               // Array of hidden classes
    uint32_t class_count;               // Number of hidden classes
    uint32_t class_capacity;            // Capacity of classes array
    
    // Transitions
    Transition* transitions;            // Array of transitions
    uint32_t transition_count;          // Number of transitions
    uint32_t transition_capacity;       // Capacity of transitions array
    
    // Object instances
    ObjectInstance* instances;          // Array of object instances
    uint32_t instance_count;            // Number of object instances
    uint32_t instance_capacity;         // Capacity of instances array
    
    // Statistics
    uint64_t total_classes_created;     // Total number of classes created
    uint64_t total_instances_created;   // Total number of instances created
    uint64_t total_transitions;         // Total number of transitions
    uint64_t total_property_accesses;   // Total number of property accesses
    double average_property_access_time; // Average property access time
    double average_class_hotness;       // Average class hotness
    double average_instance_hotness;    // Average instance hotness
    
    // Performance tracking
    uint64_t creation_start_time;       // Creation start time
    uint64_t creation_end_time;         // Creation end time
    double total_creation_time_ms;      // Total creation time (milliseconds)
    double creation_overhead;           // Creation overhead percentage
} HiddenClassesContext;

// ============================================================================
// HIDDEN CLASSES FUNCTIONS
// ============================================================================

/**
 * @brief Core class management functions
 * 
 * These functions handle the main hidden class management process,
 * from initialization to class creation and optimization.
 */

/**
 * @brief Create a new hidden classes context
 * @return New HiddenClassesContext, or NULL on failure
 * @note The context is initialized with default settings
 */
HiddenClassesContext* hidden_classes_create(void);

/**
 * @brief Free a hidden classes context
 * @param context Context to free (can be NULL)
 * @note This function is safe to call with NULL
 */
void hidden_classes_free(HiddenClassesContext* context);

/**
 * @brief Create a new hidden class
 * @param context Hidden classes context
 * @param class_name Class name (optional)
 * @param parent_class_id Parent class ID (for transitions)
 * @return Class ID, or 0 on failure
 * @note This function creates a new hidden class
 */
uint32_t hidden_classes_create_class(HiddenClassesContext* context,
                                   const char* class_name,
                                   uint32_t parent_class_id);

/**
 * @brief Add a property to a hidden class
 * @param context Hidden classes context
 * @param class_id Class ID
 * @param property_name Property name
 * @param property_type Property type
 * @param is_writable Is property writable?
 * @param is_enumerable Is property enumerable?
 * @param is_configurable Is property configurable?
 * @return Property index, or -1 on failure
 * @note This function adds a property to a hidden class
 */
int hidden_classes_add_property(HiddenClassesContext* context,
                               uint32_t class_id,
                               const char* property_name,
                               uint8_t property_type,
                               int is_writable,
                               int is_enumerable,
                               int is_configurable);

/**
 * @brief Create a transition between hidden classes
 * @param context Hidden classes context
 * @param source_class_id Source class ID
 * @param property_name Property name that causes transition
 * @param property_type Property type that causes transition
 * @return Target class ID, or 0 on failure
 * @note This function creates a transition between hidden classes
 */
uint32_t hidden_classes_create_transition(HiddenClassesContext* context,
                                         uint32_t source_class_id,
                                         const char* property_name,
                                         uint8_t property_type);

/**
 * @brief Find or create a hidden class for an object
 * @param context Hidden classes context
 * @param object_properties Array of property names
 * @param property_count Number of properties
 * @param property_types Array of property types
 * @return Class ID, or 0 on failure
 * @note This function finds or creates a hidden class for an object
 */
uint32_t hidden_classes_find_or_create_class(HiddenClassesContext* context,
                                            const char** object_properties,
                                            uint32_t property_count,
                                            const uint8_t* property_types);

/**
 * @brief Property access functions
 * 
 * These functions handle property access and optimization.
 */

/**
 * @brief Get a property from an object instance
 * @param context Hidden classes context
 * @param instance_id Instance ID
 * @param property_name Property name
 * @return Property value, or NULL if not found
 * @note This function gets a property value from an object instance
 */
void* hidden_classes_get_property(HiddenClassesContext* context,
                                 uint32_t instance_id,
                                 const char* property_name);

/**
 * @brief Set a property on an object instance
 * @param context Hidden classes context
 * @param instance_id Instance ID
 * @param property_name Property name
 * @param property_value Property value
 * @param property_type Property type
 * @return 1 on success, 0 on failure
 * @note This function sets a property value on an object instance
 */
int hidden_classes_set_property(HiddenClassesContext* context,
                               uint32_t instance_id,
                               const char* property_name,
                               const void* property_value,
                               uint8_t property_type);

/**
 * @brief Check if an object instance has a property
 * @param context Hidden classes context
 * @param instance_id Instance ID
 * @param property_name Property name
 * @return 1 if property exists, 0 otherwise
 * @note This function checks if an object instance has a property
 */
int hidden_classes_has_property(HiddenClassesContext* context,
                               uint32_t instance_id,
                               const char* property_name);

/**
 * @brief Delete a property from an object instance
 * @param context Hidden classes context
 * @param instance_id Instance ID
 * @param property_name Property name
 * @return 1 on success, 0 on failure
 * @note This function deletes a property from an object instance
 */
int hidden_classes_delete_property(HiddenClassesContext* context,
                                  uint32_t instance_id,
                                  const char* property_name);

/**
 * @brief Instance management functions
 * 
 * These functions handle object instance creation and management.
 */

/**
 * @brief Create a new object instance
 * @param context Hidden classes context
 * @param class_id Class ID
 * @return Instance ID, or 0 on failure
 * @note This function creates a new object instance
 */
uint32_t hidden_classes_create_instance(HiddenClassesContext* context, uint32_t class_id);

/**
 * @brief Get an object instance by ID
 * @param context Hidden classes context
 * @param instance_id Instance ID
 * @return Object instance, or NULL if not found
 * @note This function retrieves an object instance by its ID
 */
ObjectInstance* hidden_classes_get_instance(HiddenClassesContext* context, uint32_t instance_id);

/**
 * @brief Remove an object instance
 * @param context Hidden classes context
 * @param instance_id Instance ID to remove
 * @return 1 on success, 0 on failure
 * @note This function removes an object instance
 */
int hidden_classes_remove_instance(HiddenClassesContext* context, uint32_t instance_id);

/**
 * @brief Clear all object instances
 * @param context Hidden classes context
 * @note This function removes all object instances
 */
void hidden_classes_clear_instances(HiddenClassesContext* context);

/**
 * @brief Optimization functions
 * 
 * These functions handle optimization of hidden classes and instances.
 */

/**
 * @brief Optimize a hidden class
 * @param context Hidden classes context
 * @param class_id Class ID to optimize
 * @return 1 on success, 0 on failure
 * @note This function optimizes a hidden class for better performance
 */
int hidden_classes_optimize_class(HiddenClassesContext* context, uint32_t class_id);

/**
 * @brief Optimize property layout
 * @param context Hidden classes context
 * @param class_id Class ID to optimize
 * @return 1 on success, 0 on failure
 * @note This function optimizes the property layout of a hidden class
 */
int hidden_classes_optimize_layout(HiddenClassesContext* context, uint32_t class_id);

/**
 * @brief Inline hot properties
 * @param context Hidden classes context
 * @param class_id Class ID to optimize
 * @return 1 on success, 0 on failure
 * @note This function inlines hot properties for better performance
 */
int hidden_classes_inline_hot_properties(HiddenClassesContext* context, uint32_t class_id);

/**
 * @brief Optimize transitions
 * @param context Hidden classes context
 * @return 1 on success, 0 on failure
 * @note This function optimizes transitions between hidden classes
 */
int hidden_classes_optimize_transitions(HiddenClassesContext* context);

/**
 * @brief Analysis functions
 * 
 * These functions analyze hidden classes and instances for optimization opportunities.
 */

/**
 * @brief Analyze class hotness
 * @param context Hidden classes context
 * @param class_id Class ID to analyze
 * @return Hotness score (0.0-1.0)
 * @note This function analyzes the hotness of a hidden class
 */
double hidden_classes_analyze_class_hotness(HiddenClassesContext* context, uint32_t class_id);

/**
 * @brief Analyze instance hotness
 * @param context Hidden classes context
 * @param instance_id Instance ID to analyze
 * @return Hotness score (0.0-1.0)
 * @note This function analyzes the hotness of an object instance
 */
double hidden_classes_analyze_instance_hotness(HiddenClassesContext* context, uint32_t instance_id);

/**
 * @brief Analyze transition patterns
 * @param context Hidden classes context
 * @return 1 on success, 0 on failure
 * @note This function analyzes transition patterns for optimization
 */
int hidden_classes_analyze_transition_patterns(HiddenClassesContext* context);

/**
 * @brief Find hot classes
 * @param context Hidden classes context
 * @param hot_classes Array to store hot class IDs
 * @param max_classes Maximum number of classes to return
 * @return Number of hot classes found
 * @note This function finds classes that are above the hotness threshold
 */
uint32_t hidden_classes_find_hot_classes(HiddenClassesContext* context,
                                        uint32_t* hot_classes,
                                        uint32_t max_classes);

/**
 * @brief Find hot transitions
 * @param context Hidden classes context
 * @param hot_transitions Array to store hot transition IDs
 * @param max_transitions Maximum number of transitions to return
 * @return Number of hot transitions found
 * @note This function finds transitions that are above the hotness threshold
 */
uint32_t hidden_classes_find_hot_transitions(HiddenClassesContext* context,
                                           uint32_t* hot_transitions,
                                           uint32_t max_transitions);

/**
 * @brief Configuration functions
 * 
 * These functions configure the hidden classes behavior.
 */

/**
 * @brief Set maximum number of classes
 * @param context Hidden classes context
 * @param max_classes Maximum number of classes
 * @note This function sets the maximum number of hidden classes
 */
void hidden_classes_set_max_classes(HiddenClassesContext* context, uint32_t max_classes);

/**
 * @brief Set hotness threshold
 * @param context Hidden classes context
 * @param threshold Hotness threshold (0.0-1.0)
 * @note This function sets the hotness threshold for optimization
 */
void hidden_classes_set_hotness_threshold(HiddenClassesContext* context, double threshold);

/**
 * @brief Enable or disable inline caching
 * @param context Hidden classes context
 * @param enable 1 to enable, 0 to disable
 * @note This function enables or disables inline caching
 */
void hidden_classes_set_inline_caching(HiddenClassesContext* context, int enable);

/**
 * @brief Enable or disable property inlining
 * @param context Hidden classes context
 * @param enable 1 to enable, 0 to disable
 * @note This function enables or disables property inlining
 */
void hidden_classes_set_property_inlining(HiddenClassesContext* context, int enable);

/**
 * @brief Enable or disable layout optimization
 * @param context Hidden classes context
 * @param enable 1 to enable, 0 to disable
 * @note This function enables or disables layout optimization
 */
void hidden_classes_set_layout_optimization(HiddenClassesContext* context, int enable);

/**
 * @brief Utility functions
 * 
 * These functions provide utility functionality for working with
 * hidden classes contexts and objects.
 */

/**
 * @brief Get hidden classes statistics
 * @param context Hidden classes context
 * @return Statistics string
 * @note Returns a formatted string with hidden classes statistics
 */
char* hidden_classes_get_statistics(HiddenClassesContext* context);

/**
 * @brief Print hidden class information
 * @param context Hidden classes context
 * @param class_id Class ID to print
 * @note Prints human-readable representation of a hidden class
 */
void hidden_classes_print_class(HiddenClassesContext* context, uint32_t class_id);

/**
 * @brief Print all hidden classes
 * @param context Hidden classes context
 * @note Prints human-readable representation of all hidden classes
 */
void hidden_classes_print_all_classes(HiddenClassesContext* context);

/**
 * @brief Print object instance information
 * @param context Hidden classes context
 * @param instance_id Instance ID to print
 * @note Prints human-readable representation of an object instance
 */
void hidden_classes_print_instance(HiddenClassesContext* context, uint32_t instance_id);

/**
 * @brief Print transition information
 * @param context Hidden classes context
 * @param transition_id Transition ID to print
 * @note Prints human-readable representation of a transition
 */
void hidden_classes_print_transition(HiddenClassesContext* context, uint32_t transition_id);

/**
 * @brief Validate hidden class
 * @param context Hidden classes context
 * @param class_id Class ID to validate
 * @return 1 if valid, 0 if invalid
 * @note Validates that a hidden class is correct and consistent
 */
int hidden_classes_validate_class(HiddenClassesContext* context, uint32_t class_id);

/**
 * @brief Export hidden classes data to file
 * @param context Hidden classes context
 * @param filename Output filename
 * @return 1 on success, 0 on failure
 * @note Exports hidden classes data to a file for analysis
 */
int hidden_classes_export_data(HiddenClassesContext* context, const char* filename);

/**
 * @brief Import hidden classes data from file
 * @param context Hidden classes context
 * @param filename Input filename
 * @return 1 on success, 0 on failure
 * @note Imports hidden classes data from a file
 */
int hidden_classes_import_data(HiddenClassesContext* context, const char* filename);

#endif // HIDDEN_CLASSES_H
