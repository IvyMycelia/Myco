#include "../../include/libs/builtin_libs.h"
#include "../../include/core/interpreter.h"
#include "../../include/core/interpreter/value_operations.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../../include/core/standardized_errors.h"
#include "../../include/utils/shared_utilities.h"

// Map operations
Value builtin_map_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 0) {
        std_error_report(ERROR_INTERNAL_ERROR, "maps", "unknown_function", "map.create() expects no arguments", line, column);
        return value_create_null();
    }
    
    return value_create_hash_map(8);
}

Value builtin_map_has(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_INTERNAL_ERROR, "maps", "unknown_function", "map.has() expects exactly 1 argument: key", line, column);
        return value_create_null();
    }
    
    Value map = args[0];
    Value key = args[1];
    
    if (map.type != VALUE_HASH_MAP) {
        std_error_report(ERROR_INTERNAL_ERROR, "maps", "unknown_function", "map.has() can only be called on a hash map", line, column);
        return value_create_null();
    }
    
    int has_key = value_hash_map_has(&map, key);
    return value_create_boolean(has_key);
}

Value builtin_map_set(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 3) {
        std_error_report(ERROR_INTERNAL_ERROR, "maps", "unknown_function", "map.set() expects exactly 2 arguments: key, value", line, column);
        return value_create_null();
    }
    
    Value map = args[0];
    Value key = args[1];
    Value value = args[2];
    
    if (map.type != VALUE_HASH_MAP) {
        std_error_report(ERROR_INTERNAL_ERROR, "maps", "unknown_function", "map.set() can only be called on a hash map", line, column);
        return value_create_null();
    }
    
    // Create a new map with all existing elements plus the new key-value pair
    Value result = value_create_hash_map(map.data.hash_map_value.capacity);
    
    // Copy all existing elements
    for (size_t i = 0; i < map.data.hash_map_value.count; i++) {
        Value* existing_key = (Value*)map.data.hash_map_value.keys[i];
        Value* existing_value = (Value*)map.data.hash_map_value.values[i];
        if (existing_key && existing_value) {
            Value cloned_key = value_clone(existing_key);
            Value cloned_value = value_clone(existing_value);
            value_hash_map_set(&result, cloned_key, cloned_value);
        }
    }
    
    // Add the new key-value pair
    value_hash_map_set(&result, key, value);
    
    return result;
}

Value builtin_map_get(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_INTERNAL_ERROR, "maps", "unknown_function", "map.get() expects exactly 1 argument: key", line, column);
        return value_create_null();
    }
    
    Value map = args[0];
    Value key = args[1];
    
    if (map.type != VALUE_HASH_MAP) {
        std_error_report(ERROR_INTERNAL_ERROR, "maps", "unknown_function", "map.get() can only be called on a hash map", line, column);
        return value_create_null();
    }
    
    return value_hash_map_get(&map, key);
}

Value builtin_map_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "maps", "unknown_function", "map.size() expects no arguments", line, column);
        return value_create_null();
    }
    
    Value map = args[0];
    
    if (map.type != VALUE_HASH_MAP) {
        std_error_report(ERROR_INTERNAL_ERROR, "maps", "unknown_function", "map.size() can only be called on a hash map", line, column);
        return value_create_null();
    }
    
    size_t size = value_hash_map_size(&map);
    return value_create_number((double)size);
}

Value builtin_map_keys(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "maps", "unknown_function", "map.keys() expects no arguments", line, column);
        return value_create_null();
    }
    
    Value map = args[0];
    
    if (map.type != VALUE_HASH_MAP) {
        std_error_report(ERROR_INTERNAL_ERROR, "maps", "unknown_function", "map.keys() can only be called on a hash map", line, column);
        return value_create_null();
    }
    
    size_t count;
    Value* keys = value_hash_map_keys(&map, &count);
    
    if (!keys || count == 0) {
        return value_create_array(0);
    }
    
    // Create array to hold the keys
    Value array = value_create_array(count);
    
    // Clone each key and store in the array
    for (size_t i = 0; i < count; i++) {
        Value cloned_key = value_clone(&keys[i]);
        array.data.array_value.elements[i] = shared_malloc_safe(sizeof(Value), "libs", "unknown_function", 71);
        *((Value*)array.data.array_value.elements[i]) = cloned_key;
    }
    array.data.array_value.count = count;
    
    // Free the original keys array
    shared_free_safe(keys, "libs", "unknown_function", 77);
    
    return array;
}

