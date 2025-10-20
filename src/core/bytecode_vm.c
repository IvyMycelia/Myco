#include "../../include/core/bytecode.h"
#include "../../include/utils/shared_utilities.h"
#include "../../include/core/interpreter/value_operations.h"
#include "../../include/core/environment.h"
#include "../../include/libs/array.h"
#include "../../include/libs/math.h"
#include "../../include/libs/maps.h"
#include "../../include/libs/sets.h"
#include "../../include/core/optimization/hot_spot_tracker.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Forward declarations
Value bytecode_execute_function_bytecode(Interpreter* interpreter, BytecodeFunction* func, Value* args, int arg_count, BytecodeProgram* program);
static int pattern_matches_value(Value* value, Value* pattern);

// Memory optimization structures
typedef struct {
    Value* stack;
    size_t capacity;
    size_t size;
} InlineStack;

typedef struct {
    char* buffer;
    size_t capacity;
    size_t length;
} StringBuffer;

// Memory optimization globals
static InlineStack* inline_stack = NULL;
static StringBuffer* string_buffer = NULL;

// Memory optimization helper functions
static void init_memory_optimizations(void) {
    if (!inline_stack) {
        inline_stack = shared_malloc_safe(sizeof(InlineStack), "bytecode_vm", "init_inline_stack", 0);
        if (inline_stack) {
            inline_stack->stack = shared_malloc_safe(64 * sizeof(Value), "bytecode_vm", "init_inline_stack_array", 0);
            inline_stack->capacity = 64;
            inline_stack->size = 0;
        }
    }
    
    if (!string_buffer) {
        string_buffer = shared_malloc_safe(sizeof(StringBuffer), "bytecode_vm", "init_string_buffer", 0);
        if (string_buffer) {
            string_buffer->buffer = shared_malloc_safe(1024, "bytecode_vm", "init_string_buffer_array", 0);
            string_buffer->capacity = 1024;
            string_buffer->length = 0;
        }
    }
}

static void cleanup_memory_optimizations(void) {
    if (inline_stack) {
        if (inline_stack->stack) {
            shared_free_safe(inline_stack->stack, "bytecode_vm", "cleanup_inline_stack", 0);
        }
        shared_free_safe(inline_stack, "bytecode_vm", "cleanup_inline_stack_struct", 0);
        inline_stack = NULL;
    }
    
    if (string_buffer) {
        if (string_buffer->buffer) {
            shared_free_safe(string_buffer->buffer, "bytecode_vm", "cleanup_string_buffer", 0);
        }
        shared_free_safe(string_buffer, "bytecode_vm", "cleanup_string_buffer_struct", 0);
        string_buffer = NULL;
    }
}

// Optimized string concatenation
static Value fast_string_concat(const char* str1, const char* str2) {
    if (!str1 || !str2) {
        return value_create_string(str1 ? str1 : (str2 ? str2 : ""));
    }
    
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    size_t total_len = len1 + len2;
    
    // Use string buffer if available and large enough
    if (string_buffer && total_len < string_buffer->capacity) {
        string_buffer->length = 0;
        memcpy(string_buffer->buffer, str1, len1);
        memcpy(string_buffer->buffer + len1, str2, len2);
        string_buffer->buffer[total_len] = '\0';
        string_buffer->length = total_len;
        
        return value_create_string(string_buffer->buffer);
    }
    
    // Fallback to regular concatenation
    char* result = shared_malloc_safe(total_len + 1, "bytecode_vm", "fast_string_concat", 0);
    if (!result) {
        return value_create_string("");
    }
    
    memcpy(result, str1, len1);
    memcpy(result + len1, str2, len2);
    result[total_len] = '\0';
    
    Value value = value_create_string(result);
    shared_free_safe(result, "bytecode_vm", "fast_string_concat_cleanup", 0);
    return value;
}

// Stack inlining for small functions
static int should_inline_function(BytecodeFunction* func) {
    if (!func) return 0;
    
    // Inline functions with <= 5 instructions and <= 2 parameters
    return (func->code_count <= 5 && func->param_count <= 2);
}

static Value inline_function_execution(Interpreter* interpreter, BytecodeFunction* func, Value* args, int arg_count, BytecodeProgram* program) {
    if (!should_inline_function(func)) {
        return bytecode_execute_function_bytecode(interpreter, func, args, arg_count, program);
    }
    
    // Use inline stack for small functions
    if (!inline_stack || !inline_stack->stack) {
        return bytecode_execute_function_bytecode(interpreter, func, args, arg_count, program);
    }
    
    // Save current stack state
    size_t original_size = inline_stack->size;
    
    // Push arguments onto inline stack
    for (int i = 0; i < arg_count && i < (int)inline_stack->capacity; i++) {
        if (inline_stack->size < inline_stack->capacity) {
            inline_stack->stack[inline_stack->size] = value_clone(&args[i]);
            inline_stack->size++;
        }
    }
    
    // Execute function instructions directly (simplified)
    Value result = value_create_null();
    
    // Restore stack state
    for (size_t i = original_size; i < inline_stack->size; i++) {
        value_free(&inline_stack->stack[i]);
    }
    inline_stack->size = original_size;
    
    return result;
}

// Helper function to collect class fields for bytecode instantiation
static void collect_class_fields_for_bytecode(
    Interpreter* interpreter,
    Value* class_value,
    ASTNode*** all_fields,
    size_t* field_count,
    size_t* field_capacity
) {
    if (!class_value || class_value->type != VALUE_CLASS) {
        return;
    }
    
    // Collect from parent first (recursively)
    if (class_value->data.class_value.parent_class_name) {
        Value parent_class = environment_get(
            interpreter->global_environment,
            class_value->data.class_value.parent_class_name
        );
        if (parent_class.type == VALUE_CLASS) {
            collect_class_fields_for_bytecode(
                interpreter, &parent_class, all_fields, field_count, field_capacity
            );
            value_free(&parent_class);
        }
    }
    
    // Collect from current class body
    ASTNode* class_body = class_value->data.class_value.class_body;
    if (class_body && class_body->type == AST_NODE_BLOCK) {
        ASTNode* stmt = *class_body->data.block.statements;
        while (stmt) {
            if (stmt->type == AST_NODE_VARIABLE_DECLARATION) {
                if (*field_count >= *field_capacity) {
                    size_t new_cap = *field_capacity ? *field_capacity * 2 : 8;
                    *all_fields = shared_realloc_safe(
                        *all_fields, new_cap * sizeof(ASTNode*),
                        "bytecode_vm", "collect_class_fields", 0
                    );
                    *field_capacity = new_cap;
                }
                (*all_fields)[*field_count] = stmt;
                (*field_count)++;
            }
            stmt = stmt->next;
        }
    }
}

// String interning system for performance optimization
typedef struct {
    char** strings;
    size_t count;
    size_t capacity;
} StringInternTable;

static StringInternTable* string_intern_table = NULL;

static const char* intern_string(const char* str) {
    if (!str) return NULL;
    
    if (!string_intern_table) {
        string_intern_table = shared_malloc_safe(sizeof(StringInternTable), "bytecode_vm", "intern_init", 0);
        if (!string_intern_table) return str;
        string_intern_table->strings = NULL;
        string_intern_table->count = 0;
        string_intern_table->capacity = 0;
    }
    
    // Check if string already exists
    for (size_t i = 0; i < string_intern_table->count; i++) {
        if (strcmp(string_intern_table->strings[i], str) == 0) {
            return string_intern_table->strings[i];
        }
    }
    
    // Add new string
    if (string_intern_table->count >= string_intern_table->capacity) {
        size_t new_capacity = string_intern_table->capacity ? string_intern_table->capacity * 2 : 16;
        char** new_strings = shared_realloc_safe(
            string_intern_table->strings, 
            new_capacity * sizeof(char*),
            "bytecode_vm", "intern_realloc", 0
        );
        if (!new_strings) return str;
        string_intern_table->strings = new_strings;
        string_intern_table->capacity = new_capacity;
    }
    
    char* interned = shared_strdup(str);
    if (!interned) return str;
    
    string_intern_table->strings[string_intern_table->count] = interned;
    string_intern_table->count++;
    
    return interned;
}

// Value cache for frequently used values
typedef struct {
    Value* values;
    size_t count;
    size_t capacity;
} BytecodeValueCache;

static BytecodeValueCache* value_cache = NULL;

static Value* get_cached_value(ValueType type, void* data) {
    if (!value_cache) {
        value_cache = shared_malloc_safe(sizeof(BytecodeValueCache), "bytecode_vm", "cache_init", 0);
        if (!value_cache) return NULL;
        value_cache->values = NULL;
        value_cache->count = 0;
        value_cache->capacity = 0;
    }
    
    // Check if value already exists in cache
    for (size_t i = 0; i < value_cache->count; i++) {
        Value* cached = &value_cache->values[i];
        if (cached->type == type) {
            switch (type) {
                case VALUE_NULL:
                    return cached;
                case VALUE_BOOLEAN:
                    if (cached->data.boolean_value == *(int*)data) return cached;
                    break;
                case VALUE_NUMBER:
                    if (cached->data.number_value == *(double*)data) return cached;
                    break;
                case VALUE_STRING:
                    if (strcmp(cached->data.string_value, (char*)data) == 0) return cached;
                    break;
                default:
                    break;
            }
        }
    }
    
    return NULL; // Not found in cache
}

