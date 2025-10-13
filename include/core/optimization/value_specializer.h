#ifndef VALUE_SPECIALIZER_H
#define VALUE_SPECIALIZER_H

#include "../interpreter/interpreter_core.h"
#include "../ast.h"
#include "bytecode_engine.h"
#include "hot_spot_tracker.h"
#include <stdint.h>
#include <stddef.h>

// ============================================================================
// VALUE SPECIALIZATION ENGINE
// ============================================================================

// Specialization types
typedef enum {
    SPECIALIZATION_NONE = 0,        // No specialization
    SPECIALIZATION_MONOMORPHIC = 1, // Single type specialization
    SPECIALIZATION_POLYMORPHIC = 2, // Multiple type specialization (2-3 types)
    SPECIALIZATION_MEGAMORPHIC = 3  // Too many types, disable specialization
} SpecializationType;

// Type specialization metadata
typedef struct {
    ValueType primary_type;         // Most common type
    ValueType secondary_type;       // Second most common type
    ValueType tertiary_type;        // Third most common type
    
    uint64_t primary_count;         // Count of primary type
    uint64_t secondary_count;       // Count of secondary type
    uint64_t tertiary_count;        // Count of tertiary type
    uint64_t total_count;           // Total type observations
    
    double primary_ratio;           // Ratio of primary type
    double secondary_ratio;         // Ratio of secondary type
    double tertiary_ratio;          // Ratio of tertiary type
    
    int is_stable;                  // Is specialization stable
    int specialization_level;       // Current specialization level (0-3)
    uint64_t last_observation_ns;   // Last type observation
} TypeSpecialization;

// Polymorphic inline cache entry
typedef struct {
    ValueType type;                 // Cached type
    void* specialized_code;         // Specialized bytecode/native code
    uint64_t hit_count;             // Number of hits
    uint64_t miss_count;            // Number of misses
    double hit_ratio;               // Hit ratio
    int is_valid;                   // Is this cache entry valid
} PolymorphicCacheEntry;

// Polymorphic inline cache
typedef struct {
    PolymorphicCacheEntry* entries; // Cache entries (max 3)
    size_t entry_count;             // Number of entries
    size_t max_entries;             // Maximum entries (3)
    uint64_t total_hits;            // Total cache hits
    uint64_t total_misses;          // Total cache misses
    double overall_hit_ratio;       // Overall hit ratio
} PolymorphicInlineCache;

// Value specialization context
typedef struct {
    // Type specialization data
    TypeSpecialization* specializations; // Per-function specializations
    size_t specialization_count;         // Number of specializations
    size_t specialization_capacity;      // Allocated capacity
    
    // Polymorphic inline caches
    PolymorphicInlineCache* caches;      // Per-function caches
    size_t cache_count;                  // Number of caches
    size_t cache_capacity;               // Allocated capacity
    
    // Specialization settings
    int enable_monomorphic;              // Enable monomorphic specialization
    int enable_polymorphic;              // Enable polymorphic specialization
    int enable_inline_caching;           // Enable inline caching
    int enable_type_guards;              // Enable type guard checks
    
    // Thresholds
    uint64_t monomorphic_threshold;      // Threshold for monomorphic specialization
    uint64_t polymorphic_threshold;      // Threshold for polymorphic specialization
    double stability_threshold;          // Stability threshold (0.8 = 80%)
    uint64_t observation_window_ns;      // Observation window in nanoseconds
    
    // Statistics
    uint64_t total_observations;         // Total type observations
    uint64_t specialization_count_total; // Total specializations created
    uint64_t guard_failures;             // Total guard failures
    uint64_t deoptimizations;            // Total deoptimizations
    double average_speedup;              // Average speedup from specialization
} ValueSpecializer;

// ============================================================================
// VALUE SPECIALIZER FUNCTIONS
// ============================================================================

// Context management
ValueSpecializer* value_specializer_create(void);
void value_specializer_free(ValueSpecializer* specializer);
void value_specializer_reset(ValueSpecializer* specializer);

