#include "codegen_variables.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Variable scoping system implementation
VariableScopeStack* variable_scope_create(void) {
    VariableScopeStack* scope = malloc(sizeof(VariableScopeStack));
    if (!scope) return NULL;
    
    scope->capacity = 1000;
    scope->count = 0;
    scope->current_scope_level = 0;
    scope->entries = malloc(sizeof(VariableScopeEntry) * scope->capacity);
    if (!scope->entries) {
        free(scope);
        return NULL;
    }
    
    return scope;
}

void variable_scope_free(VariableScopeStack* scope) {
    if (!scope) return;
    
    for (int i = 0; i < scope->count; i++) {
        free(scope->entries[i].original_name);
        free(scope->entries[i].c_name);
    }
    free(scope->entries);
    free(scope);
}

void variable_scope_enter(VariableScopeStack* scope) {
    if (!scope) return;
    scope->current_scope_level++;
}

void variable_scope_exit(VariableScopeStack* scope) {
    if (!scope) return;
    
    // Remove all variables from the current scope level
    int write_index = 0;
    for (int i = 0; i < scope->count; i++) {
        if (scope->entries[i].scope_level < scope->current_scope_level) {
            if (write_index != i) {
                scope->entries[write_index] = scope->entries[i];
            }
            write_index++;
        } else {
            // Free the memory for variables being removed
            free(scope->entries[i].original_name);
            free(scope->entries[i].c_name);
        }
    }
    scope->count = write_index;
    scope->current_scope_level--;
}

char* variable_scope_get_c_name(VariableScopeStack* scope, const char* original_name) {
    if (!scope || !original_name) return NULL;
    
    // Look for the variable in the current scope and all parent scopes
    for (int i = scope->count - 1; i >= 0; i--) {
        if (strcmp(scope->entries[i].original_name, original_name) == 0) {
            return (scope->entries[i].c_name ? strdup(scope->entries[i].c_name) : NULL);
        }
    }
    
    // Variable not found, return the original name
    return (original_name ? strdup(original_name) : NULL);
}

char* variable_scope_declare_variable(VariableScopeStack* scope, const char* original_name) {
    if (!scope || !original_name) return NULL;
    
    // Create new variable entry
    if (scope->count >= scope->capacity) {
        scope->capacity *= 2;
        scope->entries = realloc(scope->entries, sizeof(VariableScopeEntry) * scope->capacity);
        if (!scope->entries) return NULL;
    }
    
    VariableScopeEntry* entry = &scope->entries[scope->count];
    entry->original_name = (original_name ? strdup(original_name) : NULL);
    entry->scope_level = scope->current_scope_level;
    entry->is_declared = 1;
    
    // Generate unique C name based on scope level and name count
    char c_name[256];
    if (scope->current_scope_level == 0) {
        // Global scope - add count suffix for duplicates
        int name_count = 0;
        for (int i = 0; i < scope->count; i++) {
            if (strcmp(scope->entries[i].original_name, original_name) == 0) {
                name_count++;
            }
        }
        if (name_count > 0) {
            snprintf(c_name, sizeof(c_name), "%s_%d", original_name, name_count + 1);
        } else {
            strcpy(c_name, original_name);
        }
    } else {
        // Local scope - add scope level suffix
        snprintf(c_name, sizeof(c_name), "%s_%d", original_name, scope->current_scope_level);
    }
    
    entry->c_name = (c_name ? strdup(c_name) : NULL);
    scope->count++;
    
    return (entry->c_name ? strdup(entry->c_name) : NULL);
}

int variable_scope_is_declared(VariableScopeStack* scope, const char* original_name) {
    if (!scope || !original_name) return 0;
    
    for (int i = 0; i < scope->count; i++) {
        if (strcmp(scope->entries[i].original_name, original_name) == 0) {
            return 1;
        }
    }
    
    return 0;
}
