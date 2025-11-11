#include "interpreter/value_operations.h"
#include "../../include/core/interpreter.h"
#include "../../include/core/bytecode.h"
#include "../../include/utils/shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

// Forward declaration
int bc_compile_ast_to_subprogram(BytecodeProgram* p, ASTNode* node, const char* name);

// ============================================================================
// FUNCTION VALUE CREATION FUNCTIONS
// ============================================================================

Value value_create_function(ASTNode* body, ASTNode** params, size_t param_count, const char* return_type, Environment* captured_env) {
    Value v = {0};
    v.type = VALUE_FUNCTION;
    
    // Check if body is actually a bytecode function ID (small integer cast as pointer)
    // Bytecode function IDs are stored as small integers (< 10000) cast to pointers
    // Real ASTNode pointers are much larger addresses
    // Note: function ID 0 is valid, so we check body_addr first, not body pointer
    uintptr_t body_addr = (uintptr_t)body;
    if (body_addr < 10000) {
        // This is likely a bytecode function ID (including 0), not a real ASTNode
        // Store it directly without cloning
        v.data.function_value.body = body;
    } else {
        // This is a real ASTNode - clone it
        v.data.function_value.body = body ? ast_clone(body) : NULL;
    }
    
    v.data.function_value.parameter_count = param_count;
    v.data.function_value.return_type = return_type ? shared_strdup(return_type) : NULL;
    v.data.function_value.captured_environment = captured_env;
    
    // Copy parameter nodes with proper error handling
    if (param_count > 0 && params) {
        v.data.function_value.parameters = (ASTNode**)shared_malloc_safe(param_count * sizeof(ASTNode*), "interpreter", "value_create_function", 0);
        if (v.data.function_value.parameters) {
            for (size_t i = 0; i < param_count; i++) {
                v.data.function_value.parameters[i] = params[i] ? ast_clone(params[i]) : NULL;
            }
        } else {
            // If parameter allocation fails, clean up return type
            if (v.data.function_value.return_type) {
                shared_free_safe(v.data.function_value.return_type, "interpreter", "value_create_function", 0);
                v.data.function_value.return_type = NULL;
            }
        }
    } else {
        v.data.function_value.parameters = NULL;
    }
    
    return v;
}

/**
 * @brief Create an async function value
 */
Value value_create_async_function(const char* name, ASTNode** params, size_t param_count, const char* return_type, ASTNode* body, Environment* captured_env) {
    Value v = {0};
    v.type = VALUE_ASYNC_FUNCTION;
    v.data.async_function_value.body = body;
    v.data.async_function_value.parameter_count = param_count;
    v.data.async_function_value.return_type = return_type ? shared_strdup(return_type) : NULL;
    v.data.async_function_value.captured_environment = captured_env;
    
    // Copy parameter nodes with proper error handling
    if (param_count > 0 && params) {
        v.data.async_function_value.parameters = (ASTNode**)shared_malloc_safe(param_count * sizeof(ASTNode*), "interpreter", "value_create_async_function", 0);
        if (v.data.async_function_value.parameters) {
            for (size_t i = 0; i < param_count; i++) {
                v.data.async_function_value.parameters[i] = params[i] ? ast_clone(params[i]) : NULL;
            }
        } else {
            // If parameter allocation fails, clean up return type
            if (v.data.async_function_value.return_type) {
                shared_free_safe(v.data.async_function_value.return_type, "interpreter", "value_create_async_function", 0);
                v.data.async_function_value.return_type = NULL;
            }
        }
    } else {
        v.data.async_function_value.parameters = NULL;
    }
    
    return v;
}

/**
 * @brief Create a promise value
 */
