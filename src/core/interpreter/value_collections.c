#include "interpreter/value_operations.h"
#include "../../include/core/interpreter.h"
#include "../../include/utils/shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ============================================================================
// ARRAY OPERATIONS
// ============================================================================

Value value_create_array(size_t initial_capacity) { 
    Value v; 
    v.type = VALUE_ARRAY; 
    v.data.array_value.elements = NULL; 
    v.data.array_value.count = 0; 
    v.data.array_value.capacity = initial_capacity; 
    
    // Allocate memory if capacity is specified
    if (initial_capacity > 0) {
        v.data.array_value.elements = (void**)shared_malloc_safe(initial_capacity * sizeof(void*), "interpreter", "value_create_array", 0);
        if (!v.data.array_value.elements) {
            v.data.array_value.capacity = 0;
        } else {
            // Initialize to NULL
            memset(v.data.array_value.elements, 0, initial_capacity * sizeof(void*));
        }
    }
    
    return v; 
}

void value_array_push(Value* array, Value element) {
    if (!array || array->type != VALUE_ARRAY) return;
    
    // Ensure array has valid elements pointer
    if (!array->data.array_value.elements || array->data.array_value.capacity == 0) {
        size_t new_capacity = 4;
        array->data.array_value.elements = (void**)shared_malloc_safe(new_capacity * sizeof(void*), "interpreter", "unknown_function", 2849);
        if (!array->data.array_value.elements) {
            return;
        }
        memset(array->data.array_value.elements, 0, new_capacity * sizeof(void*));
        array->data.array_value.capacity = new_capacity;
    }
    
    // Check if we need to resize
    if (array->data.array_value.count >= array->data.array_value.capacity) {
        size_t new_capacity = array->data.array_value.capacity * 2;
        void** new_elements = shared_realloc_safe(array->data.array_value.elements, new_capacity * sizeof(void*), "interpreter", "unknown_function", 2850);
        if (!new_elements) {
            return;
        }
        
        array->data.array_value.elements = new_elements;
        array->data.array_value.capacity = new_capacity;
        
        for (size_t i = array->data.array_value.count; i < new_capacity; i++) {
            array->data.array_value.elements[i] = NULL;
        }
    }
    
    // Add the element
    Value* stored_element = shared_malloc_safe(sizeof(Value), "interpreter", "unknown_function", 2860);
    if (stored_element) {
        Value cloned_element = value_clone(&element);
        *stored_element = cloned_element;
        array->data.array_value.elements[array->data.array_value.count] = stored_element;
        array->data.array_value.count++;
    }
}

Value value_array_pop(Value* array, int index) { 
    if (!array || array->type != VALUE_ARRAY || array->data.array_value.count == 0) {
        return value_create_null();
    }
    
    size_t array_len = array->data.array_value.count;
    size_t pop_index;
    
    // If index is -1 or not provided, pop from the end (default behavior)
    if (index < 0) {
        pop_index = array_len - 1;
    } else {
        pop_index = (size_t)index;
        // Validate index
        if (pop_index >= array_len) {
            return value_create_null();
        }
    }
    
    // Get the element to pop
    Value* element = (Value*)array->data.array_value.elements[pop_index];
    if (!element) {
        return value_create_null();
    }
    
    Value result = value_clone(element);
    
    // Remove the element by shifting all elements after it to the left
    for (size_t i = pop_index; i < array_len - 1; i++) {
        array->data.array_value.elements[i] = array->data.array_value.elements[i + 1];
    }
    
    // Free the last element (which was moved)
    shared_free_safe(element, "interpreter", "value_array_pop", 2872);
    array->data.array_value.count--;
    
    return result;
}

Value value_array_get(Value* array, size_t index) { 
    if (!array || array->type != VALUE_ARRAY || index >= array->data.array_value.count) {
        return value_create_null();
    }
    
    Value* element = (Value*)array->data.array_value.elements[index];
    if (element) {
        return value_clone(element);
    }
    return value_create_null();
}

void value_array_set(Value* array, size_t index, Value element) {
    if (!array || array->type != VALUE_ARRAY || index >= array->data.array_value.count) return;
    
    Value* stored_element = (Value*)array->data.array_value.elements[index];
    if (stored_element) {
        value_free(stored_element);
        *stored_element = value_clone(&element);
    }
}

size_t value_array_length(Value* array) { 
    if (!array || array->type != VALUE_ARRAY) return 0;
    return array->data.array_value.count;
}

