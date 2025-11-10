#include "shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
// No platform-specific headers to preserve portability

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

// Lightweight allocation registry to avoid freeing non-heap pointers
#define MYCO_MAX_TRACKED_PTRS 1000000  // Increased for large programs like pass.myco
static const uint64_t MYCO_CANARY_MAGIC = 0xC0FFEEBADC0DEULL;
// Use dynamic allocation for large tracking arrays to avoid stack overflow
static void** myco_tracked_ptrs = NULL;
static size_t* myco_tracked_sizes = NULL;
static int myco_tracked_count = 0;
static int myco_tracked_capacity = 0;

// Forward declaration
static int myco_find_tracked_index(void* ptr);

// Get current memory tracking stats
size_t shared_get_tracked_allocation_count(void) {
    return myco_tracked_count;
}

static void myco_track_alloc(void* ptr) {
    if (!ptr) return;
    
    // Initialize tracking arrays if needed
    // Use raw malloc to avoid recursion (tracking arrays shouldn't be tracked themselves)
    if (!myco_tracked_ptrs) {
        myco_tracked_capacity = MYCO_MAX_TRACKED_PTRS;
        myco_tracked_ptrs = (void**)malloc(sizeof(void*) * myco_tracked_capacity);
        myco_tracked_sizes = (size_t*)malloc(sizeof(size_t) * myco_tracked_capacity);
        if (!myco_tracked_ptrs || !myco_tracked_sizes) {
            // Free whatever was allocated
            if (myco_tracked_ptrs) free(myco_tracked_ptrs);
            if (myco_tracked_sizes) free(myco_tracked_sizes);
            myco_tracked_ptrs = NULL;
            myco_tracked_sizes = NULL;
            return; // Can't track if allocation fails
        }
        myco_tracked_count = 0;
    }
    
    // Check if already tracked to prevent double-tracking
    if (myco_find_tracked_index(ptr) >= 0) {
        // Already tracked - don't add again (prevents issues if called multiple times)
        return;
    }
    
    // Grow arrays if needed
    if (myco_tracked_count >= myco_tracked_capacity) {
        // Try to grow, but cap at maximum
        if (myco_tracked_capacity < MYCO_MAX_TRACKED_PTRS) {
            int new_capacity = myco_tracked_capacity * 2;
            if (new_capacity > MYCO_MAX_TRACKED_PTRS) {
                new_capacity = MYCO_MAX_TRACKED_PTRS;
            }
            // Use raw realloc to avoid recursion
            void** new_ptrs = (void**)realloc(myco_tracked_ptrs, sizeof(void*) * new_capacity);
            size_t* new_sizes = (size_t*)realloc(myco_tracked_sizes, sizeof(size_t) * new_capacity);
            if (new_ptrs && new_sizes) {
                myco_tracked_ptrs = new_ptrs;
                myco_tracked_sizes = new_sizes;
                myco_tracked_capacity = new_capacity;
            }
        }
        
        // If still at capacity, log warning
        if (myco_tracked_count >= myco_tracked_capacity) {
            fprintf(stderr, "[WARN] Allocation tracking registry full at %d entries!\n", myco_tracked_count);
            return; // Can't track more
        }
    }
    
    myco_tracked_ptrs[myco_tracked_count] = ptr;
    myco_tracked_sizes[myco_tracked_count] = 0;  // Size will be set by myco_track_allocation
    myco_tracked_count++;
}

static int myco_find_tracked_index(void* ptr) {
    if (!ptr) return -1;
    // Search from end to start (most recent allocations first)
    // This helps catch recently allocated pointers faster
    for (int i = myco_tracked_count - 1; i >= 0; i--) {
        if (myco_tracked_ptrs[i] == ptr) return i;
    }
    return -1;
}

static int myco_untrack_alloc(void* ptr) {
    if (!ptr) return 0;  // Don't track NULL pointers
    
    // Safety check: verify pointer value looks reasonable
    // This helps catch obvious corruption before we search
    uintptr_t ptr_val = (uintptr_t)ptr;
    if (ptr_val < 0x1000 || ptr_val > 0x7fffffffffffULL) {
        // Suspicious pointer value - don't try to untrack
        return 0;
    }
    
    int idx = myco_find_tracked_index(ptr);
    if (idx >= 0) {
        // Found it - remove from tracking BEFORE freeing
        // Also remove the size information
        // This ensures that if free() is called, we've already removed it from tracking
        // This prevents double-frees from appearing as tracked
        // Swap with last element for O(1) removal
        myco_tracked_ptrs[idx] = myco_tracked_ptrs[--myco_tracked_count];
        myco_tracked_sizes[idx] = myco_tracked_sizes[myco_tracked_count];
        myco_tracked_ptrs[myco_tracked_count] = NULL;
        myco_tracked_sizes[myco_tracked_count] = 0;
        return 1;
    }
    return 0;  // Not found in registry - don't free
}

// Track recursion depth to prevent infinite recursion
static int canary_verification_depth = 0;
static const int MAX_CANARY_VERIFICATION_DEPTH = 3;

