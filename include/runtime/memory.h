#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdint.h>

// Memory configuration constants
#define MYCO_DEFAULT_POOL_SIZE (1024 * 1024)  // 1MB
#define MYCO_DEFAULT_ARENA_SIZE (1024 * 1024) // 1MB
#define MYCO_MAX_POOL_COUNT 100
#define MYCO_MAX_ARENA_COUNT 100

// Memory allocation strategies
typedef enum {
    MEMORY_STRATEGY_SYSTEM,      // Use system malloc/free
    MEMORY_STRATEGY_POOL,        // Use memory pools
    MEMORY_STRATEGY_ARENA,       // Use arena allocation
    MEMORY_STRATEGY_GENERATIONAL // Use generational GC
} MemoryStrategy;

// Memory pool configuration
typedef struct {
    size_t initial_size;
    size_t growth_factor;
    size_t max_size;
    int enable_tracking;
    int enable_validation;
} MemoryPoolConfig;

// Memory statistics
typedef struct {
    size_t total_allocated;
    size_t total_freed;
    size_t peak_usage;
    size_t current_usage;
    size_t allocation_count;
    size_t deallocation_count;
    size_t pool_count;
    size_t arena_count;
} MemoryStats;

// Memory pool
typedef struct MemoryPool {
    char* start;
    char* current;
    char* end;
    size_t size;
    size_t used;
    struct MemoryPool* next;
    char* name;
    int id;
} MemoryPool;

// Memory arena
typedef struct MemoryArena {
    char* start;
    char* current;
    char* end;
    size_t size;
    size_t used;
    struct MemoryArena* parent;
    struct MemoryArena* children;
    struct MemoryArena* next;
    char* name;
    int id;
} MemoryArena;

// Memory tracker
typedef struct {
    MemoryStrategy strategy;
    MemoryPoolConfig pool_config;
    MemoryStats stats;
    MemoryPool* pools;
    MemoryArena* arenas;
    int pool_counter;
    int arena_counter;
    int enable_tracking;
    int enable_validation;
    int enable_leak_detection;
} MemoryTracker;

// Memory management initialization and cleanup
MemoryTracker* memory_tracker_create(MemoryStrategy strategy);
void memory_tracker_free(MemoryTracker* tracker);
void memory_tracker_reset(MemoryTracker* tracker);

// Memory allocation functions
void* memory_allocate(MemoryTracker* tracker, size_t size);
void* memory_allocate_aligned(MemoryTracker* tracker, size_t size, size_t alignment);
void* memory_reallocate(MemoryTracker* tracker, void* ptr, size_t new_size);
void memory_free(MemoryTracker* tracker, void* ptr);

// Memory pool management
MemoryPool* memory_pool_create(MemoryTracker* tracker, size_t size, const char* name);
void memory_pool_free(MemoryTracker* tracker, MemoryPool* pool);
void* memory_pool_allocate(MemoryPool* pool, size_t size);
void memory_pool_reset(MemoryPool* pool);
size_t memory_pool_get_usage(MemoryPool* pool);
size_t memory_pool_get_capacity(MemoryPool* pool);

// Memory arena management
MemoryArena* memory_arena_create(MemoryTracker* tracker, size_t size, const char* name);
MemoryArena* memory_arena_create_child(MemoryArena* parent, size_t size, const char* name);
void memory_arena_free(MemoryTracker* tracker, MemoryArena* arena);
void* memory_arena_allocate(MemoryArena* arena, size_t size);
void memory_arena_reset(MemoryArena* arena);
void memory_arena_free_all_children(MemoryArena* arena);
size_t memory_arena_get_usage(MemoryArena* arena);
size_t memory_arena_get_capacity(MemoryArena* arena);

// Memory tracking and validation
void memory_tracker_enable_tracking(MemoryTracker* tracker, int enable);
void memory_tracker_enable_validation(MemoryTracker* tracker, int enable);
void memory_tracker_enable_leak_detection(MemoryTracker* tracker, int enable);
int memory_tracker_validate_pointer(MemoryTracker* tracker, void* ptr);
int memory_tracker_is_valid_pointer(MemoryTracker* tracker, void* ptr);
void memory_tracker_validate_all_pointers(MemoryTracker* tracker);

// Memory statistics and reporting
MemoryStats memory_tracker_get_stats(MemoryTracker* tracker);
void memory_tracker_print_stats(MemoryTracker* tracker);
void memory_tracker_print_pools(MemoryTracker* tracker);
void memory_tracker_print_arenas(MemoryTracker* tracker);
void memory_tracker_print_leaks(MemoryTracker* tracker);

// Memory debugging and diagnostics
void memory_tracker_dump_memory_map(MemoryTracker* tracker);
void memory_tracker_dump_pool_map(MemoryTracker* tracker);
void memory_tracker_dump_arena_map(MemoryTracker* tracker);
int memory_tracker_check_integrity(MemoryTracker* tracker);
int memory_tracker_check_for_leaks(MemoryTracker* tracker);

// Memory utilities
size_t memory_align_size(size_t size, size_t alignment);
size_t memory_power_of_two_ceil(size_t size);
int memory_is_power_of_two(size_t size);
void memory_zero(void* ptr, size_t size);
void memory_copy(void* dest, const void* src, size_t size);
void memory_move(void* dest, const void* src, size_t size);
int memory_compare(const void* ptr1, const void* ptr2, size_t size);

// Memory safety functions
void memory_guard_begin(void* ptr, size_t size);
void memory_guard_end(void* ptr);
int memory_guard_check(void* ptr);
void memory_canary_set(void* ptr, size_t size);
int memory_canary_check(void* ptr);

// Memory profiling
void memory_profiler_start(void);
void memory_profiler_stop(void);
void memory_profiler_reset(void);
void memory_profiler_print_report(void);

// Convenience macros
#define MEMORY_ALLOC(tracker, type) (type*)memory_allocate(tracker, sizeof(type))
#define MEMORY_ALLOC_ARRAY(tracker, type, count) (type*)memory_allocate(tracker, sizeof(type) * (count))
#define MEMORY_FREE(tracker, ptr) do { if (ptr) { memory_free(tracker, ptr); ptr = NULL; } } while(0)

// Memory pool convenience macros
#define MEMORY_POOL_ALLOC(pool, type) (type*)memory_pool_allocate(pool, sizeof(type))
#define MEMORY_POOL_ALLOC_ARRAY(pool, type, count) (type*)memory_pool_allocate(pool, sizeof(type) * (count))

// Memory arena convenience macros
#define MEMORY_ARENA_ALLOC(arena, type) (type*)memory_arena_allocate(arena, sizeof(type))
#define MEMORY_ARENA_ALLOC_ARRAY(arena, type, count) (type*)memory_arena_allocate(arena, sizeof(type) * (count))

#endif // MEMORY_H