// ============================================================================
// OBJECT OPERATIONS
// ============================================================================

Value value_create_object(size_t initial_capacity) {
    Value v = {0};
    v.type = VALUE_OBJECT;
    v.data.object_value.count = 0;
    v.data.object_value.capacity = initial_capacity > 0 ? initial_capacity : 4;
    
    // Allocate memory with proper error handling using shared_malloc_safe for consistency
    v.data.object_value.keys = shared_malloc_safe(v.data.object_value.capacity * sizeof(char*), "interpreter", "value_create_object", 401);
    v.data.object_value.values = shared_malloc_safe(v.data.object_value.capacity * sizeof(void*), "interpreter", "value_create_object", 402);
    
    // If allocation fails, return a NULL value instead of a broken object
    if (!v.data.object_value.keys || !v.data.object_value.values) {
        if (v.data.object_value.keys) {
            shared_free_safe(v.data.object_value.keys, "interpreter", "value_create_object", 407);
        }
        if (v.data.object_value.values) {
            shared_free_safe(v.data.object_value.values, "interpreter", "value_create_object", 410);
        }
        return value_create_null();  // Return NULL value instead of broken object
    }
    
    // Initialize allocated memory to zero
    memset(v.data.object_value.keys, 0, v.data.object_value.capacity * sizeof(char*));
    memset(v.data.object_value.values, 0, v.data.object_value.capacity * sizeof(void*));
    
    return v;
}

void value_object_set_member(Value* object, const char* member_name, Value member_value) {
    if (!object || object->type != VALUE_OBJECT || !member_name) return;
    
    // Check if member already exists
    for (size_t i = 0; i < object->data.object_value.count; i++) {
        if (object->data.object_value.keys[i] && 
            strcmp(object->data.object_value.keys[i], member_name) == 0) {
            // Update existing member
            Value* existing_value = (Value*)object->data.object_value.values[i];
            if (existing_value) {
                value_free(existing_value);
                *existing_value = value_clone(&member_value);
            }
            return;
        }
    }
    
    // Add new member if there's space
    if (object->data.object_value.count < object->data.object_value.capacity) {
        object->data.object_value.keys[object->data.object_value.count] = shared_strdup(member_name);
        object->data.object_value.values[object->data.object_value.count] = shared_malloc_safe(sizeof(Value), "interpreter", "unknown_function", 420);
        if (object->data.object_value.values[object->data.object_value.count]) {
            Value* stored_value = (Value*)object->data.object_value.values[object->data.object_value.count];
            *stored_value = value_clone(&member_value);
            object->data.object_value.count++;
        }
    }
}

void value_object_set(Value* obj, const char* key, Value value) {
    if (!obj || obj->type != VALUE_OBJECT || !key) return;
    
    // Check if key already exists - overwrite it
    for (size_t i = 0; i < obj->data.object_value.count; i++) {
        if (obj->data.object_value.keys[i] && 
            strcmp(obj->data.object_value.keys[i], key) == 0) {
            // Update existing member
            Value* existing_value = (Value*)obj->data.object_value.values[i];
            if (existing_value) {
                value_free(existing_value);
                *existing_value = value_clone(&value);
            }
            return;
        }
    }
    
    // Add new member if there's space
    if (obj->data.object_value.count < obj->data.object_value.capacity) {
        obj->data.object_value.keys[obj->data.object_value.count] = key ? shared_strdup(key) : NULL;
        Value* new_value = shared_malloc_safe(sizeof(Value), "interpreter", "unknown_function", 2912);
        if (new_value) {
            *new_value = value_clone(&value);
            obj->data.object_value.values[obj->data.object_value.count] = new_value;
            obj->data.object_value.count++;
        } else {
            // If value allocation failed, free the key to prevent memory leak
            if (obj->data.object_value.keys[obj->data.object_value.count]) {
                shared_free_safe(obj->data.object_value.keys[obj->data.object_value.count], "interpreter", "value_object_set", 0);
                obj->data.object_value.keys[obj->data.object_value.count] = NULL;
            }
        }
    }
}

Value value_object_get(Value* obj, const char* key) { 
    if (!obj || obj->type != VALUE_OBJECT || !key) {
        return value_create_null();
    }
    
    for (size_t i = 0; i < obj->data.object_value.count; i++) {
        if (obj->data.object_value.keys[i] && 
            strcmp(obj->data.object_value.keys[i], key) == 0) {
            Value* member_value = (Value*)obj->data.object_value.values[i];
            if (member_value) {
                return value_clone(member_value);
            }
        }
    }
    
    return value_create_null();
}