Value builtin_map_delete(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_INTERNAL_ERROR, "maps", "unknown_function", "map.delete() expects exactly 1 argument: key", line, column);
        return value_create_null();
    }
    
    Value map = args[0];
    Value key = args[1];
    
    if (map.type != VALUE_HASH_MAP) {
        std_error_report(ERROR_INTERNAL_ERROR, "maps", "unknown_function", "map.delete() can only be called on a hash map", line, column);
        return value_create_null();
    }
    
    // Create a new map with all existing elements except the one to delete
    Value result = value_create_hash_map(map.data.hash_map_value.capacity);
    
    // Copy all existing elements except the one to delete
    for (size_t i = 0; i < map.data.hash_map_value.count; i++) {
        Value* existing_key = (Value*)map.data.hash_map_value.keys[i];
        Value* existing_value = (Value*)map.data.hash_map_value.values[i];
        if (existing_key && existing_value) {
            // Check if this is the key to delete
            if (!value_equals(existing_key, &key)) {
                Value cloned_key = value_clone(existing_key);
                Value cloned_value = value_clone(existing_value);
                value_hash_map_set(&result, cloned_key, cloned_value);
            }
        }
    }
    
    return result;
}

Value builtin_map_clear(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        std_error_report(ERROR_INTERNAL_ERROR, "maps", "unknown_function", "map.clear() expects no arguments", line, column);
        return value_create_null();
    }
    
    Value map = args[0];
    
    if (map.type != VALUE_HASH_MAP) {
        std_error_report(ERROR_INTERNAL_ERROR, "maps", "unknown_function", "map.clear() can only be called on a hash map", line, column);
        return value_create_null();
    }
    
    // Create a new empty map (immutable operation)
    return value_create_hash_map(8);
}

Value builtin_map_update(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        std_error_report(ERROR_INTERNAL_ERROR, "maps", "unknown_function", "map.update() expects exactly 1 argument: other_map", line, column);
        return value_create_null();
    }
    
    Value map = args[0];
    Value other_map = args[1];
    
    if (map.type != VALUE_HASH_MAP || other_map.type != VALUE_HASH_MAP) {
        std_error_report(ERROR_INVALID_ARGUMENT, "maps", "builtin_map_update", "map.update() argument must be a hash map", line, column);
        return value_create_null();
    }
    
    // Create a new map with all elements from both maps
    Value result = value_create_hash_map(map.data.hash_map_value.capacity + other_map.data.hash_map_value.capacity);
    
    // Copy all elements from the original map
    for (size_t i = 0; i < map.data.hash_map_value.count; i++) {
        Value* existing_key = (Value*)map.data.hash_map_value.keys[i];
        Value* existing_value = (Value*)map.data.hash_map_value.values[i];
        if (existing_key && existing_value) {
            Value cloned_key = value_clone(existing_key);
            Value cloned_value = value_clone(existing_value);
            value_hash_map_set(&result, cloned_key, cloned_value);
        }
    }
    
    // Add all elements from the other map (this will overwrite any duplicate keys)
    for (size_t i = 0; i < other_map.data.hash_map_value.count; i++) {
        Value* other_key = (Value*)other_map.data.hash_map_value.keys[i];
        Value* other_value = (Value*)other_map.data.hash_map_value.values[i];
        if (other_key && other_value) {
            Value cloned_key = value_clone(other_key);
            Value cloned_value = value_clone(other_value);
            value_hash_map_set(&result, cloned_key, cloned_value);
        }
    }
    
    return result;
}

// Register the maps library
void maps_library_register(Interpreter* interpreter) {
    if (!interpreter) return;
    if (!interpreter->global_environment) return;
    
    // Expose a library object for maps with __type__="Library"
    Value maps_lib = value_create_object(8);
    value_object_set(&maps_lib, "__type__", value_create_string("Library"));
    value_object_set(&maps_lib, "type", value_create_string("Library"));
    
    // Add functions
    value_object_set(&maps_lib, "create", value_create_builtin_function(builtin_map_create));
    value_object_set(&maps_lib, "set", value_create_builtin_function(builtin_map_set));
    value_object_set(&maps_lib, "get", value_create_builtin_function(builtin_map_get));
    value_object_set(&maps_lib, "has", value_create_builtin_function(builtin_map_has));
    value_object_set(&maps_lib, "size", value_create_builtin_function(builtin_map_size));
    value_object_set(&maps_lib, "keys", value_create_builtin_function(builtin_map_keys));
    value_object_set(&maps_lib, "delete", value_create_builtin_function(builtin_map_delete));
    value_object_set(&maps_lib, "clear", value_create_builtin_function(builtin_map_clear));
    value_object_set(&maps_lib, "update", value_create_builtin_function(builtin_map_update));
    
    environment_define(interpreter->global_environment, "maps", maps_lib);
}