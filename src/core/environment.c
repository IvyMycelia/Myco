#include "interpreter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../../include/utils/shared_utilities.h"

// Environment management functions
Environment* environment_create(Environment* parent) {
    Environment* env = shared_malloc_safe(sizeof(Environment), "core", "unknown_function", 9);
    if (!env) return NULL;
    
    env->parent = parent;
    env->names = NULL;
    env->values = NULL;
    env->count = 0;
    env->capacity = 0;
    
    return env;
}

void environment_free(Environment* env) {
    if (!env) return;
    
    if (env->names) {
        for (size_t i = 0; i < env->count; i++) {
            shared_free_safe(env->names[i], "core", "unknown_function", 26);
        }
        shared_free_safe(env->names, "core", "unknown_function", 28);
    }
    
    if (env->values) {
        for (size_t i = 0; i < env->count; i++) {
            value_free(&env->values[i]);
        }
        shared_free_safe(env->values, "core", "unknown_function", 35);
    }
    
    shared_free_safe(env, "core", "unknown_function", 38);
}

static int environment_find_index(Environment* env, const char* name) {
    for (size_t i = 0; i < env->count; i++) {
        if (strcmp(env->names[i], name) == 0) {
            return (int)i;
        }
    }
    return -1;
}

void environment_define(Environment* env, const char* name, Value value) {
    if (!env || !name) return;
    
    // Check if variable already exists
    int index = environment_find_index(env, name);
    if (index >= 0) {
        // Update existing variable
        value_free(&env->values[index]);
        env->values[index] = value_clone(&value);
        return;
    }
    
    // Resize if needed
    if (env->count >= env->capacity) {
        size_t new_capacity = env->capacity == 0 ? 8 : env->capacity * 2;
        char** new_names = shared_realloc_safe(env->names, new_capacity * sizeof(char*), "core", "unknown_function", 65);
        Value* new_values = shared_realloc_safe(env->values, new_capacity * sizeof(Value), "core", "unknown_function", 66);
        
        if (!new_names || !new_values) {
            shared_free_safe(new_names, "core", "unknown_function", 69);
            shared_free_safe(new_values, "core", "unknown_function", 70);
            return;
        }
        
        env->names = new_names;
        env->values = new_values;
        env->capacity = new_capacity;
    }
    
    // Add new variable
    env->names[env->count] = strdup(name);
    env->values[env->count] = value_clone(&value);
    env->count++;
}

Environment* environment_copy(Environment* env) {
    if (!env) return NULL;
    
    Environment* copy = environment_create(env->parent);
    if (!copy) return NULL;
    
    for (size_t i = 0; i < env->count; i++) {
        environment_define(copy, env->names[i], env->values[i]);
    }
    
    return copy;
}

Value environment_get(Environment* env, const char* name) {
    if (!env || !name) return value_create_null();
    
    int index = environment_find_index(env, name);
    if (index >= 0) {
        return value_clone(&env->values[index]);
    }
    
    // Check parent environment
    if (env->parent) {
        return environment_get(env->parent, name);
    }
    
    return value_create_null();
}

void environment_assign(Environment* env, const char* name, Value value) {
    if (!env || !name) return;
    
    int index = environment_find_index(env, name);
    if (index >= 0) {
        // Check if this is a self-assignment (same memory address)
        if (&env->values[index] == &value) {
            // Self-assignment: just return without doing anything
            return;
        }
        
        // Clone the new value first, then free the old one
        Value new_value = value_clone(&value);
        value_free(&env->values[index]);
        env->values[index] = new_value;
        return;
    }
    
    // Check parent environment
    if (env->parent) {
        environment_assign(env->parent, name, value);
    } else {
        // No parent, create new variable in current environment
        environment_define(env, name, value);
    }
}

int environment_exists(Environment* env, const char* name) {
    if (!env || !name) return 0;
    
    int index = environment_find_index(env, name);
    if (index >= 0) return 1;
    
    if (env->parent) {
        return environment_exists(env->parent, name);
    }
    
    return 0;
}