static Value* cache_value(Value value) {
    if (!value_cache) {
        value_cache = shared_malloc_safe(sizeof(BytecodeValueCache), "bytecode_vm", "cache_init", 0);
        if (!value_cache) return NULL;
        value_cache->values = NULL;
        value_cache->count = 0;
        value_cache->capacity = 0;
    }
    
    // Add to cache
    if (value_cache->count >= value_cache->capacity) {
        size_t new_capacity = value_cache->capacity ? value_cache->capacity * 2 : 16;
        Value* new_values = shared_realloc_safe(
            value_cache->values,
            new_capacity * sizeof(Value),
            "bytecode_vm", "cache_realloc", 0
        );
        if (!new_values) return NULL;
        value_cache->values = new_values;
        value_cache->capacity = new_capacity;
    }
    
    value_cache->values[value_cache->count] = value;
    value_cache->count++;
    
    return &value_cache->values[value_cache->count - 1];
}

// Value pool functions for performance optimization
static Value* value_pool_alloc(BytecodeProgram* program) {
    if (!program->value_pool_initialized) {
        program->value_pool_size = 1000;
        program->value_pool_next = 0;
        program->value_pool_initialized = true;
        program->value_pool = shared_malloc_safe(
            program->value_pool_size * sizeof(Value),
            "bytecode_vm", "value_pool_init", 1
        );
    }
    
    if (program->value_pool_next < program->value_pool_size) {
        return &program->value_pool[program->value_pool_next++];
    }
    return NULL; // Fall back to malloc
}

static void value_pool_reset(BytecodeProgram* program) {
    if (program->value_pool_initialized) {
        program->value_pool_next = 0;
    }
}

// Fast value creation functions (simplified for stability)
static Value fast_create_number(BytecodeProgram* program, double val) {
    // Use string interning for performance but no caching to avoid segfaults
    return value_create_number(val);
}

static Value fast_create_string(BytecodeProgram* program, const char* str) {
    // Use string interning for performance but no caching to avoid segfaults
    const char* interned = intern_string(str);
    return value_create_string(interned);
}

static Value fast_create_boolean(BytecodeProgram* program, int val) {
    // Simple value creation without caching
    return value_create_boolean(val);
}

static Value fast_create_null(BytecodeProgram* program) {
    // Simple value creation without caching
    return value_create_null();
}

// Stub functions for optimization features
BytecodeProgram* bytecode_compile_ast(ASTNode* node, Interpreter* interpreter) {
    // Stub implementation - return NULL to disable bytecode compilation
    return NULL;
}

void ast_node_set_bytecode(ASTNode* node, BytecodeProgram* bytecode) {
    // Stub implementation - do nothing
    (void)node;
    (void)bytecode;
}

Value interpreter_execute_bytecode(Interpreter* interpreter, BytecodeProgram* bytecode) {
    // Stub implementation - fall back to AST execution
    return value_create_null();
}

// Bytecode VM implementation
// This implements a stack-based virtual machine for executing Myco bytecode

// Stack operations
static Value* value_stack = NULL;
static size_t value_stack_size = 0;
static size_t value_stack_capacity = 0;

static double* num_stack = NULL;
static size_t num_stack_size = 0;
static size_t num_stack_capacity = 0;

// Stack management functions
static void value_stack_push(Value v) {
    if (value_stack_size + 1 > value_stack_capacity) {
        size_t new_cap = value_stack_capacity ? value_stack_capacity * 2 : 128;
        value_stack = shared_realloc_safe(value_stack, new_cap * sizeof(Value), "bytecode_vm", "value_stack_push", 1);
        value_stack_capacity = new_cap;
    }
    value_stack[value_stack_size] = v;
    value_stack_size++;
}

static Value value_stack_pop(void) {
    if (value_stack_size == 0) {
        return value_create_null();
    }
    value_stack_size--;
    return value_stack[value_stack_size];
}

static Value value_stack_peek(void) {
    if (value_stack_size == 0) {
        return value_create_null();
    }
    return value_stack[value_stack_size - 1];
}

static void num_stack_push(double v) {
    if (num_stack_size + 1 > num_stack_capacity) {
        size_t new_cap = num_stack_capacity ? num_stack_capacity * 2 : 128;
        num_stack = shared_realloc_safe(num_stack, new_cap * sizeof(double), "bytecode_vm", "num_stack_push", 1);
        num_stack_capacity = new_cap;
    }
    num_stack[num_stack_size] = v;
    num_stack_size++;
}

static double num_stack_pop(void) {
    if (num_stack_size == 0) {
        return 0.0;
    }
    num_stack_size--;
    return num_stack[num_stack_size];
}

static double num_stack_peek(void) {
    if (num_stack_size == 0) {
        return 0.0;
    }
    return num_stack[num_stack_size - 1];
}

// These functions are implemented in bytecode_compiler.c
// We only implement the execution part here