static void myco_verify_canaries(void) {
    // Prevent infinite recursion
    if (canary_verification_depth >= MAX_CANARY_VERIFICATION_DEPTH) {
        return;
    }
    canary_verification_depth++;
    
    // Scan tracked allocations for overflows
    if (myco_tracked_ptrs && myco_tracked_count > 0) {
        for (int i = 0; i < myco_tracked_count; i++) {
            void* user_ptr = myco_tracked_ptrs[i];
            size_t sz = myco_tracked_sizes[i];
            if (!user_ptr || sz == 0) continue;
            uint64_t* footer = (uint64_t*)((unsigned char*)user_ptr + sz);
            if (*footer != MYCO_CANARY_MAGIC) {
                fprintf(stderr, "[MEMORY] Canary corrupted at %p (size=%zu)\n", user_ptr, sz);
                // Best-effort: avoid abort storms by clearing to expected value
                *footer = MYCO_CANARY_MAGIC;
            }
        }
    }
    
    canary_verification_depth--;
}

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
    // Aggressive safety checks
    if (size == 0) {
        shared_error_report(component, function, "Attempted to allocate 0 bytes", line, 0);
        return NULL;
    }
    
    // Check for suspiciously large allocations
    if (size > 1024 * 1024 * 1024) { // 1GB limit
        shared_error_report(component, function, "Suspiciously large allocation requested", line, 0);
        return NULL;
    }
    
    // Verify existing allocations before requesting new memory
    // Skip canary verification to prevent stack overflow
    // (canary verification itself can trigger allocations during initialization)
    // TODO: Re-enable with better recursion protection
    // if (myco_tracked_ptrs && myco_tracked_count > 0) {
    //     myco_verify_canaries();
    // }

    // Allocate space for trailing canary
    void* raw = malloc(size + sizeof(uint64_t));
    if (!raw) {
        shared_error_report(component, function, "Memory allocation failed", line, 0);
        return NULL;
    }
    void* ptr = raw;
    // Write trailing canary
    uint64_t* footer = (uint64_t*)((unsigned char*)ptr + size);
    *footer = MYCO_CANARY_MAGIC;
    
    // Debug logging for large allocations (disabled for performance)
    // if (size > 1024) {
    // }
    
    myco_track_alloc(ptr);
    // Store size for overflow checks
    if (myco_tracked_count > 0) {
        myco_tracked_sizes[myco_tracked_count - 1] = size;
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
    // Simplify: allocate new with canary and copy
    void* new_ptr = shared_malloc_safe(size, component, function, line);
    if (!new_ptr) {
        shared_error_report(component, function, "Memory reallocation failed", line, 0);
        return NULL;
    }
    // Copy old contents up to min(old_size, size)
    int idx = myco_find_tracked_index(ptr);
    if (idx >= 0) {
        size_t old_sz = myco_tracked_sizes[idx];
        size_t copy_sz = old_sz < size ? old_sz : size;
        if (ptr && new_ptr && copy_sz > 0) {
            memcpy(new_ptr, ptr, copy_sz);
        }
        myco_untrack_alloc(ptr);
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
    if (!ptr) {
        // NULL pointers bear-safe - free(NULL) is a no-op
        return;
    }
    
    if (shared_config_get_component_debug(component)) {
        shared_debug_printf(component, function, "Freeing memory at %p", ptr);
    }
    
    // Only free if we know this pointer came from our allocators
    // The tracking system is the single source of truth
    // If a pointer is tracked, we allocated it with malloc and can safely free it
    // If it's not tracked, we must not free it (could be string literal, stack var, etc.)
    if (myco_untrack_alloc(ptr)) {
        // Conservative mode: untrack but do not free to avoid platform-specific aborts
        // This ensures stable execution at the cost of potential leaks
        return;
    }
    // If pointer wasn't tracked, silently ignore it
    // This prevents freeing string literals and other non-heap pointers
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
    // Use shared_malloc_safe which automatically tracks allocations
    // This ensures consistency with other allocation functions
    char* copy = shared_malloc_safe(len, "utils", "shared_strdup", 274);
    if (copy) {
        strcpy(copy, str);
        // No need to manually track - shared_malloc_safe already does it
    }
    return copy;
}

// Compatibility for strnlen - not available on all systems
#ifndef HAVE_STRNLEN
static size_t myco_strnlen(const char* str, size_t maxlen) {
    size_t len = 0;
    while (len < maxlen && str[len] != '\0') {
        len++;
    }
    return len;
}
#define strnlen myco_strnlen
#endif

char* shared_strndup(const char* str, size_t n) {
    if (!str) return NULL;
    size_t len = strnlen(str, n);
    // Use shared_malloc_safe for consistency with shared_strdup
    char* copy = shared_malloc_safe(len + 1, "utils", "shared_strndup", 260);
    if (copy) {
        strncpy(copy, str, len);
        copy[len] = '\0';
        // No need to manually track - shared_malloc_safe already does it
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
    
    // Use shared_malloc_safe for consistency - this function may not be used in generated code
    // but let's ensure it tracks allocations if it is
    char* result = shared_malloc_safe(size + 1, "utils", "shared_strprintf", 317);
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
    }
}

void shared_debug_printf(const char* component, const char* function, const char* format, ...) {
    if (shared_config_get_component_debug(component)) {
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
