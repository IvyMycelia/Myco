#include "shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

// ============================================================================
// GLOBAL CONFIGURATION
// ============================================================================

static bool global_debug_enabled = false;
static bool global_verbose_enabled = false;

// Component-specific debug settings
#define MAX_COMPONENTS 32
static struct {
    char name[64];
    bool debug_enabled;
} component_debug_settings[MAX_COMPONENTS];
static int component_count = 0;

// Memory tracking structures
typedef struct {
    char component[64];
    size_t total_allocated;
    size_t total_freed;
    size_t current_usage;
    int allocation_count;
    int free_count;
    int peak_usage;
} MemoryStats;

#define MAX_MEMORY_TRACKING 32
static MemoryStats memory_stats[MAX_MEMORY_TRACKING];
static int memory_stats_count = 0;
static bool memory_tracking_enabled = false;

// ============================================================================
// SHARED ERROR HANDLING UTILITIES
// ============================================================================

void shared_error_report(const char* component, const char* function, 
                        const char* message, int line, int column) {
    if (!component || !function || !message) {
        fprintf(stderr, "Error: Invalid parameters to shared_error_report\n");
        return;
    }
    
    fprintf(stderr, "Error [%s::%s]: %s", component, function, message);
    if (line > 0) {
        fprintf(stderr, " (Line %d", line);
        if (column > 0) {
            fprintf(stderr, ", Column %d", column);
        }
        fprintf(stderr, ")");
    }
    fprintf(stderr, "\n");
}

void shared_error_report_code(const char* component, const char* function,
                             int error_code, const char* message, int line, int column) {
    if (!component || !function || !message) {
        fprintf(stderr, "Error: Invalid parameters to shared_error_report_code\n");
        return;
    }
    
    fprintf(stderr, "Error [%s::%s] E%d: %s", component, function, error_code, message);
    if (line > 0) {
        fprintf(stderr, " (Line %d", line);
        if (column > 0) {
            fprintf(stderr, ", Column %d", column);
        }
        fprintf(stderr, ")");
    }
    fprintf(stderr, "\n");
}

void* shared_malloc_safe(size_t size, const char* component, const char* function, int line) {
    if (size == 0) {
        shared_error_report(component, function, "Attempted to allocate 0 bytes", line, 0);
        return NULL;
    }
    
    void* ptr = malloc(size);
    if (!ptr) {
        shared_error_report(component, function, "Memory allocation failed", line, 0);
        return NULL;
    }
    
    if (shared_config_get_component_debug(component)) {
        shared_debug_printf(component, function, "Allocated %zu bytes at %p", size, ptr);
    }
    
    return ptr;
}

void* shared_realloc_safe(void* ptr, size_t size, const char* component, const char* function, int line) {
    if (size == 0) {
        shared_free(ptr);
        return NULL;
    }
    
    void* new_ptr = realloc(ptr, size);
    if (!new_ptr) {
        shared_error_report(component, function, "Memory reallocation failed", line, 0);
        return NULL;
    }
    
    if (shared_config_get_component_debug(component)) {
        shared_debug_printf(component, function, "Reallocated %zu bytes at %p", size, new_ptr);
    }
    
    return new_ptr;
}

// ============================================================================
// SHARED MEMORY MANAGEMENT UTILITIES
// ============================================================================

void* shared_malloc(size_t size) {
    if (size == 0) return NULL;
    return malloc(size);
}

void* shared_realloc(void* ptr, size_t size) {
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    return realloc(ptr, size);
}

void shared_free(void* ptr) {
    if (ptr) {
        free(ptr);
    }
}

void shared_free_safe(void* ptr, const char* component, const char* function, int line) {
    if (ptr) {
        if (shared_config_get_component_debug(component)) {
            shared_debug_printf(component, function, "Freeing memory at %p", ptr);
        }
        free(ptr);
    }
}

void shared_memzero(void* ptr, size_t size) {
    if (ptr && size > 0) {
        memset(ptr, 0, size);
    }
}

void shared_memcpy_safe(void* dest, const void* src, size_t size) {
    if (dest && src && size > 0) {
        memcpy(dest, src, size);
    }
}

// ============================================================================
// SHARED STRING UTILITIES
// ============================================================================

char* shared_strdup(const char* str) {
    if (!str) return NULL;
    size_t len = strlen(str) + 1;
    char* copy = shared_malloc(len);
    if (copy) {
        strcpy(copy, str);
    }
    return copy;
}

