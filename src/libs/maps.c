#include "../../include/libs/builtin_libs.h"
#include "../../include/core/interpreter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Map operations
Value builtin_map_has(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "maps.has() expects exactly 2 arguments: map and key", line, column);
        return value_create_null();
    }
    
    Value map = args[0];
    Value key = args[1];
    
    if (map.type != VALUE_HASH_MAP) {
        interpreter_set_error(interpreter, "maps.has() first argument must be a hash map", line, column);
        return value_create_null();
    }
    
    int has_key = value_hash_map_has(&map, key);
    return value_create_boolean(has_key);
}

Value builtin_map_size(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "maps.size() expects exactly 1 argument: map", line, column);
        return value_create_null();
    }
    
    Value map = args[0];
    
    if (map.type != VALUE_HASH_MAP) {
        interpreter_set_error(interpreter, "maps.size() argument must be a hash map", line, column);
        return value_create_null();
    }
    
    size_t size = value_hash_map_size(&map);
    return value_create_number((double)size);
}

Value builtin_map_keys(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "maps.keys() expects exactly 1 argument: map", line, column);
        return value_create_null();
    }
    
    Value map = args[0];
    
    if (map.type != VALUE_HASH_MAP) {
        interpreter_set_error(interpreter, "maps.keys() argument must be a hash map", line, column);
        return value_create_null();
    }
    
    size_t count;
    Value* keys = value_hash_map_keys(&map, &count);
    
    if (!keys) {
        return value_create_array(0);
    }
    
    // For now, just return a simple message indicating the keys exist
    // TODO: Fix array creation to properly handle the keys
    char message[100];
    snprintf(message, sizeof(message), "Map has %zu keys", count);
    
    // Free the keys array
    free(keys);
    
    return value_create_string(strdup(message));
}

Value builtin_map_delete(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "maps.delete() expects exactly 2 arguments: map and key", line, column);
        return value_create_null();
    }
    
    Value* map = &args[0];
    Value key = args[1];
    
    if (map->type != VALUE_HASH_MAP) {
        interpreter_set_error(interpreter, "maps.delete() first argument must be a hash map", line, column);
        return value_create_null();
    }
    
    value_hash_map_delete(map, key);
    return value_create_null();
}

Value builtin_map_clear(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "maps.clear() expects exactly 1 argument: map", line, column);
        return value_create_null();
    }
    
    Value* map = &args[0];
    
    if (map->type != VALUE_HASH_MAP) {
        interpreter_set_error(interpreter, "maps.clear() argument must be a hash map", line, column);
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
    
    return value_create_null();
}

Value builtin_map_update(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "maps.update() expects exactly 2 arguments: map and other_map", line, column);
        return value_create_null();
    }
    
    Value* map = &args[0];
    Value other_map = args[1];
    
    if (map->type != VALUE_HASH_MAP || other_map.type != VALUE_HASH_MAP) {
        interpreter_set_error(interpreter, "maps.update() both arguments must be hash maps", line, column);
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
    
    return value_create_null();
}

// Register the maps library
void maps_library_register(Interpreter* interpreter) {
    if (!interpreter) return;
    
    // Create maps module
    Value maps_module = value_create_object(16);
    
    // Map functions
    value_object_set(&maps_module, "has", value_create_builtin_function(builtin_map_has));
    value_object_set(&maps_module, "size", value_create_builtin_function(builtin_map_size));
    value_object_set(&maps_module, "keys", value_create_builtin_function(builtin_map_keys));
    value_object_set(&maps_module, "delete", value_create_builtin_function(builtin_map_delete));
    value_object_set(&maps_module, "clear", value_create_builtin_function(builtin_map_clear));
    value_object_set(&maps_module, "update", value_create_builtin_function(builtin_map_update));
    
    // Register the module
    environment_define(interpreter->global_environment, "maps", maps_module);
}