Value value_create_promise(Value resolved_value, int is_resolved, Value error_value) {
    Value v = {0};
    v.type = VALUE_PROMISE;
    
    v.data.promise_value.is_resolved = is_resolved;
    v.data.promise_value.is_rejected = !is_resolved;
    v.data.promise_value.error_message = NULL;
    v.data.promise_value.resolved_data = NULL;
    
    if (is_resolved) {
        // Convert resolved value to string representation
        Value str_value = value_to_string(&resolved_value);
        if (str_value.type == VALUE_STRING) {
            v.data.promise_value.resolved_data = str_value.data.string_value ? shared_strdup(str_value.data.string_value) : NULL;
        }
        value_free(&str_value);
    } else if (error_value.type == VALUE_STRING) {
        v.data.promise_value.error_message = error_value.data.string_value ? shared_strdup(error_value.data.string_value) : NULL;
    }
    
    return v;
}

Value value_create_builtin_function(Value (*func)(Interpreter*, Value*, size_t, int, int)) {
    Value v = {0};
    v.type = VALUE_FUNCTION;
    v.data.function_value.body = NULL;
    v.data.function_value.parameters = NULL;
    v.data.function_value.parameter_count = 0;
    v.data.function_value.return_type = NULL;
    // Store the builtin function pointer in the body field for now
    // In a real implementation, you'd want a separate field for this
    v.data.function_value.body = (ASTNode*)func;
    // Mark this as a built-in function by setting a special flag
    v.flags |= VALUE_FLAG_CACHED; // Use this flag to mark built-in functions
    return v;
}

// Helper function to find a method in the inheritance chain
Value find_method_in_inheritance_chain(Interpreter* interpreter, Value* class_value, const char* method_name) {
    if (!class_value || class_value->type != VALUE_CLASS) {
        return value_create_null();
    }
    
    // Ensure metadata is compiled
    if (!class_value->data.class_value.metadata) {
        compile_class_metadata(interpreter, class_value);
    }
    
    // Search in current class metadata
    if (class_value->data.class_value.metadata) {
        ClassMetadata* metadata = class_value->data.class_value.metadata;
        for (size_t i = 0; i < metadata->method_count; i++) {
            if (metadata->methods[i].name && strcmp(metadata->methods[i].name, method_name) == 0) {
                // Found the method! Return function with bytecode function ID
                return value_create_function(
                    (ASTNode*)(uintptr_t)metadata->methods[i].bytecode_func_id,  // Store function ID as pointer
                    NULL,  // No AST parameters for bytecode functions
                    metadata->methods[i].param_count,
                    metadata->methods[i].return_type,
                    class_value->data.class_value.class_environment
                );
            }
        }
    }
    
    // Fallback: search in AST if metadata not available
    ASTNode* class_body = class_value->data.class_value.class_body;
    if (class_body && class_body->type == AST_NODE_BLOCK) {
        for (size_t i = 0; i < class_body->data.block.statement_count; i++) {
            ASTNode* stmt = class_body->data.block.statements[i];
            if (stmt && stmt->type == AST_NODE_FUNCTION && 
                stmt->data.function_definition.function_name &&
                strcmp(stmt->data.function_definition.function_name, method_name) == 0) {
                // Found in AST - compile on-the-fly (shouldn't happen if metadata compiled correctly)
                ASTNode* method_body = stmt->data.function_definition.body;
                if (method_body && interpreter && interpreter->bytecode_program_cache) {
                    BytecodeProgram* program = (BytecodeProgram*)interpreter->bytecode_program_cache;
                    int func_id = bc_compile_ast_to_subprogram(program, method_body, method_name);
                    if (func_id >= 0 && func_id < (int)program->function_count) {
                        BytecodeFunction* bc_func = &program->functions[func_id];
                        bc_func->param_count = stmt->data.function_definition.parameter_count;
                        if (stmt->data.function_definition.parameter_count > 0 && stmt->data.function_definition.parameters) {
                            bc_func->param_names = (char**)shared_malloc_safe(
                                stmt->data.function_definition.parameter_count * sizeof(char*),
                                "value_functions", "find_method_in_inheritance_chain", 1
                            );
                            if (bc_func->param_names) {
                                for (size_t j = 0; j < stmt->data.function_definition.parameter_count; j++) {
                                    if (stmt->data.function_definition.parameters[j] &&
                                        stmt->data.function_definition.parameters[j]->data.identifier_value) {
                                        bc_func->param_names[j] = shared_strdup(stmt->data.function_definition.parameters[j]->data.identifier_value);
                                    } else {
                                        bc_func->param_names[j] = shared_strdup("");
                                    }
                                }
                            }
                        }
                return value_create_function(
                            (ASTNode*)(uintptr_t)func_id,
                            NULL,
                    stmt->data.function_definition.parameter_count,
                    stmt->data.function_definition.return_type,
                    class_value->data.class_value.class_environment
                );
                    }
                }
            }
        }
    }
    
    // If not found in current class, search in parent class
    if (class_value->data.class_value.parent_class_name) {
        Value parent_class = environment_get(interpreter->global_environment, class_value->data.class_value.parent_class_name);
        if (parent_class.type == VALUE_CLASS) {
            Value result = find_method_in_inheritance_chain(interpreter, &parent_class, method_name);
            value_free(&parent_class);
            return result;
        }
    }
    
    return value_create_null();
}