char* shared_strndup(const char* str, size_t n) {
    if (!str) return NULL;
    size_t len = strnlen(str, n);
    char* copy = shared_malloc(len + 1);
    if (copy) {
        strncpy(copy, str, len);
        copy[len] = '\0';
    }
    return copy;
}

int shared_strcmp_safe(const char* s1, const char* s2) {
    if (!s1 && !s2) return 0;
    if (!s1) return -1;
    if (!s2) return 1;
    return strcmp(s1, s2);
}

size_t shared_strlen_safe(const char* str) {
    return str ? strlen(str) : 0;
}

char* shared_strprintf(const char* format, ...) {
    if (!format) return NULL;
    
    va_list args;
    va_start(args, format);
    
    // Get the required size
    va_list args_copy;
    va_copy(args_copy, args);
    int size = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);
    
    if (size < 0) {
        va_end(args);
        return NULL;
    }
    
    char* result = shared_malloc(size + 1);
    if (result) {
        vsnprintf(result, size + 1, format, args);
    }
    
    va_end(args);
    return result;
}

char* shared_strcat_safe(char* dest, const char* src, size_t dest_size) {
    if (!dest || !src || dest_size == 0) return dest;
    
    size_t dest_len = strnlen(dest, dest_size - 1);
    size_t src_len = strlen(src);
    
    if (dest_len + src_len >= dest_size) {
        src_len = dest_size - dest_len - 1;
    }
    
    if (src_len > 0) {
        strncat(dest, src, src_len);
    }
    
    return dest;
}

// ============================================================================
// SHARED TYPE UTILITIES
// ============================================================================

bool shared_is_numeric_type(int type) {
    // This would need to be adapted based on your type system
    // For now, assuming some numeric type constants
    return (type >= 1 && type <= 5); // Example range
}

bool shared_is_collection_type(int type) {
    // This would need to be adapted based on your type system
    return (type >= 10 && type <= 20); // Example range
}

bool shared_is_primitive_type(int type) {
    return shared_is_numeric_type(type) || (type >= 6 && type <= 9);
}

const char* shared_type_to_string(int type) {
    // This would need to be adapted based on your actual type system
    switch (type) {
        case 1: return "Int";
        case 2: return "Float";
        case 3: return "String";
        case 4: return "Bool";
        case 5: return "Null";
        default: return "Unknown";
    }
}

int shared_string_to_type(const char* type_str) {
    if (!type_str) return -1;
    
    if (strcmp(type_str, "Int") == 0) return 1;
    if (strcmp(type_str, "Float") == 0) return 2;
    if (strcmp(type_str, "String") == 0) return 3;
    if (strcmp(type_str, "Bool") == 0) return 4;
    if (strcmp(type_str, "Null") == 0) return 5;
    
    return -1;
}

// ============================================================================
// SHARED VALIDATION UTILITIES
// ============================================================================

bool shared_validate_not_null(const void* ptr, const char* component, const char* function) {
    if (!ptr) {
        shared_error_report(component, function, "Null pointer validation failed", 0, 0);
        return false;
    }
    return true;
}

bool shared_validate_range(int value, int min, int max, const char* component, const char* function) {
    if (value < min || value > max) {
        char msg[256];
        snprintf(msg, sizeof(msg), "Value %d out of range [%d, %d]", value, min, max);
        shared_error_report(component, function, msg, 0, 0);
        return false;
    }
    return true;
}

bool shared_validate_string(const char* str, const char* component, const char* function) {
    if (!str) {
        shared_error_report(component, function, "Null string validation failed", 0, 0);
        return false;
    }
    if (strlen(str) == 0) {
        shared_error_report(component, function, "Empty string validation failed", 0, 0);
        return false;
    }
    return true;
}

bool shared_validate_array_bounds(size_t index, size_t array_size, const char* component, const char* function) {
    if (index >= array_size) {
        char msg[256];
        snprintf(msg, sizeof(msg), "Array index %zu out of bounds (size: %zu)", index, array_size);
        shared_error_report(component, function, msg, 0, 0);
        return false;
    }
    return true;
}

// ============================================================================
// SHARED DEBUG UTILITIES
// ============================================================================

void shared_debug_print(const char* component, const char* function, const char* message) {
    if (shared_config_get_component_debug(component)) {
        fprintf(stderr, "[DEBUG %s::%s] %s\n", component, function, message);
    }
}

