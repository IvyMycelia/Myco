/**
 * @file arena_allocator.c
 * @brief Arena allocator implementation for zero-copy optimization
 */

#include "../../include/core/optimization/arena_allocator.h"
#include "../../include/core/interpreter/value_operations.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// ============================================================================
// ARENA ALLOCATOR IMPLEMENTATION
// ============================================================================

ArenaAllocator* arena_allocator_create(size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    ArenaAllocator* arena = malloc(sizeof(ArenaAllocator));
    if (!arena) {
        return NULL;
    }
    
    arena->memory = malloc(size);
    if (!arena->memory) {
        free(arena);
        return NULL;
    }
    
    arena->size = size;
    arena->offset = 0;
    arena->peak_usage = 0;
    arena->is_allocated = 1;
    
    return arena;
}

void arena_allocator_free(ArenaAllocator* arena) {
    if (!arena) {
        return;
    }
    
    if (arena->memory) {
        free(arena->memory);
    }
    
    free(arena);
}

static size_t align_size(size_t size, size_t alignment) {
    return (size + alignment - 1) & ~(alignment - 1);
}

void* arena_allocator_alloc(ArenaAllocator* arena, size_t size, size_t alignment) {
    if (!arena || !arena->memory || size == 0) {
        return NULL;
    }
    
    // Align the size
    size_t aligned_size = align_size(size, alignment);
    
    // Check if we have enough space
    if (arena->offset + aligned_size > arena->size) {
        return NULL; // Arena full
    }
    
    // Allocate from current offset
    void* ptr = arena->memory + arena->offset;
    arena->offset += aligned_size;
    
    // Update peak usage
    if (arena->offset > arena->peak_usage) {
        arena->peak_usage = arena->offset;
    }
    
    return ptr;
}

void arena_allocator_reset(ArenaAllocator* arena) {
    if (!arena) {
        return;
    }
    
    arena->offset = 0;
}

void arena_allocator_get_stats(ArenaAllocator* arena,
                               size_t* used_bytes,
                               size_t* peak_bytes,
                               size_t* total_bytes) {
    if (!arena || !used_bytes || !peak_bytes || !total_bytes) {
        return;
    }
    
    *used_bytes = arena->offset;
    *peak_bytes = arena->peak_usage;
    *total_bytes = arena->size;
}

// ============================================================================
// FUNCTION ARENA IMPLEMENTATION
// ============================================================================

FunctionArena* function_arena_create(size_t function_id, size_t arena_size) {
    if (arena_size == 0) {
        return NULL;
    }
    
    FunctionArena* func_arena = malloc(sizeof(FunctionArena));
    if (!func_arena) {
        return NULL;
    }
    
    func_arena->arena = arena_allocator_create(arena_size);
    if (!func_arena->arena) {
        free(func_arena);
        return NULL;
    }
    
    func_arena->function_id = function_id;
    func_arena->allocation_count = 0;
    func_arena->total_allocated = 0;
    
    return func_arena;
}

void function_arena_free(FunctionArena* func_arena) {
    if (!func_arena) {
        return;
    }
    
    if (func_arena->arena) {
        arena_allocator_free(func_arena->arena);
    }
    
    free(func_arena);
}

Value* function_arena_alloc_value(FunctionArena* func_arena, ValueType value_type) {
    if (!func_arena || !func_arena->arena) {
        return NULL;
    }
    
    // Allocate value structure
    Value* value = arena_allocator_alloc(func_arena->arena, sizeof(Value), 8);
    if (!value) {
        return NULL;
    }
    
    // Initialize value
    value->type = value_type;
    value->ref_count = 1;
    
    // Initialize based on type
    switch (value_type) {
        case VALUE_NUMBER:
            value->data.number_value = 0.0;
            break;
        case VALUE_STRING:
            value->data.string_value = NULL;
            break;
        case VALUE_BOOLEAN:
            value->data.boolean_value = 0;
            break;
        case VALUE_NULL:
            break;
        case VALUE_ARRAY:
            value->data.array_value.elements = NULL;
            value->data.array_value.count = 0;
            value->data.array_value.capacity = 0;
            break;
        case VALUE_OBJECT:
            value->data.object_value.values = NULL;
            value->data.object_value.count = 0;
            value->data.object_value.capacity = 0;
            break;
        case VALUE_FUNCTION:
            value->data.function_value.body = NULL;
            value->data.function_value.parameters = NULL;
            value->data.function_value.parameter_count = 0;
            value->data.function_value.return_type = NULL;
            value->data.function_value.captured_environment = NULL;
            break;
        default:
            break;
    }
    
    func_arena->allocation_count++;
    func_arena->total_allocated += sizeof(Value);
    
    return value;
}