// ============================================================================
// CLASS VALUE CREATION FUNCTIONS
// ============================================================================

Value value_create_class(const char* name, const char* parent_name, ASTNode* class_body, Environment* class_env) {
    Value v = {0};
    v.type = VALUE_CLASS;
    v.data.class_value.class_name = name ? shared_strdup(name) : NULL;
    v.data.class_value.parent_class_name = parent_name ? shared_strdup(parent_name) : NULL;
    // Store the class body directly without cloning
    v.data.class_value.class_body = class_body;
    v.data.class_value.class_environment = class_env;
    v.data.class_value.metadata = NULL;  // Will be compiled on demand
    return v;
}

// Compile class body to bytecode metadata
void compile_class_metadata(Interpreter* interpreter, Value* class_value) {
    if (!interpreter || !class_value || class_value->type != VALUE_CLASS) {
        return;
    }
    
    // Already compiled
    if (class_value->data.class_value.metadata) {
        return;
    }
    
    // Need bytecode program cache
    if (!interpreter->bytecode_program_cache) {
        return;
    }
    
    ASTNode* class_body = class_value->data.class_value.class_body;
    if (!class_body || class_body->type != AST_NODE_BLOCK) {
        return;
    }
    
    BytecodeProgram* program = (BytecodeProgram*)interpreter->bytecode_program_cache;
    
    // Allocate metadata structure
    ClassMetadata* metadata = (ClassMetadata*)shared_malloc_safe(sizeof(ClassMetadata), "value_functions", "compile_class_metadata", 1);
    
    if (!metadata) {
        return;
    }
    
    metadata->field_count = 0;
    metadata->method_count = 0;
    metadata->fields = NULL;
    metadata->methods = NULL;
    
    // Allocate arrays
    size_t field_capacity = 8;
    size_t method_capacity = 8;
    metadata->fields = (ClassFieldMetadata*)shared_malloc_safe(field_capacity * sizeof(ClassFieldMetadata), "value_functions", "compile_class_metadata", 2);
    metadata->methods = (ClassMethodMetadata*)shared_malloc_safe(method_capacity * sizeof(ClassMethodMetadata), "value_functions", "compile_class_metadata", 3);
    
    if (!metadata->fields || !metadata->methods) {
        if (metadata->fields) shared_free_safe(metadata->fields, "value_functions", "compile_class_metadata", 4);
        if (metadata->methods) shared_free_safe(metadata->methods, "value_functions", "compile_class_metadata", 5);
        shared_free_safe(metadata, "value_functions", "compile_class_metadata", 6);
        return;
    }
    
    // Parse class body
    for (size_t i = 0; i < class_body->data.block.statement_count; i++) {
        ASTNode* stmt = class_body->data.block.statements[i];
        if (!stmt) continue;
        
        if (stmt->type == AST_NODE_VARIABLE_DECLARATION) {
            // Field declaration
            if (metadata->field_count >= field_capacity) {
                field_capacity *= 2;
                metadata->fields = (ClassFieldMetadata*)shared_realloc_safe(metadata->fields, field_capacity * sizeof(ClassFieldMetadata), "value_functions", "compile_class_metadata", 7);
                if (!metadata->fields) break;
            }
            
            metadata->fields[metadata->field_count].name = stmt->data.variable_declaration.variable_name;
            metadata->fields[metadata->field_count].type = stmt->data.variable_declaration.type_name;
            metadata->fields[metadata->field_count].default_value = stmt->data.variable_declaration.initial_value;
            metadata->field_count++;
        } else if (stmt->type == AST_NODE_FUNCTION && stmt->data.function_definition.function_name) {
            // Method definition - compile to bytecode
            if (metadata->method_count >= method_capacity) {
                method_capacity *= 2;
                metadata->methods = (ClassMethodMetadata*)shared_realloc_safe(metadata->methods, method_capacity * sizeof(ClassMethodMetadata), "value_functions", "compile_class_metadata", 8);
                if (!metadata->methods) break;
            }
            
            ASTNode* method_body = stmt->data.function_definition.body;
            int func_id = bc_compile_ast_to_subprogram(program, method_body, stmt->data.function_definition.function_name);
            
            if (func_id >= 0 && func_id < (int)program->function_count) {
                BytecodeFunction* bc_func = &program->functions[func_id];
                bc_func->param_count = stmt->data.function_definition.parameter_count;
                
                // Store parameter names
                const char** param_names = NULL;
                if (stmt->data.function_definition.parameter_count > 0 && stmt->data.function_definition.parameters) {
                    param_names = (const char**)shared_malloc_safe(stmt->data.function_definition.parameter_count * sizeof(const char*), "value_functions", "compile_class_metadata", 9);
                    if (param_names) {
                        for (size_t j = 0; j < stmt->data.function_definition.parameter_count; j++) {
                            if (stmt->data.function_definition.parameters[j] &&
                                stmt->data.function_definition.parameters[j]->data.identifier_value) {
                                param_names[j] = shared_strdup(stmt->data.function_definition.parameters[j]->data.identifier_value);
                            } else {
                                param_names[j] = shared_strdup("");
                            }
                        }
                    }
                    bc_func->param_names = (char**)param_names;
                }
                
                metadata->methods[metadata->method_count].name = shared_strdup(stmt->data.function_definition.function_name);
                metadata->methods[metadata->method_count].bytecode_func_id = func_id;
                metadata->methods[metadata->method_count].param_names = param_names;
                metadata->methods[metadata->method_count].param_count = stmt->data.function_definition.parameter_count;
                metadata->methods[metadata->method_count].return_type = stmt->data.function_definition.return_type ? shared_strdup(stmt->data.function_definition.return_type) : NULL;
                metadata->method_count++;
            }
        }
    }
    
    // Store metadata in class value
    class_value->data.class_value.metadata = (void*)metadata;
}