// Configuration
void value_specializer_set_monomorphic(ValueSpecializer* specializer, int enabled);
void value_specializer_set_polymorphic(ValueSpecializer* specializer, int enabled);
void value_specializer_set_inline_caching(ValueSpecializer* specializer, int enabled);
void value_specializer_set_type_guards(ValueSpecializer* specializer, int enabled);
void value_specializer_set_thresholds(ValueSpecializer* specializer, 
                                     uint64_t mono_threshold, 
                                     uint64_t poly_threshold,
                                     double stability_threshold);

// Type observation and analysis
void value_specializer_observe_type(ValueSpecializer* specializer, 
                                   ASTNode* function_node,
                                   size_t parameter_index,
                                   ValueType type);

void value_specializer_observe_return_type(ValueSpecializer* specializer,
                                          ASTNode* function_node,
                                          ValueType return_type);

SpecializationType value_specializer_analyze_function(ValueSpecializer* specializer,
                                                     ASTNode* function_node);

TypeSpecialization* value_specializer_get_specialization(ValueSpecializer* specializer,
                                                        ASTNode* function_node);

// Specialized code generation
BytecodeProgram* value_specializer_generate_monomorphic_bytecode(
    ValueSpecializer* specializer,
    ASTNode* function_node,
    TypeSpecialization* specialization);

BytecodeProgram* value_specializer_generate_polymorphic_bytecode(
    ValueSpecializer* specializer,
    ASTNode* function_node,
    PolymorphicInlineCache* cache);

// Polymorphic inline cache management
PolymorphicInlineCache* value_specializer_get_or_create_cache(
    ValueSpecializer* specializer,
    ASTNode* function_node);

int value_specializer_cache_lookup(PolymorphicInlineCache* cache,
                                  ValueType type,
                                  void** specialized_code);

void value_specializer_cache_update(PolymorphicInlineCache* cache,
                                   ValueType type,
                                   void* specialized_code,
                                   int hit);

// Type guard checking
int value_specializer_check_type_guards(ValueSpecializer* specializer,
                                       ASTNode* function_node,
                                       Value* arguments,
                                       size_t argument_count);

void value_specializer_record_guard_failure(ValueSpecializer* specializer,
                                           ASTNode* function_node);

// Specialization optimization
int value_specializer_optimize_bytecode(ValueSpecializer* specializer,
                                       BytecodeProgram* bytecode,
                                       TypeSpecialization* specialization);

int value_specializer_inline_specialized_operations(ValueSpecializer* specializer,
                                                   BytecodeProgram* bytecode,
                                                   TypeSpecialization* specialization);

// Deoptimization
void value_specializer_deoptimize_function(ValueSpecializer* specializer,
                                          ASTNode* function_node);

void value_specializer_deoptimize_all(ValueSpecializer* specializer);

// Statistics and reporting
void value_specializer_print_statistics(ValueSpecializer* specializer);
void value_specializer_print_specializations(ValueSpecializer* specializer);
double value_specializer_get_average_speedup(ValueSpecializer* specializer);
uint64_t value_specializer_get_total_observations(ValueSpecializer* specializer);

// Integration with interpreter
void value_specializer_initialize_for_interpreter(Interpreter* interpreter);
void value_specializer_cleanup_for_interpreter(Interpreter* interpreter);
ValueSpecializer* value_specializer_get_from_interpreter(Interpreter* interpreter);

// ============================================================================
// TYPE SPECIALIZATION UTILITIES
// ============================================================================

// Type analysis
int value_specializer_is_type_stable(TypeSpecialization* specialization);
int value_specializer_is_monomorphic(TypeSpecialization* specialization);
int value_specializer_is_polymorphic(TypeSpecialization* specialization);
int value_specializer_is_megamorphic(TypeSpecialization* specialization);

// Specialization lookup (already declared above)

// Type ratio calculations
double value_specializer_calculate_primary_ratio(TypeSpecialization* specialization);
double value_specializer_calculate_secondary_ratio(TypeSpecialization* specialization);
double value_specializer_calculate_tertiary_ratio(TypeSpecialization* specialization);

// Specialization decisions
SpecializationType value_specializer_determine_specialization_type(
    TypeSpecialization* specialization);

int value_specializer_should_specialize(ValueSpecializer* specializer,
                                       TypeSpecialization* specialization);

int value_specializer_should_deoptimize(ValueSpecializer* specializer,
                                       TypeSpecialization* specialization);

// Memory management
void value_specializer_free_specialization(TypeSpecialization* specialization);
void value_specializer_free_cache(PolymorphicInlineCache* cache);