Value* function_arena_alloc_array(FunctionArena* func_arena, size_t element_count) {
    if (!func_arena || !func_arena->arena) {
        return NULL;
    }
    
    // Allocate value
    Value* value = function_arena_alloc_value(func_arena, VALUE_ARRAY);
    if (!value) {
        return NULL;
    }
    
    // Allocate array elements
    if (element_count > 0) {
        Value** elements = arena_allocator_alloc(func_arena->arena, 
                                                 element_count * sizeof(Value*), 
                                                 8);
        if (!elements) {
            return NULL;
        }
        
        // Initialize elements
        for (size_t i = 0; i < element_count; i++) {
            elements[i] = NULL;
        }
        
        value->data.array_value.elements = (void**)elements;
        value->data.array_value.count = element_count;
        value->data.array_value.capacity = element_count;
        
        func_arena->total_allocated += element_count * sizeof(Value*);
    }
    
    return value;
}

Value* function_arena_alloc_string(FunctionArena* func_arena, size_t string_length) {
    if (!func_arena || !func_arena->arena) {
        return NULL;
    }
    
    // Allocate value
    Value* value = function_arena_alloc_value(func_arena, VALUE_STRING);
    if (!value) {
        return NULL;
    }
    
    // Allocate string data
    if (string_length > 0) {
        char* string_data = arena_allocator_alloc(func_arena->arena, 
                                                  string_length + 1, 
                                                  1);
        if (!string_data) {
            return NULL;
        }
        
        string_data[string_length] = '\0';
        value->data.string_value = string_data;
        
        func_arena->total_allocated += string_length + 1;
    }
    
    return value;
}

// ============================================================================
// ARENA MANAGER IMPLEMENTATION
// ============================================================================

ArenaManager* arena_manager_create(size_t default_size, size_t max_size) {
    if (default_size == 0 || max_size == 0 || default_size > max_size) {
        return NULL;
    }
    
    ArenaManager* manager = malloc(sizeof(ArenaManager));
    if (!manager) {
        return NULL;
    }
    
    manager->arenas = NULL;
    manager->arena_count = 0;
    manager->arena_capacity = 0;
    manager->default_arena_size = default_size;
    manager->max_arena_size = max_size;
    
    return manager;
}

void arena_manager_free(ArenaManager* manager) {
    if (!manager) {
        return;
    }
    
    if (manager->arenas) {
        for (size_t i = 0; i < manager->arena_count; i++) {
            arena_allocator_free(manager->arenas[i]);
        }
        free(manager->arenas);
    }
    
    free(manager);
}

FunctionArena* arena_manager_get_function_arena(ArenaManager* manager, size_t function_id) {
    if (!manager) {
        return NULL;
    }
    
    // For now, create a new arena for each function
    // In a more sophisticated implementation, we would reuse arenas
    return function_arena_create(function_id, manager->default_arena_size);
}

void arena_manager_reset_all(ArenaManager* manager) {
    if (!manager || !manager->arenas) {
        return;
    }
    
    for (size_t i = 0; i < manager->arena_count; i++) {
        arena_allocator_reset(manager->arenas[i]);
    }
}

void arena_manager_get_stats(ArenaManager* manager,
                             size_t* total_arenas,
                             size_t* total_memory,
                             size_t* peak_memory) {
    if (!manager || !total_arenas || !total_memory || !peak_memory) {
        return;
    }
    
    *total_arenas = manager->arena_count;
    *total_memory = 0;
    *peak_memory = 0;
    
    if (manager->arenas) {
        for (size_t i = 0; i < manager->arena_count; i++) {
            size_t used, peak, total;
            arena_allocator_get_stats(manager->arenas[i], &used, &peak, &total);
            *total_memory += total;
            if (peak > *peak_memory) {
                *peak_memory = peak;
            }
        }
    }
}