// Helper function to collect all fields from the inheritance chain
void collect_inherited_fields(Interpreter* interpreter, Value* class_value, ASTNode*** all_fields, size_t* field_count, size_t* field_capacity) {
    if (!class_value || class_value->type != VALUE_CLASS) {
        return;
    }
    
    // First, collect fields from parent class (if any)
    if (class_value->data.class_value.parent_class_name) {
        Value parent_class = environment_get(interpreter->global_environment, class_value->data.class_value.parent_class_name);
        if (parent_class.type == VALUE_CLASS) {
            collect_inherited_fields(interpreter, &parent_class, all_fields, field_count, field_capacity);
            value_free(&parent_class);
        }
    }
    
    // Then, collect fields from current class
    ASTNode* class_body = class_value->data.class_value.class_body;
    if (class_body && class_body->type == AST_NODE_BLOCK) {
        for (size_t i = 0; i < class_body->data.block.statement_count; i++) {
            ASTNode* stmt = class_body->data.block.statements[i];
            if (stmt && stmt->type == AST_NODE_VARIABLE_DECLARATION) {
                // Resize array if needed
                if (*field_count >= *field_capacity) {
                    *field_capacity = *field_capacity == 0 ? 4 : *field_capacity * 2;
                    *all_fields = (ASTNode**)shared_realloc_safe(*all_fields, *field_capacity * sizeof(ASTNode*), "interpreter", "collect_inherited_fields", 0);
                    if (!*all_fields) {
                        *field_capacity = 0;
                        *field_count = 0;
                        return;
                    }
                }
                
                (*all_fields)[*field_count] = stmt;
                (*field_count)++;
            }
        }
    }
}