int value_object_has(Value* obj, const char* key) { 
    if (!obj || obj->type != VALUE_OBJECT || !key) {
        return 0;
    }
    
    for (size_t i = 0; i < obj->data.object_value.count; i++) {
        if (obj->data.object_value.keys[i] && 
            strcmp(obj->data.object_value.keys[i], key) == 0) {
            return 1;
        }
    }
    
    return 0;
}

void value_object_delete(Value* obj, const char* key) {
    if (!obj || obj->type != VALUE_OBJECT || !key) {
        return;
    }
    
    for (size_t i = 0; i < obj->data.object_value.count; i++) {
        if (obj->data.object_value.keys[i] && 
            strcmp(obj->data.object_value.keys[i], key) == 0) {
            // Free the key
            shared_free_safe(obj->data.object_value.keys[i], "interpreter", "unknown_function", 2920);
            obj->data.object_value.keys[i] = NULL;
            
            // Free the value
            Value* value = (Value*)obj->data.object_value.values[i];
            if (value) {
                value_free(value);
                shared_free_safe(value, "interpreter", "unknown_function", 2927);
                obj->data.object_value.values[i] = NULL;
            }
            
            // Shift remaining elements
            for (size_t j = i; j < obj->data.object_value.count - 1; j++) {
                obj->data.object_value.keys[j] = obj->data.object_value.keys[j + 1];
                obj->data.object_value.values[j] = obj->data.object_value.values[j + 1];
            }
            
            obj->data.object_value.count--;
            break;
        }
    }
}

size_t value_object_size(Value* obj) {
    if (!obj || obj->type != VALUE_OBJECT) return 0;
    return obj->data.object_value.count;
}

char** value_object_keys(Value* obj, size_t* count) { 
    if (!obj || obj->type != VALUE_OBJECT || !count) {
        if (count) *count = 0;
        return NULL;
    }
    
    *count = obj->data.object_value.count;
    if (*count == 0) {
        return NULL;
    }
    
    char** keys = shared_malloc_safe(*count * sizeof(char*), "interpreter", "unknown_function", 2992);
    if (!keys) {
        *count = 0;
        return NULL;
    }
    
    for (size_t i = 0; i < *count; i++) {
        if (obj->data.object_value.keys[i]) {
            keys[i] = obj->data.object_value.keys[i] ? shared_strdup(obj->data.object_value.keys[i]) : NULL;
            if (!keys[i]) {
                // Clean up on failure
                for (size_t j = 0; j < i; j++) {
                    shared_free_safe(keys[j], "interpreter", "unknown_function", 3004);
                }
                shared_free_safe(keys, "interpreter", "unknown_function", 3006);
                *count = 0;
                return NULL;
            }
        } else {
            keys[i] = NULL;
        }
    }
    
    return keys;
}

// ============================================================================
// HASH MAP OPERATIONS
// ============================================================================

Value value_create_hash_map(size_t initial_capacity) {
    Value v = {0};  // Initialize entire struct to zero
    v.type = VALUE_HASH_MAP;
    v.flags = 0;
    v.ref_count = 1;
    v.data.hash_map_value.count = 0;
    v.data.hash_map_value.capacity = initial_capacity > 0 ? initial_capacity : 8;
    v.data.hash_map_value.keys = shared_malloc_safe(v.data.hash_map_value.capacity * sizeof(void*), "interpreter", "value_create_hash_map", 0);
    v.data.hash_map_value.values = shared_malloc_safe(v.data.hash_map_value.capacity * sizeof(void*), "interpreter", "value_create_hash_map", 1);
    
    // Initialize to NULL
    if (v.data.hash_map_value.keys) {
        memset(v.data.hash_map_value.keys, 0, v.data.hash_map_value.capacity * sizeof(void*));
    }
    if (v.data.hash_map_value.values) {
        memset(v.data.hash_map_value.values, 0, v.data.hash_map_value.capacity * sizeof(void*));
    }
    
    // Initialize cache
    v.cache.cached_ptr = NULL;
    v.cache.cached_length = 0;
    v.cache.cached_numeric = 0.0;
    
    return v;
}

