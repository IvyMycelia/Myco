#ifndef SHARED_UTILITIES_H
#define SHARED_UTILITIES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// SHARED ERROR HANDLING UTILITIES
// ============================================================================

// Standardized error reporting function
void shared_error_report(const char* component, const char* function, 
                        const char* message, int line, int column);

// Error reporting with error codes
void shared_error_report_code(const char* component, const char* function,
                             int error_code, const char* message, int line, int column);

// Memory allocation error reporting
void* shared_malloc_safe(size_t size, const char* component, const char* function, int line);
void* shared_realloc_safe(void* ptr, size_t size, const char* component, const char* function, int line);
void shared_free_safe(void* ptr, const char* component, const char* function, int line);

// Enhanced memory management with tracking
void* shared_malloc_tracked(size_t size, const char* component, const char* function, int line);
void* shared_realloc_tracked(void* ptr, size_t size, const char* component, const char* function, int line);
void shared_free_tracked(void* ptr, const char* component, const char* function, int line);

// Memory validation and debugging
bool shared_validate_memory(void* ptr, size_t size, const char* component, const char* function, int line);
void shared_memory_stats(const char* component);
void shared_memory_cleanup(const char* component);
void shared_memory_report_all(void);

// Memory tracking control
void shared_memory_tracking_enable(bool enable);
bool shared_memory_tracking_is_enabled(void);
size_t shared_get_tracked_allocation_count(void);
void shared_print_allocation_stats(void);

// ============================================================================
// SHARED MEMORY MANAGEMENT UTILITIES
// ============================================================================

// Safe memory operations with error reporting
void* shared_malloc(size_t size);
void* shared_realloc(void* ptr, size_t size);
void shared_free(void* ptr);

// Memory initialization utilities
void shared_memzero(void* ptr, size_t size);
void shared_memcpy_safe(void* dest, const void* src, size_t size);

// ============================================================================
// SHARED STRING UTILITIES
// ============================================================================

// String operations with consistent error handling
char* shared_strdup(const char* str);
char* shared_strndup(const char* str, size_t n);
int shared_strcmp_safe(const char* s1, const char* s2);
size_t shared_strlen_safe(const char* str);

// String formatting utilities
char* shared_strprintf(const char* format, ...);
char* shared_strcat_safe(char* dest, const char* src, size_t dest_size);

// ============================================================================
// SHARED TYPE UTILITIES
// ============================================================================

// Type checking utilities
bool shared_is_numeric_type(int type);
bool shared_is_collection_type(int type);
bool shared_is_primitive_type(int type);

// Type conversion utilities
const char* shared_type_to_string(int type);
int shared_string_to_type(const char* type_str);

// ============================================================================
// SHARED VALIDATION UTILITIES
// ============================================================================

// Parameter validation
bool shared_validate_not_null(const void* ptr, const char* component, const char* function);
bool shared_validate_range(int value, int min, int max, const char* component, const char* function);
bool shared_validate_string(const char* str, const char* component, const char* function);

// Array bounds checking
bool shared_validate_array_bounds(size_t index, size_t array_size, const char* component, const char* function);

// ============================================================================
// SHARED DEBUG UTILITIES
// ============================================================================

// Debug output with component identification
void shared_debug_print(const char* component, const char* function, const char* message);
void shared_debug_printf(const char* component, const char* function, const char* format, ...);

// Performance timing utilities
uint64_t shared_time_get_microseconds(void);
void shared_time_measure_start(const char* label);
void shared_time_measure_end(const char* label);

// ============================================================================
// SHARED CONFIGURATION UTILITIES
// ============================================================================

// Global configuration management
void shared_config_set_debug(bool enable);
void shared_config_set_verbose(bool enable);
bool shared_config_get_debug(void);
bool shared_config_get_verbose(void);

// Component-specific configuration
void shared_config_set_component_debug(const char* component, bool enable);
bool shared_config_get_component_debug(const char* component);

// ============================================================================
// MACROS FOR COMMON PATTERNS
// ============================================================================

// Safe memory allocation with automatic error reporting
#define SHARED_MALLOC(size) shared_malloc_safe(size, __FILE__, __func__, __LINE__)
#define SHARED_REALLOC(ptr, size) shared_realloc_safe(ptr, size, __FILE__, __func__, __LINE__)

// Safe string operations
#define SHARED_STRDUP(str) shared_strdup(str)
#define SHARED_STRNDUP(str, n) shared_strndup(str, n)

// Validation macros
#define SHARED_VALIDATE_NOT_NULL(ptr) shared_validate_not_null(ptr, __FILE__, __func__)
#define SHARED_VALIDATE_RANGE(value, min, max) shared_validate_range(value, min, max, __FILE__, __func__)
#define SHARED_VALIDATE_STRING(str) shared_validate_string(str, __FILE__, __func__)

// Debug macros
#define SHARED_DEBUG(msg) shared_debug_print(__FILE__, __func__, msg)
#define SHARED_DEBUG_PRINTF(fmt, ...) shared_debug_printf(__FILE__, __func__, fmt, __VA_ARGS__)

#endif // SHARED_UTILITIES_H