// ============================================================================
// MODULE VALUE CREATION FUNCTIONS
// ============================================================================

Value value_create_module(const char* name, void* exports) {
    Value v = {0};
    v.type = VALUE_MODULE;
    v.data.module_value.module_name = name ? shared_strdup(name) : NULL;
    v.data.module_value.module_environment = (Environment*)exports;
    v.data.module_value.exports = (Environment*)exports;
    v.data.module_value.is_loaded = 1;
    return v;
}

// ============================================================================
// CLASS INSTANTIATION FUNCTIONS
// ============================================================================

static Value create_class_instance_from_args(Interpreter* interpreter, Value* class_value, Value* args, size_t arg_count) {
    if (!class_value || class_value->type != VALUE_CLASS) {
        return value_create_null();
    }
    
    // Create an instance object to store instance variables
    Value instance = value_create_object(16);
    
    // Store the class name as a string for method lookup (safer than storing the full class)
    Value class_name_value = value_create_string(class_value->data.class_value.class_name);
    value_object_set(&instance, "__class_name__", class_name_value);
    value_free(&class_name_value);
    
    // Collect all fields from the inheritance chain
    ASTNode** all_fields = NULL;
    size_t field_count = 0;
    size_t field_capacity = 0;
    
    collect_inherited_fields(interpreter, class_value, &all_fields, &field_count, &field_capacity);
    
    // Initialize fields from constructor arguments
    size_t field_index = 0;
    for (size_t i = 0; i < field_count; i++) {
        ASTNode* stmt = all_fields[i];
        if (stmt && stmt->type == AST_NODE_VARIABLE_DECLARATION) {
            const char* field_name = stmt->data.variable_declaration.variable_name;
            if (field_name) {
                Value field_value;
                if (field_index < arg_count) {
                    // Use constructor argument
                    field_value = value_clone(&args[field_index]);
                    field_index++;
                } else {
                    // Use default value from field declaration
                    if (stmt->data.variable_declaration.initial_value) {
                        field_value = interpreter_execute(interpreter, stmt->data.variable_declaration.initial_value);
                    } else {
                        field_value = value_create_null();
                    }
                }
                
                value_object_set(&instance, field_name, field_value);
                value_free(&field_value);
            }
        }
    }
    
    // Clean up
    if (all_fields) {
        shared_free_safe(all_fields, "interpreter", "create_class_instance_from_args", 0);
    }
    
    return instance;
}

Value create_class_instance(Interpreter* interpreter, Value* class_value, ASTNode* call_node) {
    if (!class_value || class_value->type != VALUE_CLASS) {
        interpreter_set_error(interpreter, "Invalid class value", call_node->line, call_node->column);
        return value_create_null();
    }
    
    // Evaluate constructor arguments here to ensure correctness
    size_t arg_count = call_node->data.function_call.argument_count;
    Value* args = arg_count > 0 ? (Value*)calloc(arg_count, sizeof(Value)) : NULL;
    if (arg_count > 0 && !args) {
        return value_create_null();
    }
    
    for (size_t i = 0; i < arg_count; i++) {
        args[i] = interpreter_execute(interpreter, call_node->data.function_call.arguments[i]);
    }
    
    Value instance = create_class_instance_from_args(interpreter, class_value, args, arg_count);
        if (args) {
            for (size_t i = 0; i < arg_count; i++) {
                value_free(&args[i]);
            }
        shared_free_safe(args, "interpreter", "create_class_instance", 0);
        }
    return instance;
}