void value_hash_map_set(Value* map, Value key, Value value) {
    if (!map || map->type != VALUE_HASH_MAP) return;
    
    // Check if key already exists
    for (size_t i = 0; i < map->data.hash_map_value.count; i++) {
        Value* existing_key = (Value*)map->data.hash_map_value.keys[i];
        if (existing_key && value_equals(existing_key, &key)) {
            // Update existing value
            Value* existing_value = (Value*)map->data.hash_map_value.values[i];
            if (existing_value) {
                value_free(existing_value);
                shared_free_safe(existing_value, "interpreter", "value_hash_map_set", 0);
                map->data.hash_map_value.values[i] = shared_malloc_safe(sizeof(Value), "interpreter", "value_hash_map_set", 1);
                *(Value*)map->data.hash_map_value.values[i] = value_clone(&value);
            }
            return;
        }
    }
    
    // Add new key-value pair
    if (map->data.hash_map_value.count >= map->data.hash_map_value.capacity) {
        // Resize
        size_t new_capacity = map->data.hash_map_value.capacity * 2;
        void** new_keys = shared_realloc_safe(map->data.hash_map_value.keys, new_capacity * sizeof(void*), "interpreter", "unknown_function", 3051);
        void** new_values = shared_realloc_safe(map->data.hash_map_value.values, new_capacity * sizeof(void*), "interpreter", "unknown_function", 3052);
        if (!new_keys || !new_values) return;
        
        map->data.hash_map_value.keys = new_keys;
        map->data.hash_map_value.values = new_values;
        map->data.hash_map_value.capacity = new_capacity;
    }
    
    // Add new entry
    Value* new_key = shared_malloc_safe(sizeof(Value), "interpreter", "unknown_function", 3061);
    if (!new_key) return;  // Safety check
    *new_key = value_clone(&key);
    map->data.hash_map_value.keys[map->data.hash_map_value.count] = new_key;
    
    Value* new_value = shared_malloc_safe(sizeof(Value), "interpreter", "unknown_function", 3063);
    if (!new_value) {
        // Clean up the key we just allocated
        value_free(new_key);
        shared_free_safe(new_key, "interpreter", "value_hash_map_set", 0);
        return;
    }
    *new_value = value_clone(&value);
    map->data.hash_map_value.values[map->data.hash_map_value.count] = new_value;
    map->data.hash_map_value.count++;
}

Value value_hash_map_get(Value* map, Value key) {
    if (!map || map->type != VALUE_HASH_MAP) {
        return value_create_null();
    }
    
    for (size_t i = 0; i < map->data.hash_map_value.count; i++) {
        Value* existing_key = (Value*)map->data.hash_map_value.keys[i];
        if (existing_key && value_equals(existing_key, &key)) {
            Value* value = (Value*)map->data.hash_map_value.values[i];
            return value ? value_clone(value) : value_create_null();
        }
    }
    
    return value_create_null();
}

int value_hash_map_has(Value* map, Value key) {
    if (!map || map->type != VALUE_HASH_MAP) return 0;
    
    for (size_t i = 0; i < map->data.hash_map_value.count; i++) {
        Value* existing_key = (Value*)map->data.hash_map_value.keys[i];
        if (existing_key && value_equals(existing_key, &key)) {
            return 1;
        }
    }
    
    return 0;
}

void value_hash_map_delete(Value* map, Value key) {
    if (!map || map->type != VALUE_HASH_MAP) return;
    
    for (size_t i = 0; i < map->data.hash_map_value.count; i++) {
        Value* existing_key = (Value*)map->data.hash_map_value.keys[i];
        if (existing_key && value_equals(existing_key, &key)) {
            // Free the key
            value_free(existing_key);
            shared_free_safe(existing_key, "interpreter", "unknown_function", 3106);
            map->data.hash_map_value.keys[i] = NULL;
            
            // Free the value
            Value* value = (Value*)map->data.hash_map_value.values[i];
            if (value) {
                value_free(value);
                shared_free_safe(value, "interpreter", "unknown_function", 3113);
                map->data.hash_map_value.values[i] = NULL;
            }
            
            // Shift remaining elements
            for (size_t j = i; j < map->data.hash_map_value.count - 1; j++) {
                map->data.hash_map_value.keys[j] = map->data.hash_map_value.keys[j + 1];
                map->data.hash_map_value.values[j] = map->data.hash_map_value.values[j + 1];
            }
            
            map->data.hash_map_value.count--;
            break;
        }
    }
}

