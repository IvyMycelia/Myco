/**
 * @file arena_allocator.h
 * @brief Arena allocator for zero-copy optimization
 * 
 * Fast bump-pointer allocation for temporary values with batch deallocation
 * on function return to eliminate allocation overhead in hot paths.
 */

#ifndef MYCO_ARENA_ALLOCATOR_H
#define MYCO_ARENA_ALLOCATOR_H

#include "../interpreter/value_operations.h"
#include <stddef.h>

/**
 * @brief Arena allocator for fast allocation
 */
typedef struct {
    uint8_t* memory;              // Arena memory block
    size_t size;                  // Total arena size
    size_t offset;                // Current allocation offset
    size_t peak_usage;            // Peak memory usage
    int is_allocated;             // Whether arena is allocated
} ArenaAllocator;

/**
 * @brief Per-function arena context
 */
typedef struct {
    ArenaAllocator* arena;        // Arena allocator
    size_t function_id;           // Function identifier
    size_t allocation_count;      // Number of allocations
    size_t total_allocated;       // Total bytes allocated
} FunctionArena;

/**
 * @brief Global arena manager
 */
typedef struct {
    ArenaAllocator** arenas;      // Array of arena pointers
    size_t arena_count;           // Number of arenas
    size_t arena_capacity;        // Arena array capacity
    size_t default_arena_size;    // Default arena size
    size_t max_arena_size;        // Maximum arena size
} ArenaManager;

/**
 * @brief Create arena allocator
 * 
 * @param size Arena size in bytes
 * @return ArenaAllocator* New arena or NULL on failure
 */
ArenaAllocator* arena_allocator_create(size_t size);

/**
 * @brief Free arena allocator
 * 
 * @param arena Arena to free
 */
void arena_allocator_free(ArenaAllocator* arena);

/**
 * @brief Allocate memory from arena
 * 
 * @param arena Arena allocator
 * @param size Size to allocate
 * @param alignment Memory alignment
 * @return void* Allocated memory or NULL on failure
 */
void* arena_allocator_alloc(ArenaAllocator* arena, size_t size, size_t alignment);

/**
 * @brief Reset arena to beginning
 * 
 * @param arena Arena to reset
 */
void arena_allocator_reset(ArenaAllocator* arena);

/**
 * @brief Get arena usage statistics
 * 
 * @param arena Arena allocator
 * @param used_bytes Bytes currently used
 * @param peak_bytes Peak bytes used
 * @param total_bytes Total arena size
 */
void arena_allocator_get_stats(ArenaAllocator* arena,
                               size_t* used_bytes,
                               size_t* peak_bytes,
                               size_t* total_bytes);

/**
 * @brief Create function arena
 * 
 * @param function_id Function identifier
 * @param arena_size Arena size for function
 * @return FunctionArena* New function arena or NULL on failure
 */
FunctionArena* function_arena_create(size_t function_id, size_t arena_size);

/**
 * @brief Free function arena
 * 
 * @param func_arena Function arena to free
 */
void function_arena_free(FunctionArena* func_arena);

/**
 * @brief Allocate value in function arena
 * 
 * @param func_arena Function arena
 * @param value_type Type of value to allocate
 * @return Value* Allocated value or NULL on failure
 */
Value* function_arena_alloc_value(FunctionArena* func_arena, ValueType value_type);

/**
 * @brief Allocate array in function arena
 * 
 * @param func_arena Function arena
 * @param element_count Number of elements
 * @return Value* Allocated array value or NULL on failure
 */
Value* function_arena_alloc_array(FunctionArena* func_arena, size_t element_count);

/**
 * @brief Allocate string in function arena
 * 
 * @param func_arena Function arena
 * @param string_length String length
 * @return Value* Allocated string value or NULL on failure
 */
Value* function_arena_alloc_string(FunctionArena* func_arena, size_t string_length);

/**
 * @brief Create arena manager
 * 
 * @param default_size Default arena size
 * @param max_size Maximum arena size
 * @return ArenaManager* New manager or NULL on failure
 */
ArenaManager* arena_manager_create(size_t default_size, size_t max_size);

/**
 * @brief Free arena manager
 * 
 * @param manager Manager to free
 */
void arena_manager_free(ArenaManager* manager);

/**
 * @brief Get arena for function
 * 
 * @param manager Arena manager
 * @param function_id Function identifier
 * @return FunctionArena* Function arena or NULL on failure
 */
FunctionArena* arena_manager_get_function_arena(ArenaManager* manager, size_t function_id);

/**
 * @brief Reset all arenas
 * 
 * @param manager Arena manager
 */
void arena_manager_reset_all(ArenaManager* manager);

/**
 * @brief Get manager statistics
 * 
 * @param manager Arena manager
 * @param total_arenas Total number of arenas
 * @param total_memory Total memory allocated
 * @param peak_memory Peak memory usage
 */
void arena_manager_get_stats(ArenaManager* manager,
                             size_t* total_arenas,
                             size_t* total_memory,
                             size_t* peak_memory);

#endif // MYCO_ARENA_ALLOCATOR_H
