#ifndef CODEGEN_VARIABLES_H
#define CODEGEN_VARIABLES_H

// Variable scope entry
typedef struct {
    char* original_name;
    char* c_name;
    int scope_level;
    int is_declared;
} VariableScopeEntry;

// Variable scope stack
typedef struct {
    VariableScopeEntry* entries;
    int capacity;
    int count;
    int current_scope_level;
} VariableScopeStack;

// Variable scoping functions
VariableScopeStack* variable_scope_create(void);
void variable_scope_free(VariableScopeStack* scope);
void variable_scope_enter(VariableScopeStack* scope);
void variable_scope_exit(VariableScopeStack* scope);
char* variable_scope_get_c_name(VariableScopeStack* scope, const char* original_name);
char* variable_scope_declare_variable(VariableScopeStack* scope, const char* original_name);
int variable_scope_is_declared(VariableScopeStack* scope, const char* original_name);

#endif // CODEGEN_VARIABLES_H
