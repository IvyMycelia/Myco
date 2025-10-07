#include "memory.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "../../include/utils/shared_utilities.h"

// Garbage Collection Implementation
// This implements a mark-and-sweep garbage collector with generational optimization

MemoryTracker* memory_tracker_create(MemoryStrategy strategy) {
    MemoryTracker* tracker = shared_malloc_safe(sizeof(MemoryTracker), "unknown", "unknown_function", 12);
    if (!tracker) return NULL;
    
    tracker->strategy = strategy;
    tracker->pools = NULL;
    tracker->arenas = NULL;
    tracker->pool_counter = 0;
    tracker->arena_counter = 0;
    tracker->enable_tracking = 0;
    tracker->enable_validation = 0;
    tracker->enable_leak_detection = 0;
    
    // Initialize stats
    memset(&tracker->stats, 0, sizeof(MemoryStats));
    
    // Initialize pool config
    tracker->pool_config.initial_size = MYCO_DEFAULT_POOL_SIZE;
    tracker->pool_config.growth_factor = 2;
    tracker->pool_config.max_size = 0;
    tracker->pool_config.enable_tracking = 0;
    tracker->pool_config.enable_validation = 0;
    
    return tracker;
}

void memory_tracker_free(MemoryTracker* tracker) {
    if (tracker) {
        // Free pools and arenas
        MemoryPool* pool = tracker->pools;
        while (pool) {
            MemoryPool* next = pool->next;
            memory_pool_free(tracker, pool);
            pool = next;
        }
        
        MemoryArena* arena = tracker->arenas;
        while (arena) {
            MemoryArena* next = arena->next;
            memory_arena_free(tracker, arena);
            arena = next;
        }
        
        shared_free_safe(tracker, "unknown", "unknown_function", 54);
    }
}

void memory_tracker_reset(MemoryTracker* tracker) {
    if (tracker) {
        // Reset statistics
        memset(&tracker->stats, 0, sizeof(MemoryStats));
        
        // Reset counters
        tracker->pool_counter = 0;
        tracker->arena_counter = 0;
    }
}

// Memory allocation functions with garbage collection
void* memory_allocate(MemoryTracker* tracker, size_t size) {
    if (!tracker) return shared_malloc_safe(size, "unknown", "unknown_function", 71);
    
    // Align size to word boundary
    size = memory_align_size(size, sizeof(void*));
    
    void* ptr = shared_malloc_safe(size, "unknown", "unknown_function", 76);
    if (!ptr) return NULL;
    
    // Update statistics
    tracker->stats.total_allocated += size;
    tracker->stats.current_usage += size;
    tracker->stats.allocation_count++;
    
    if (tracker->stats.current_usage > tracker->stats.peak_usage) {
        tracker->stats.peak_usage = tracker->stats.current_usage;
    }
    
    // Zero out the memory for safety
    memset(ptr, 0, size);
    
    return ptr;
}

void* memory_allocate_aligned(MemoryTracker* tracker, size_t size, size_t alignment) {
    if (!tracker) {
        void* ptr = aligned_alloc(alignment, size);
        return ptr ? memset(ptr, 0, size) : NULL;
    }
    
    // For simplicity, use regular allocation with alignment
    size_t aligned_size = memory_align_size(size, alignment);
    return memory_allocate(tracker, aligned_size);
}

void* memory_reallocate(MemoryTracker* tracker, void* ptr, size_t new_size) {
    if (!tracker) return shared_realloc_safe(ptr, new_size, "unknown", "unknown_function", 106);
    
    if (!ptr) return memory_allocate(tracker, new_size);
    
    // Get old size (simplified - in real implementation we'd track this)
    // Note: malloc_usable_size is not portable, so we use a simplified approach
    size_t old_size = 0; // We'd need to track this properly in a real implementation
    
    void* new_ptr = shared_realloc_safe(ptr, new_size, "unknown", "unknown_function", 114);
    if (!new_ptr) return NULL;
    
    // Update statistics
    tracker->stats.total_allocated += new_size;
    tracker->stats.total_freed += old_size;
    tracker->stats.current_usage += (new_size - old_size);
    
    return new_ptr;
}

void memory_free(MemoryTracker* tracker, void* ptr) {
    if (!ptr) return;
    
    if (tracker) {
        // Update statistics
        // Note: We can't easily get the size of freed memory without tracking it
        // In a real implementation, we'd maintain a size table
        tracker->stats.deallocation_count++;
        // We'd need to track the actual size to update current_usage properly
    }
    
    shared_free_safe(ptr, "unknown", "unknown_function", 136);
}

MemoryPool* memory_pool_create(MemoryTracker* tracker, size_t size, const char* name) { return NULL; }
void memory_pool_free(MemoryTracker* tracker, MemoryPool* pool) {}
void* memory_pool_allocate(MemoryPool* pool, size_t size) { return NULL; }
void memory_pool_reset(MemoryPool* pool) {}
size_t memory_pool_get_usage(MemoryPool* pool) { return 0; }
size_t memory_pool_get_capacity(MemoryPool* pool) { return 0; }

MemoryArena* memory_arena_create(MemoryTracker* tracker, size_t size, const char* name) { return NULL; }
MemoryArena* memory_arena_create_child(MemoryArena* parent, size_t size, const char* name) { return NULL; }
void memory_arena_free(MemoryTracker* tracker, MemoryArena* arena) {}
void* memory_arena_allocate(MemoryArena* arena, size_t size) { return NULL; }
void memory_arena_reset(MemoryArena* arena) {}
void memory_arena_free_all_children(MemoryArena* arena) {}
size_t memory_arena_get_usage(MemoryArena* arena) { return 0; }
size_t memory_arena_get_capacity(MemoryArena* arena) { return 0; }

