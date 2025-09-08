#include "../../include/libs/builtin_libs.h"
#include "../../include/core/interpreter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Map operations
Value builtin_map_has(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "map.has() expects exactly 1 argument: key", line, column);
        return value_create_null();
    }
    
    Value map = args[0];
    Value key = args[1];
    
    if (map.type != VALUE_HASH_MAP) {
        interpreter_set_error(interpreter, "map.has() can only be called on a hash map", line, column);
        return value_create_null();
    }
    
    int has_key = value_hash_map_has(&map, key);
    return value_create_boolean(has_key);
}

Value builtin_map_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "map.size() expects no arguments", line, column);
        return value_create_null();
    }
    
    Value map = args[0];
    
    if (map.type != VALUE_HASH_MAP) {
        interpreter_set_error(interpreter, "map.size() can only be called on a hash map", line, column);
        return value_create_null();
    }
    
    size_t size = value_hash_map_size(&map);
    return value_create_number((double)size);
}

Value builtin_map_keys(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "map.keys() expects no arguments", line, column);
        return value_create_null();
    }
    
    Value map = args[0];
    
    if (map.type != VALUE_HASH_MAP) {
        interpreter_set_error(interpreter, "map.keys() can only be called on a hash map", line, column);
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
        array.data.array_value.elements[i] = malloc(sizeof(Value));
        *((Value*)array.data.array_value.elements[i]) = cloned_key;
    }
    array.data.array_value.count = count;
    
    // Free the original keys array
    free(keys);
    
    return array;
}

Value builtin_map_delete(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "map.delete() expects exactly 1 argument: key", line, column);
        return value_create_null();
    }
    
    Value* map = &args[0];
    Value key = args[1];
    
    if (map->type != VALUE_HASH_MAP) {
        interpreter_set_error(interpreter, "map.delete() can only be called on a hash map", line, column);
        return value_create_null();
    }
    
    value_hash_map_delete(map, key);
    return value_clone(map);
}

Value builtin_map_clear(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "map.clear() expects no arguments", line, column);
        return value_create_null();
    }
    
    Value* map = &args[0];
    
    if (map->type != VALUE_HASH_MAP) {
        interpreter_set_error(interpreter, "map.clear() can only be called on a hash map", line, column);
        return value_create_null();
    }
    
    // Clear all key-value pairs
    size_t count;
    Value* keys = value_hash_map_keys(map, &count);
    
    if (keys) {
        for (size_t i = 0; i < count; i++) {
            value_hash_map_delete(map, keys[i]);
        }
        free(keys);
    }
    
    return value_clone(map);
}

Value builtin_map_update(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "map.update() expects exactly 1 argument: other_map", line, column);
        return value_create_null();
    }
    
    Value* map = &args[0];
    Value other_map = args[1];
    
    if (map->type != VALUE_HASH_MAP || other_map.type != VALUE_HASH_MAP) {
        interpreter_set_error(interpreter, "map.update() argument must be a hash map", line, column);
        return value_create_null();
    }
    
    // Get all keys from other_map and add them to map
    size_t count;
    Value* keys = value_hash_map_keys(&other_map, &count);
    
    if (keys) {
        for (size_t i = 0; i < count; i++) {
            Value value = value_hash_map_get(&other_map, keys[i]);
            value_hash_map_set(map, keys[i], value);
            value_free(&value);
        }
        free(keys);
    }
    
    return value_clone(map);
}

// Register the maps library
void maps_library_register(Interpreter* interpreter) {
    if (!interpreter) return;
    
    // Map methods are now called directly on hash map values, not as global functions
    // No need to register global functions anymore
}