void shared_debug_printf(const char* component, const char* function, const char* format, ...) {
    if (shared_config_get_component_debug(component)) {
        fprintf(stderr, "[DEBUG %s::%s] ", component, function);
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        fprintf(stderr, "\n");
    }
}

uint64_t shared_time_get_microseconds(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

// Simple timing measurement (could be enhanced with a proper timing system)
static uint64_t timing_start = 0;

void shared_time_measure_start(const char* label) {
    if (shared_config_get_verbose()) {
        timing_start = shared_time_get_microseconds();
        fprintf(stderr, "[TIMING] Started: %s\n", label);
    }
}

void shared_time_measure_end(const char* label) {
    if (shared_config_get_verbose() && timing_start > 0) {
        uint64_t end_time = shared_time_get_microseconds();
        uint64_t duration = end_time - timing_start;
        fprintf(stderr, "[TIMING] %s: %llu microseconds\n", label, (unsigned long long)duration);
    }
}

// ============================================================================
// SHARED CONFIGURATION UTILITIES
// ============================================================================

void shared_config_set_debug(bool enable) {
    global_debug_enabled = enable;
}

void shared_config_set_verbose(bool enable) {
    global_verbose_enabled = enable;
}

bool shared_config_get_debug(void) {
    return global_debug_enabled;
}

bool shared_config_get_verbose(void) {
    return global_verbose_enabled;
}

void shared_config_set_component_debug(const char* component, bool enable) {
    if (!component) return;
    
    // Find existing component
    for (int i = 0; i < component_count; i++) {
        if (strcmp(component_debug_settings[i].name, component) == 0) {
            component_debug_settings[i].debug_enabled = enable;
            return;
        }
    }
    
    // Add new component if there's space
    if (component_count < MAX_COMPONENTS) {
        strncpy(component_debug_settings[component_count].name, component, sizeof(component_debug_settings[component_count].name) - 1);
        component_debug_settings[component_count].name[sizeof(component_debug_settings[component_count].name) - 1] = '\0';
        component_debug_settings[component_count].debug_enabled = enable;
        component_count++;
    }
}

bool shared_config_get_component_debug(const char* component) {
    if (!component) return global_debug_enabled;
    
    for (int i = 0; i < component_count; i++) {
        if (strcmp(component_debug_settings[i].name, component) == 0) {
            return component_debug_settings[i].debug_enabled;
        }
    }
    
    return global_debug_enabled;
}


// ============================================================================
// ENHANCED MEMORY MANAGEMENT WITH TRACKING
// ============================================================================

// Helper function to get or create memory stats for a component
static MemoryStats* get_memory_stats(const char* component) {
    if (!component) return NULL;
    
    // Find existing component
    for (int i = 0; i < memory_stats_count; i++) {
        if (strcmp(memory_stats[i].component, component) == 0) {
            return &memory_stats[i];
        }
    }
    
    // Add new component if there is space
    if (memory_stats_count < MAX_MEMORY_TRACKING) {
        strncpy(memory_stats[memory_stats_count].component, component, sizeof(memory_stats[memory_stats_count].component) - 1);
        memory_stats[memory_stats_count].component[sizeof(memory_stats[memory_stats_count].component) - 1] = 0;
        memory_stats[memory_stats_count].total_allocated = 0;
        memory_stats[memory_stats_count].total_freed = 0;
        memory_stats[memory_stats_count].current_usage = 0;
        memory_stats[memory_stats_count].allocation_count = 0;
        memory_stats[memory_stats_count].free_count = 0;
        memory_stats[memory_stats_count].peak_usage = 0;
        return &memory_stats[memory_stats_count++];
    }
    
    return NULL;
}

void* shared_malloc_tracked(size_t size, const char* component, const char* function, int line) {
    if (size == 0) {
        shared_error_report(component, function, "Attempted to allocate 0 bytes", line, 0);
        return NULL;
    }
    
    void* ptr = malloc(size);
    if (!ptr) {
        shared_error_report(component, function, "Memory allocation failed", line, 0);
        return NULL;
    }
    
    // Update memory tracking
    if (memory_tracking_enabled) {
        MemoryStats* stats = get_memory_stats(component);
        if (stats) {
            stats->total_allocated += size;
            stats->current_usage += size;
            stats->allocation_count++;
            if (stats->current_usage > stats->peak_usage) {
                stats->peak_usage = stats->current_usage;
            }
        }
    }
    
    if (shared_config_get_component_debug(component)) {
        shared_debug_printf(component, function, "Allocated %zu bytes at %p", size, ptr);
    }
    
    return ptr;
}

void* shared_realloc_tracked(void* ptr, size_t size, const char* component, const char* function, int line) {
    if (size == 0) {
        shared_free_tracked(ptr, component, function, line);
        return NULL;
    }
    
    void* new_ptr = realloc(ptr, size);
    if (!new_ptr) {
        shared_error_report(component, function, "Memory reallocation failed", line, 0);
        return NULL;
    }
    
    // Update memory tracking
    if (memory_tracking_enabled) {
        MemoryStats* stats = get_memory_stats(component);
        if (stats) {
            stats->total_allocated += size;
            stats->current_usage += size;
            stats->allocation_count++;
            if (stats->current_usage > stats->peak_usage) {
                stats->peak_usage = stats->current_usage;
            }
        }
    }
    
    if (shared_config_get_component_debug(component)) {
        shared_debug_printf(component, function, "Reallocated %zu bytes at %p", size, new_ptr);
    }
    
    return new_ptr;
}

void shared_free_tracked(void* ptr, const char* component, const char* function, int line) {
    if (ptr) {
        if (memory_tracking_enabled) {
            MemoryStats* stats = get_memory_stats(component);
            if (stats) {
                stats->free_count++;
            }
        }
        
        if (shared_config_get_component_debug(component)) {
            shared_debug_printf(component, function, "Freeing memory at %p", ptr);
        }
        free(ptr);
    }
}

bool shared_validate_memory(void* ptr, size_t size, const char* component, const char* function, int line) {
    if (!ptr) {
        shared_error_report(component, function, "Null pointer validation failed", line, 0);
        return false;
    }
    
    if (size == 0) {
        shared_error_report(component, function, "Zero size validation failed", line, 0);
        return false;
    }
    
    return true;
}

void shared_memory_stats(const char* component) {
    if (!component) {
        shared_memory_report_all();
        return;
    }
    
    MemoryStats* stats = get_memory_stats(component);
    if (stats) {
        printf("Memory Stats for %s:\n", component);
        printf("  Total Allocated: %zu bytes\n", stats->total_allocated);
        printf("  Total Freed: %zu bytes\n", stats->total_freed);
        printf("  Current Usage: %zu bytes\n", stats->current_usage);
        printf("  Peak Usage: %zu bytes\n", stats->peak_usage);
        printf("  Allocation Count: %d\n", stats->allocation_count);
        printf("  Free Count: %d\n", stats->free_count);
        printf("  Leak Detection: %s\n", (stats->current_usage > 0) ? "POTENTIAL LEAK" : "OK");
    } else {
        printf("No memory stats available for component: %s\n", component);
    }
}

void shared_memory_cleanup(const char* component) {
    if (!component) return;
    
    MemoryStats* stats = get_memory_stats(component);
    if (stats) {
        printf("Memory cleanup for %s: %zu bytes potentially leaked\n", component, stats->current_usage);
        stats->total_allocated = 0;
        stats->total_freed = 0;
        stats->current_usage = 0;
        stats->allocation_count = 0;
        stats->free_count = 0;
        stats->peak_usage = 0;
    }
}

void shared_memory_report_all(void) {
    printf("=== Memory Usage Report ===\n");
    for (int i = 0; i < memory_stats_count; i++) {
        MemoryStats* stats = &memory_stats[i];
        printf("Component: %s\n", stats->component);
        printf("  Total Allocated: %zu bytes\n", stats->total_allocated);
        printf("  Total Freed: %zu bytes\n", stats->total_freed);
        printf("  Current Usage: %zu bytes\n", stats->current_usage);
        printf("  Peak Usage: %zu bytes\n", stats->peak_usage);
        printf("  Allocation Count: %d\n", stats->allocation_count);
        printf("  Free Count: %d\n", stats->free_count);
        printf("  Leak Detection: %s\n", (stats->current_usage > 0) ? "POTENTIAL LEAK" : "OK");
        printf("\n");
    }
}

void shared_memory_tracking_enable(bool enable) {
    memory_tracking_enabled = enable;
}

bool shared_memory_tracking_is_enabled(void) {
    return memory_tracking_enabled;
}