// ============================================================================
// SPECIALIZED BYTECODE GENERATION
// ============================================================================

// Type-specific bytecode instructions
typedef enum {
    // Specialized arithmetic operations
    BC_ADD_INT_INT = 100,           // Add two integers
    BC_ADD_FLOAT_FLOAT = 101,       // Add two floats
    BC_ADD_INT_FLOAT = 102,         // Add int and float
    BC_ADD_FLOAT_INT = 103,         // Add float and int
    
    BC_SUB_INT_INT = 104,           // Subtract two integers
    BC_SUB_FLOAT_FLOAT = 105,       // Subtract two floats
    BC_SUB_INT_FLOAT = 106,         // Subtract int from float
    BC_SUB_FLOAT_INT = 107,         // Subtract float from int
    
    BC_MUL_INT_INT = 108,           // Multiply two integers
    BC_MUL_FLOAT_FLOAT = 109,       // Multiply two floats
    BC_MUL_INT_FLOAT = 110,         // Multiply int and float
    BC_MUL_FLOAT_INT = 111,         // Multiply float and int
    
    BC_DIV_INT_INT = 112,           // Divide two integers
    BC_DIV_FLOAT_FLOAT = 113,       // Divide two floats
    BC_DIV_INT_FLOAT = 114,         // Divide int by float
    BC_DIV_FLOAT_INT = 115,         // Divide float by int
    
    // Specialized comparison operations
    BC_EQ_INT_INT = 116,            // Compare two integers
    BC_EQ_FLOAT_FLOAT = 117,        // Compare two floats
    BC_EQ_INT_FLOAT = 118,          // Compare int and float
    BC_EQ_FLOAT_INT = 119,          // Compare float and int
    
    BC_LT_INT_INT = 120,            // Less than (integers)
    BC_LT_FLOAT_FLOAT = 121,        // Less than (floats)
    BC_LT_INT_FLOAT = 122,          // Less than (int, float)
    BC_LT_FLOAT_INT = 123,          // Less than (float, int)
    
    BC_GT_INT_INT = 124,            // Greater than (integers)
    BC_GT_FLOAT_FLOAT = 125,        // Greater than (floats)
    BC_GT_INT_FLOAT = 126,          // Greater than (int, float)
    BC_GT_FLOAT_INT = 127,          // Greater than (float, int)
    
    // Specialized array operations
    BC_LOAD_ARRAY_INT = 128,        // Load from integer array
    BC_LOAD_ARRAY_FLOAT = 129,      // Load from float array
    BC_LOAD_ARRAY_STRING = 130,     // Load from string array
    BC_LOAD_ARRAY_OBJECT = 131,     // Load from object array
    
    BC_STORE_ARRAY_INT = 132,       // Store to integer array
    BC_STORE_ARRAY_FLOAT = 133,     // Store to float array
    BC_STORE_ARRAY_STRING = 134,    // Store to string array
    BC_STORE_ARRAY_OBJECT = 135,    // Store to object array
    
    // Type guard operations
    BC_GUARD_TYPE = 136,            // Type guard check
    BC_GUARD_INT = 137,             // Integer type guard
    BC_GUARD_FLOAT = 138,           // Float type guard
    BC_GUARD_STRING = 139,          // String type guard
    BC_GUARD_OBJECT = 140,          // Object type guard
    
    // Deoptimization operations
    BC_DEOPTIMIZE = 141,            // Deoptimize to generic code
    BC_DEOPTIMIZE_IF_FALSE = 142    // Deoptimize if condition is false
} SpecializedBytecodeOp;

// Specialized bytecode generation functions
BytecodeProgram* value_specializer_generate_int_specialized_bytecode(
    ASTNode* function_node,
    TypeSpecialization* specialization);

BytecodeProgram* value_specializer_generate_float_specialized_bytecode(
    ASTNode* function_node,
    TypeSpecialization* specialization);

BytecodeProgram* value_specializer_generate_string_specialized_bytecode(
    ASTNode* function_node,
    TypeSpecialization* specialization);

// BytecodeProgram* value_specializer_generate_polymorphic_bytecode(
//     ASTNode* function_node,
//     PolymorphicInlineCache* cache);

#endif // VALUE_SPECIALIZER_H