Value* value_hash_map_keys(Value* map, size_t* count) {
    if (!map || map->type != VALUE_HASH_MAP) {
        if (count) *count = 0;
        return NULL;
    }
    
    if (count) *count = map->data.hash_map_value.count;
    if (map->data.hash_map_value.count == 0) {
        return NULL;
    }
    
    // Safety check: ensure keys array exists
    if (!map->data.hash_map_value.keys) {
        if (count) *count = 0;
        return NULL;
    }
    
    Value* keys = shared_malloc_safe(map->data.hash_map_value.count * sizeof(Value), "interpreter", "value_hash_map_keys", 0);
    if (!keys) {
        if (count) *count = 0;
        return NULL;
    }
    
    for (size_t i = 0; i < map->data.hash_map_value.count; i++) {
        Value* existing_key = (Value*)map->data.hash_map_value.keys[i];
        if (existing_key) {
            keys[i] = value_clone(existing_key);
        } else {
            keys[i] = value_create_null();
        }
    }
    
    return keys;
}

size_t value_hash_map_size(Value* map) {
    if (!map || map->type != VALUE_HASH_MAP) return 0;
    return map->data.hash_map_value.count;
}

// ============================================================================
// SET OPERATIONS
// ============================================================================

Value value_create_set(size_t initial_capacity) {
    Value v;
    v.type = VALUE_SET;
    v.data.set_value.count = 0;
    v.data.set_value.capacity = initial_capacity > 0 ? initial_capacity : 8;
    v.data.set_value.elements = shared_malloc_safe(v.data.set_value.capacity * sizeof(void*), "interpreter", "value_create_set", 0);
    
    // Initialize to NULL
    if (v.data.set_value.elements) {
        memset(v.data.set_value.elements, 0, v.data.set_value.capacity * sizeof(void*));
    }
    
    return v;
}

void value_set_add(Value* set, Value element) {
    if (!set || set->type != VALUE_SET) return;
    
    // Check if element already exists
    for (size_t i = 0; i < set->data.set_value.count; i++) {
        Value* existing = (Value*)set->data.set_value.elements[i];
        if (existing && value_equals(existing, &element)) {
            return; // Element already exists
        }
    }
    
    // Resize if needed
    if (set->data.set_value.count >= set->data.set_value.capacity) {
        size_t new_capacity = set->data.set_value.capacity > 0 ? set->data.set_value.capacity * 2 : 8;
        void** new_elements = shared_realloc_safe(set->data.set_value.elements, new_capacity * sizeof(void*), "interpreter", "value_set_add", 0);
        if (!new_elements) return;
        
        set->data.set_value.elements = new_elements;
        set->data.set_value.capacity = new_capacity;
    }
    
    // Add new element
    Value* new_element = shared_malloc_safe(sizeof(Value), "interpreter", "unknown_function", 3186);
    if (new_element) {
        *new_element = value_clone(&element);
        set->data.set_value.elements[set->data.set_value.count] = new_element;
        set->data.set_value.count++;
    }
}

int value_set_has(Value* set, Value element) {
    if (!set || set->type != VALUE_SET) return 0;
    
    for (size_t i = 0; i < set->data.set_value.count; i++) {
        Value* existing = (Value*)set->data.set_value.elements[i];
        if (existing && value_equals(existing, &element)) {
            return 1;
        }
    }
    
    return 0;
}

void value_set_remove(Value* set, Value element) {
    if (!set || set->type != VALUE_SET) return;
    
    for (size_t i = 0; i < set->data.set_value.count; i++) {
        Value* existing = (Value*)set->data.set_value.elements[i];
        if (existing && value_equals(existing, &element)) {
            // Free the element
            value_free(existing);
            shared_free_safe(existing, "interpreter", "unknown_function", 3214);
            set->data.set_value.elements[i] = NULL;
            
            // Shift remaining elements
            for (size_t j = i; j < set->data.set_value.count - 1; j++) {
                set->data.set_value.elements[j] = set->data.set_value.elements[j + 1];
            }
            
            // Clear the last element after shift
            set->data.set_value.elements[set->data.set_value.count - 1] = NULL;
            set->data.set_value.count--;
            break;
        }
    }
}

size_t value_set_size(Value* set) {
    if (!set || set->type != VALUE_SET) return 0;
    return set->data.set_value.count;
}

Value value_set_to_array(Value* set) {
    if (!set || set->type != VALUE_SET) {
        return value_create_array(0);
    }
    
    Value array = value_create_array(set->data.set_value.count);
    for (size_t i = 0; i < set->data.set_value.count; i++) {
        Value* element = (Value*)set->data.set_value.elements[i];
        if (element) {
            value_array_push(&array, value_clone(element));
        }
    }
    
    return array;
}