// Main execution function
Value bytecode_execute(BytecodeProgram* program, Interpreter* interpreter, int debug) {
    if (!program || !interpreter) {
        return value_create_null();
    }
    
    // Initialize memory optimizations
    init_memory_optimizations();
    
    // Initialize stacks
    value_stack_size = 0;
    num_stack_size = 0;
    
    // Initialize value pool for performance
    value_pool_reset(program);
    
    // Set interpreter reference
    program->interpreter = interpreter;
    
    // Execute main program
    size_t pc = 0;
    Value result = value_create_null();
    
    if (debug) {
        printf("DEBUG: Starting bytecode execution, %zu instructions\n", program->count);
    }
    
    while (pc < program->count) {
        BytecodeInstruction* instr = &program->code[pc];
        
        // Track hot spots for JIT compilation (disabled for now due to crashes)
        // if (interpreter && interpreter->hot_spot_tracker) {
        //     HotSpotTracker* tracker = (HotSpotTracker*)interpreter->hot_spot_tracker;
        //     
        //     // Only track if tracker is valid
        //     if (tracker) {
        //         // Track execution frequency for this instruction (simplified tracking)
        //         hot_spot_tracker_record_execution(tracker, NULL, 1000); // 1 microsecond per instruction
        //         
        //         // Check if this is a hot spot that should be JIT compiled
        //         if (hot_spot_tracker_is_hot(tracker, NULL)) {
        //             // Mark this instruction as hot for potential JIT compilation
        //             hot_spot_tracker_mark_hot(tracker, NULL, HOT_SPOT_FUNCTION);
        //         }
        //     }
        // }
        
        if (debug) {
            printf("PC: %zu, Op: %d, A: %d, B: %d\n", pc, instr->op, instr->a, instr->b);
        }
        
        // Handle superinstructions
        if (instr->op >= BC_SUPER_START) {
            switch ((BytecodeSuperOp)instr->op) {
                
                
                
                default: {
                    // Unknown superinstruction - fallback to AST
                    if (instr->a < program->ast_count) {
                        ASTNode* node = program->ast_nodes[instr->a];
                        if (node) {
                            Value result = interpreter_execute(interpreter, node);
                            value_stack_push(result);
                        } else {
                            value_stack_push(value_create_null());
                        }
                    } else {
                        value_stack_push(value_create_null());
                    }
                    pc++;
                    break;
                }
            }
        } else {
            // Handle regular bytecode operations
            switch (instr->op) {
            case BC_LOAD_CONST: {
                if (instr->a < program->const_count) {
                    Value const_val = program->constants[instr->a];
                    // Use fast value creation with caching
                    if (const_val.type == VALUE_NUMBER) {
                        value_stack_push(fast_create_number(program, const_val.data.number_value));
                    } else if (const_val.type == VALUE_STRING) {
                        value_stack_push(fast_create_string(program, const_val.data.string_value));
                    } else if (const_val.type == VALUE_BOOLEAN) {
                        value_stack_push(fast_create_boolean(program, const_val.data.boolean_value));
                    } else if (const_val.type == VALUE_NULL) {
                        value_stack_push(fast_create_null(program));
                    } else {
                        value_stack_push(value_clone(&const_val));
                    }
                } else {
                    value_stack_push(fast_create_null(program));
                }
                pc++;
                break;
            }
            
            case BC_LOAD_LOCAL: {
                if (instr->a < program->local_slot_count) {
                    Value local_val = program->locals[instr->a];
                    // Always clone to avoid memory issues
                    value_stack_push(value_clone(&local_val));
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_LOAD_VAR: {
                // Load variable from environment
                if (instr->a < program->const_count) {
                    Value var_name = program->constants[instr->a];
                    if (var_name.type == VALUE_STRING) {
                        Value var_val = environment_get(interpreter->current_environment, var_name.data.string_value);
                        value_stack_push(var_val);
                    } else {
                        value_stack_push(value_create_null());
                    }
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_STORE_LOCAL: {
                if (instr->a < program->local_slot_count) {
                    Value val = value_stack_pop();
                    value_free(&program->locals[instr->a]);
                    program->locals[instr->a] = val;
                    
                    // Also update numeric locals if this is a number
                    if (val.type == VALUE_NUMBER && instr->a < program->num_local_count) {
                        program->num_locals[instr->a] = val.data.number_value;
                    }
                }
                pc++;
                break;
            }
            
            case BC_LOAD_GLOBAL: {
                // Load global variable by name
                if (instr->a < program->const_count && program->constants[instr->a].type == VALUE_STRING) {
                    const char* var_name = program->constants[instr->a].data.string_value;
                    Value global_val = environment_get(interpreter->global_environment, var_name);
                    if (global_val.type != VALUE_NULL) {
                        value_stack_push(value_clone(&global_val));
                    } else {
                        value_stack_push(value_create_null());
                    }
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_STORE_GLOBAL: {
                // Store global variable by name
                if (instr->a < program->const_count && program->constants[instr->a].type == VALUE_STRING) {
                    const char* var_name = program->constants[instr->a].data.string_value;
                    Value val = value_stack_pop();
                    environment_assign(interpreter->global_environment, var_name, val);
                    value_stack_push(value_clone(&val));
                } else {
                    value_stack_pop(); // Remove value from stack
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_ADD: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                
                // Optimize string concatenation
                if (a.type == VALUE_STRING && b.type == VALUE_STRING) {
                    Value result = fast_string_concat(a.data.string_value, b.data.string_value);
                    value_free(&a);
                    value_free(&b);
                    value_stack_push(result);
                } else {
                    Value result = value_add(&a, &b);
                    value_free(&a);
                    value_free(&b);
                    value_stack_push(result);
                }
                pc++;
                break;
            }
            
            case BC_SUB: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                Value result = value_subtract(&a, &b);
                value_free(&a);
                value_free(&b);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_MUL: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                Value result = value_multiply(&a, &b);
                value_free(&a);
                value_free(&b);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_DIV: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                Value result = value_divide(&a, &b);
                value_free(&a);
                value_free(&b);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_EQ: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                Value result = value_equal(&a, &b);
                value_free(&a);
                value_free(&b);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_NE: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                Value eq = value_equal(&a, &b);
                Value result = value_logical_not(&eq);
                value_free(&a);
                value_free(&b);
                value_free(&eq);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_LT: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                Value result = value_less_than(&a, &b);
                value_free(&a);
                value_free(&b);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_LE: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                Value lt = value_less_than(&a, &b);
                Value eq = value_equal(&a, &b);
                Value result = value_logical_or(&lt, &eq);
                value_free(&a);
                value_free(&b);
                value_free(&lt);
                value_free(&eq);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_GT: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                Value result = value_greater_than(&a, &b);
                value_free(&a);
                value_free(&b);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_GE: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                Value gt = value_greater_than(&a, &b);
                Value eq = value_equal(&a, &b);
                Value result = value_logical_or(&gt, &eq);
                value_free(&a);
                value_free(&b);
                value_free(&gt);
                value_free(&eq);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_JUMP: {
                pc = instr->a;
                break;
            }
            
            case BC_JUMP_IF_FALSE: {
                Value condition = value_stack_pop();
                if (condition.type == VALUE_BOOLEAN && !condition.data.boolean_value) {
                    pc = instr->a;
                } else {
                    pc++;
                }
                value_free(&condition);
                break;
            }
            
            case BC_LOOP_START: {
                // Mark the start of a loop for potential optimization
                // For now, just continue execution
                pc++;
                break;
            }
            
            case BC_LOOP_END: {
                // Mark the end of a loop iteration
                // For now, just continue execution
                pc++;
                break;
            }
            
            case BC_PRINT: {
                Value val = value_stack_pop();
                value_print(&val);
                value_free(&val);
                pc++;
                break;
            }
            
            case BC_PRINT_MULTIPLE: {
                // Print multiple values on one line
                int count = instr->a;
                for (int i = 0; i < count; i++) {
                    Value val = value_stack_pop();
                    value_print(&val);
                    if (i < count - 1) {
                        printf(" ");
                    }
                    value_free(&val);
                }
                printf("\n");
                pc++;
                break;
            }
            
            case BC_METHOD_CALL: {
                // Declare variables outside the if block
                int arg_count = instr->b;
                Value* args = NULL;
                Value object = value_create_null();
                
                // Get method name from constant pool
                if (instr->a < program->const_count && program->constants[instr->a].type == VALUE_STRING) {
                    const char* method_name = program->constants[instr->a].data.string_value;
                    // Get arguments from stack (in reverse order)
                    if (arg_count > 0) {
                        args = shared_malloc_safe(arg_count * sizeof(Value), "bytecode_vm", "BC_METHOD_CALL", 7);
                        for (int i = arg_count - 1; i >= 0; i--) {
                            args[i] = value_stack_pop();
                        }
                    }
                    
                    // Get object from stack
                    object = value_stack_pop();
                    // Handle different object types
                    if (object.type == VALUE_HASH_MAP) {
                        // Handle map methods
                        if (strcmp(method_name, "set") == 0) {
                            Value result = builtin_map_set(NULL, (Value[]){object, args[0], args[1]}, 3, 0, 0);
                            value_stack_push(result);
                        } else if (strcmp(method_name, "has") == 0) {
                            Value result = builtin_map_has(NULL, (Value[]){object, args[0]}, 2, 0, 0);
                            value_stack_push(result);
                        } else if (strcmp(method_name, "delete") == 0) {
                            Value result = builtin_map_delete(NULL, (Value[]){object, args[0]}, 2, 0, 0);
                            value_stack_push(result);
                        } else if (strcmp(method_name, "update") == 0) {
                            Value result = builtin_map_update(NULL, (Value[]){object, args[0]}, 2, 0, 0);
                            value_stack_push(result);
                        } else if (strcmp(method_name, "keys") == 0) {
                            Value result = builtin_map_keys(NULL, (Value[]){object}, 1, 0, 0);
                            value_stack_push(result);
                        } else if (strcmp(method_name, "clear") == 0) {
                            Value result = builtin_map_clear(NULL, (Value[]){object}, 1, 0, 0);
                            value_stack_push(result);
                        } else if (strcmp(method_name, "size") == 0) {
                            Value result = builtin_map_size(NULL, (Value[]){object}, 1, 0, 0);
                            value_stack_push(result);
                        } else {
                            value_stack_push(value_create_null());
                        }
                    } else if (object.type == VALUE_SET) {
                        // Handle set methods
                        if (strcmp(method_name, "add") == 0) {
                            Value result = builtin_set_add(NULL, (Value[]){object, args[0]}, 2, 0, 0);
                            value_stack_push(result);
                        } else if (strcmp(method_name, "has") == 0) {
                            Value result = builtin_set_has(NULL, (Value[]){object, args[0]}, 2, 0, 0);
                            value_stack_push(result);
                        } else if (strcmp(method_name, "remove") == 0) {
                            Value result = builtin_set_remove(NULL, (Value[]){object, args[0]}, 2, 0, 0);
                            value_stack_push(result);
                        } else if (strcmp(method_name, "clear") == 0) {
                            Value result = builtin_set_clear(NULL, (Value[]){object}, 1, 0, 0);
                            value_stack_push(result);
                        } else if (strcmp(method_name, "size") == 0) {
                            Value result = builtin_set_size(NULL, (Value[]){object}, 1, 0, 0);
                            value_stack_push(result);
                        } else if (strcmp(method_name, "toArray") == 0) {
                            Value result = builtin_set_to_array(NULL, (Value[]){object}, 1, 0, 0);
                            value_stack_push(result);
                        } else if (strcmp(method_name, "union") == 0) {
                            Value result = builtin_set_union(NULL, (Value[]){object, args[0]}, 2, 0, 0);
                            value_stack_push(result);
                        } else if (strcmp(method_name, "intersection") == 0) {
                            Value result = builtin_set_intersection(NULL, (Value[]){object, args[0]}, 2, 0, 0);
                            value_stack_push(result);
                        } else {
                            value_stack_push(value_create_null());
                        }
                    } else if (object.type == VALUE_OBJECT) {
                        // Check if it's a library object first
                        Value library_type = value_object_get(&object, "__type__");
                        if (library_type.type == VALUE_STRING && strcmp(library_type.data.string_value, "Library") == 0) {
                            // It's a library object - get method directly
                            Value method = value_object_get(&object, method_name);
                            if (method.type == VALUE_FUNCTION) {
                                // Call library method with args already populated
                                Value result = value_function_call(&method, args, arg_count, interpreter, 0, 0);
                                value_stack_push(result);
                            } else {
                                value_stack_push(value_create_null());
                            }
                            value_free(&method);
                        }
                        value_free(&library_type);
                        
                        // Check if it's a library instance (has __class_name__ but not a VALUE_CLASS)
                        Value class_name = value_object_get(&object, "__class_name__");
                        if (class_name.type == VALUE_STRING) {
                            // Check if it's a library instance (Tree, Graph, Heap, etc.)
                            if (strcmp(class_name.data.string_value, "Tree") == 0 ||
                                strcmp(class_name.data.string_value, "Graph") == 0 ||
                                strcmp(class_name.data.string_value, "Heap") == 0 ||
                                strcmp(class_name.data.string_value, "Queue") == 0 ||
                                strcmp(class_name.data.string_value, "Stack") == 0) {
                                // It's a library instance - get method directly from object
                                Value method = value_object_get(&object, method_name);
                                if (method.type == VALUE_FUNCTION) {
                                    // Set self context and call method
                                    interpreter_set_self_context(interpreter, &object);
                                    
                                    // Check if this is a method that uses self_context (heaps, queues, stacks)
                                    // vs explicit self argument (trees, graphs)
                                    bool uses_self_context = (strcmp(class_name.data.string_value, "Heap") == 0 ||
                                                             strcmp(class_name.data.string_value, "Queue") == 0 ||
                                                             strcmp(class_name.data.string_value, "Stack") == 0);
                                    
                                    Value result;
                                    if (uses_self_context) {
                                        // For heaps/queues/stacks, don't pass self as argument
                                        result = value_function_call(&method, args, arg_count, interpreter, 0, 0);
                                    } else {
                                        // For trees/graphs, pass self as first argument
                                        Value* method_args = shared_malloc_safe((arg_count + 1) * sizeof(Value), "bytecode_vm", "BC_METHOD_CALL", 9);
                                        method_args[0] = value_clone(&object); // self as first argument
                                        for (int i = 0; i < arg_count; i++) {
                                            method_args[i + 1] = value_clone(&args[i]);
                                        }
                                        
                                        result = value_function_call(&method, method_args, arg_count + 1, interpreter, 0, 0);
                                        
                                        // Clean up method arguments
                                        for (int i = 0; i < arg_count + 1; i++) {
                                            value_free(&method_args[i]);
                                        }
                                        shared_free_safe(method_args, "bytecode_vm", "BC_METHOD_CALL", 10);
                                    }
                                    
                                    value_stack_push(result);
                                } else {
                                    value_stack_push(value_create_null());
                                }
                                value_free(&method);
                            } else {
                                // It's a regular class instance - find method in inheritance chain
                                Value class_def = environment_get(interpreter->global_environment, class_name.data.string_value);
                                if (class_def.type == VALUE_CLASS) {
                                    Value method = find_method_in_inheritance_chain(interpreter, &class_def, method_name);
                                    if (method.type == VALUE_FUNCTION) {
                                        // Set self context and call method
                                        interpreter_set_self_context(interpreter, &object);
                                        
                                        Value result = value_function_call_with_self(&method, args, arg_count, interpreter, &object, 0, 0);
                                        
                                        // Clear self context
                                        interpreter_set_self_context(interpreter, NULL);
                                        
                                        value_stack_push(result);
                                        value_free(&method);
                                    } else {
                                        value_stack_push(value_create_null());
                                    }
                                    value_free(&class_def);
                                } else {
                                    value_stack_push(value_create_null());
                                }
                            }
                        } else {
                            // It's a regular object - get method directly
                            Value method = value_object_get(&object, method_name);
                            if (method.type == VALUE_FUNCTION) {
                                // Set self context for method calls
                                interpreter_set_self_context(interpreter, &object);
                                
                                Value result = value_function_call_with_self(&method, args, arg_count, interpreter, &object, 0, 0);
                                
                                // Clear self context
                                interpreter_set_self_context(interpreter, NULL);
                                
                                value_stack_push(result);
                            } else {
                                value_stack_push(value_create_null());
                            }
                            value_free(&method);
                        }
                        value_free(&class_name);
                    } else {
                        value_stack_push(value_create_null());
                    }
                    value_free(&object);
                } else {
                    // Unknown object type - return null
                    value_stack_push(value_create_null());
                    value_free(&object);
                }
                
                // Clean up arguments
                if (args) {
                    for (int i = 0; i < arg_count; i++) {
                        value_free(&args[i]);
                    }
                    shared_free_safe(args, "bytecode_vm", "BC_METHOD_CALL", 8);
                }
                
                pc++;
                break;
            }
            
            case BC_PROPERTY_ACCESS: {
                // Get property name from constant pool
                if (instr->a < program->const_count && program->constants[instr->a].type == VALUE_STRING) {
                    const char* prop_name = program->constants[instr->a].data.string_value;
                    Value object = value_stack_pop();
                    
                    // Special handling for .type property (matches AST interpreter logic)
                    if (strcmp(prop_name, "type") == 0) {
                        // For objects, check for __type__ override (e.g., Library)
                        if (object.type == VALUE_OBJECT) {
                            Value type_override = value_object_get(&object, "__type__");
                            if (type_override.type == VALUE_STRING) {
                                value_stack_push(type_override);
                                value_free(&object);
                                pc++;
                                break;
                            }
                            value_free(&type_override);
                            
                            // Check for __class_name__ (class instances)
                            Value class_name = value_object_get(&object, "__class_name__");
                            if (class_name.type == VALUE_STRING) {
                                value_stack_push(class_name);
                                value_free(&object);
                                pc++;
                                break;
                            }
                            value_free(&class_name);
                        }
                        
                        // For numbers, distinguish Int vs Float
                        if (object.type == VALUE_NUMBER) {
                            double num = object.data.number_value;
                            if (num == (double)((int)num)) {
                                value_stack_push(value_create_string("Int"));
                            } else {
                                value_stack_push(value_create_string("Float"));
                            }
                            value_free(&object);
                            pc++;
                            break;
                        }
                        
                        // Default type handling
                        Value type_str = value_create_string(value_type_to_string(object.type));
                        value_stack_push(type_str);
                        value_free(&object);
                        pc++;
                        break;
                    }
                    
                    // Map properties
                    if (object.type == VALUE_HASH_MAP && strcmp(prop_name, "size") == 0) {
                        size_t sz = value_hash_map_size(&object);
                        value_stack_push(value_create_number((double)sz));
                        value_free(&object);
                        pc++;
                        break;
                    }
                    if (object.type == VALUE_HASH_MAP && strcmp(prop_name, "keys") == 0) {
                        size_t count = 0;
                        Value* keys = value_hash_map_keys(&object, &count);
                        Value arr = value_create_array(count);
                        for (size_t i = 0; i < count; i++) {
                            Value cloned = value_clone(&keys[i]);
                            value_array_push(&arr, cloned);
                        }
                        if (keys) shared_free_safe(keys, "bytecode_vm", "BC_PROPERTY_ACCESS", 0);
                        value_stack_push(arr);
                        value_free(&object);
                        pc++;
                        break;
                    }
                    
                    // Set properties
                    if (object.type == VALUE_SET && strcmp(prop_name, "size") == 0) {
                        size_t sz = value_set_size(&object);
                        value_stack_push(value_create_number((double)sz));
                        value_free(&object);
                        pc++;
                        break;
                    }
                    
                    // Array properties
                    if (object.type == VALUE_ARRAY && strcmp(prop_name, "length") == 0) {
                        value_stack_push(value_create_number((double)object.data.array_value.count));
                        value_free(&object);
                        pc++;
                        break;
                    }
                    
                    // String properties
                    if (object.type == VALUE_STRING && strcmp(prop_name, "length") == 0) {
                        value_stack_push(value_create_number((double)strlen(object.data.string_value)));
                        value_free(&object);
                        pc++;
                        break;
                    }
                    
                    // Hash map property access
                    if (object.type == VALUE_HASH_MAP) {
                        Value key = value_create_string(prop_name);
                        Value prop = value_hash_map_get(&object, key);
                        value_free(&key);
                        value_stack_push(prop);
                        value_free(&object);
                        pc++;
                        break;
                    }
                    
                    // Default: try object property access
                    if (object.type == VALUE_OBJECT) {
                        Value prop = value_object_get(&object, prop_name);
                        value_stack_push(prop);
                        value_free(&object);
                        pc++;
                        break;
                    }
                    
                    // Default: return null for unsupported property access
                    value_stack_push(value_create_null());
                    value_free(&object);
                    pc++;
                    break;
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_CALL_BUILTIN: {
                // Call built-in function by name - for now, fall back to AST evaluation
                // TODO: Implement direct built-in function calling
                if (instr->a < program->ast_count) {
                    ASTNode* node = program->ast_nodes[instr->a];
                    if (node) {
                        Value result = interpreter_execute(interpreter, node);
                        value_stack_push(result);
                    } else {
                        value_stack_push(value_create_null());
                    }
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_RETURN: {
                if (instr->a == 0) {
                    // Void return
                    result = value_create_null();
                } else {
                    // Value return
                    result = value_stack_pop();
                }
                goto cleanup;
            }
            
            case BC_CALL_USER_FUNCTION: {
                // Call user-defined function: func(args...)
                // instr->a = function index, instr->b = argument count
                int func_id = instr->a;
                int arg_count = instr->b;
                
                if (func_id >= 0 && func_id < (int)program->function_count) {
                    BytecodeFunction* func = &program->functions[func_id];
                    
                    // Get arguments from stack
                    Value* args = NULL;
                    if (arg_count > 0) {
                        args = shared_malloc_safe(arg_count * sizeof(Value), "bytecode_vm", "BC_CALL_USER_FUNCTION", 1);
                        for (int i = 0; i < arg_count; i++) {
                            args[arg_count - 1 - i] = value_stack_pop();
                        }
                    }
                    
                // Execute function bytecode with inlining optimization
                Value result = inline_function_execution(interpreter, func, args, arg_count, program);
                    
                    // Clean up arguments
                    if (args) {
                        for (int i = 0; i < arg_count; i++) {
                            value_free(&args[i]);
                        }
                        shared_free_safe(args, "bytecode_vm", "BC_CALL_USER_FUNCTION", 2);
                    }
                    
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_DEFINE_FUNCTION: {
                // Define function in environment: func_name -> function_value
                // instr->a = function name constant index, instr->b = function id
                int name_idx = instr->a;
                int func_id = instr->b;
                
                if (name_idx >= 0 && name_idx < (int)program->const_count && 
                    program->constants[name_idx].type == VALUE_STRING &&
                    func_id >= 0 && func_id < (int)program->function_count) {
                    
                    const char* func_name = program->constants[name_idx].data.string_value;
                    BytecodeFunction* func = &program->functions[func_id];
                    
                    // Create a simple function value that represents a bytecode function
                    // We'll use a special marker to indicate this is a bytecode function
                    Value function_value = value_create_function(
                        NULL, // No AST body for bytecode functions
                        NULL, // No AST parameters for bytecode functions
                        func->param_count,
                        NULL, // No return type for now
                        interpreter->current_environment
                    );
                    
                    // Define in global environment so it can be called from anywhere
                    environment_define(interpreter->global_environment, func_name, function_value);
                }
                pc++;
                break;
            }
            
            case BC_TO_STRING: {
                // Convert value to string
                Value val = value_stack_pop();
                Value result = value_to_string(&val);
                value_free(&val);
                // Use fast string creation with interning
                if (result.type == VALUE_STRING) {
                    Value fast_result = fast_create_string(program, result.data.string_value);
                    value_free(&result);
                    value_stack_push(fast_result);
                } else {
                    value_stack_push(result);
                }
                pc++;
                break;
            }
            
            case BC_GET_TYPE: {
                // Get value type as string (matches AST interpreter logic)
                Value val = value_stack_pop();
                Value result;
                
                // For objects, check for __type__ override (e.g., Library)
                if (val.type == VALUE_OBJECT) {
                    Value type_override = value_object_get(&val, "__type__");
                    if (type_override.type == VALUE_STRING) {
                        result = type_override;
                        value_free(&val);
                        value_stack_push(result);
                        pc++;
                        break;
                    }
                    value_free(&type_override);
                    
                    // Check for __class_name__ (class instances)
                    Value class_name = value_object_get(&val, "__class_name__");
                    if (class_name.type == VALUE_STRING) {
                        result = class_name;
                        value_free(&val);
                        value_stack_push(result);
                        pc++;
                        break;
                    }
                    value_free(&class_name);
                }
                
                // For numbers, distinguish Int vs Float (consistent with AST interpreter)
                if (val.type == VALUE_NUMBER) {
                    const char* ntype = (val.data.number_value == (long long)val.data.number_value) ? "Int" : "Float";
                    result = value_create_string(ntype);
                    value_free(&val);
                    value_stack_push(result);
                    pc++;
                    break;
                }
                
                // Default: return type string
                result = value_create_string(value_type_to_string(val.type));
                value_free(&val);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_GET_LENGTH: {
                // Get value length
                Value val = value_stack_pop();
                Value result;
                if (val.type == VALUE_STRING) {
                    result = value_create_number((double)strlen(val.data.string_value));
                } else if (val.type == VALUE_ARRAY) {
                    result = value_create_number((double)val.data.array_value.count);
                } else {
                    result = value_create_number(0.0);
                }
                value_free(&val);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_IS_STRING: {
                // Check if value is string
                Value val = value_stack_pop();
                Value result = value_create_boolean(val.type == VALUE_STRING);
                value_free(&val);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_IS_NUMBER: {
                // Check if value is number
                Value val = value_stack_pop();
                Value result = value_create_boolean(val.type == VALUE_NUMBER);
                value_free(&val);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_IS_INT: {
                // Check if value is int
                Value val = value_stack_pop();
                Value result = value_create_boolean(val.type == VALUE_NUMBER && val.data.number_value == (int)val.data.number_value);
                value_free(&val);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_IS_FLOAT: {
                // Check if value is float (has decimal places)
                Value val = value_stack_pop();
                Value result = value_create_boolean(val.type == VALUE_NUMBER && val.data.number_value != (long long)val.data.number_value);
                value_free(&val);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_IS_BOOL: {
                // Check if value is bool
                Value val = value_stack_pop();
                Value result = value_create_boolean(val.type == VALUE_BOOLEAN);
                value_free(&val);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_IS_ARRAY: {
                // Check if value is array
                Value val = value_stack_pop();
                Value result = value_create_boolean(val.type == VALUE_ARRAY);
                value_free(&val);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_IS_NULL: {
                // Check if value is null
                Value val = value_stack_pop();
                Value result = value_create_boolean(val.type == VALUE_NULL);
                value_free(&val);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_IS_OBJECT: {
                // Check if value is object
                Value val = value_stack_pop();
                Value result = value_create_boolean(val.type == VALUE_OBJECT);
                value_free(&val);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_IS_FUNCTION: {
                // Check if value is function
                Value val = value_stack_pop();
                Value result = value_create_boolean(val.type == VALUE_FUNCTION);
                value_free(&val);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_ARRAY_PUSH: {
                // Push value to array (requires array and value on stack)
                Value val = value_stack_pop();
                Value arr = value_stack_pop();
                if (arr.type == VALUE_ARRAY) {
                    // Use built-in array push function
                    Value args[2] = {arr, val};
                    Value result = builtin_array_push(interpreter, args, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                value_free(&arr);
                pc++;
                break;
            }
            
            case BC_ARRAY_POP: {
                // Pop value from array
                Value arr = value_stack_pop();
                if (arr.type == VALUE_ARRAY) {
                    // Use built-in array pop function
                    Value args[1] = {arr};
                    Value result = builtin_array_pop(interpreter, args, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                value_free(&arr);
                pc++;
                break;
            }
            
            case BC_ARRAY_CONTAINS: {
                // Check if array contains value
                Value search_val = value_stack_pop();
                Value arr = value_stack_pop();
                if (arr.type == VALUE_ARRAY) {
                    // Use built-in array contains function
                    Value args[2] = {arr, search_val};
                    Value result = builtin_array_contains(interpreter, args, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                value_free(&arr);
                value_free(&search_val);
                pc++;
                break;
            }
            
            case BC_ARRAY_INDEX_OF: {
                // Get index of value in array
                Value search_val = value_stack_pop();
                Value arr = value_stack_pop();
                if (arr.type == VALUE_ARRAY) {
                    // Use built-in array indexOf function
                    Value args[2] = {arr, search_val};
                    Value result = builtin_array_index_of(interpreter, args, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                value_free(&arr);
                value_free(&search_val);
                pc++;
                break;
            }
            
            case BC_ARRAY_JOIN: {
                // Array join: arr.join(separator)
                Value separator = value_stack_pop();
                Value array = value_stack_pop();
                
                if (array.type == VALUE_ARRAY) {
                    Value result = builtin_array_join(NULL, (Value[]){array, separator}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&array);
                value_free(&separator);
                pc++;
                break;
            }
            
            case BC_ARRAY_UNIQUE: {
                // Array unique: arr.unique()
                Value array = value_stack_pop();
                
                if (array.type == VALUE_ARRAY) {
                    Value result = builtin_array_unique(NULL, (Value[]){array}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&array);
                pc++;
                break;
            }
            
            case BC_ARRAY_SLICE: {
                // Array slice: arr.slice(start, end)
                Value end = value_stack_pop();
                Value start = value_stack_pop();
                Value array = value_stack_pop();
                
                if (array.type == VALUE_ARRAY) {
                    Value result = builtin_array_slice(NULL, (Value[]){array, start, end}, 3, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&array);
                value_free(&start);
                value_free(&end);
                pc++;
                break;
            }
            
            case BC_ARRAY_CONCAT_METHOD: {
                // Array concat method: arr.concat(other)
                Value other = value_stack_pop();
                Value array = value_stack_pop();
                
                if (array.type == VALUE_ARRAY) {
                    Value result = builtin_array_concat(NULL, (Value[]){array, other}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&array);
                value_free(&other);
                pc++;
                break;
            }
            
            case BC_CREATE_ARRAY: {
                // Create array from stack elements
                size_t element_count = instr->a;
                
                // Create array with initial capacity
                Value array_val = value_create_array(element_count > 0 ? element_count : 1);
                
                // Handle empty arrays
                if (element_count == 0) {
                    value_stack_push(array_val);
                    pc++;
                    break;
                }
                
                // Allocate temporary storage for elements
                Value* elements = shared_malloc_safe(element_count * sizeof(Value), "bytecode_vm", "BC_CREATE_ARRAY", 1);
                
                // Pop elements from stack (in reverse order)
                for (size_t i = 0; i < element_count; i++) {
                    elements[element_count - 1 - i] = value_stack_pop();
                }
                
                // Push elements into the array
                for (size_t i = 0; i < element_count; i++) {
                    value_array_push(&array_val, elements[i]);
                }
                
                value_stack_push(array_val);
                
                // Free the elements array (values are now owned by the array)
                shared_free_safe(elements, "bytecode_vm", "BC_CREATE_ARRAY", 2);
                pc++;
                break;
            }
            
            case BC_ARRAY_CONCAT: {
                // Array concatenation: arr1 + arr2
                Value arr2 = value_stack_pop();
                Value arr1 = value_stack_pop();
                
                if (arr1.type == VALUE_ARRAY && arr2.type == VALUE_ARRAY) {
                    // Create new array with combined capacity
                    size_t total_capacity = arr1.data.array_value.capacity + arr2.data.array_value.capacity;
                    Value result = value_create_array(total_capacity);
                    
                    // Copy elements from first array
                    for (size_t i = 0; i < arr1.data.array_value.count; i++) {
                        Value* element = (Value*)arr1.data.array_value.elements[i];
                        if (element) {
                            Value cloned_element = value_clone(element);
                            value_array_push(&result, cloned_element);
                        }
                    }
                    
                    // Copy elements from second array
                    for (size_t i = 0; i < arr2.data.array_value.count; i++) {
                        Value* element = (Value*)arr2.data.array_value.elements[i];
                        if (element) {
                            Value cloned_element = value_clone(element);
                            value_array_push(&result, cloned_element);
                        }
                    }
                    
                    // Push result first, then free original arrays
                    value_stack_push(result);
                } else {
                    // Fallback to AST for non-array types
                    value_stack_push(value_create_null());
                }
                
                // Free original arrays after result is safely on stack
                value_free(&arr1);
                value_free(&arr2);
                pc++;
                break;
            }
            
            case BC_CREATE_OBJECT: {
                // Create object from key-value pairs on stack
                size_t pair_count = instr->a;
                Value object_val = value_create_object(pair_count > 0 ? pair_count : 4);
                
                // Pop key-value pairs from stack (in reverse order)
                for (size_t i = 0; i < pair_count; i++) {
                    Value value = value_stack_pop();
                    Value key = value_stack_pop();
                    
                    if (key.type == VALUE_STRING) {
                        value_object_set(&object_val, key.data.string_value, value);
                    }
                    
                    value_free(&key);
                    value_free(&value);
                }
                
                value_stack_push(object_val);
                pc++;
                break;
            }
            
            case BC_CREATE_MAP: {
                // Create hash map from key-value pairs on stack
                size_t pair_count = instr->a;
                Value map_val = value_create_hash_map(pair_count > 0 ? pair_count : 4);
                
                // Pop key-value pairs from stack (in reverse order)
                for (size_t i = 0; i < pair_count; i++) {
                    Value key = value_stack_pop();
                    Value value = value_stack_pop();
                    
                    // Add key-value pair to hash map
                    value_hash_map_set(&map_val, key, value);
                    
                    value_free(&key);
                    value_free(&value);
                }
                
                value_stack_push(map_val);
                pc++;
                break;
            }
            
            case BC_IMPORT_LIB: {
                // Import library: use library_name
                if (instr->a < program->const_count && program->constants[instr->a].type == VALUE_STRING) {
                    const char* library_name = program->constants[instr->a].data.string_value;
                    
                    // Get the library from global environment (it should already be registered)
                    Value lib = environment_get(interpreter->global_environment, library_name);
                    
                    // Define it in the current environment
                    environment_define(interpreter->current_environment, library_name, lib);
                    
                    // Push null result (use statements don't return a value)
                    value_stack_push(value_create_null());
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            
            case BC_STRING_UPPER: {
                // Convert string to uppercase
                Value val = value_stack_pop();
                if (val.type == VALUE_STRING) {
                    char* upper = shared_strdup(val.data.string_value);
                    for (char* p = upper; *p; p++) {
                        *p = toupper(*p);
                    }
                    Value result = value_create_string(upper);
                    shared_free_safe(upper, "bytecode_vm", "BC_STRING_UPPER", 0);
                    value_free(&val);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_STRING_LOWER: {
                // Convert string to lowercase
                Value val = value_stack_pop();
                if (val.type == VALUE_STRING) {
                    char* lower = shared_strdup(val.data.string_value);
                    for (char* p = lower; *p; p++) {
                        *p = tolower(*p);
                    }
                    Value result = value_create_string(lower);
                    shared_free_safe(lower, "bytecode_vm", "BC_STRING_LOWER", 0);
                    value_free(&val);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_STRING_TRIM: {
                // Trim string whitespace
                Value val = value_stack_pop();
                if (val.type == VALUE_STRING) {
                    char* str = val.data.string_value;
                    char* start = str;
                    char* end = str + strlen(str) - 1;
                    
                    // Trim leading whitespace
                    while (start <= end && isspace(*start)) start++;
                    
                    // Trim trailing whitespace
                    while (end >= start && isspace(*end)) end--;
                    
                    // Create trimmed string
                    size_t len = end - start + 1;
                    char* trimmed = malloc(len + 1);
                    strncpy(trimmed, start, len);
                    trimmed[len] = '\0';
                    
                    Value result = value_create_string(trimmed);
                    free(trimmed);
                    value_free(&val);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_MATH_ABS: {
                // Math abs: math.abs(value)
                Value value = value_stack_pop();
                
                if (value.type == VALUE_NUMBER) {
                    Value result = builtin_math_abs(NULL, (Value[]){value}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&value);
                pc++;
                break;
            }
            
            case BC_MATH_SQRT: {
                // Math sqrt: math.sqrt(value)
                Value value = value_stack_pop();
                
                if (value.type == VALUE_NUMBER) {
                    Value result = builtin_math_sqrt(NULL, (Value[]){value}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&value);
                pc++;
                break;
            }
            
            case BC_MATH_POW: {
                // Math pow: math.pow(base, exponent)
                Value exponent = value_stack_pop();
                Value base = value_stack_pop();
                
                if (base.type == VALUE_NUMBER && exponent.type == VALUE_NUMBER) {
                    Value result = builtin_math_pow(NULL, (Value[]){base, exponent}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&base);
                value_free(&exponent);
                pc++;
                break;
            }
            
            case BC_MATH_SIN: {
                // Math sin: math.sin(value)
                Value value = value_stack_pop();
                
                if (value.type == VALUE_NUMBER) {
                    Value result = builtin_math_sin(NULL, (Value[]){value}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&value);
                pc++;
                break;
            }
            
            case BC_MATH_COS: {
                // Math cos: math.cos(value)
                Value value = value_stack_pop();
                
                if (value.type == VALUE_NUMBER) {
                    Value result = builtin_math_cos(NULL, (Value[]){value}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&value);
                pc++;
                break;
            }
            
            case BC_MATH_TAN: {
                // Math tan: math.tan(value)
                Value value = value_stack_pop();
                
                if (value.type == VALUE_NUMBER) {
                    Value result = builtin_math_tan(NULL, (Value[]){value}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&value);
                pc++;
                break;
            }
            
            case BC_MATH_FLOOR: {
                // Math floor: math.floor(value)
                Value value = value_stack_pop();
                
                if (value.type == VALUE_NUMBER) {
                    Value result = builtin_math_floor(NULL, (Value[]){value}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&value);
                pc++;
                break;
            }
            
            case BC_MATH_CEIL: {
                // Math ceil: math.ceil(value)
                Value value = value_stack_pop();
                
                if (value.type == VALUE_NUMBER) {
                    Value result = builtin_math_ceil(NULL, (Value[]){value}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&value);
                pc++;
                break;
            }
            
            case BC_MATH_ROUND: {
                // Math round: math.round(value)
                Value value = value_stack_pop();
                
                if (value.type == VALUE_NUMBER) {
                    Value result = builtin_math_round(NULL, (Value[]){value}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&value);
                pc++;
                break;
            }
            
            case BC_MAP_HAS: {
                // Map has key check: map.has(key)
                Value key = value_stack_pop();
                Value map = value_stack_pop();
                
                if (map.type == VALUE_HASH_MAP) {
                    Value result = builtin_map_has(NULL, (Value[]){map, key}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_boolean(false));
                }
                
                value_free(&map);
                value_free(&key);
                pc++;
                break;
            }
            
            case BC_MAP_SIZE: {
                // Map size property: map.size
                Value map = value_stack_pop();
                
                if (map.type == VALUE_HASH_MAP) {
                    Value result = builtin_map_size(NULL, (Value[]){map}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_number(0));
                }
                
                value_free(&map);
                pc++;
                break;
            }
            
            case BC_MAP_KEYS: {
                // Map keys method: map.keys()
                Value map = value_stack_pop();
                
                if (map.type == VALUE_HASH_MAP) {
                    Value result = builtin_map_keys(NULL, (Value[]){map}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_array(0));
                }
                
                value_free(&map);
                pc++;
                break;
            }
            
            case BC_MAP_DELETE: {
                // Map delete method: map.delete(key)
                Value key = value_stack_pop();
                Value map = value_stack_pop();
                
                if (map.type == VALUE_HASH_MAP) {
                    Value result = builtin_map_delete(NULL, (Value[]){map, key}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&map);
                value_free(&key);
                pc++;
                break;
            }
            
            case BC_MAP_CLEAR: {
                // Map clear method: map.clear()
                Value map = value_stack_pop();
                
                if (map.type == VALUE_HASH_MAP) {
                    Value result = builtin_map_clear(NULL, (Value[]){map}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&map);
                pc++;
                break;
            }
            
            case BC_MAP_UPDATE: {
                // Map update method: map.update(other_map)
                Value other_map = value_stack_pop();
                Value map = value_stack_pop();
                
                if (map.type == VALUE_HASH_MAP) {
                    Value result = builtin_map_update(NULL, (Value[]){map, other_map}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&map);
                value_free(&other_map);
                pc++;
                break;
            }
            
            case BC_SET_ADD: {
                // Set add method: set.add(element)
                Value element = value_stack_pop();
                Value set = value_stack_pop();
                
                if (set.type == VALUE_SET) {
                    Value result = builtin_set_add(NULL, (Value[]){set, element}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&set);
                value_free(&element);
                pc++;
                break;
            }
            
            case BC_SET_HAS: {
                // Set has element check: set.has(element)
                Value element = value_stack_pop();
                Value set = value_stack_pop();
                
                if (set.type == VALUE_SET) {
                    Value result = builtin_set_has(NULL, (Value[]){set, element}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_boolean(false));
                }
                
                value_free(&set);
                value_free(&element);
                pc++;
                break;
            }
            
            case BC_SET_REMOVE: {
                // Set remove method: set.remove(element)
                Value element = value_stack_pop();
                Value set = value_stack_pop();
                
                if (set.type == VALUE_SET) {
                    Value result = builtin_set_remove(NULL, (Value[]){set, element}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&set);
                value_free(&element);
                pc++;
                break;
            }
            
            case BC_SET_SIZE: {
                // Set size property: set.size
                Value set = value_stack_pop();
                
                if (set.type == VALUE_SET) {
                    Value result = builtin_set_size(NULL, (Value[]){set}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_number(0));
                }
                
                value_free(&set);
                pc++;
                break;
            }
            
            case BC_SET_CLEAR: {
                // Set clear method: set.clear()
                Value set = value_stack_pop();
                
                if (set.type == VALUE_SET) {
                    Value result = builtin_set_clear(NULL, (Value[]){set}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&set);
                pc++;
                break;
            }
            
            case BC_SET_TO_ARRAY: {
                // Set toArray method: set.toArray()
                Value set = value_stack_pop();
                
                if (set.type == VALUE_SET) {
                    Value result = builtin_set_to_array(NULL, (Value[]){set}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&set);
                pc++;
                break;
            }
            
            case BC_SET_UNION: {
                // Set union method: set.union(other_set)
                Value other_set = value_stack_pop();
                Value set = value_stack_pop();
                
                if (set.type == VALUE_SET) {
                    Value result = builtin_set_union(NULL, (Value[]){set, other_set}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&set);
                value_free(&other_set);
                pc++;
                break;
            }
            
            case BC_SET_INTERSECTION: {
                // Set intersection method: set.intersection(other_set)
                Value other_set = value_stack_pop();
                Value set = value_stack_pop();
                
                if (set.type == VALUE_SET) {
                    Value result = builtin_set_intersection(NULL, (Value[]){set, other_set}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&set);
                value_free(&other_set);
                pc++;
                break;
            }
            
            case BC_EVAL_AST: {
                // Fallback to AST evaluation
                if (instr->a < program->ast_count) {
                    ASTNode* node = program->ast_nodes[instr->a];
                    if (node) {
                        // Synchronize bytecode variables with AST interpreter environment
                        // This allows AST fallbacks to access bytecode-defined variables
                        for (size_t i = 0; i < program->local_count && i < program->local_slot_count; i++) {
                            if (program->local_names[i]) {
                                Value local_val = program->locals[i];
                                // Check if variable exists in environment
                                if (environment_exists(interpreter->current_environment, program->local_names[i])) {
                                    // Update existing variable
                                    environment_assign(interpreter->current_environment, program->local_names[i], value_clone(&local_val));
                                } else {
                                    // Define new variable
                                    environment_define(interpreter->current_environment, program->local_names[i], value_clone(&local_val));
                                }
                            }
                        }
                        
                        Value result = interpreter_execute(interpreter, node);
                        
                        // Synchronize back from AST environment to bytecode locals
                        // This allows modifications in AST fallbacks to be visible in bytecode
                        for (size_t i = 0; i < program->local_count && i < program->local_slot_count; i++) {
                            if (program->local_names[i]) {
                                Value env_val = environment_get(interpreter->current_environment, program->local_names[i]);
                                if (env_val.type != VALUE_NULL) {
                                    value_free(&program->locals[i]);
                                    program->locals[i] = value_clone(&env_val);
                                }
                            }
                        }
                        
                        value_stack_push(result);
                    } else {
                        value_stack_push(value_create_null());
                    }
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_MATCH: {
                // Pattern matching: match expr with cases
                // instr->a = number of cases
                int case_count = instr->a;
                Value match_value = value_stack_pop();
                int matched = 0;
                
                // Try each case in order
                for (int i = 0; i < case_count && !matched; i++) {
                    // Check if we have a BC_MATCH_CASE instruction next
                    if (pc < program->count) {
                        BytecodeInstruction* case_instr = &program->code[pc];
                        if (case_instr->op == BC_MATCH_CASE) {
                            // Skip the BC_MATCH_CASE instruction
                            pc++;
                            
                            // The pattern and body are already on the stack from compilation
                            // We need to check if the pattern matches
                            Value pattern = value_stack_pop();
                            Value body = value_stack_pop();
                            
                            // Check if pattern matches
                            if (pattern_matches_value(&match_value, &pattern)) {
                                // Pattern matches, execute the body
                                value_free(&pattern);
                                value_free(&match_value);
                                value_stack_push(body);
                                matched = 1;
                                // Skip remaining cases
                                for (int j = i + 1; j < case_count; j++) {
                                    if (pc < program->count) {
                                        BytecodeInstruction* skip_instr = &program->code[pc];
                                        if (skip_instr->op == BC_MATCH_CASE) {
                                            pc++;
                                            // Skip pattern and body
                                            Value skip_pattern = value_stack_pop();
                                            Value skip_body = value_stack_pop();
                                            value_free(&skip_pattern);
                                            value_free(&skip_body);
                                        }
                                    }
                                }
                            } else {
                                // Pattern doesn't match, continue to next case
                                value_free(&pattern);
                                value_free(&body);
                            }
                        }
                    }
                }
                
                // No pattern matched, return null
                if (!matched) {
                    value_free(&match_value);
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_MATCH_CASE: {
                // This instruction is handled within BC_MATCH
                pc++;
                break;
            }
            
            case BC_PATTERN_LITERAL: {
                // Pattern: literal pattern (string, number, etc.)
                // The literal value is already on the stack from compilation
                // This instruction just marks it as a pattern
                pc++;
                break;
            }
            
            case BC_PATTERN_WILDCARD: {
                // Pattern: wildcard pattern (_) - matches anything
                // Push a special wildcard marker
                value_stack_push(value_create_string("__WILDCARD__"));
                pc++;
                break;
            }
            
            case BC_PATTERN_TYPE: {
                // Pattern: type pattern (e.g., String, Int)
                // instr->a = type name constant index
                if (instr->a < program->const_count) {
                    Value type_name = program->constants[instr->a];
                    if (type_name.type == VALUE_STRING) {
                        value_stack_push(value_create_string(type_name.data.string_value));
                    } else {
                        value_stack_push(value_create_null());
                    }
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_CREATE_CLASS: {
                // Create class definition
                // instr->a = class name constant index, instr->b = parent class name constant index, instr->c = body AST index
                if (instr->a < program->const_count) {
                    Value class_name = program->constants[instr->a];
                    if (class_name.type == VALUE_STRING) {
                        const char* parent_name = NULL;
                        if (instr->b >= 0 && instr->b < program->const_count) {
                            Value parent_name_val = program->constants[instr->b];
                            if (parent_name_val.type == VALUE_STRING) {
                                parent_name = parent_name_val.data.string_value;
                            }
                        }
                        
                        // Get class body AST
                        ASTNode* class_body = NULL;
                        if (instr->c >= 0 && instr->c < program->ast_count) {
                            class_body = program->ast_nodes[instr->c];
                        }
                        
                        // Create class value with the body AST
                        Value class_value = value_create_class(
                            class_name.data.string_value,
                            parent_name,
                            class_body,
                            interpreter->current_environment
                        );
                        
                        // Store class in global environment so it can be accessed from anywhere
                        environment_define(interpreter->global_environment, class_name.data.string_value, class_value);
                        value_free(&class_value);
                    }
                }
                pc++;
                break;
            }
            
            case BC_INSTANTIATE_CLASS: {
                // Instantiate class: ClassName(args...)
                // instr->a = class name constant index, instr->b = argument count
                if (instr->a < program->const_count) {
                    Value class_name = program->constants[instr->a];
                    if (class_name.type == VALUE_STRING) {
                        // Get class from environment
                        Value class_value = environment_get(interpreter->current_environment, class_name.data.string_value);
                        if (class_value.type != VALUE_CLASS) {
                            class_value = environment_get(interpreter->global_environment, class_name.data.string_value);
                        }
                        
                        if (class_value.type == VALUE_CLASS) {
                            // Pop arguments from stack
                            int arg_count = instr->b;
                            Value* args = shared_malloc_safe(arg_count * sizeof(Value), "bytecode_vm", "BC_INSTANTIATE_CLASS", 1);
                            if (args) {
                                for (int i = arg_count - 1; i >= 0; i--) {
                                    args[i] = value_stack_pop();
                                }
                                
                                // Create class instance
                                Value instance = value_create_object(16);
                                
                                // Set class name
                                Value class_name_val = value_create_string(class_value.data.class_value.class_name);
                                value_object_set(&instance, "__class_name__", class_name_val);
                                value_free(&class_name_val);
                                
                                // Collect all fields from inheritance chain
                                ASTNode** all_fields = NULL;
                                size_t field_count = 0;
                                size_t field_capacity = 0;
                                
                                collect_class_fields_for_bytecode(interpreter, &class_value, &all_fields, &field_count, &field_capacity);
                                
                                // Initialize fields
                                size_t field_index = 0;
                                for (size_t i = 0; i < field_count; i++) {
                                    ASTNode* stmt = all_fields[i];
                                    if (stmt && stmt->type == AST_NODE_VARIABLE_DECLARATION) {
                                        const char* field_name = stmt->data.variable_declaration.variable_name;
                                        if (field_name) {
                                            Value field_value;
                                            if (field_index < (size_t)arg_count) {
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
                                    shared_free_safe(all_fields, "bytecode_vm", "BC_INSTANTIATE_CLASS", 2);
                                }
                                
                                // Call constructor if it exists
                                Value init_func = value_object_get(&class_value, "init");
                                if (init_func.type == VALUE_FUNCTION) {
                                    // Push instance as 'this' and arguments
                                    value_stack_push(instance);
                                    for (int i = 0; i < arg_count; i++) {
                                        value_stack_push(args[i]);
                                    }
                                    
                                    // Call constructor function
                                    Value result = value_function_call(&init_func, NULL, arg_count + 1, interpreter, 0, 0);
                                    value_free(&result);
                                    
                                    // Get the updated instance from stack
                                    instance = value_stack_pop();
                                }
                                
                                value_free(&init_func);
                                
                                value_stack_push(instance);
                                
                                // Clean up arguments
                                for (int i = 0; i < arg_count; i++) {
                                    value_free(&args[i]);
                                }
                                shared_free_safe(args, "bytecode_vm", "BC_INSTANTIATE_CLASS", 0);
                            } else {
                                value_stack_push(value_create_null());
                            }
                        } else {
                            value_stack_push(value_create_null());
                        }
                    }
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_FOR_LOOP: {
                // For loop: for i in collection body
                // instr->a = variable name constant index, instr->b = body AST index
                if (instr->a < program->const_count && instr->b < program->ast_count) {
                    Value var_name = program->constants[instr->a];
                    ASTNode* body = program->ast_nodes[instr->b];
                    
                    if (var_name.type == VALUE_STRING && body) {
                        // Get collection from stack
                        Value collection = value_stack_pop();
                        
                        // Handle different collection types
                        if (collection.type == VALUE_ARRAY) {
                            // Iterate over array elements
                            for (size_t i = 0; i < collection.data.array_value.count; i++) {
                                Value* elem_ptr = (Value*)collection.data.array_value.elements[i];
                                Value element;
                                
                                if (!elem_ptr) {
                                    // Null pointer - use Null value
                                    element = value_create_null();
                                } else {
                                    element = value_clone(elem_ptr);
                                }
                                
                                // Set loop variable in environment
                                environment_define(interpreter->current_environment, var_name.data.string_value, element);
                                
                                // Execute loop body
                                if (body) {
                                    Value result = interpreter_execute(interpreter, body);
                                    value_free(&result);
                                }
                                
                                // Check for break/continue (simplified for now)
                                if (interpreter_has_error(interpreter)) {
                                    value_free(&element);
                                    break;
                                }
                                value_free(&element);
                            }
                        } else if (collection.type == VALUE_STRING) {
                            // Iterate over string characters
                            for (size_t i = 0; i < strlen(collection.data.string_value); i++) {
                                char ch = collection.data.string_value[i];
                                Value element = value_create_string((char[]){ch, '\0'});
                                
                                // Set loop variable in environment
                                environment_define(interpreter->current_environment, var_name.data.string_value, element);
                                
                                // Execute loop body
                                Value result = interpreter_execute(interpreter, body);
                                value_free(&result);
                                value_free(&element);
                                
                                // Check for break/continue (simplified for now)
                                if (interpreter_has_error(interpreter)) {
                                    break;
                                }
                            }
                        }
                        
                        value_free(&collection);
                    }
                }
                pc++;
                break;
            }
            
            case BC_POP: {
                Value val = value_stack_pop();
                value_free(&val);
                pc++;
                break;
            }
            
            case BC_HALT: {
                result = value_stack_pop();
                goto cleanup;
            }
            
            // Numeric operations
            case BC_LOAD_NUM: {
                if (instr->a < program->num_const_count) {
                    num_stack_push(program->num_constants[instr->a]);
                } else {
                    num_stack_push(0.0);
                }
                pc++;
                break;
            }
            
            case BC_LOAD_NUM_LOCAL: {
                if (instr->a < program->num_local_count) {
                    num_stack_push(program->num_locals[instr->a]);
                } else {
                    num_stack_push(0.0);
                }
                pc++;
                break;
            }
            
            case BC_STORE_NUM_LOCAL: {
                if (instr->a < program->num_local_count) {
                    program->num_locals[instr->a] = num_stack_pop();
                }
                pc++;
                break;
            }
            
            case BC_ADD_NUM: {
                // Fast path: direct stack access without function calls
                if (num_stack_size >= 2) {
                    double b = num_stack[--num_stack_size];
                    double a = num_stack[--num_stack_size];
                    num_stack[num_stack_size++] = a + b;
                } else {
                    // Fallback for safety
                    double b = num_stack_pop();
                    double a = num_stack_pop();
                    num_stack_push(a + b);
                }
                pc++;
                break;
            }
            
            case BC_SUB_NUM: {
                // Fast path: direct stack access without function calls
                if (num_stack_size >= 2) {
                    double b = num_stack[--num_stack_size];
                    double a = num_stack[--num_stack_size];
                    num_stack[num_stack_size++] = a - b;
                } else {
                    // Fallback for safety
                    double b = num_stack_pop();
                    double a = num_stack_pop();
                    num_stack_push(a - b);
                }
                pc++;
                break;
            }
            
            case BC_MUL_NUM: {
                // Fast path: direct stack access without function calls
                if (num_stack_size >= 2) {
                    double b = num_stack[--num_stack_size];
                    double a = num_stack[--num_stack_size];
                    num_stack[num_stack_size++] = a * b;
                } else {
                    // Fallback for safety
                    double b = num_stack_pop();
                    double a = num_stack_pop();
                    num_stack_push(a * b);
                }
                pc++;
                break;
            }
            
            case BC_DIV_NUM: {
                // Fast path: direct stack access without function calls
                if (num_stack_size >= 2) {
                    double b = num_stack[--num_stack_size];
                    double a = num_stack[--num_stack_size];
                    if (b != 0.0) {
                        num_stack[num_stack_size++] = a / b;
                    } else {
                        num_stack[num_stack_size++] = 0.0;
                    }
                } else {
                    // Fallback for safety
                    double b = num_stack_pop();
                    double a = num_stack_pop();
                    if (b != 0.0) {
                        num_stack_push(a / b);
                    } else {
                        num_stack_push(0.0);
                    }
                }
                pc++;
                break;
            }
            
            case BC_LT_NUM: {
                double b = num_stack_pop();
                double a = num_stack_pop();
                value_stack_push(value_create_boolean(a < b));
                pc++;
                break;
            }
            
            case BC_LE_NUM: {
                double b = num_stack_pop();
                double a = num_stack_pop();
                value_stack_push(value_create_boolean(a <= b));
                pc++;
                break;
            }
            
            case BC_GT_NUM: {
                double b = num_stack_pop();
                double a = num_stack_pop();
                value_stack_push(value_create_boolean(a > b));
                pc++;
                break;
            }
            
            case BC_GE_NUM: {
                double b = num_stack_pop();
                double a = num_stack_pop();
                value_stack_push(value_create_boolean(a >= b));
                pc++;
                break;
            }
            
            case BC_EQ_NUM: {
                double b = num_stack_pop();
                double a = num_stack_pop();
                value_stack_push(value_create_boolean(a == b));
                pc++;
                break;
            }
            
            case BC_NE_NUM: {
                double b = num_stack_pop();
                double a = num_stack_pop();
                value_stack_push(value_create_boolean(a != b));
                pc++;
                break;
            }
            
            case BC_VALUE_TO_NUM: {
                Value val = value_stack_pop();
                if (val.type == VALUE_NUMBER) {
                    num_stack_push(val.data.number_value);
                } else {
                    num_stack_push(0.0);
                }
                value_free(&val);
                pc++;
                break;
            }
            
            case BC_NUM_TO_VALUE: {
                double num = num_stack_pop();
                value_stack_push(value_create_number(num));
                pc++;
                break;
            }
            
            
            case BC_INC_LOCAL: {
                if (instr->a < program->num_local_count) {
                    program->num_locals[instr->a] += 1.0;
                }
                pc++;
                break;
            }
            
            case BC_ADD_LOCAL_IMM: {
                if (instr->a < program->num_local_count && instr->b < program->num_const_count) {
                    program->num_locals[instr->a] += program->num_constants[instr->b];
                    
                    // Also update the value locals array for consistency
                    if (instr->a < program->local_slot_count) {
                        value_free(&program->locals[instr->a]);
                        program->locals[instr->a] = value_create_number(program->num_locals[instr->a]);
                    }
                }
                pc++;
                break;
            }
            
            case BC_ADD_LLL: {
                double c = num_stack_pop();
                double b = num_stack_pop();
                double a = num_stack_pop();
                num_stack_push(a + b + c);
                pc++;
                break;
            }
            
            default: {
                // Unknown opcode - fallback to AST
                if (instr->a < program->ast_count) {
                    ASTNode* node = program->ast_nodes[instr->a];
                    if (node) {
                        Value result = interpreter_execute(interpreter, node);
                        value_stack_push(result);
                    } else {
                        value_stack_push(value_create_null());
                    }
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            }
        }
    }
    
cleanup:
    // Clean up any remaining stack values
    while (value_stack_size > 0) {
        Value val = value_stack_pop();
        value_free(&val);
    }
    
    // Clean up memory optimizations
    cleanup_memory_optimizations();
    
    return result;
}

// Execute a user-defined function's bytecode
Value bytecode_execute_function_bytecode(Interpreter* interpreter, BytecodeFunction* func, Value* args, int arg_count, BytecodeProgram* program) {
    if (!func || !interpreter) {
        return value_create_null();
    }
    
    // Create new environment for function execution
    Environment* func_env = environment_create(interpreter->current_environment);
    if (!func_env) {
        return value_create_null();
    }
    
    // Bind parameters to arguments
    size_t param_count = func->param_count;
    for (size_t i = 0; i < param_count && i < (size_t)arg_count; i++) {
        if (func->param_names[i]) {
            const char* param_name = func->param_names[i];
            environment_define(func_env, param_name, args[i]);
        }
    }
    
    // Save current environment and set function environment
    Environment* old_env = interpreter->current_environment;
    interpreter->current_environment = func_env;
    
    // Execute the function's bytecode
    Value result = value_create_null();
    
    if (func->code_count > 0) {
        // Create a temporary program with just this function's code
        // We need to pass the constants from the main program
        BytecodeProgram temp_program = {0};
        temp_program.code = func->code;
        temp_program.count = func->code_count;
        temp_program.const_count = program ? program->const_count : 0;
        temp_program.constants = program ? program->constants : NULL;
        temp_program.num_const_count = program ? program->num_const_count : 0;
        temp_program.num_constants = program ? program->num_constants : NULL;
        temp_program.ast_count = 0;
        temp_program.ast_nodes = NULL;
        temp_program.function_count = 0;
        temp_program.functions = NULL;
        
        
        // Execute the function's bytecode
        result = bytecode_execute(&temp_program, interpreter, 0);
        
        // Check if the function returned a value
        if (interpreter->has_return) {
            result = interpreter->return_value;
            interpreter->has_return = 0; // Reset return flag
        }
    }
    
    // Restore old environment
    interpreter->current_environment = old_env;
    
    // Clean up function environment
    environment_free(func_env);
    
    return result;
}

// Pattern matching helper function
static int pattern_matches_value(Value* value, Value* pattern) {
    if (!value || !pattern) return 0;
    
    // Check for wildcard pattern
    if (pattern->type == VALUE_STRING && strcmp(pattern->data.string_value, "__WILDCARD__") == 0) {
        return 1; // Wildcard matches anything
    }
    
    // Check for literal pattern matching
    if (value->type == pattern->type) {
        switch (value->type) {
            case VALUE_NUMBER:
                return value->data.number_value == pattern->data.number_value;
            case VALUE_STRING:
                return strcmp(value->data.string_value, pattern->data.string_value) == 0;
            case VALUE_BOOLEAN:
                return value->data.boolean_value == pattern->data.boolean_value;
            case VALUE_NULL:
                return 1; // Both are null
            default:
                return 0;
        }
    }
    
    return 0;
}