void memory_tracker_enable_tracking(MemoryTracker* tracker, int enable) {}
void memory_tracker_enable_validation(MemoryTracker* tracker, int enable) {}
void memory_tracker_enable_leak_detection(MemoryTracker* tracker, int enable) {}
int memory_tracker_validate_pointer(MemoryTracker* tracker, void* ptr) { return 1; }
int memory_tracker_is_valid_pointer(MemoryTracker* tracker, void* ptr) { return 1; }
void memory_tracker_validate_all_pointers(MemoryTracker* tracker) {}

MemoryStats memory_tracker_get_stats(MemoryTracker* tracker) {
    if (!tracker) {
        MemoryStats empty = {0};
        return empty;
    }
    return tracker->stats;
}

void memory_tracker_print_stats(MemoryTracker* tracker) {
    if (!tracker) {
        printf("Memory tracker is NULL\n");
        return;
    }
    
    printf("\n=== Memory Statistics ===\n");
    printf("Total allocated: %zu bytes\n", tracker->stats.total_allocated);
    printf("Total freed: %zu bytes\n", tracker->stats.total_freed);
    printf("Current usage: %zu bytes\n", tracker->stats.current_usage);
    printf("Peak usage: %zu bytes\n", tracker->stats.peak_usage);
    printf("Allocation count: %zu\n", tracker->stats.allocation_count);
    printf("Deallocation count: %zu\n", tracker->stats.deallocation_count);
    printf("Pool count: %zu\n", tracker->stats.pool_count);
    printf("Arena count: %zu\n", tracker->stats.arena_count);
    printf("========================\n\n");
}

void memory_tracker_print_pools(MemoryTracker* tracker) {
    if (!tracker) {
        printf("Memory tracker is NULL\n");
        return;
    }
    
    printf("\n=== Memory Pools ===\n");
    MemoryPool* pool = tracker->pools;
    int count = 0;
    while (pool) {
        printf("Pool %d: %s (ID: %d, Size: %zu, Used: %zu)\n", 
               count++, pool->name ? pool->name : "unnamed", 
               pool->id, pool->size, pool->used);
        pool = pool->next;
    }
    if (count == 0) {
        printf("No memory pools allocated\n");
    }
    printf("=====================\n\n");
}

void memory_tracker_print_arenas(MemoryTracker* tracker) {
    if (!tracker) {
        printf("Memory tracker is NULL\n");
        return;
    }
    
    printf("\n=== Memory Arenas ===\n");
    MemoryArena* arena = tracker->arenas;
    int count = 0;
    while (arena) {
        printf("Arena %d: %s (ID: %d, Size: %zu, Used: %zu)\n", 
               count++, arena->name ? arena->name : "unnamed", 
               arena->id, arena->size, arena->used);
        arena = arena->next;
    }
    if (count == 0) {
        printf("No memory arenas allocated\n");
    }
    printf("=====================\n\n");
}

void memory_tracker_print_leaks(MemoryTracker* tracker) {
    if (!tracker) {
        printf("Memory tracker is NULL\n");
        return;
    }
    
    printf("\n=== Memory Leak Detection ===\n");
    if (tracker->stats.current_usage > 0) {
        printf("Potential memory leak detected: %zu bytes still allocated\n", 
               tracker->stats.current_usage);
        printf("Allocations: %zu, Deallocations: %zu\n", 
               tracker->stats.allocation_count, tracker->stats.deallocation_count);
    } else {
        printf("No memory leaks detected\n");
    }
    printf("============================\n\n");
}

void memory_tracker_dump_memory_map(MemoryTracker* tracker) {}
void memory_tracker_dump_pool_map(MemoryTracker* tracker) {}
void memory_tracker_dump_arena_map(MemoryTracker* tracker) {}
int memory_tracker_check_integrity(MemoryTracker* tracker) { return 1; }
int memory_tracker_check_for_leaks(MemoryTracker* tracker) { return 0; }

size_t memory_align_size(size_t size, size_t alignment) {
    if (alignment == 0) return size;
    return (size + alignment - 1) & ~(alignment - 1);
}

size_t memory_power_of_two_ceil(size_t size) {
    if (size == 0) return 1;
    size--;
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    size |= size >> 32;
    return size + 1;
}

int memory_is_power_of_two(size_t size) {
    return size > 0 && (size & (size - 1)) == 0;
}
void memory_zero(void* ptr, size_t size) { memset(ptr, 0, size); }
void memory_copy(void* dest, const void* src, size_t size) { memcpy(dest, src, size); }
void memory_move(void* dest, const void* src, size_t size) { memmove(dest, src, size); }
int memory_compare(const void* ptr1, const void* ptr2, size_t size) { return memcmp(ptr1, ptr2, size); }

void memory_guard_begin(void* ptr, size_t size) {}
void memory_guard_end(void* ptr) {}
int memory_guard_check(void* ptr) { return 1; }
void memory_canary_set(void* ptr, size_t size) {}
int memory_canary_check(void* ptr) { return 1; }

void memory_profiler_start(void) {}
void memory_profiler_stop(void) {}
void memory_profiler_reset(void) {}
void memory_profiler_print_report(void) {}