// ============================================================================
// FUNCTION CALL FUNCTIONS
// ============================================================================

Value value_function_call(Value* func, Value* args, size_t arg_count, Interpreter* interpreter, int line, int column) {
    return value_function_call_with_self(func, args, arg_count, interpreter, NULL, line, column);
}

Value value_function_call_with_self(Value* func, Value* args, size_t arg_count, Interpreter* interpreter, Value* self, int line, int column) {
    if (!func || func->type != VALUE_FUNCTION) {
        return value_create_null();
    }
    
    // Check if this is a built-in function
    // Built-in functions are marked with VALUE_FLAG_CACHED and have function pointer in body field
    if ((func->flags & VALUE_FLAG_CACHED) &&
        func->data.function_value.body && 
        func->data.function_value.parameters == NULL && 
        func->data.function_value.parameter_count == 0) {
        // This is a built-in function - the body field contains the function pointer
        Value (*builtin_func)(Interpreter*, Value*, size_t, int, int) = 
            (Value (*)(Interpreter*, Value*, size_t, int, int))func->data.function_value.body;
        
        // Set self_context if self is provided (needed for library methods like stacks.push)
        if (self) {
            interpreter_set_self_context(interpreter, self);
        }
        
        // Push a frame for built-in call (Python-like traceback)
        interpreter_push_call_frame(interpreter, "<builtin>", interpreter->current_file ? interpreter->current_file : "<stdin>", line, column);
        Value result = builtin_func(interpreter, args, arg_count, line, column);
        interpreter_pop_call_frame(interpreter);
        
        // Clear self_context if it was set
        if (self) {
            interpreter_set_self_context(interpreter, NULL);
        }
        
        return result;
    }
    
    // Check if this is a bytecode function (function ID stored in body field)
    // Bytecode function IDs are small integers (< 10000) cast as pointers
    // Note: function ID 0 is valid (NULL pointer), so we check body_addr first, not body pointer
    ASTNode* body_ptr = (ASTNode*)func->data.function_value.body;
    uintptr_t body_addr = (uintptr_t)body_ptr;
    int is_bytecode_function = (body_addr < 10000);
    
    // If body is NULL and not a function ID, return null
    if (!body_ptr && !is_bytecode_function) {
        return value_create_null();
    }
    
    if (is_bytecode_function) {
        // This is a bytecode function - execute it directly
        int func_id = (int)body_addr;
        
        // Get the bytecode program from interpreter cache
        if (!interpreter || !interpreter->bytecode_program_cache) {
            interpreter_set_error(interpreter, "Bytecode program not available for function call", line, column);
            return value_create_null();
        }
        
        BytecodeProgram* program = (BytecodeProgram*)interpreter->bytecode_program_cache;
        if (func_id < 0 || func_id >= (int)program->function_count) {
            interpreter_set_error(interpreter, "Invalid bytecode function ID", line, column);
            return value_create_null();
        }
        
        BytecodeFunction* bc_func = &program->functions[func_id];
        
        // Create new environment for function execution
        Environment* captured_env = func->data.function_value.captured_environment;
        Environment* func_env = environment_create(captured_env ? captured_env : interpreter->current_environment);
        
        if (!func_env) {
            interpreter_set_error(interpreter, "Failed to create function environment", line, column);
            return value_create_null();
        }
        
        // If this is a method call, add 'self' to the environment
        if (self) {
            environment_define(func_env, "self", *self);
            interpreter_set_self_context(interpreter, self);
        }
        
        // Bind parameters to arguments
        size_t param_count = bc_func->param_count;
        for (size_t i = 0; i < param_count && i < arg_count; i++) {
            if (bc_func->param_names && bc_func->param_names[i]) {
                const char* param_name = bc_func->param_names[i];
                environment_define(func_env, param_name, args[i]);
            }
        }
        
        // Save current environment and set function environment
        Environment* old_env = interpreter->current_environment;
        interpreter->current_environment = func_env;
        
        // Execute bytecode function
        // Note: args need to be cloned since bytecode_execute_function_bytecode will free them
        Value* cloned_args = NULL;
        if (arg_count > 0) {
            cloned_args = (Value*)shared_malloc_safe(arg_count * sizeof(Value), "value_functions", "value_function_call", 1);
            if (cloned_args) {
                for (size_t i = 0; i < arg_count; i++) {
                    cloned_args[i] = value_clone(&args[i]);
                }
            }
        }
        Value result = bytecode_execute_function_bytecode(interpreter, bc_func, cloned_args, (int)arg_count, program);
        if (cloned_args) {
            shared_free_safe(cloned_args, "value_functions", "value_function_call", 2);
        }
        
        // Check if the function returned a value
        if (interpreter->has_return) {
            result = interpreter->return_value;
            interpreter->has_return = 0;
        }
        
        // Restore environment
        interpreter->current_environment = old_env;
        
        // Clear self context if it was set
        if (self) {
            interpreter_set_self_context(interpreter, NULL);
        }
        
        // Clean up function environment
        environment_free(func_env);
        
        return result;
    }
    
    // This is an AST function - check if body is valid AST node
    ASTNode* body_node = (ASTNode*)func->data.function_value.body;
    if (!body_node || body_node->type < 1 || body_node->type > 100) {
        // This looks like a function pointer or invalid node, not an AST node
        // This shouldn't happen for user-defined functions
        return value_create_null();
    }
    
    // Create new environment for function execution using the function's captured environment
    Environment* captured_env = func->data.function_value.captured_environment;
    Environment* func_env = environment_create(captured_env ? captured_env : interpreter->current_environment);
    
    if (!func_env) {
        interpreter_set_error(interpreter, "Failed to create function environment", line, column);
        return value_create_null();
    }
    
    // If this is a method call, add 'self' to the environment and set self context
    if (self) {
        environment_define(func_env, "self", *self);
        interpreter_set_self_context(interpreter, self);
    }
    
    // Bind parameters to arguments
    size_t param_count = func->data.function_value.parameter_count;
    for (size_t i = 0; i < param_count && i < arg_count; i++) {
        if (func->data.function_value.parameters[i]) {
            const char* param_name = NULL;
            if (func->data.function_value.parameters[i]->type == AST_NODE_IDENTIFIER) {
                param_name = func->data.function_value.parameters[i]->data.identifier_value;
            } else if (func->data.function_value.parameters[i]->type == AST_NODE_TYPED_PARAMETER) {
                param_name = func->data.function_value.parameters[i]->data.typed_parameter.parameter_name;
            }
            if (param_name) {
                environment_define(func_env, param_name, args[i]);
            }
        }
    }
    
    // Save current environment and set function environment
    Environment* old_env = interpreter->current_environment;
    interpreter->current_environment = func_env;
    
    // Execute function body
    Value result = interpreter_execute(interpreter, func->data.function_value.body);
    
    // Check if the function returned a value
    if (interpreter->has_return) {
        result = interpreter->return_value;
        interpreter->has_return = 0; // Reset return flag
    }
    
    // Restore environment
    interpreter->current_environment = old_env;
    
    // Clear self context if it was set
    if (self) {
        interpreter_set_self_context(interpreter, NULL);
    }
    
    // Clean up function environment
    environment_free(func_env);
    
    return result;
}
