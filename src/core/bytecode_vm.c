#include "../../include/core/bytecode.h"
#include "../../include/utils/shared_utilities.h"
#include "../../include/core/interpreter/value_operations.h"
#include "../../include/core/interpreter/eval_engine.h"
#include "../../include/core/interpreter/method_handlers.h"
#include "../../include/core/interpreter/interpreter_core.h"
#include "../../include/core/environment.h"
#include "../../include/core/lexer.h"
#include "../../include/core/parser.h"
#include "../../include/core/ast.h"
#include "../../include/libs/array.h"
#include "../../include/libs/math.h"
#include "../../include/libs/maps.h"
#include "../../include/libs/sets.h"
#include "../../include/libs/graphics.h"
#include "../../include/core/optimization/hot_spot_tracker.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>

// Branch prediction hints for better CPU performance
#ifdef __GNUC__
#define LIKELY(x)   __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define LIKELY(x)   (x)
#define UNLIKELY(x) (x)
#endif

// Cache optimization hints
#ifdef __GNUC__
#define PREFETCH_READ(addr)  __builtin_prefetch((addr), 0, 3)
#define PREFETCH_WRITE(addr) __builtin_prefetch((addr), 1, 3)
#else
#define PREFETCH_READ(addr)  ((void)0)
#define PREFETCH_WRITE(addr) ((void)0)
#endif

// Memory alignment for cache-friendly access
#define CACHE_LINE_SIZE 64
#define ALIGN_CACHE __attribute__((aligned(CACHE_LINE_SIZE)))

// Forward declarations
Value bytecode_execute_function_bytecode(Interpreter* interpreter, BytecodeFunction* func, Value* args, int arg_count, BytecodeProgram* program);
static int pattern_matches_value(Value* value, Value* pattern);

// Async/await runtime functions
static void async_task_queue_add(Interpreter* interpreter, AsyncTask* task);
static AsyncTask* async_task_queue_pop(Interpreter* interpreter);
static void async_resolve_promise(Interpreter* interpreter, Value* promise, Value* value);
static void async_reject_promise(Interpreter* interpreter, Value* promise, Value* error);
void async_event_loop_run(Interpreter* interpreter);
static uint64_t promise_registry_add(Interpreter* interpreter, Value promise);
static Value* promise_registry_get(Interpreter* interpreter, uint64_t promise_id);
static void promise_registry_remove(Interpreter* interpreter, uint64_t promise_id);

// Phase 4: Module cache helper functions (forward declarations)
static char* normalize_file_path(const char* path);
static time_t get_file_mtime(const char* file_path);
static ModuleCacheEntry* find_cached_module(Interpreter* interpreter, const char* file_path);
static void cache_module(Interpreter* interpreter, const char* file_path, Environment* module_env, Value module_value, BytecodeProgram* module_bytecode);
static int check_circular_import(Interpreter* interpreter, const char* module_path);
static void push_import_chain(Interpreter* interpreter, const char* module_path);
static void pop_import_chain(Interpreter* interpreter);

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
    
    // Allocate a temporary buffer for concatenation
    // Note: value_create_string will process escape sequences and allocate its own buffer,
    // so we need to allocate here and let it handle the processing
    char* result = shared_malloc_safe(total_len + 1, "bytecode_vm", "fast_string_concat", 0);
    if (!result) {
        return value_create_string("");
    }
    
    // Bounds-checked copy
    if (len1 > 0 && len1 <= total_len) {
        memcpy(result, str1, len1);
    }
    if (len2 > 0 && len1 + len2 <= total_len) {
        memcpy(result + len1, str2, len2);
    }
    result[total_len] = '\0';
    
    // value_create_string will process escape sequences and allocate its own buffer
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
    
    // Ensure metadata is compiled
    if (!class_value->data.class_value.metadata) {
        compile_class_metadata(interpreter, class_value);
    }
    
    // Use metadata if available (preferred path)
    if (class_value->data.class_value.metadata) {
        ClassMetadata* metadata = class_value->data.class_value.metadata;
        for (size_t i = 0; i < metadata->field_count; i++) {
            // Need to find the AST node for this field to maintain compatibility
            // Search in class body for the variable declaration node
            ASTNode* class_body = class_value->data.class_value.class_body;
            if (class_body && class_body->type == AST_NODE_BLOCK) {
                for (size_t j = 0; j < class_body->data.block.statement_count; j++) {
                    ASTNode* stmt = class_body->data.block.statements[j];
                    if (stmt && stmt->type == AST_NODE_VARIABLE_DECLARATION &&
                        stmt->data.variable_declaration.variable_name &&
                        strcmp(stmt->data.variable_declaration.variable_name, metadata->fields[i].name) == 0) {
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
                        break;
                    }
                }
            }
        }
        return;  // Metadata path complete
    }
    
    // Fallback: Collect from current class body AST
    ASTNode* class_body = class_value->data.class_value.class_body;
    if (class_body && class_body->type == AST_NODE_BLOCK) {
        // Use array access for block statements (like collect_inherited_fields)
        size_t statement_count = class_body->data.block.statement_count;
        if (statement_count > 0 && class_body->data.block.statements) {
            for (size_t i = 0; i < statement_count; i++) {
                ASTNode* stmt = class_body->data.block.statements[i];
                if (stmt && stmt->type == AST_NODE_VARIABLE_DECLARATION) {
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
            }
        } else {
            // Fallback to linked list traversal for older AST format
            ASTNode* stmt = class_body->data.block.statements ? *class_body->data.block.statements : NULL;
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
    if (!node || !interpreter) {
        return NULL;
    }
    
    // Create a new bytecode program
    BytecodeProgram* program = bytecode_program_create();
    if (!program) {
        return NULL;
    }
    
    // Compile the AST node to bytecode
    int success = bytecode_compile_program(program, node, interpreter);
    if (!success) {
        // Compilation failed - free the program and return NULL
        bytecode_program_free(program);
        return NULL;
    }
    
    return program;
}

void ast_node_set_bytecode(ASTNode* node, BytecodeProgram* bytecode) {
    if (!node) {
        return;
    }
    
    // Store the bytecode in the AST node's cached_bytecode field
    node->cached_bytecode = bytecode;
}

Value interpreter_execute_bytecode(Interpreter* interpreter, BytecodeProgram* bytecode) {
    if (!interpreter || !bytecode) {
        return value_create_null();
    }
    
    // Execute the bytecode program
    Value result = bytecode_execute(bytecode, interpreter, 0);
    
    // Return the result (bytecode_execute handles errors internally)
    return result;
}

// Bytecode VM implementation
// This implements a stack-based virtual machine for executing Myco bytecode

// Stack operations
static Value* value_stack = NULL;
size_t value_stack_size = 0;  // Made non-static for debugging
size_t value_stack_capacity = 0;  // Made non-static for debugging

static double* num_stack = NULL;
static size_t num_stack_size = 0;
static size_t num_stack_capacity = 0;

// Forward declarations
static Value value_stack_pop(void);
static double num_stack_pop(void);

// Stack management functions
static void value_stack_push(Value v) {
    // Prevent stack from growing too large (safety limit)
    if (value_stack_size > 100000) {
        // Stack too large - likely infinite loop or unbalanced push/pop
        // Pop some values to prevent memory exhaustion
        while (value_stack_size > 50000) {
            Value val = value_stack_pop();
            value_free(&val);
        }
    }
    if (value_stack_size + 1 > value_stack_capacity) {
        size_t new_cap = value_stack_capacity ? value_stack_capacity * 2 : 128;
        // Cap maximum stack capacity to prevent excessive memory usage
        if (new_cap > 200000) {
            new_cap = 200000;
        }
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
    // Prevent stack from growing too large (safety limit)
    if (num_stack_size > 100000) {
        // Stack too large - likely infinite loop or unbalanced push/pop
        // Pop some values to prevent memory exhaustion
        while (num_stack_size > 50000) {
            num_stack_pop();
        }
    }
    if (num_stack_size + 1 > num_stack_capacity) {
        size_t new_cap = num_stack_capacity ? num_stack_capacity * 2 : 128;
        // Cap maximum stack capacity to prevent excessive memory usage
        if (new_cap > 200000) {
            new_cap = 200000;
        }
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

// ============================================================================
// ASYNC/AWAIT RUNTIME FUNCTIONS
// ============================================================================

static uint64_t promise_registry_add(Interpreter* interpreter, Value promise) {
    if (!interpreter) return 0;
    
    // Initialize registry if needed
    if (!interpreter->promise_registry) {
        interpreter->promise_registry_capacity = 16;
        interpreter->promise_registry = (Value*)shared_malloc_safe(
            interpreter->promise_registry_capacity * sizeof(Value),
            "bytecode_vm", "promise_registry_add", 0);
        interpreter->promise_registry_size = 0;
    }
    
    // Expand registry if needed
    if (interpreter->promise_registry_size >= interpreter->promise_registry_capacity) {
        size_t new_cap = interpreter->promise_registry_capacity * 2;
        interpreter->promise_registry = (Value*)shared_realloc_safe(
            interpreter->promise_registry,
            new_cap * sizeof(Value),
            "bytecode_vm", "promise_registry_add", 1);
        interpreter->promise_registry_capacity = new_cap;
    }
    
    // Assign ID and add to registry
    uint64_t id = interpreter->next_promise_id++;
    promise.data.promise_value.promise_id = id;
    interpreter->promise_registry[interpreter->promise_registry_size++] = promise;
    
    return id;
}

static Value* promise_registry_get(Interpreter* interpreter, uint64_t promise_id) {
    if (!interpreter || promise_id == 0) return NULL;
    if (!interpreter->promise_registry || interpreter->promise_registry_size == 0) return NULL;
    
    // Linear search for the promise (could be optimized with a hash map)
    for (size_t i = 0; i < interpreter->promise_registry_size; i++) {
        if (interpreter->promise_registry[i].type == VALUE_PROMISE &&
            interpreter->promise_registry[i].data.promise_value.promise_id == promise_id) {
            return &interpreter->promise_registry[i];
        }
    }
    
    return NULL;
}

static void promise_registry_remove(Interpreter* interpreter, uint64_t promise_id) {
    if (!interpreter || promise_id == 0) return;
    if (!interpreter->promise_registry || interpreter->promise_registry_size == 0) return;
    
    // Find and remove the promise
    for (size_t i = 0; i < interpreter->promise_registry_size; i++) {
        if (interpreter->promise_registry[i].type == VALUE_PROMISE &&
            interpreter->promise_registry[i].data.promise_value.promise_id == promise_id) {
            // Free the promise
            value_free(&interpreter->promise_registry[i]);
            
            // Shift remaining promises
            for (size_t j = i + 1; j < interpreter->promise_registry_size; j++) {
                interpreter->promise_registry[j - 1] = interpreter->promise_registry[j];
            }
            interpreter->promise_registry_size--;
            break;
        }
    }
}

static void async_task_queue_add(Interpreter* interpreter, AsyncTask* task) {
    if (!interpreter || !task) return;
    
    // Lock mutex for thread-safe access
    if (interpreter->async_enabled && interpreter->worker_thread_count > 0) {
        pthread_mutex_lock(&interpreter->task_queue_mutex);
    }
    
    // Initialize queue if needed
    if (!interpreter->task_queue) {
        interpreter->task_queue_capacity = 16;
        interpreter->task_queue = (AsyncTask**)shared_malloc_safe(
            interpreter->task_queue_capacity * sizeof(AsyncTask*), 
            "bytecode_vm", "async_task_queue_add", 0);
        interpreter->task_queue_size = 0;
    }
    
    // Expand queue if needed
    if (interpreter->task_queue_size >= interpreter->task_queue_capacity) {
        size_t new_cap = interpreter->task_queue_capacity * 2;
        interpreter->task_queue = (AsyncTask**)shared_realloc_safe(
            interpreter->task_queue,
            new_cap * sizeof(AsyncTask*),
            "bytecode_vm", "async_task_queue_add", 1);
        interpreter->task_queue_capacity = new_cap;
    }
    
    interpreter->task_queue[interpreter->task_queue_size++] = task;
    
    // Signal worker threads that a task is available
    if (interpreter->async_enabled && interpreter->worker_thread_count > 0) {
        pthread_cond_signal(&interpreter->task_available);
        pthread_mutex_unlock(&interpreter->task_queue_mutex);
    }
}

static AsyncTask* async_task_queue_pop(Interpreter* interpreter) {
    if (!interpreter) return NULL;
    
    // Lock mutex for thread-safe access
    if (interpreter->async_enabled && interpreter->worker_thread_count > 0) {
        pthread_mutex_lock(&interpreter->task_queue_mutex);
    }
    
    if (!interpreter->task_queue || interpreter->task_queue_size == 0) {
        if (interpreter->async_enabled && interpreter->worker_thread_count > 0) {
            pthread_mutex_unlock(&interpreter->task_queue_mutex);
        }
        return NULL;
    }
    
    AsyncTask* task = interpreter->task_queue[0];
    
    // Shift remaining tasks
    for (size_t i = 1; i < interpreter->task_queue_size; i++) {
        interpreter->task_queue[i - 1] = interpreter->task_queue[i];
    }
    interpreter->task_queue_size--;
    
    if (interpreter->async_enabled && interpreter->worker_thread_count > 0) {
        pthread_mutex_unlock(&interpreter->task_queue_mutex);
    }
    
    return task;
}

static void async_resolve_promise(Interpreter* interpreter, Value* promise, Value* value) {
    if (!promise || promise->type != VALUE_PROMISE) return;
    
    // Lock promise registry mutex for thread-safe access
    if (interpreter && interpreter->async_enabled && interpreter->worker_thread_count > 0) {
        pthread_mutex_lock(&interpreter->promise_registry_mutex);
    }
    
    promise->data.promise_value.is_resolved = 1;
    promise->data.promise_value.is_rejected = 0;
    
    // Free old resolved value if it exists
    if (promise->data.promise_value.resolved_value) {
        value_free(promise->data.promise_value.resolved_value);
        shared_free_safe(promise->data.promise_value.resolved_value, "bytecode_vm", "async_resolve_promise", 0);
        promise->data.promise_value.resolved_value = NULL;
    }
    
    // Free rejected value if it exists (shouldn't happen, but be safe)
    if (promise->data.promise_value.rejected_value) {
        value_free(promise->data.promise_value.rejected_value);
        shared_free_safe(promise->data.promise_value.rejected_value, "bytecode_vm", "async_resolve_promise", 1);
        promise->data.promise_value.rejected_value = NULL;
    }
    
    // Store resolved value
    promise->data.promise_value.resolved_value = (Value*)shared_malloc_safe(sizeof(Value), "bytecode_vm", "async_resolve_promise", 2);
    if (promise->data.promise_value.resolved_value && value) {
        *promise->data.promise_value.resolved_value = value_clone(value);
    } else if (!promise->data.promise_value.resolved_value) {
        // Allocation failed - still mark as resolved but with null value
        promise->data.promise_value.resolved_value = NULL;
    }
    
    if (interpreter && interpreter->async_enabled && interpreter->worker_thread_count > 0) {
        pthread_mutex_unlock(&interpreter->promise_registry_mutex);
    }
    
    // Execute then callbacks (simplified - just process immediately)
    // TODO: Add proper callback execution
}

static void async_reject_promise(Interpreter* interpreter, Value* promise, Value* error) {
    if (!promise || promise->type != VALUE_PROMISE) return;
    
    // Lock promise registry mutex for thread-safe access
    if (interpreter && interpreter->async_enabled && interpreter->worker_thread_count > 0) {
        pthread_mutex_lock(&interpreter->promise_registry_mutex);
    }
    
    promise->data.promise_value.is_resolved = 0;
    promise->data.promise_value.is_rejected = 1;
    
    if (promise->data.promise_value.rejected_value) {
        value_free(promise->data.promise_value.rejected_value);
        shared_free_safe(promise->data.promise_value.rejected_value, "bytecode_vm", "async_reject_promise", 0);
    }
    
    promise->data.promise_value.rejected_value = (Value*)shared_malloc_safe(sizeof(Value), "bytecode_vm", "async_reject_promise", 0);
    if (promise->data.promise_value.rejected_value && error) {
        *promise->data.promise_value.rejected_value = value_clone(error);
    }
    
    if (interpreter && interpreter->async_enabled && interpreter->worker_thread_count > 0) {
        pthread_mutex_unlock(&interpreter->promise_registry_mutex);
    }
    
    // Execute catch callbacks (simplified - just process immediately)
    // TODO: Add proper callback execution
}

// Worker thread function for concurrent task execution
static void* async_worker_thread(void* arg) {
    Interpreter* interpreter = (Interpreter*)arg;
    if (!interpreter) return NULL;
    
    while (!interpreter->shutdown_workers) {
        AsyncTask* task = NULL;
        
        // Lock mutex and wait for tasks
        pthread_mutex_lock(&interpreter->task_queue_mutex);
        
        // Wait for tasks or shutdown signal
        while (interpreter->task_queue_size == 0 && !interpreter->shutdown_workers) {
            pthread_cond_wait(&interpreter->task_available, &interpreter->task_queue_mutex);
        }
        
        // Check if we should shutdown
        if (interpreter->shutdown_workers) {
            pthread_mutex_unlock(&interpreter->task_queue_mutex);
            break;
        }
        
        // Pop a task
        if (interpreter->task_queue && interpreter->task_queue_size > 0) {
            task = interpreter->task_queue[0];
            // Shift remaining tasks
            for (size_t i = 1; i < interpreter->task_queue_size; i++) {
                interpreter->task_queue[i - 1] = interpreter->task_queue[i];
            }
            interpreter->task_queue_size--;
        }
        
        pthread_mutex_unlock(&interpreter->task_queue_mutex);
        
        if (!task) continue;
        
        // Execute task (outside of mutex lock)
        if (task->is_resolved) {
            // Task already completed - resolve promise
            if (task->promise_ptr && task->promise_ptr->type == VALUE_PROMISE) {
                async_resolve_promise(interpreter, task->promise_ptr, &task->result);
            }
        } else {
            // Execute task
            BytecodeProgram* program = (BytecodeProgram*)task->program;
            if (program && task->function_id >= 0 && task->function_id < (int)program->function_count && program->functions) {
                BytecodeFunction* func = &program->functions[task->function_id];
                
                // Create a local interpreter context for this thread
                // Note: We need to be careful about shared state
                Environment* old_env = interpreter->current_environment;
                interpreter->current_environment = task->environment ? task->environment : interpreter->global_environment;
                
                // Execute function
                Value result = bytecode_execute_function_bytecode(
                    interpreter, func, task->args, (int)task->arg_count, program);
                
                // Handle return value
                if (result.type == VALUE_NULL) {
                    if (interpreter->has_return && interpreter->return_value.type != VALUE_NULL) {
                        result = value_clone(&interpreter->return_value);
                        interpreter->has_return = 0;
                        value_free(&interpreter->return_value);
                        interpreter->return_value = value_create_null();
                    }
                }
                
                // Restore environment
                interpreter->current_environment = old_env;
                
                // Resolve promise
                Value result_to_resolve = value_clone(&result);
                task->result = value_clone(&result);
                task->is_resolved = 1;
                if (task->promise_ptr && task->promise_ptr->type == VALUE_PROMISE) {
                    async_resolve_promise(interpreter, task->promise_ptr, &result_to_resolve);
                }
                value_free(&result);
                value_free(&result_to_resolve);
            } else {
                // Invalid task - reject promise
                Value error = value_create_string("Invalid async task");
                if (task->promise_ptr) {
                    async_reject_promise(interpreter, task->promise_ptr, &error);
                }
                value_free(&error);
            }
        }
        
        // Free task
        if (task->args) {
            for (size_t i = 0; i < task->arg_count; i++) {
                value_free(&task->args[i]);
            }
            shared_free_safe(task->args, "bytecode_vm", "worker_thread", 0);
        }
        value_free(&task->promise_copy);
        value_free(&task->result);
        shared_free_safe(task, "bytecode_vm", "worker_thread", 1);
    }
    
    return NULL;
}

// Initialize async concurrency system
static void async_init_concurrency(Interpreter* interpreter) {
    if (!interpreter || !interpreter->async_enabled) return;
    
    // Initialize mutexes and condition variable
    pthread_mutex_init(&interpreter->task_queue_mutex, NULL);
    pthread_mutex_init(&interpreter->promise_registry_mutex, NULL);
    pthread_cond_init(&interpreter->task_available, NULL);
    interpreter->shutdown_workers = 0;
    
    // Determine number of worker threads (use CPU count or default to 4)
    #ifdef _SC_NPROCESSORS_ONLN
    long cpu_count = sysconf(_SC_NPROCESSORS_ONLN);
    interpreter->worker_thread_count = (cpu_count > 0) ? (size_t)cpu_count : 4;
    #else
    interpreter->worker_thread_count = 4;  // Default to 4 threads
    #endif
    
    // Limit to reasonable number
    if (interpreter->worker_thread_count > 16) {
        interpreter->worker_thread_count = 16;
    }
    
    // Allocate worker threads
    interpreter->worker_threads = (pthread_t*)shared_malloc_safe(
        interpreter->worker_thread_count * sizeof(pthread_t),
        "bytecode_vm", "async_init_concurrency", 0);
    
    if (!interpreter->worker_threads) {
        interpreter->worker_thread_count = 0;
        return;
    }
    
    // Start worker threads
    for (size_t i = 0; i < interpreter->worker_thread_count; i++) {
        if (pthread_create(&interpreter->worker_threads[i], NULL, async_worker_thread, interpreter) != 0) {
            // Failed to create thread - reduce count
            interpreter->worker_thread_count = i;
            break;
        }
    }
}

// Shutdown async concurrency system
static void async_shutdown_concurrency(Interpreter* interpreter) {
    if (!interpreter || interpreter->worker_thread_count == 0) return;
    
    // Signal shutdown
    pthread_mutex_lock(&interpreter->task_queue_mutex);
    interpreter->shutdown_workers = 1;
    pthread_cond_broadcast(&interpreter->task_available);
    pthread_mutex_unlock(&interpreter->task_queue_mutex);
    
    // Wait for all threads to finish
    for (size_t i = 0; i < interpreter->worker_thread_count; i++) {
        pthread_join(interpreter->worker_threads[i], NULL);
    }
    
    // Cleanup
    if (interpreter->worker_threads) {
        shared_free_safe(interpreter->worker_threads, "bytecode_vm", "async_shutdown_concurrency", 0);
        interpreter->worker_threads = NULL;
    }
    
    interpreter->worker_thread_count = 0;
    pthread_mutex_destroy(&interpreter->task_queue_mutex);
    pthread_mutex_destroy(&interpreter->promise_registry_mutex);
    pthread_cond_destroy(&interpreter->task_available);
}

void async_event_loop_run(Interpreter* interpreter) {
    if (!interpreter || !interpreter->async_enabled) return;
    
    // Initialize concurrency system if not already initialized
    if (interpreter->worker_thread_count == 0 && interpreter->async_enabled) {
        async_init_concurrency(interpreter);
    }
    
    // Process WebSocket connections (non-blocking I/O)
    // This handles message receiving, ping/pong, reconnection, etc.
    extern void websocket_process_connections(Interpreter* interpreter);
    websocket_process_connections(interpreter);
    
    // Process Gateway connections (heartbeat, message parsing, etc.)
    extern void gateway_process_all_connections(Interpreter* interpreter);
    gateway_process_all_connections(interpreter);
    
    // If using worker threads, tasks are processed concurrently by workers
    // Otherwise, fall back to synchronous processing
    if (interpreter->worker_thread_count == 0) {
        // Fallback: synchronous processing (for compatibility)
        if (!interpreter->task_queue || interpreter->task_queue_size == 0) return;
        
        while (interpreter->task_queue && interpreter->task_queue_size > 0) {
            AsyncTask* task = async_task_queue_pop(interpreter);
            if (!task) break;
            
            if (task->is_resolved) {
                // Task already completed - resolve promise
                if (task->promise_ptr && task->promise_ptr->type == VALUE_PROMISE) {
                    async_resolve_promise(interpreter, task->promise_ptr, &task->result);
                }
            } else {
                // Execute task
                BytecodeProgram* program = (BytecodeProgram*)task->program;
                if (program && task->function_id >= 0 && task->function_id < (int)program->function_count && program->functions) {
                    BytecodeFunction* func = &program->functions[task->function_id];
                    
                    // Save current environment
                    Environment* old_env = interpreter->current_environment;
                    interpreter->current_environment = task->environment ? task->environment : interpreter->global_environment;
                    
                    // Execute function - this should return the function's return value
                    Value result = bytecode_execute_function_bytecode(
                        interpreter, func, task->args, (int)task->arg_count, program);
                    
                    // bytecode_execute_function_bytecode should have already captured the return value
                    // and set it in the result. However, if interpreter->has_return is still set,
                    // that means the return value is in interpreter->return_value and wasn't
                    // captured in result. Use interpreter->return_value as the authoritative source.
                    // This can happen if bytecode_execute_function_bytecode didn't properly capture it.
                    // Also check if result is NULL - this shouldn't happen if the function returned a value
                    if (result.type == VALUE_NULL) {
                        // Result is NULL - check if interpreter->has_return is still set
                        // This means bytecode_execute_function_bytecode didn't capture the return value
                        if (interpreter->has_return && interpreter->return_value.type != VALUE_NULL) {
                            // The return value is still in interpreter->return_value
                            // Use it as the result
                            result = value_clone(&interpreter->return_value);
                            interpreter->has_return = 0;
                            value_free(&interpreter->return_value);
                            interpreter->return_value = value_create_null();
                        }
                        // If result is still NULL and has_return is not set, the function didn't return a value
                        // This is fine - we'll resolve the promise with NULL
                    }
                    
                    // At this point, result should contain the function's return value
                    // (or VALUE_NULL if the function didn't return anything)
                    // If result is still NULL, that means the function didn't return a value,
                    // which is fine - we'll resolve the promise with NULL
                    
                    // Restore environment
                    interpreter->current_environment = old_env;
                    
                    // Resolve promise with result - resolve the promise pointer so it updates the original
                    // Make sure we have a valid result (even if it's null)
                    // Clone the result before resolving to ensure we have our own copy
                    Value result_to_resolve = value_clone(&result);
                    task->result = value_clone(&result);  // Clone result for task
                    task->is_resolved = 1;
                    if (task->promise_ptr && task->promise_ptr->type == VALUE_PROMISE) {
                        // Always resolve, even if result is null (that's a valid return value)
                        async_resolve_promise(interpreter, task->promise_ptr, &result_to_resolve);
                    } else {
                        // Promise pointer is invalid - this shouldn't happen
                        // But handle it gracefully
                    }
                    value_free(&result);  // Free the original result
                    value_free(&result_to_resolve);  // Free the cloned result
                } else {
                    // Invalid task - reject promise
                    Value error = value_create_string("Invalid async task");
                    if (task->promise_ptr) {
                        async_reject_promise(interpreter, task->promise_ptr, &error);
                    }
                    value_free(&error);
                }
            }
            
            // Free task
            if (task->args) {
                for (size_t i = 0; i < task->arg_count; i++) {
                    value_free(&task->args[i]);
                }
                shared_free_safe(task->args, "bytecode_vm", "async_event_loop_run", 0);
            }
            value_free(&task->promise_copy);
            value_free(&task->result);
            // Don't free promise_ptr here - it's owned by the caller who received the promise
            // The promise_ptr will be freed when the promise Value is freed
            shared_free_safe(task, "bytecode_vm", "async_event_loop_run", 0);
        }
    }
    // Worker threads handle task execution concurrently
}

// These functions are implemented in bytecode_compiler.c
// We only implement the execution part here

// Execute bytecode without resetting the stack (for sub-programs)
static Value bytecode_execute_no_reset(BytecodeProgram* program, Interpreter* interpreter) {
    if (!program || !interpreter || !program->code || program->count == 0) {
        return value_create_null();
    }
    
    // Don't reset stacks - use existing stack state
    // Don't initialize memory optimizations (already initialized)
    
    // Set interpreter reference
    program->interpreter = interpreter;
    
    // Ensure program cache is set for function calls
    if (!interpreter->bytecode_program_cache) {
        interpreter->bytecode_program_cache = program;
    }
    
    // Execute program
    size_t pc = 0;
    Value result = value_create_null();
    
    while (pc < program->count) {
        // Check for errors before executing next instruction
        if (interpreter && interpreter_has_error(interpreter)) {
            break;
        }
        
        BytecodeInstruction* instr = &program->code[pc];
        
        
        break; // Placeholder - will implement properly
    }
    
    return result;
}

// Main execution function
Value bytecode_execute(BytecodeProgram* program, Interpreter* interpreter, int debug) {
    fprintf(stderr, "[DEBUG EXEC] bytecode_execute: START (program=%p)\n", (void*)program);
    fflush(stderr);
    
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
    
    // Ensure program cache is set for function calls
    if (!interpreter->bytecode_program_cache) {
        interpreter->bytecode_program_cache = program;
    }
    
    // Execute main program
    size_t pc = 0;
    Value result = value_create_null();
    
    if (debug) {
    }
    
    while (pc < program->count) {
        // Check for errors before executing next instruction (like AST interpreter)
        // But allow execution to continue if we're inside a try block (try_depth > 0)
        // This allows BC_TRY_END and BC_CATCH to handle the error
        if (!program->code) {
            if (interpreter) {
                interpreter_set_error(interpreter, "Bytecode program code is NULL", 0, 0);
            }
            break;
        }
        BytecodeInstruction* instr = &program->code[pc];
        
        // If we have an error and we're not in a try block, clear error and continue execution
        // Error was already reported by interpreter_set_error
        if (interpreter && interpreter_has_error(interpreter) && interpreter->try_depth == 0) {
            interpreter_clear_error(interpreter);
        }
        
        // If we have an error and we're in a try block, skip instructions until BC_TRY_END or BC_CATCH
        if (interpreter && interpreter_has_error(interpreter) && interpreter->try_depth > 0) {
            // Skip instructions until we find BC_TRY_END or BC_CATCH
            if (instr->op != BC_TRY_END && instr->op != BC_CATCH) {
                pc++;
                continue;
            }
            // Found BC_TRY_END or BC_CATCH - continue normal execution to handle the error
        }
        
        // Prefetch next instruction for better cache performance
        if (LIKELY(pc + 1 < program->count)) {
            PREFETCH_READ(&program->code[pc + 1]);
        }
        
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
                    // Unknown superinstruction - report error and fail (like LuaJIT)
                    if (interpreter) {
                        char error_msg[256];
                        snprintf(error_msg, sizeof(error_msg), "Unknown superinstruction opcode: %d at PC %zu", instr->op, pc);
                        interpreter_set_error(interpreter, error_msg, 0, 0);
                    }
                    value_stack_push(value_create_null());
                    pc++;
                    break;
                }
            }
        } else {
            // Handle regular bytecode operations
            switch (instr->op) {
            case BC_LOAD_CONST: {
                if (LIKELY(instr->a < program->const_count)) {
                    Value const_val = program->constants[instr->a];
                    // String constants are already processed during compilation, so just clone them
                    // This avoids reprocessing escape sequences and allocating new buffers every time
                    if (LIKELY(const_val.type == VALUE_STRING)) {
                        value_stack_push(value_clone(&const_val));
                    } else if (LIKELY(const_val.type == VALUE_NUMBER)) {
                        value_stack_push(fast_create_number(program, const_val.data.number_value));
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
                if (LIKELY(instr->a < program->local_slot_count)) {
                    // Check if variable exists in environment (may have been updated by AST interpreter)
                    // If so, use the environment value; otherwise use local slot
                    Value local_val = program->locals[instr->a];
                    if (interpreter && interpreter->current_environment && program->local_names && 
                        instr->a >= 0 && (size_t)instr->a < program->local_count) {
                        const char* var_name = program->local_names[instr->a];
                        if (var_name && environment_exists(interpreter->current_environment, var_name)) {
                            // Variable exists in environment - use that (may be newer)
                            Value env_val = environment_get(interpreter->current_environment, var_name);
                            
                            // Also check global environment for modules
                            if (env_val.type == VALUE_NULL && interpreter->global_environment) {
                                env_val = environment_get(interpreter->global_environment, var_name);
                            }
                            
                            // Update local slot to match environment if local is Null or different
                            if (local_val.type == VALUE_NULL || 
                                (local_val.type == VALUE_OBJECT && env_val.type == VALUE_OBJECT && 
                                 local_val.data.object_value.count != env_val.data.object_value.count)) {
                                value_free(&program->locals[instr->a]);
                                program->locals[instr->a] = value_clone(&env_val);
                            }
                            // Push the environment value (clone it since value_stack_push does shallow copy)
                            Value cloned_env_val = value_clone(&env_val);
                            value_free(&env_val);
                            value_stack_push(cloned_env_val);
                        } else {
                            // Variable not in environment - use local slot
                    value_stack_push(value_clone(&local_val));
                        }
                    } else {
                        // No environment or local_names - use local slot
                        value_stack_push(value_clone(&local_val));
                    }
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_LOAD_VAR: {
                // Load variable from environment
                if (LIKELY(instr->a < program->const_count)) {
                    Value var_name = program->constants[instr->a];
                    if (LIKELY(var_name.type == VALUE_STRING)) {
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
                    
                    // Also update numeric locals if this is a number
                    if (val.type == VALUE_NUMBER && instr->a < program->num_local_count) {
                        program->num_locals[instr->a] = val.data.number_value;
                    }
                    
                    // For complex types (objects, arrays), clone to ensure internal pointers are valid
                    // For simple types (numbers, booleans, null), store directly
                    Value stored_val;
                    if (val.type == VALUE_OBJECT || val.type == VALUE_ARRAY || val.type == VALUE_FUNCTION) {
                        stored_val = value_clone(&val);
                        value_free(&val);
                    } else {
                        // Store the value directly (don't clone) - the value on stack is already a clone from BC_LOAD_LOCAL
                        stored_val = val;
                    }
                    program->locals[instr->a] = stored_val;
                    
                    // Also store in environment so AST-interpreted code (like for loop bodies) can access it
                    // Find the variable name from the local slot index
                    if (interpreter && interpreter->current_environment && program->local_names && 
                        instr->a >= 0 && (size_t)instr->a < program->local_count) {
                        const char* var_name = program->local_names[instr->a];
                        if (var_name) {
                            // Store in environment (clones the value)
                            // Use stored_val which is already cloned for complex types
                            if (environment_exists(interpreter->current_environment, var_name)) {
                                environment_assign(interpreter->current_environment, var_name, stored_val);
                            } else {
                                environment_define(interpreter->current_environment, var_name, stored_val);
                            }
                        }
                    }
                    
                    // Free val only if we cloned it (stored_val is a separate copy)
                    if (val.type == VALUE_OBJECT || val.type == VALUE_ARRAY || val.type == VALUE_FUNCTION) {
                        value_free(&val);
                    }
                }
                pc++;
                break;
            }
            
            case BC_LOAD_GLOBAL: {
                // Load global variable by name
                // Check current environment first (for loop variables, local scope), then global
                Value loaded_val = value_create_null();
                if (instr->a >= 0 && instr->a < (int)program->const_count && program->constants) {
                    if (program->constants[instr->a].type == VALUE_STRING) {
                    const char* var_name = program->constants[instr->a].data.string_value;
                        
                        // Try current environment first (for loop variables, local scope)
                        // environment_get already checks parent chain
                        if (var_name && interpreter && interpreter->current_environment) {
                            loaded_val = environment_get(interpreter->current_environment, var_name);
                        }
                        
                        // If not found in current, try global environment (where modules are stored)
                        if (loaded_val.type == VALUE_NULL && var_name && interpreter && interpreter->global_environment) {
                            loaded_val = environment_get(interpreter->global_environment, var_name);
                        }
                    }
                }
                value_stack_push(loaded_val);
                pc++;
                break;
            }
            
            case BC_STORE_GLOBAL: {
                // Store global variable by name
                // Store in current environment if available, otherwise global environment
                if (instr->a < program->const_count && program->constants[instr->a].type == VALUE_STRING) {
                    const char* var_name = program->constants[instr->a].data.string_value;
                    Value val = value_stack_pop();
                    Environment* target_env = interpreter->current_environment ? interpreter->current_environment : interpreter->global_environment;
                    
                    // Use environment_assign to update existing variable, or environment_define if it doesn't exist
                    // environment_assign/define will clone the value internally
                    if (environment_exists(target_env, var_name)) {
                        environment_assign(target_env, var_name, val);
                    } else {
                        environment_define(target_env, var_name, val);
                    }
                    // Free the value we just stored (it was cloned by environment_assign/define)
                    value_free(&val);
                    // Don't push the value back - BC_STORE_GLOBAL is a statement, not an expression
                    // The value is already stored in the environment, no need to keep it on the stack // environment_get already returns a clone
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
                if (LIKELY(a.type == VALUE_STRING && b.type == VALUE_STRING)) {
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
            
            case BC_MOD: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                Value result = value_modulo(&a, &b);
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
            
            case BC_AND: {
                // Logical AND: a && b (short-circuit evaluation)
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                
                // Convert both to boolean
                Value a_bool = value_to_boolean(&a);
                Value b_bool = value_to_boolean(&b);
                
                // Short-circuit: if a is false, return false without evaluating b
                bool a_is_true = (a_bool.type == VALUE_BOOLEAN && a_bool.data.boolean_value);
                bool b_is_true = (b_bool.type == VALUE_BOOLEAN && b_bool.data.boolean_value);
                
                Value result = value_create_boolean(a_is_true && b_is_true);
                
                value_free(&a);
                value_free(&b);
                value_free(&a_bool);
                value_free(&b_bool);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_OR: {
                // Logical OR: a || b (short-circuit evaluation)
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                
                // Convert both to boolean
                Value a_bool = value_to_boolean(&a);
                Value b_bool = value_to_boolean(&b);
                
                // Short-circuit: if a is true, return true without evaluating b
                bool a_is_true = (a_bool.type == VALUE_BOOLEAN && a_bool.data.boolean_value);
                bool b_is_true = (b_bool.type == VALUE_BOOLEAN && b_bool.data.boolean_value);
                
                Value result = value_create_boolean(a_is_true || b_is_true);
                
                value_free(&a);
                value_free(&b);
                value_free(&a_bool);
                value_free(&b_bool);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_JUMP: {
                // Validate jump target to prevent jumping out of bounds
                if (instr->a >= 0 && instr->a < (int)program->count) {
                    // Jump to target - even if it's BC_HALT, let the VM loop handle it naturally
                    pc = instr->a;
                } else {
                    // Invalid jump target - stop execution
                    if (interpreter) {
                        interpreter_set_error(interpreter, "Invalid jump target in bytecode", 0, 0);
                    }
                    goto cleanup;
                }
                break;
            }
            
            case BC_JUMP_IF_FALSE: {
                // Check if stack is empty (shouldn't happen, but handle gracefully)
                if (value_stack_size == 0) {
                    if (interpreter) {
                        interpreter_set_error(interpreter, "Stack underflow in BC_JUMP_IF_FALSE - condition not on stack", 0, 0);
                    }
                    goto cleanup;
                }
                
                Value condition = value_stack_pop();
                // Convert to boolean if needed
                Value bool_condition = value_to_boolean(&condition);
                bool should_jump = (bool_condition.type == VALUE_BOOLEAN && !bool_condition.data.boolean_value);
                value_free(&bool_condition);
                
                if (should_jump) {
                    // Validate jump target to prevent jumping out of bounds
                    if (instr->a >= 0 && instr->a < (int)program->count) {
                    pc = instr->a;
                    } else {
                        // Invalid jump target - stop execution
                        if (interpreter) {
                            interpreter_set_error(interpreter, "Invalid jump target in BC_JUMP_IF_FALSE", 0, 0);
                        }
                        value_free(&condition);
                        goto cleanup;
                    }
                } else {
                    pc++;
                }
                value_free(&condition);
                break;
            }
            
            case BC_NOT: {
                // Logical NOT: convert value to boolean and negate
                Value operand = value_stack_pop();
                Value bool_val = value_to_boolean(&operand);
                // Check if the boolean is true (both type and value must be true)
                bool is_true = (bool_val.type == VALUE_BOOLEAN && bool_val.data.boolean_value);
                // Negate it
                Value result = value_create_boolean(!is_true);
                value_free(&operand);
                value_free(&bool_val);
                value_stack_push(result);
                pc++;
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
                    // Stack order: compiler pushes object first (line 1831), then argument (line 1983)
                    // So stack is: [object, arg1] with arg1 on top
                    // Match BC_ARRAY_PUSH exactly: pop argument first (top), then object (below)
                    // Match BC_CALL_BUILTIN: pop arguments in reverse order
                    // Match BC_ARRAY_PUSH exactly: pop argument first (top), then object (below)
                    // BC_ARRAY_PUSH: Value val = value_stack_pop(); Value arr = value_stack_pop();
                    // Stack is [object, arg1] with arg1 on top
                    // Match BC_ARRAY_PUSH exactly: pop argument first (top), then object (below)
                    // BC_ARRAY_PUSH: Value val = value_stack_pop(); Value arr = value_stack_pop();
                    // Stack is [object, arg1] with arg1 on top
                    // For single arg: pop arg first (top), then object (below)
                    // Check stack size before popping to ensure arguments are present
                    if (value_stack_size < (size_t)(arg_count + 1)) {
                        // Stack doesn't have enough values - arguments weren't pushed correctly
                        value_stack_push(value_create_null());
                        pc++;
                        break;
                    }
                    // Match BC_ARRAY_PUSH exactly: pop argument first (top), then object (below)
                    // BC_ARRAY_PUSH: Value val = value_stack_pop(); Value arr = value_stack_pop();
                    // For arrays, stack is [object, arg1] with arg1 on top
                    // Pop arg first (top), then object (below)
                    if (arg_count > 0) {
                        args = shared_malloc_safe(arg_count * sizeof(Value), "bytecode_vm", "BC_METHOD_CALL", 7);
                        // Pop arguments in reverse order (last arg is on top)
                        // For single arg: args[0] = pop() = arg1 (top)
                        for (int i = 0; i < arg_count; i++) {
                            args[arg_count - 1 - i] = value_stack_pop();
                        }
                    }
                    // Pop object (it's below the arguments)
                    object = value_stack_pop();
                    // Handle different object types
                    if (object.type == VALUE_ARRAY) {
                        // Handle array methods
                        if (strcmp(method_name, "push") == 0 && arg_count == 1) {
                            // value_array_push clones the element internally
                            value_array_push(&object, args[0]);
                            // Transfer ownership of object to stack (don't free it)
                            value_stack_push(object);
                        } else if (strcmp(method_name, "pop") == 0 && arg_count <= 1) {
                            // pop() can take 0 or 1 argument (index)
                            int pop_index = -1; // Default: pop last element
                            if (arg_count == 1 && args[0].type == VALUE_NUMBER) {
                                pop_index = (int)args[0].data.number_value;
                            }
                            Value result = value_array_pop(&object, pop_index);
                            // Push popped value first, then modified array (consistent with BC_ARRAY_POP)
                            // Stack order: [popped_value, modified_array] with modified_array on top
                            value_stack_push(result);
                            // Transfer ownership of object to stack (don't free it)
                            value_stack_push(object);
                        } else {
                            value_stack_push(value_create_null());
                            value_free(&object);
                        }
                        // Clean up arguments
                        if (args) {
                            for (int i = 0; i < arg_count; i++) {
                                value_free(&args[i]);
                            }
                            shared_free_safe(args, "bytecode_vm", "BC_METHOD_CALL", 16);
                        }
                        pc++;
                        break;
                    } else if (object.type == VALUE_HASH_MAP) {
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
                        // Check if it's a module object first
                        Value object_type = value_object_get(&object, "__type__");
                        bool method_handled = false;
                        BytecodeProgram* saved_cache_for_module = NULL;
                        
                        if (object_type.type == VALUE_STRING && strcmp(object_type.data.string_value, "Module") == 0) {
                            // It's a module object - get module path and look up bytecode program
                            Value module_path_val = value_object_get(&object, "__module_path__");
                            if (module_path_val.type == VALUE_STRING && module_path_val.data.string_value && interpreter && interpreter->module_cache) {
                                // Find module in cache and temporarily set bytecode program cache
                                ModuleCacheEntry* module_entry = find_cached_module(interpreter, module_path_val.data.string_value);
                                if (module_entry && module_entry->is_valid && module_entry->module_bytecode_program) {
                                    saved_cache_for_module = interpreter->bytecode_program_cache;
                                    interpreter->bytecode_program_cache = (BytecodeProgram*)module_entry->module_bytecode_program;
                                }
                            }
                            value_free(&module_path_val);
                            
                            // Get method from module
                            Value method = value_object_get(&object, method_name);
                            if (method.type == VALUE_FUNCTION) {
                                // Call the method - value_function_call handles both builtin and user functions
                                Value result = value_function_call(&method, args, arg_count, interpreter, 0, 0);
                                Value cloned_result = value_clone(&result);
                                value_free(&result);  // Free original result
                                value_stack_push(cloned_result);  // Push cloned result (stack doesn't clone)
                                method_handled = true;
                            } else {
                                value_stack_push(value_create_null());
                                method_handled = true;
                            }
                            value_free(&method);
                            
                            // Restore cache if we changed it
                            if (saved_cache_for_module && interpreter) {
                                interpreter->bytecode_program_cache = saved_cache_for_module;
                            }
                        } else if (object_type.type == VALUE_STRING && strcmp(object_type.data.string_value, "Library") == 0) {
                            // It's a library object - get method directly
                            Value method = value_object_get(&object, method_name);
                            if (method.type == VALUE_FUNCTION) {
                                // Call the method - value_function_call handles both builtin and user functions
                                Value result = value_function_call(&method, args, arg_count, interpreter, 0, 0);
                                Value cloned_result = value_clone(&result);
                                value_free(&result);  // Free original result
                                value_stack_push(cloned_result);  // Push cloned result (stack doesn't clone)
                                method_handled = true;
                            } else {
                                value_stack_push(value_create_null());
                                method_handled = true;
                            }
                            value_free(&method);
                        }
                        value_free(&object_type);
                        
                        // Only check other conditions if method wasn't handled
                        if (!method_handled) {
                            // CRITICAL OPTIMIZATION: For known Window.isOpen() calls, skip all lookups
                            // This avoids value_object_get allocations for both class_name and method
                            // Only apply this for Window objects - check __class_name__ first to avoid false matches
                            // We do a minimal check: if it's an object with isOpen method and 0 args,
                            // and it has a __class_name__ property that we can quickly verify
                            if (arg_count == 0 && strcmp(method_name, "isOpen") == 0 && object.type == VALUE_OBJECT) {
                                // Quick check: peek at __class_name__ without cloning to see if it's "Window"
                                // This is a minimal check to avoid false matches with other objects
                                bool is_window = false;
                                for (size_t i = 0; i < object.data.object_value.count; i++) {
                                    if (object.data.object_value.keys[i] && 
                                        strcmp(object.data.object_value.keys[i], "__class_name__") == 0) {
                                        Value* class_name_val = (Value*)object.data.object_value.values[i];
                                        if (class_name_val && class_name_val->type == VALUE_STRING &&
                                            strcmp(class_name_val->data.string_value, "Window") == 0) {
                                            is_window = true;
                                        }
                                        break;
                                    }
                                }
                                
                                if (is_window) {
                                    // Fast path: Direct call to isOpen without any lookups
                                    Value method_args[1];
                                    method_args[0] = object; // Pass object directly without cloning
                                    Value result = builtin_graphics_is_open(interpreter, method_args, 1, 0, 0);
                                    value_stack_push(result);
                                    // Free object - it was popped from stack and we're done with it
                                    value_free(&object);
                                    // Don't free args - there are none (arg_count == 0)
                                    if (args) {
                                        shared_free_safe(args, "bytecode_vm", "BC_METHOD_CALL", 16);
                                    }
                                    pc++;
                                    break;
                                }
                            }
                            
                            // Check if it's a library instance (has __class_name__ but not a VALUE_CLASS)
                            Value class_name = value_object_get(&object, "__class_name__");
                        if (class_name.type == VALUE_STRING) {
                            // Check if it's a Server or Window instance (needs special handling - pass object as first arg)
                            if (strcmp(class_name.data.string_value, "Server") == 0 ||
                                strcmp(class_name.data.string_value, "Window") == 0) {
                                // Server and Window methods need object as first argument
                                Value method = {0};
                                bool method_found = false;
                                bool is_builtin = false;
                                
                                // Look up the method
                                method = value_object_get(&object, method_name);
                                if (method.type == VALUE_FUNCTION) {
                                    method_found = true;
                                    is_builtin = (method.data.function_value.body != NULL &&
                                        method.data.function_value.parameter_count == 0 &&
                                                       !method.data.function_value.parameters &&
                                                       (uintptr_t)method.data.function_value.body > 0x100);
                                }
                                
                                if (method_found && is_builtin) {
                                        // This is a builtin function - call it with object as first argument
                                        Value (*builtin_func)(Interpreter*, Value*, size_t, int, int) = 
                                            (Value (*)(Interpreter*, Value*, size_t, int, int))method.data.function_value.body;
                                    
                                    // Optimize zero-argument method calls to avoid allocations in tight loops
                                    Value result;
                                    if (arg_count == 0) {
                                        // Zero arguments - use stack-allocated array to avoid malloc
                                        // CRITICAL: Don't clone the object - builtin functions don't modify it
                                        Value method_args[1];
                                        method_args[0] = object; // Pass object directly without cloning
                                        result = builtin_func(interpreter, method_args, 1, 0, 0);
                                        // Don't free method_args[0] - it's the original object, not a clone
                                    } else {
                                        // Has arguments - allocate array
                                        Value* method_args = shared_malloc_safe((arg_count + 1) * sizeof(Value), "bytecode_vm", "BC_METHOD_CALL", 13);
                                        method_args[0] = value_clone(&object); // self as first argument
                                        for (int i = 0; i < arg_count; i++) {
                                            method_args[i + 1] = value_clone(&args[i]);
                                        }
                                        result = builtin_func(interpreter, method_args, arg_count + 1, 0, 0);
                                        // Clean up method arguments
                                        for (int i = 0; i < arg_count + 1; i++) {
                                            value_free(&method_args[i]);
                                        }
                                        shared_free_safe(method_args, "bytecode_vm", "BC_METHOD_CALL", 14);
                                    }
                                        value_stack_push(result);
                                        value_free(&method);
                                        value_free(&class_name);
                                        value_free(&object);
                                        // Clean up arguments
                                        if (args) {
                                            for (int i = 0; i < arg_count; i++) {
                                                value_free(&args[i]);
                                            }
                                            shared_free_safe(args, "bytecode_vm", "BC_METHOD_CALL", 15);
                                        }
                                        pc++;
                                        break;
                                }
                                value_free(&method);
                            }
                            // Check if it's a library instance (Tree, Graph, Heap, etc.)
                            if (strcmp(class_name.data.string_value, "Tree") == 0 ||
                                strcmp(class_name.data.string_value, "Graph") == 0 ||
                                strcmp(class_name.data.string_value, "Heap") == 0 ||
                                strcmp(class_name.data.string_value, "Queue") == 0 ||
                                strcmp(class_name.data.string_value, "Stack") == 0) {
                                // It's a library instance - get method directly from object
                                Value method = value_object_get(&object, method_name);
                                if (method.type == VALUE_FUNCTION) {
                                    // Check if this is a method that uses self_context (heaps, queues, stacks)
                                    // vs explicit self argument (trees, graphs)
                                    bool uses_self_context = (strcmp(class_name.data.string_value, "Heap") == 0 ||
                                                             strcmp(class_name.data.string_value, "Queue") == 0 ||
                                                             strcmp(class_name.data.string_value, "Stack") == 0);
                                    
                                    Value result;
                                    if (uses_self_context) {
                                        // For heaps/queues/stacks, use self_context
                                        // The _method versions will get self from interpreter_get_self_context
                                        // value_function_call_with_self now sets self_context for builtin functions
                                        result = value_function_call_with_self(&method, args, arg_count, interpreter, &object, 0, 0);
                                        // Now safe to free object
                                        value_free(&object);
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
                                        // Free object after call
                                        value_free(&object);
                                    }
                                    
                                    // Push result directly - don't clone or free
                                    // BC_STORE_LOCAL will clone when storing, which is sufficient
                                    value_stack_push(result);
                                } else {
                                    value_stack_push(value_create_null());
                                    // Free object if method not found
                                    value_free(&object);
                                }
                                value_free(&method);
                                value_free(&class_name);
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
                                // Check if this is a builtin function (function pointer stored in body field)
                                // Builtin functions have parameter_count == 0, no parameters, and body is a function pointer
                                // Function pointers are typically in the code segment, not heap-allocated AST nodes
                                bool is_builtin = (method.data.function_value.body != NULL &&
                                    method.data.function_value.parameter_count == 0 &&
                                                   !method.data.function_value.parameters &&
                                                   (uintptr_t)method.data.function_value.body > 0x100);
                                if (is_builtin) {
                                    // This is a builtin function - call it with object as first argument
                                    Value (*builtin_func)(Interpreter*, Value*, size_t, int, int) = 
                                        (Value (*)(Interpreter*, Value*, size_t, int, int))method.data.function_value.body;
                                    // Optimize zero-argument method calls to avoid allocations in tight loops
                                    Value result;
                                    if (arg_count == 0) {
                                        // Zero arguments - use stack-allocated array to avoid malloc
                                        Value method_args[1];
                                        method_args[0] = value_clone(&object); // self as first argument
                                        result = builtin_func(interpreter, method_args, 1, 0, 0);
                                        value_free(&method_args[0]);
                                    } else {
                                        // Has arguments - allocate array
                                    Value* method_args = shared_malloc_safe((arg_count + 1) * sizeof(Value), "bytecode_vm", "BC_METHOD_CALL", 11);
                                    method_args[0] = value_clone(&object); // self as first argument
                                    for (int i = 0; i < arg_count; i++) {
                                        method_args[i + 1] = value_clone(&args[i]);
                                    }
                                        result = builtin_func(interpreter, method_args, arg_count + 1, 0, 0);
                                    // Clean up method arguments
                                    for (int i = 0; i < arg_count + 1; i++) {
                                        value_free(&method_args[i]);
                                    }
                                    shared_free_safe(method_args, "bytecode_vm", "BC_METHOD_CALL", 12);
                                    }
                                    value_stack_push(result);
                                } else {
                                    // Regular user function - use self context
                                    interpreter_set_self_context(interpreter, &object);
                                    Value result = value_function_call_with_self(&method, args, arg_count, interpreter, &object, 0, 0);
                                    interpreter_set_self_context(interpreter, NULL);
                                    value_stack_push(result);
                                }
                            } else {
                                value_stack_push(value_create_null());
                            }
                            value_free(&method);
                        }
                        value_free(&class_name);
                        }  // Close if (!library_method_handled)
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
                        // For objects, check for regular "type" property first (matches AST behavior)
                        if (object.type == VALUE_OBJECT) {
                            Value regular_type = value_object_get(&object, "type");
                            if (regular_type.type != VALUE_NULL) {
                                // Regular "type" property exists - use it
                                value_stack_push(regular_type);
                                value_free(&object);
                                pc++;
                                break;
                            }
                            value_free(&regular_type);
                            
                            // No regular "type" property - check for __type__ override (e.g., Library)
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
                        
                        // For functions, return "Function"
                        if (object.type == VALUE_FUNCTION) {
                            value_stack_push(value_create_string("Function"));
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
                // Call built-in function by name
                // instr->a = function name constant index, instr->b = argument count
                if (instr->a < program->const_count && program->constants[instr->a].type == VALUE_STRING) {
                    const char* func_name = program->constants[instr->a].data.string_value;
                    size_t arg_count = (size_t)instr->b;
                    
                    // Pop arguments from stack (in reverse order)
                    Value* args = NULL;
                    if (arg_count > 0) {
                        args = (Value*)shared_malloc_safe(arg_count * sizeof(Value), "bytecode_vm", "BC_CALL_BUILTIN", 1);
                        if (!args) {
                            // Allocation failed - pop all arguments and return null
                            for (size_t i = 0; i < arg_count; i++) {
                                Value val = value_stack_pop();
                                value_free(&val);
                            }
                            value_stack_push(value_create_null());
                            pc++;
                            break;
                        }
                        
                        // Pop arguments in reverse order (they were pushed in forward order)
                        for (size_t i = 0; i < arg_count; i++) {
                            args[arg_count - 1 - i] = value_stack_pop();
                        }
                    }
                    
                    // Look up and call builtin function
                    Value result = value_create_null();
                    
                    // Handle common builtin functions
                    if (strcmp(func_name, "print") == 0) {
                        result = builtin_print(interpreter, args, arg_count, 0, 0);
                    } else if (strcmp(func_name, "input") == 0) {
                        result = builtin_input(interpreter, args, arg_count, 0, 0);
                    } else if (strcmp(func_name, "len") == 0) {
                        result = builtin_len(interpreter, args, arg_count, 0, 0);
                    } else if (strcmp(func_name, "assert") == 0) {
                        result = builtin_assert(interpreter, args, arg_count, 0, 0);
                    } else if (strcmp(func_name, "int") == 0) {
                        result = builtin_int(interpreter, args, arg_count, 0, 0);
                    } else if (strcmp(func_name, "float") == 0) {
                        result = builtin_float(interpreter, args, arg_count, 0, 0);
                    } else if (strcmp(func_name, "bool") == 0) {
                        result = builtin_bool(interpreter, args, arg_count, 0, 0);
                    } else {
                        // Try to get builtin function from environment
                        Value builtin_func = environment_get(interpreter->global_environment, func_name);
                        if (builtin_func.type == VALUE_FUNCTION) {
                            // Check if it's a builtin (function pointer in body)
                            if (builtin_func.data.function_value.body && 
                                builtin_func.data.function_value.parameter_count == 0 &&
                                !builtin_func.data.function_value.parameters) {
                                Value (*func_ptr)(Interpreter*, Value*, size_t, int, int) = 
                                    (Value (*)(Interpreter*, Value*, size_t, int, int))builtin_func.data.function_value.body;
                                if (func_ptr) {
                                    result = func_ptr(interpreter, args, arg_count, 0, 0);
                                }
                            } else {
                                // Regular function call
                                result = value_function_call(&builtin_func, args, arg_count, interpreter, 0, 0);
                            }
                            value_free(&builtin_func);
                        } else {
                            value_free(&builtin_func);
                            // Builtin not found - return null
                            result = value_create_null();
                        }
                    }
                    
                    // Free arguments
                    if (args) {
                        for (size_t i = 0; i < arg_count; i++) {
                            value_free(&args[i]);
                        }
                        shared_free_safe(args, "bytecode_vm", "BC_CALL_BUILTIN", 2);
                    }
                    
                    value_stack_push(result);
                } else {
                    // Invalid function name - return null
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_RETURN: {
                // Set return flag and value for function execution
                if (instr->a == 0) {
                    // Void return
                    interpreter->has_return = 1;
                    interpreter->return_value = value_create_null();
                    result = value_create_null();
                } else {
                    // Value return
                    Value return_val = value_stack_pop();
                    interpreter->has_return = 1;
                    interpreter->return_value = value_clone(&return_val);
                    result = value_clone(&return_val);  // Clone for result too
                    value_free(&return_val);  // Free the popped value
                }
                goto cleanup;
            }
            
            case BC_CALL_USER_FUNCTION: {
                // Call user-defined function: func(args...)
                // instr->a = function index, instr->b = argument count
                int func_id = instr->a;
                int arg_count = instr->b;
                
                // Get the function from the main program (cache) to ensure recursive calls work
                BytecodeProgram* func_program = program;
                if (interpreter && interpreter->bytecode_program_cache) {
                    func_program = interpreter->bytecode_program_cache;
                }
                
                if (func_id >= 0 && func_id < (int)func_program->function_count && func_program->functions) {
                    BytecodeFunction* func = &func_program->functions[func_id];
                    
                    // Get arguments from stack
                    Value* args = NULL;
                    if (arg_count > 0) {
                        args = shared_malloc_safe(arg_count * sizeof(Value), "bytecode_vm", "BC_CALL_USER_FUNCTION", 1);
                        if (!args) {
                            // Allocation failed - return null
                            value_stack_push(value_create_null());
                            pc++;
                            break;
                        }
                        for (int i = 0; i < arg_count; i++) {
                            args[arg_count - 1 - i] = value_stack_pop();
                        }
                    }
                    
                    // Save stack size before function call
                    size_t stack_size_before = value_stack_size;
                    
                    // Execute function bytecode using the main program
                    Value result = bytecode_execute_function_bytecode(interpreter, func, args, arg_count, func_program);
                    
                    // Clean up arguments
                    if (args) {
                        for (int i = 0; i < arg_count; i++) {
                            value_free(&args[i]);
                        }
                        shared_free_safe(args, "bytecode_vm", "BC_CALL_USER_FUNCTION", 2);
                    }
                    
                    // Check if return value was already pushed onto stack by bytecode_execute_function_bytecode
                    // (it pushes the return value onto the restored stack)
                    if (value_stack_size > stack_size_before) {
                        // Return value was already pushed, just free the result
                        value_free(&result);
                    } else {
                        // Return value was not pushed, push it now
                    value_stack_push(result);
                    }
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_CALL_FUNCTION_VALUE: {
                // Call function value from stack: func(args...)
                // instr->a = argument count
                // This can also be a class instantiation if the value is a class
                int arg_count = instr->a;
                // Get function/value from stack (should be on top, after arguments)
                Value func_value = value_stack_pop();
                
                // Get arguments from stack (in reverse order)
                Value* args = NULL;
                if (arg_count > 0) {
                    args = shared_malloc_safe(arg_count * sizeof(Value), "bytecode_vm", "BC_CALL_FUNCTION_VALUE", 1);
                    if (!args) {
                        value_free(&func_value);
                        value_stack_push(value_create_null());
                        pc++;
                        break;
                    }
                    for (int i = 0; i < arg_count; i++) {
                        args[arg_count - 1 - i] = value_stack_pop();
                    }
                }
                
                // Check if this is a class (runtime class detection for forward references)
                Value result = value_create_null();
                if (func_value.type == VALUE_CLASS) {
                    // This is a class instantiation - instantiate it
                    Value class_value = func_value;
                    
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
                                        // Compile field initializer to bytecode and execute
                                        BytecodeProgram* field_init_bytecode = bytecode_compile_ast(stmt->data.variable_declaration.initial_value, interpreter);
                                        if (field_init_bytecode) {
                                            field_value = interpreter_execute_bytecode(interpreter, field_init_bytecode);
                                            bytecode_program_free(field_init_bytecode);
                                        } else {
                                            field_value = value_create_null();
                                        }
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
                        shared_free_safe(all_fields, "bytecode_vm", "BC_CALL_FUNCTION_VALUE", 2);
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
                        Value constructor_result = value_function_call(&init_func, NULL, arg_count + 1, interpreter, 0, 0);
                        value_free(&constructor_result);
                        
                        // Get the updated instance from stack
                        instance = value_stack_pop();
                    }
                    
                    value_free(&init_func);
                    result = instance;
                } else if (func_value.type == VALUE_ASYNC_FUNCTION) {
                    // Async function call - create async task
                    // Check if this is a bytecode async function
                    ASTNode* body_ptr = (ASTNode*)func_value.data.async_function_value.body;
                    uintptr_t body_addr = (uintptr_t)body_ptr;
                    if (body_addr < 10000) {
                        // This is a bytecode async function
                        int func_id = (int)body_addr;
                        BytecodeProgram* func_program = interpreter && interpreter->bytecode_program_cache ? 
                            interpreter->bytecode_program_cache : program;
                        
                        if (func_id >= 0 && func_id < (int)func_program->function_count) {
                            // Create pending promise and register it
                            Value promise = value_create_pending_promise();
                            uint64_t promise_id = promise_registry_add(interpreter, promise);
                            
                            if (promise_id == 0) {
                                // Registry allocation failed - free args and return null
                                if (args) {
                                    for (int i = 0; i < arg_count; i++) {
                                        value_free(&args[i]);
                                    }
                                    shared_free_safe(args, "bytecode_vm", "BC_CALL_FUNCTION_VALUE", 0);
                                }
                                result = value_create_null();
                            } else {
                                // Get pointer to promise in registry
                                Value* registry_promise = promise_registry_get(interpreter, promise_id);
                                if (!registry_promise) {
                                    // Registry lookup failed - remove from registry, free args, return null
                                    promise_registry_remove(interpreter, promise_id);
                                    if (args) {
                                        for (int i = 0; i < arg_count; i++) {
                                            value_free(&args[i]);
                                        }
                                        shared_free_safe(args, "bytecode_vm", "BC_CALL_FUNCTION_VALUE", 1);
                                    }
                                    result = value_create_null();
                                } else {
                                    // Ensure promise has the ID set (it should already, but be safe)
                                    registry_promise->data.promise_value.promise_id = promise_id;
                                    
                                    // Create async task
                                    AsyncTask* task = (AsyncTask*)shared_malloc_safe(sizeof(AsyncTask), "bytecode_vm", "BC_CALL_FUNCTION_VALUE", 2);
                                    if (task) {
                                        task->promise_ptr = registry_promise;  // Get pointer from registry
                                        task->promise_copy = *registry_promise;  // Keep a copy for reference
                                        task->program = func_program;
                                        task->function_id = func_id;
                                        task->args = args; // Transfer ownership
                                        task->arg_count = arg_count;
                                        task->environment = interpreter ? interpreter->current_environment : NULL;
                                        task->is_resolved = 0;
                                        task->result = value_create_null();
                                        
                                        // Add to task queue
                                        async_task_queue_add(interpreter, task);
                                        
                                        // Don't free args here - task owns them now
                                        args = NULL;
                                        
                                        // Return the promise with ID set (caller will free it, which will remove it from registry)
                                        result = *registry_promise;
                                        result.data.promise_value.promise_id = promise_id;  // Ensure ID is set
                                    } else {
                                        // Task allocation failed - remove from registry, free args, create rejected promise
                                        promise_registry_remove(interpreter, promise_id);
                                        if (args) {
                                            for (int i = 0; i < arg_count; i++) {
                                                value_free(&args[i]);
                                            }
                                            shared_free_safe(args, "bytecode_vm", "BC_CALL_FUNCTION_VALUE", 3);
                                        }
                                        Value error = value_create_string("Failed to create async task");
                                        result = value_create_promise(value_create_null(), 0, error);
                                        value_free(&error);
                                    }
                                }
                            }
                        } else {
                            // Invalid function ID
                            if (args) {
                                for (int i = 0; i < arg_count; i++) {
                                    value_free(&args[i]);
                                }
                                shared_free_safe(args, "bytecode_vm", "BC_CALL_FUNCTION_VALUE", 4);
                            }
                            result = value_create_null();
                        }
                    } else {
                        // AST async function - execute synchronously for now
                        // TODO: Implement proper async execution for AST functions
                        result = value_function_call(&func_value, args, arg_count, interpreter, 0, 0);
                        // Wrap result in resolved promise
                        Value promise_result = value_create_promise(result, 1, value_create_null());
                        value_free(&result);
                        result = promise_result;
                    }
                } else if (func_value.type == VALUE_FUNCTION) {
                    // Check if this is a bytecode function
                    // Bytecode functions have the function ID stored in the body field (cast as pointer)
                    // Check if this is a bytecode function (function ID stored in body field)
                    // Note: function ID 0 is valid (NULL pointer), so we check body_addr first
                    ASTNode* body_ptr = (ASTNode*)func_value.data.function_value.body;
                    uintptr_t body_addr = (uintptr_t)body_ptr;
                    if (body_addr < 10000) {
                        // This looks like a function ID (small integer cast as pointer)
                        // Extract the function ID
                        int func_id = (int)body_addr;
                        // Try to use current program first, then fall back to cache
                        BytecodeProgram* target_program = program;
                        if (!target_program || func_id < 0 || func_id >= (int)target_program->function_count) {
                            // Current program doesn't have the function, try cache
                            target_program = interpreter ? interpreter->bytecode_program_cache : NULL;
                        }
                        if (target_program && func_id >= 0 && func_id < (int)target_program->function_count) {
                            // Found bytecode function - execute it directly
                            BytecodeFunction* bc_func = &target_program->functions[func_id];
                            result = bytecode_execute_function_bytecode(interpreter, bc_func, args, arg_count, target_program);
                        } else {
                            // No valid program or invalid function ID - fall through to value_function_call
                            // But first, ensure the program cache is set if we have a program
                            // Use the main program from cache if available, otherwise use current program
                            BytecodeProgram* cache_program = interpreter ? interpreter->bytecode_program_cache : NULL;
                            if (!cache_program && program) {
                                // Set cache to current program if cache is not set
                                interpreter->bytecode_program_cache = program;
                                cache_program = program;
                            }
                            result = value_function_call(&func_value, args, arg_count, interpreter, 0, 0);
                        }
                    } else {
                        // Regular AST function or NULL body - use value_function_call
                        // This should handle bytecode functions that weren't detected above
                        result = value_function_call(&func_value, args, arg_count, interpreter, 0, 0);
                    }
                }
                
                // Clean up
                value_free(&func_value);
                if (args) {
                    for (int i = 0; i < arg_count; i++) {
                        value_free(&args[i]);
                    }
                    shared_free_safe(args, "bytecode_vm", "BC_CALL_FUNCTION_VALUE", 2);
                }
                
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_DEFINE_FUNCTION: {
                // Define function in environment: func_name -> function_value
                // instr->a = function name constant index, instr->b = function id
                int name_idx = instr->a;
                int func_id = instr->b;
                
                if (name_idx >= 0 && name_idx < (int)program->const_count && 
                    program->constants && program->constants[name_idx].type == VALUE_STRING &&
                    func_id >= 0 && func_id < (int)program->function_count &&
                    program->functions && interpreter && interpreter->global_environment) {
                    
                    const char* func_name = program->constants[name_idx].data.string_value;
                    if (!func_name) {
                        pc++;
                        break;
                    }
                    
                    BytecodeFunction* func = &program->functions[func_id];
                    if (!func) {
                        pc++;
                        break;
                    }
                    
                    // Check if this is an async function (bit 2 = async)
                    int flags = instr->c;
                    int is_async = (flags & 4) != 0;
                    
                    Environment* target_env = interpreter->current_environment ? interpreter->current_environment : interpreter->global_environment;
                    Value function_value;
                    
                    if (is_async) {
                        // Create async function value
                        function_value = value_create_async_function(
                            func_name,
                            NULL, // No AST parameters for bytecode functions
                            func->param_count,
                            NULL, // No return type for now
                            (ASTNode*)(uintptr_t)func_id, // Store function ID as "body" pointer
                            target_env ? target_env : interpreter->global_environment
                        );
                    } else {
                        // Create regular function value
                    // Store the function ID in the body field (cast as pointer) so we can find it later
                        function_value = value_create_function(
                        (ASTNode*)(uintptr_t)func_id, // Store function ID as "body" pointer
                        NULL, // No AST parameters for bytecode functions
                        func->param_count,
                        NULL, // No return type for now
                            target_env
                        );
                    }
                    
                    // Define in current environment (or global if no current) so it can be called from the correct scope
                    environment_define(target_env, func_name, function_value);
                    
                    // Store export/private metadata if flags are present (instr->c)
                    // bit 0 = is_export, bit 1 = is_private
                    // flags was already declared above
                    if (flags != 0) {
                        if (flags & 1) { // is_export
                            char export_key[256];
                            snprintf(export_key, sizeof(export_key), "__export__%s", func_name);
                            environment_define(target_env, export_key, value_create_boolean(1));
                        }
                        if (flags & 2) { // is_private
                            char private_key[256];
                            snprintf(private_key, sizeof(private_key), "__private__%s", func_name);
                            environment_define(target_env, private_key, value_create_boolean(1));
                        }
                    }
                }
                pc++;
                break;
            }
            
            case BC_SET_SYMBOL_FLAGS: {
                // Set export/private flags for a symbol: a = name_idx, b = flags
                if (instr->a >= 0 && instr->a < (int)program->const_count && 
                    program->constants[instr->a].type == VALUE_STRING) {
                    const char* symbol_name = program->constants[instr->a].data.string_value;
                    int flags = instr->b;
                    Environment* target_env = interpreter->current_environment ? interpreter->current_environment : interpreter->global_environment;
                    
                    if (flags & 1) { // is_export
                        char export_key[256];
                        snprintf(export_key, sizeof(export_key), "__export__%s", symbol_name);
                        environment_define(target_env, export_key, value_create_boolean(1));
                    }
                    if (flags & 2) { // is_private
                        char private_key[256];
                        snprintf(private_key, sizeof(private_key), "__private__%s", symbol_name);
                        environment_define(target_env, private_key, value_create_boolean(1));
                    }
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
                
                // For objects, check for regular "type" property first
                if (val.type == VALUE_OBJECT) {
                    Value regular_type = value_object_get(&val, "type");
                    if (regular_type.type != VALUE_NULL) {
                        // Regular "type" property exists - use it
                        result = regular_type;
                        value_free(&val);
                        value_stack_push(result);
                        pc++;
                        break;
                    }
                    value_free(&regular_type);
                    
                    // No regular "type" property - check for __type__ override (e.g., Library)
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
                    
                    // Fallback: return "Object" for plain objects
                    result = value_create_string("Object");
                    value_free(&val);
                    value_stack_push(result);
                    pc++;
                    break;
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
                
                // For functions, return "Function"
                if (val.type == VALUE_FUNCTION) {
                    result = value_create_string("Function");
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
                // BC_ARRAY_PUSH expects stack: [arr, val] with val on top
                // Pop val first (top), then arr (below)
                Value val = value_stack_pop();
                Value arr = value_stack_pop();
                if (arr.type == VALUE_ARRAY) {
                    // value_array_push clones val internally
                    value_array_push(&arr, val);
                    // Transfer ownership of arr to stack (don't free it)
                    value_stack_push(arr);
                } else {
                    // Not an array - return Null for non-arrays
                    value_stack_push(value_create_null());
                    // Free arr since we're not using it
                    value_free(&arr);
                }
                // Free val since it was cloned by value_array_push
                value_free(&val);
                pc++;
                break;
            }
            
            case BC_ARRAY_POP: {
                // Pop value from array
                // Stack: [arr, index] or [arr] (if no index provided)
                // The compiler puts index on top if provided, then array below it
                // Push popped value first, then modified array (so modified_array is on top)
                // Stack order: [popped_value, modified_array] with modified_array on top
                int pop_index = -1; // Default: pop last element
                
                // Check instruction operand 'b' for argument count
                // For BC_ARRAY_POP: a = unused, b = 1 if index provided, 0 if not
                if (instr->b == 1) {
                    // Index argument was provided - it's on top of stack
                    Value index_val = value_stack_pop();
                    if (index_val.type == VALUE_NUMBER) {
                        pop_index = (int)index_val.data.number_value;
                    }
                    value_free(&index_val);
                }
                
                // Now pop the array (it's on top now)
                Value arr = value_stack_pop();
                
                if (arr.type == VALUE_ARRAY) {
                    // Pop value from array at specified index (modifies arr in place)
                    Value result = value_array_pop(&arr, pop_index);
                    // Push popped value first, then modified array (so modified_array is on top)
                    value_stack_push(result);
                    // Transfer ownership of arr to stack (don't free it)
                    value_stack_push(arr);
                } else {
                    // Not an array - return Null for both
                    value_stack_push(value_create_null());
                    value_stack_push(value_create_null());
                value_free(&arr);
                }
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
            
            case BC_CREATE_RANGE: {
                // Create range: start..end (step = 1.0)
                // Stack: end (top), start
                Value end_val = value_stack_pop();
                Value start_val = value_stack_pop();
                
                if (end_val.type == VALUE_NUMBER && start_val.type == VALUE_NUMBER) {
                    Value range_val = value_create_range(
                        start_val.data.number_value,
                        end_val.data.number_value,
                        1.0,  // default step
                        0     // exclusive
                    );
                    value_stack_push(range_val);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&end_val);
                value_free(&start_val);
                pc++;
                break;
            }
            
            case BC_CREATE_RANGE_STEP: {
                // Create range with step: start..end..step
                // Stack: step (top), end, start
                Value step_val = value_stack_pop();
                Value end_val = value_stack_pop();
                Value start_val = value_stack_pop();
                
                if (step_val.type == VALUE_NUMBER && 
                    end_val.type == VALUE_NUMBER && 
                    start_val.type == VALUE_NUMBER) {
                    Value range_val = value_create_range(
                        start_val.data.number_value,
                        end_val.data.number_value,
                        step_val.data.number_value,
                        0  // exclusive
                    );
                    value_stack_push(range_val);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&step_val);
                value_free(&end_val);
                value_free(&start_val);
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
            
            case BC_CREATE_SET: {
                // Create set from elements on stack
                size_t element_count = instr->a;
                Value set_val = value_create_set(element_count > 0 ? element_count : 4);
                
                // Pop elements from stack (in reverse order)
                for (size_t i = 0; i < element_count; i++) {
                    Value element = value_stack_pop();
                    value_set_add(&set_val, element);
                    value_free(&element);
                }
                
                value_stack_push(set_val);
                pc++;
                break;
            }
            
            case BC_CREATE_MAP: {
                // Create hash map from key-value pairs on stack
                // Pairs are pushed as (value, key) so we pop as (key, value)
                size_t pair_count = instr->a;
                Value map_val = value_create_hash_map(pair_count > 0 ? pair_count : 4);
                
                // Pop key-value pairs from stack (in reverse order of push)
                // Stack has: ... value1, key1, value2, key2, ...
                // We pop: key2, value2, key1, value1
                for (size_t i = 0; i < pair_count; i++) {
                    Value key = value_stack_pop();
                    Value value = value_stack_pop();
                    
                    // Add key-value pair to hash map (only if key is string)
                    if (key.type == VALUE_STRING) {
                        value_hash_map_set(&map_val, key, value);
                    }
                    
                    value_free(&key);
                    value_free(&value);
                }
                
                value_stack_push(map_val);
                pc++;
                break;
            }
            
            case BC_IMPORT_LIB: {
                // Import library or file module: use library_name [as alias]
                if (instr->a < program->const_count && program->constants[instr->a].type == VALUE_STRING) {
                    const char* library_name = program->constants[instr->a].data.string_value;
                    const char* alias = NULL;
                    // Check if alias was provided (instr->b > 0 means alias index, 0 means no alias)
                    if (instr->b > 0 && instr->b < (int)program->const_count && program->constants[instr->b].type == VALUE_STRING) {
                        alias = program->constants[instr->b].data.string_value;
                    }
                    
                    // Check if this is a file import (contains .myco or starts with ./)
                    // File imports are detected by:
                    // 1. String contains ".myco" extension
                    // 2. String starts with "./" (relative path)
                    // 3. String doesn't match a known library name (heuristic)
                    bool is_file_import = (strstr(library_name, ".myco") != NULL) || 
                                         (library_name && library_name[0] == '.' && library_name[1] == '/') ||
                                         (library_name && strchr(library_name, '/') != NULL); // Path contains directory separator
                    
                    if (is_file_import) {
                        // Phase 4: Normalize path and check for circular import
                        char* normalized_path = normalize_file_path(library_name);
                        if (!normalized_path) {
                            normalized_path = shared_strdup(library_name);
                        }
                        
                        // Check for circular import
                        if (check_circular_import(interpreter, normalized_path)) {
                            char error_msg[512];
                            snprintf(error_msg, sizeof(error_msg), "Circular import detected: %s", normalized_path);
                            interpreter_set_error(interpreter, error_msg, 0, 0);
                            shared_free_safe(normalized_path, "bytecode_vm", "BC_IMPORT_LIB", 0);
                            value_stack_push(value_create_null());
                            pc++;
                            break;
                        }
                        
                        // Phase 4: Check cache first
                        ModuleCacheEntry* cached = find_cached_module(interpreter, normalized_path);
                        char* actual_path = NULL;
                        Value module_value;
                        int use_cache = 0;
                        
                        // Variable to store saved cache for restoration
                        BytecodeProgram* saved_program_cache_for_restore = NULL;
                        
                        if (cached && cached->is_valid) {
                            // Verify file hasn't changed
                            time_t current_mtime = get_file_mtime(normalized_path);
                            if (current_mtime == cached->file_mtime) {
                                // Cache hit - use cached module
                                Value* cached_val = (Value*)cached->module_value_storage;
                                if (cached_val) {
                                    module_value = value_clone(cached_val);
                                    use_cache = 1;
                                    actual_path = shared_strdup(normalized_path);
                                    
                                    // Temporarily set bytecode program cache to module's program
                                    // This ensures functions from cached modules can find their bytecode program
                                    BytecodeProgram* module_program = cached->module_bytecode_program ? (BytecodeProgram*)cached->module_bytecode_program : NULL;
                                    if (module_program && interpreter) {
                                        saved_program_cache_for_restore = interpreter->bytecode_program_cache;
                                        interpreter->bytecode_program_cache = module_program;
                                    }
                                }
                            } else {
                                // File changed - invalidate cache
                                cached->is_valid = 0;
                            }
                        }
                        
                        if (use_cache) {
                            // Cache hit - handle module storage and specific imports
                            // Use alias if provided, otherwise extract filename from path
                            char* var_name_to_free = NULL;
                            const char* var_name;
                            if (alias) {
                                var_name = alias;
                            } else {
                                const char* filename = strrchr(normalized_path, '/');
                                if (filename) {
                                    filename++;
                                } else {
                                    filename = normalized_path;
                                }
                                char* ext = strstr((char*)filename, ".myco");
                                if (ext) {
                                    var_name_to_free = shared_malloc_safe(ext - filename + 1, "bytecode_vm", "BC_IMPORT_LIB", 0);
                                    if (var_name_to_free) {
                                        strncpy(var_name_to_free, filename, ext - filename);
                                        var_name_to_free[ext - filename] = '\0';
                                    }
                                    var_name = var_name_to_free ? var_name_to_free : filename;
                                } else {
                                    var_name = filename;
                                }
                            }
                            
                            // Store in global and current environment
                            // environment_define clones the value internally, so we can pass module_value directly
                            // module_value is from cache, and environment_define will clone it before storing
                            Environment* target_env = interpreter->global_environment;
                            if (!target_env) {
                                target_env = interpreter->current_environment;
                            }
                            environment_define(target_env, var_name, module_value);
                            
                            if (target_env == interpreter->global_environment && 
                                interpreter->current_environment && 
                                interpreter->current_environment != interpreter->global_environment) {
                                environment_define(interpreter->current_environment, var_name, module_value);
                            }
                            
                            // Handle specific imports if present
                            if (instr->c > 0 && instr->c < (int)program->const_count) {
                                Value items_array = program->constants[instr->c];
                                if (items_array.type == VALUE_ARRAY) {
                                    size_t item_count = items_array.data.array_value.count;
                                    Value aliases_array = value_create_null();
                                    bool has_aliases = false;
                                    if (instr->c + 1 < (int)program->const_count) {
                                        Value next_const = program->constants[instr->c + 1];
                                        if (next_const.type == VALUE_ARRAY && 
                                            next_const.data.array_value.count == item_count) {
                                            aliases_array = next_const;
                                            has_aliases = true;
                                        }
                                    }
                                    
                                    for (size_t i = 0; i < item_count; i++) {
                                        Value* item_name_ptr = (Value*)items_array.data.array_value.elements[i];
                                        if (item_name_ptr && item_name_ptr->type == VALUE_STRING && item_name_ptr->data.string_value) {
                                            const char* item_name = item_name_ptr->data.string_value;
                                            Value item_value = value_object_get(&module_value, item_name);
                                            
                                            if (item_value.type != VALUE_NULL) {
                                                const char* import_name = item_name;
                                                if (has_aliases && i < aliases_array.data.array_value.count) {
                                                    Value* alias_ptr = (Value*)aliases_array.data.array_value.elements[i];
                                                    if (alias_ptr && alias_ptr->type == VALUE_STRING && alias_ptr->data.string_value) {
                                                        import_name = alias_ptr->data.string_value;
                                                    }
                                                }
                                                
                                                Value cloned_item = value_clone(&item_value);
                                                
                                                // If this is a function from a module, store module path in its captured environment
                                                // so it can find its bytecode program when called
                                                if (cloned_item.type == VALUE_FUNCTION) {
                                                    // Get module path from module_value
                                                    Value module_path_val = value_object_get(&module_value, "__module_path__");
                                                    if (module_path_val.type == VALUE_STRING && module_path_val.data.string_value) {
                                                        // Ensure function has a captured environment
                                                        if (!cloned_item.data.function_value.captured_environment) {
                                                            cloned_item.data.function_value.captured_environment = environment_create(NULL);
                                                        }
                                                        // Store module path in captured environment
                                                        environment_define(cloned_item.data.function_value.captured_environment, 
                                                                           "__module_path__", 
                                                                           value_create_string(module_path_val.data.string_value));
                                                    }
                                                    value_free(&module_path_val);
                                                }
                                                
                                                environment_define(interpreter->current_environment, import_name, cloned_item);
                                            }
                                        }
                                    }
                                }
                            }
                            
                            // Restore bytecode program cache if we changed it for cached module
                            // But only if we're not in the middle of calling functions from the module
                            // For now, restore it immediately - functions from cached modules will need
                            // to find their bytecode program another way (via module cache lookup)
                            if (saved_program_cache_for_restore && interpreter) {
                                interpreter->bytecode_program_cache = saved_program_cache_for_restore;
                            }
                            
                            if (var_name_to_free) {
                                shared_free_safe(var_name_to_free, "bytecode_vm", "BC_IMPORT_LIB", 0);
                            }
                            shared_free_safe(normalized_path, "bytecode_vm", "BC_IMPORT_LIB", 0);
                            if (actual_path) {
                                shared_free_safe(actual_path, "bytecode_vm", "BC_IMPORT_LIB", 0);
                            }
                            value_stack_push(value_create_null());
                            pc++;
                            break;
                        }
                        
                        if (!use_cache) {
                            // File import: load, parse, and execute in isolated environment
                            FILE* file = NULL;
                            
                            // Try opening the file as-is first
                            file = fopen(normalized_path, "r");
                            if (file) {
                                actual_path = shared_strdup(normalized_path);
                            } else {
                                // Try with .myco extension if not present
                                if (!strstr(normalized_path, ".myco")) {
                                    actual_path = shared_malloc_safe(strlen(normalized_path) + 6, "bytecode_vm", "BC_IMPORT_LIB", 0);
                                    if (actual_path) {
                                        strcpy(actual_path, normalized_path);
                                        strcat(actual_path, ".myco");
                                        file = fopen(actual_path, "r");
                                        if (!file) {
                                            shared_free_safe(actual_path, "bytecode_vm", "BC_IMPORT_LIB", 0);
                                            actual_path = NULL;
                                        } else {
                                            // Update normalized_path to match actual_path for cache consistency
                                            shared_free_safe(normalized_path, "bytecode_vm", "BC_IMPORT_LIB", 0);
                                            normalized_path = shared_strdup(actual_path);
                                        }
                                    }
                                }
                            }
                            
                            if (!file) {
                                // File not found - set error and continue
                                char error_msg[256];
                                snprintf(error_msg, sizeof(error_msg), "Cannot open module file: %s", library_name);
                                interpreter_set_error(interpreter, error_msg, 0, 0);
                                shared_free_safe(normalized_path, "bytecode_vm", "BC_IMPORT_LIB", 0);
                                value_stack_push(value_create_null());
                                pc++;
                                break;
                            }
                            
                            // Push to import chain for circular detection
                            push_import_chain(interpreter, actual_path ? actual_path : normalized_path);
                            
                            // Set current loading module for capability checks
                            // Use normalized_path for consistency (same format as library_name in import)
                            char* saved_loading_module = interpreter->current_loading_module ? 
                                                          shared_strdup(interpreter->current_loading_module) : NULL;
                            // Use normalized_path consistently to match what will be used in error messages
                            interpreter->current_loading_module = shared_strdup(normalized_path);
                        
                        // Read file content
                        fseek(file, 0, SEEK_END);
                        long file_size = ftell(file);
                        fseek(file, 0, SEEK_SET);
                        
                        char* source = shared_malloc_safe(file_size + 1, "bytecode_vm", "BC_IMPORT_LIB", 0);
                        if (!source) {
                            fclose(file);
                            if (actual_path) shared_free_safe(actual_path, "bytecode_vm", "BC_IMPORT_LIB", 0);
                            // Restore current loading module on error
                            shared_free_safe(interpreter->current_loading_module, "bytecode_vm", "BC_IMPORT_LIB", 0);
                            interpreter->current_loading_module = saved_loading_module ? 
                                                                   shared_strdup(saved_loading_module) : NULL;
                            if (saved_loading_module) {
                                shared_free_safe(saved_loading_module, "bytecode_vm", "BC_IMPORT_LIB", 0);
                            }
                            interpreter_set_error(interpreter, "Out of memory loading module", 0, 0);
                            value_stack_push(value_create_null());
                            pc++;
                            break;
                        }
                        
                        size_t bytes_read = fread(source, 1, file_size, file);
                        source[bytes_read] = '\0';
                        fclose(file);
                        
                        // Use actual_path for parsing if we have it, otherwise use library_name
                        const char* file_path_for_parsing = actual_path ? actual_path : library_name;
                        
                        // Create isolated environment for module
                        Environment* module_env = environment_create(interpreter->global_environment);
                        
                        // Save current environment
                        Environment* saved_env = interpreter->current_environment;
                        interpreter->current_environment = module_env;
                        
                        // Variable to store module's bytecode program for caching
                        BytecodeProgram* module_bytecode = NULL;
                        
                        // Parse and execute module
                        Lexer* lexer = lexer_initialize(source);
                        if (lexer) {
                            lexer_scan_all(lexer);
                            Parser* parser = parser_initialize(lexer);
                            if (parser) {
                                ASTNode* module_ast = parser_parse_program_with_filename(parser, file_path_for_parsing);
                                
                                // Store file directive state in module environment
                                if (parser->file_directive_export) {
                                    environment_define(module_env, "__file_directive_export__", value_create_boolean(1));
                                }
                                if (parser->file_directive_private) {
                                    environment_define(module_env, "__file_directive_private__", value_create_boolean(1));
                                }
                                if (parser->file_directive_strict) {
                                    environment_define(module_env, "__file_directive_strict__", value_create_boolean(1));
                                }
                                if (parser->file_directive_unstrict) {
                                    environment_define(module_env, "__file_directive_unstrict__", value_create_boolean(1));
                                }
                                
                                // Process required capabilities from module
                                // Note: The host can choose to auto-grant these or require explicit approval
                                // For now, we'll auto-grant them, but this can be made configurable
                                // Use current_loading_module for consistency with capability checks
                                if (parser->required_capability_count > 0 && interpreter->current_loading_module) {
                                    for (size_t i = 0; i < parser->required_capability_count; i++) {
                                        if (parser->required_capabilities[i]) {
                                            // Auto-grant the capability (host can override this behavior)
                                            // Use the same path format that will be used for checking
                                            interpreter_grant_capability_to_module(interpreter, interpreter->current_loading_module, parser->required_capabilities[i]);
                                        }
                                    }
                                }
                                
                                if (module_ast) {
                                    // Save the current bytecode program cache (main program)
                                    BytecodeProgram* saved_program_cache = interpreter->bytecode_program_cache;
                                    
                                    // Execute module in isolated environment
                                    Value module_result = interpreter_execute_program(interpreter, module_ast);
                                    
                                    // Capture the module's bytecode program (set by interpreter_execute_program)
                                    module_bytecode = interpreter->bytecode_program_cache;
                                    
                                    // Restore the main program cache
                                    interpreter->bytecode_program_cache = saved_program_cache;
                                    
                                    value_free(&module_result);
                                    
                                    // Clear any errors from module execution (they shouldn't prevent export)
                                    // Module might have unsupported AST nodes, but exports should still work
                                    if (interpreter_has_error(interpreter)) {
                                        interpreter_clear_error(interpreter);
                                    }
                                    ast_free(module_ast);
                                }
                                parser_free(parser);
                            }
                            lexer_free(lexer);
                        }
                        
                        // Restore current environment
                        interpreter->current_environment = saved_env;
                        
                        // Restore current loading module (before creating module value)
                        shared_free_safe(interpreter->current_loading_module, "bytecode_vm", "BC_IMPORT_LIB", 0);
                        interpreter->current_loading_module = saved_loading_module ? 
                                                               shared_strdup(saved_loading_module) : NULL;
                        if (saved_loading_module) {
                            shared_free_safe(saved_loading_module, "bytecode_vm", "BC_IMPORT_LIB", 0);
                        }
                        
                        // Create module value (VALUE_OBJECT) that wraps the module environment
                        // Note: Don't free actual_path yet - it's needed for caching
                        // Note: Use assignment, not declaration, to avoid shadowing the outer module_value variable
                        module_value = value_create_object(16); // Initial capacity for module properties
                        
                        // Verify object creation succeeded
                        if (module_value.type != VALUE_OBJECT) {
                            // Object creation failed - this shouldn't happen but handle gracefully
                            interpreter_set_error(interpreter, "Failed to create module object", 0, 0);
                            shared_free_safe(normalized_path, "bytecode_vm", "BC_IMPORT_LIB", 0);
                            if (actual_path) shared_free_safe(actual_path, "bytecode_vm", "BC_IMPORT_LIB", 0);
                            if (source) shared_free_safe(source, "bytecode_vm", "BC_IMPORT_LIB", 0);
                            pop_import_chain(interpreter);
                            value_stack_push(value_create_null());
                            pc++;
                            break;
                        }
                        
                        value_object_set(&module_value, "__type__", value_create_string("Module"));
                        value_object_set(&module_value, "type", value_create_string("Module")); // Also set regular type property for .type access
                        
                        // Store module path in module object so we can look up its bytecode program later
                        // This allows functions from cached modules to find their bytecode program
                        const char* module_path_for_cache = actual_path ? actual_path : normalized_path;
                        if (module_path_for_cache) {
                            value_object_set(&module_value, "__module_path__", value_create_string(module_path_for_cache));
                        }
                        
                        // Export all top-level symbols (func, let, class) from module environment
                        // Check export/private flags to determine what to export
                        // Priority: private > export > default (implicit export for now)
                        size_t export_count = 0;
                        for (size_t i = 0; i < module_env->count; i++) {
                            if (module_env->names[i] && module_env->values[i].type != VALUE_NULL) {
                                // Skip internal symbols starting with __ (including metadata)
                                if (strncmp(module_env->names[i], "__", 2) != 0) {
                                    const char* symbol_name = module_env->names[i];
                                    
                                    // Check for private flag
                                    char private_key[256];
                                    snprintf(private_key, sizeof(private_key), "__private__%s", symbol_name);
                                    Value private_flag = environment_get(module_env, private_key);
                                    bool is_private = (private_flag.type == VALUE_BOOLEAN && private_flag.data.boolean_value);
                                    value_free(&private_flag);
                                    
                                    if (is_private) {
                                        continue; // Never export private symbols
                                    }
                                    
                                    // Check for export flag
                                    char export_key[256];
                                    snprintf(export_key, sizeof(export_key), "__export__%s", symbol_name);
                                    Value export_flag = environment_get(module_env, export_key);
                                    bool is_export = (export_flag.type == VALUE_BOOLEAN && export_flag.data.boolean_value);
                                    value_free(&export_flag);
                                    
                                    // Check file-level directive for default behavior
                                    Value export_directive = environment_get(module_env, "__file_directive_export__");
                                    Value private_directive = environment_get(module_env, "__file_directive_private__");
                                    bool file_export_mode = (export_directive.type == VALUE_BOOLEAN && export_directive.data.boolean_value);
                                    bool file_private_mode = (private_directive.type == VALUE_BOOLEAN && private_directive.data.boolean_value);
                                    value_free(&export_directive);
                                    value_free(&private_directive);
                                    
                                    // Determine if symbol should be exported based on flags and file directive
                                    bool should_export = false;
                                    if (is_export) {
                                        // Explicitly exported - always export
                                        should_export = true;
                                    } else if (is_private) {
                                        // Explicitly private - never export (already handled above, but keep for clarity)
                                        should_export = false;
                                    } else if (file_export_mode) {
                                        // File directive says export by default
                                        should_export = true;
                                    } else if (file_private_mode) {
                                        // File directive says private by default
                                        should_export = false;
                                    } else {
                                        // No directive, default to implicit export (current behavior)
                                        should_export = true;
                                    }
                                    
                                    if (should_export) {
                                        Value cloned = value_clone(&module_env->values[i]);
                                        value_object_set(&module_value, symbol_name, cloned);
                                        export_count++;
                                    }
                                }
                            }
                        }
                            
                            // Phase 4: Cache the module (before storing in environment)
                            // Use actual_path if available, otherwise normalized_path
                            const char* cache_path = actual_path ? actual_path : normalized_path;
                            cache_module(interpreter, cache_path, module_env, module_value, module_bytecode);
                            
                            // Pop from import chain
                            pop_import_chain(interpreter);
                            
                            // Free paths (after caching, which may have stored the path)
                            if (actual_path) {
                                shared_free_safe(actual_path, "bytecode_vm", "BC_IMPORT_LIB", 0);
                            }
                            shared_free_safe(normalized_path, "bytecode_vm", "BC_IMPORT_LIB", 0);
                            shared_free_safe(source, "bytecode_vm", "BC_IMPORT_LIB", 0);
                        }
                        
                        // Note: module_env is kept alive as long as module_value exists
                        // since the exported values are cloned from it
                        // Even if module_env is empty, we still create a valid Module object
                        
                        // Use alias if provided, otherwise extract filename from path
                        char* var_name_to_free = NULL;
                        const char* var_name;
                        if (alias) {
                            var_name = alias;
                        } else {
                            // Extract filename from path
                            const char* filename = strrchr(library_name, '/');
                            if (filename) {
                                filename++; // Skip '/'
                            } else {
                                filename = library_name;
                            }
                            // Remove .myco extension
                            var_name_to_free = shared_strdup(filename);
                            char* ext = strstr(var_name_to_free, ".myco");
                            if (ext) {
                                *ext = '\0';
                            }
                            var_name = var_name_to_free;
                        }
                        
                        // Define module in global environment so it can be accessed from anywhere
                        // Modules should be stored in global environment, not current environment
                        // environment_define clones the value internally, so we can pass module_value directly
                        // module_value is a local variable, but environment_define will clone it before storing
                        Environment* target_env = interpreter->global_environment;
                        if (!target_env) {
                            target_env = interpreter->current_environment;
                        }
                        environment_define(target_env, var_name, module_value);
                        
                        // Also store in current environment if it's different from global
                        // This ensures the module is accessible even if current_env doesn't have global as parent
                        if (target_env == interpreter->global_environment && 
                            interpreter->current_environment && 
                            interpreter->current_environment != interpreter->global_environment) {
                            environment_define(interpreter->current_environment, var_name, module_value);
                        }
                        
                        // Handle specific imports if present (instr->c > 0 means specific_items array index)
                        if (instr->c > 0 && instr->c < (int)program->const_count) {
                            Value items_array = program->constants[instr->c];
                            if (items_array.type == VALUE_ARRAY) {
                                size_t item_count = items_array.data.array_value.count;
                                
                                // Check if aliases array exists at next constant index
                                Value aliases_array = value_create_null();
                                bool has_aliases = false;
                                if (instr->c + 1 < (int)program->const_count) {
                                    Value next_const = program->constants[instr->c + 1];
                                    if (next_const.type == VALUE_ARRAY && 
                                        next_const.data.array_value.count == item_count) {
                                        aliases_array = next_const;
                                        has_aliases = true;
                                    }
                                }
                                
                                // Import each item into current environment
                                for (size_t i = 0; i < item_count; i++) {
                                    Value* item_name_ptr = (Value*)items_array.data.array_value.elements[i];
                                    if (item_name_ptr && item_name_ptr->type == VALUE_STRING && item_name_ptr->data.string_value) {
                                        const char* item_name = item_name_ptr->data.string_value;
                                        
                                        // Get the item from the module
                                        Value item_value = value_object_get(&module_value, item_name);
                                        
                                        if (item_value.type != VALUE_NULL) {
                                            // Determine the name to use (alias if present, otherwise item name)
                                            const char* import_name = item_name;
                                            if (has_aliases && i < aliases_array.data.array_value.count) {
                                                Value* alias_ptr = (Value*)aliases_array.data.array_value.elements[i];
                                                if (alias_ptr && alias_ptr->type == VALUE_STRING && alias_ptr->data.string_value) {
                                                    import_name = alias_ptr->data.string_value;
                                                }
                                            }
                                            
                                            // Clone and import into current environment
                                            Value cloned_item = value_clone(&item_value);
                                            
                                            // If this is a function from a module, store module path in its captured environment
                                            // so it can find its bytecode program when called
                                            if (cloned_item.type == VALUE_FUNCTION) {
                                                // Get module path from module_value
                                                Value module_path_val = value_object_get(&module_value, "__module_path__");
                                                if (module_path_val.type == VALUE_STRING && module_path_val.data.string_value) {
                                                    // Ensure function has a captured environment
                                                    if (!cloned_item.data.function_value.captured_environment) {
                                                        cloned_item.data.function_value.captured_environment = environment_create(NULL);
                                                    }
                                                    // Store module path in captured environment
                                                    environment_define(cloned_item.data.function_value.captured_environment, 
                                                                       "__module_path__", 
                                                                       value_create_string(module_path_val.data.string_value));
                                                }
                                                value_free(&module_path_val);
                                            }
                                            
                                            environment_define(interpreter->current_environment, import_name, cloned_item);
                                        }
                                    }
                                }
                                
                                if (has_aliases) {
                                    // Don't free aliases_array - it's from constant pool
                                }
                            }
                        }
                        
                        if (var_name_to_free) {
                            shared_free_safe(var_name_to_free, "bytecode_vm", "BC_IMPORT_LIB", 0);
                        }
                        
                        // Push null result (use statements don't return a value)
                        value_stack_push(value_create_null());
                    } else {
                        // Library import: get from global environment
                        // Check capability-based security if we're in a module context
                        const char* current_module = interpreter->current_loading_module;
                        Value lib = value_create_null();
                        
                        if (current_module && interpreter->module_security_enabled) {
                            // We're loading a library from within a module - check capabilities
                            // Map library names to capability names
                            // Dangerous libraries require explicit capabilities
                            const char* capability_name = NULL;
                            
                            // Map common dangerous libraries to capabilities
                            if (strcmp(library_name, "file") == 0) {
                                capability_name = "fs";
                            } else if (strcmp(library_name, "dir") == 0) {
                                capability_name = "fs";
                            } else if (strcmp(library_name, "http") == 0) {
                                capability_name = "net";
                            } else if (strcmp(library_name, "server") == 0) {
                                capability_name = "net";
                            } else if (strcmp(library_name, "websocket") == 0) {
                                capability_name = "net";
                            } else if (strcmp(library_name, "gateway") == 0) {
                                capability_name = "net";
                            } else if (strcmp(library_name, "database") == 0) {
                                capability_name = "database";
                            }
                            
                            // If this is a dangerous library, check for capability
                            if (capability_name) {
                                if (interpreter_module_has_capability(interpreter, current_module, capability_name)) {
                                    // Module has capability - try to get capability implementation first
                                    Value capability_impl = interpreter_get_capability(interpreter, capability_name);
                                    if (capability_impl.type != VALUE_NULL) {
                                        // Use capability implementation (safe wrapper)
                                        lib = capability_impl;
                                    } else {
                                        // Fall back to direct library access if no capability implementation
                                        lib = environment_get(interpreter->global_environment, library_name);
                                    }
                                } else {
                                    // Module doesn't have capability - deny access
                                    char error_msg[512];
                                    snprintf(error_msg, sizeof(error_msg), 
                                            "Module '%s' attempted to import restricted library '%s' without capability '%s'",
                                            current_module, library_name, capability_name);
                                    interpreter_set_error(interpreter, error_msg, 0, 0);
                                    value_stack_push(value_create_null());
                                    pc++;
                                    break;
                                }
                            } else {
                                // Safe library (math, string, array, etc.) - allow direct access
                                lib = environment_get(interpreter->global_environment, library_name);
                            }
                        } else {
                            // Not in a module context or security disabled - allow direct access
                            lib = environment_get(interpreter->global_environment, library_name);
                        }
                        
                        if (lib.type == VALUE_NULL) {
                            // Library not found
                            char error_msg[256];
                            snprintf(error_msg, sizeof(error_msg), "Library '%s' not found", library_name);
                            interpreter_set_error(interpreter, error_msg, 0, 0);
                            value_stack_push(value_create_null());
                            pc++;
                            break;
                        }
                        
                        // Use alias if provided, otherwise use library_name
                        const char* var_name = alias ? alias : library_name;
                    
                    // Define it in the current environment
                        environment_define(interpreter->current_environment, var_name, lib);
                    
                    // Push null result (use statements don't return a value)
                    value_stack_push(value_create_null());
                    }
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
                // AST fallback removed - this should never be reached in bytecode-only mode
                // If we encounter this, it means bytecode compilation failed to handle something
                if (interpreter) {
                    interpreter_set_error(interpreter, "BC_EVAL_AST instruction encountered - bytecode compilation incomplete. AST fallback has been removed.", 0, 0);
                }
                value_stack_push(value_create_null());
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
                        
                        // Compile class metadata to bytecode (methods and fields)
                        compile_class_metadata(interpreter, &class_value);
                        
                        // Store class in current environment (which will be module_env during module execution)
                        // This allows classes to be exported from modules
                        Environment* target_env = interpreter->current_environment;
                        if (!target_env) {
                            target_env = interpreter->global_environment;
                        }
                        environment_define(target_env, class_name.data.string_value, class_value);
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
                                                    // Compile field initializer to bytecode and execute
                                                    BytecodeProgram* field_init_bytecode = bytecode_compile_ast(stmt->data.variable_declaration.initial_value, interpreter);
                                                    if (field_init_bytecode) {
                                                        field_value = interpreter_execute_bytecode(interpreter, field_init_bytecode);
                                                        bytecode_program_free(field_init_bytecode);
                                                    } else {
                                                        field_value = value_create_null();
                                                    }
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
                if (instr->a < program->const_count && instr->b >= 0 && instr->b < (int)program->function_count) {
                    Value var_name = program->constants[instr->a];
                    int body_func_id = instr->b;
                    
                    if (var_name.type == VALUE_STRING && body_func_id >= 0) {
                        // Get collection from stack
                        Value collection = value_stack_pop();
                        
                        // Create a new environment for the loop scope (like AST interpreter)
                        Environment* loop_env = environment_create(interpreter->current_environment);
                        Environment* old_env = interpreter->current_environment;
                        interpreter->current_environment = loop_env;
                        
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
                                
                                // Set loop variable in loop environment
                                environment_define(loop_env, var_name.data.string_value, element);
                                value_free(&element);
                                
                                // Execute loop body (bytecode sub-program)
                                // Loop bodies execute in the current environment (loop_env), not a new function environment
                                if (body_func_id >= 0 && body_func_id < (int)program->function_count && program->functions) {
                                    BytecodeFunction* body_func = &program->functions[body_func_id];
                                    if (body_func && body_func->code && body_func->code_count > 0 && body_func->code_count <= 1000000) {
                                        // Create temporary program for sub-program execution
                                        BytecodeProgram temp_program = {0};
                                        temp_program.code = body_func->code;
                                        temp_program.count = body_func->code_count;
                                        temp_program.capacity = body_func->code_capacity;
                                        temp_program.const_count = program ? program->const_count : 0;
                                        temp_program.constants = program ? program->constants : NULL;
                                        temp_program.num_const_count = program ? program->num_const_count : 0;
                                        temp_program.num_constants = program ? program->num_constants : NULL;
                                        temp_program.ast_count = program ? program->ast_count : 0;
                                        temp_program.ast_nodes = program ? program->ast_nodes : NULL;
                                        temp_program.function_count = program ? program->function_count : 0;
                                        temp_program.functions = program ? program->functions : NULL;
                                        temp_program.interpreter = interpreter;
                                        temp_program.local_count = 0;
                                        temp_program.local_names = NULL;
                                        temp_program.local_slot_count = 0;
                                        temp_program.locals = NULL;
                                        temp_program.num_local_count = 0;
                                        temp_program.num_locals = NULL;
                                        
                                        // Save stack state before sub-program execution
                                        size_t saved_stack_size = value_stack_size;
                                        Value* saved_stack = NULL;
                                        if (saved_stack_size > 0) {
                                            saved_stack = shared_malloc_safe(saved_stack_size * sizeof(Value), "bytecode_vm", "BC_FOR_LOOP", 1);
                                            if (saved_stack) {
                                                for (size_t i = 0; i < saved_stack_size; i++) {
                                                    saved_stack[i] = value_clone(&value_stack[i]);
                                                }
                                            }
                                        }
                                        
                                        // Execute sub-program in current environment (no new environment created)
                                        // bytecode_execute will reset the stack, so we need to restore it after
                                        Value body_result = bytecode_execute(&temp_program, interpreter, 0);
                                        
                                        // Restore stack state after sub-program execution
                                        // Clear any values left by sub-program
                                        while (value_stack_size > 0) {
                                            Value val = value_stack_pop();
                                            value_free(&val);
                                        }
                                        
                                        // Restore saved stack
                                        if (saved_stack && saved_stack_size > 0) {
                                            for (size_t i = 0; i < saved_stack_size; i++) {
                                                value_stack_push(saved_stack[i]);
                                            }
                                            // Free saved stack copies
                                            for (size_t i = 0; i < saved_stack_size; i++) {
                                                value_free(&saved_stack[i]);
                                            }
                                            shared_free_safe(saved_stack, "bytecode_vm", "BC_FOR_LOOP", 2);
                                        }
                                        
                                    if (interpreter_has_error(interpreter)) {
                                        value_free(&body_result);
                                        // Restore environment before returning
                                        interpreter->current_environment = old_env;
                                        environment_free(loop_env);
                                        value_free(&collection);
                                        value_stack_push(value_create_null());
                                        pc++;
                                        break;
                                    }
                                    // Check for break - if break, exit loop
                                    if (interpreter->break_depth > 0) {
                                        interpreter->break_depth = 0;  // Consume the break
                                        value_free(&body_result);
                                        // Restore environment before breaking
                                        interpreter->current_environment = old_env;
                                        environment_free(loop_env);
                                        value_free(&collection);
                                        value_stack_push(value_create_null());
                                        pc++;
                                        break;
                                    }
                                    // Check for continue - if continue, go to next iteration
                                    if (interpreter->continue_depth > 0) {
                                        interpreter->continue_depth = 0;  // Consume the continue
                                        value_free(&body_result);
                                        continue;  // Next iteration
                                    }
                                    // CRITICAL: Free the body result to prevent memory leak
                                    value_free(&body_result);
                                    }
                                }
                            }
                        } else if (collection.type == VALUE_STRING) {
                            // Iterate over string characters
                            for (size_t i = 0; i < strlen(collection.data.string_value); i++) {
                                char ch = collection.data.string_value[i];
                                char char_str[2] = {ch, '\0'};
                                Value element = value_create_string(char_str);
                                
                                // Set loop variable in loop environment
                                environment_define(loop_env, var_name.data.string_value, element);
                                value_free(&element);
                                
                                // Execute loop body (bytecode sub-program)
                                // Loop bodies execute in the current environment (loop_env), not a new function environment
                                if (body_func_id >= 0 && body_func_id < (int)program->function_count && program->functions) {
                                    BytecodeFunction* body_func = &program->functions[body_func_id];
                                    if (body_func && body_func->code && body_func->code_count > 0 && body_func->code_count <= 1000000) {
                                        // Create temporary program for sub-program execution
                                        BytecodeProgram temp_program = {0};
                                        temp_program.code = body_func->code;
                                        temp_program.count = body_func->code_count;
                                        temp_program.capacity = body_func->code_capacity;
                                        temp_program.const_count = program ? program->const_count : 0;
                                        temp_program.constants = program ? program->constants : NULL;
                                        temp_program.num_const_count = program ? program->num_const_count : 0;
                                        temp_program.num_constants = program ? program->num_constants : NULL;
                                        temp_program.ast_count = program ? program->ast_count : 0;
                                        temp_program.ast_nodes = program ? program->ast_nodes : NULL;
                                        temp_program.function_count = program ? program->function_count : 0;
                                        temp_program.functions = program ? program->functions : NULL;
                                        temp_program.interpreter = interpreter;
                                        temp_program.local_count = 0;
                                        temp_program.local_names = NULL;
                                        temp_program.local_slot_count = 0;
                                        temp_program.locals = NULL;
                                        temp_program.num_local_count = 0;
                                        temp_program.num_locals = NULL;
                                        
                                        // Save stack state before sub-program execution
                                        size_t saved_stack_size = value_stack_size;
                                        Value* saved_stack = NULL;
                                        if (saved_stack_size > 0) {
                                            saved_stack = shared_malloc_safe(saved_stack_size * sizeof(Value), "bytecode_vm", "BC_FOR_LOOP", 5);
                                            if (saved_stack) {
                                                for (size_t i = 0; i < saved_stack_size; i++) {
                                                    saved_stack[i] = value_clone(&value_stack[i]);
                                                }
                                            }
                                        }
                                        
                                        // Execute sub-program in current environment (no new environment created)
                                        // bytecode_execute will reset the stack, so we need to restore it after
                                        Value body_result = bytecode_execute(&temp_program, interpreter, 0);
                                        
                                        // Restore stack state after sub-program execution
                                        // Clear any values left by sub-program
                                        while (value_stack_size > 0) {
                                            Value val = value_stack_pop();
                                            value_free(&val);
                                        }
                                        
                                        // Restore saved stack
                                        if (saved_stack && saved_stack_size > 0) {
                                            for (size_t i = 0; i < saved_stack_size; i++) {
                                                value_stack_push(saved_stack[i]);
                                            }
                                            // Free saved stack copies
                                            for (size_t i = 0; i < saved_stack_size; i++) {
                                                value_free(&saved_stack[i]);
                                            }
                                            shared_free_safe(saved_stack, "bytecode_vm", "BC_FOR_LOOP", 6);
                                        }
                                        
                                    if (interpreter_has_error(interpreter)) {
                                        value_free(&body_result);
                                        // Restore environment before returning
                                        interpreter->current_environment = old_env;
                                        environment_free(loop_env);
                                        value_free(&collection);
                                        value_stack_push(value_create_null());
                                        pc++;
                                        break;
                                    }
                                    // Check for break
                                    if (interpreter->break_depth > 0) {
                                        interpreter->break_depth = 0;
                                        value_free(&body_result);
                                        interpreter->current_environment = old_env;
                                        environment_free(loop_env);
                                        value_free(&collection);
                                        value_stack_push(value_create_null());
                                        pc++;
                                        break;
                                    }
                                    // Check for continue
                                    if (interpreter->continue_depth > 0) {
                                        interpreter->continue_depth = 0;
                                        value_free(&body_result);
                                        continue;
                                    }
                                    value_free(&body_result);
                                    }
                                }
                            }
                        } else if (collection.type == VALUE_RANGE) {
                            // Handle range iteration (like AST interpreter)
                            double start = collection.data.range_value.start;
                            double end = collection.data.range_value.end;
                            double step = collection.data.range_value.step;
                            
                            // Iterate through the range (exclusive of end) with step
                            for (double i = start; i < end; i += step) {
                                // Define the iterator variable
                                Value iterator_value = value_create_number(i);
                                environment_define(loop_env, var_name.data.string_value, iterator_value);
                                value_free(&iterator_value);
                                
                                // Execute the loop body (bytecode sub-program)
                                // Loop bodies execute in the current environment (loop_env), not a new function environment
                                if (body_func_id >= 0 && body_func_id < (int)program->function_count && program->functions) {
                                    BytecodeFunction* body_func = &program->functions[body_func_id];
                                    if (body_func && body_func->code && body_func->code_count > 0 && body_func->code_count <= 1000000) {
                                        // Create temporary program for sub-program execution
                                        BytecodeProgram temp_program = {0};
                                        temp_program.code = body_func->code;
                                        temp_program.count = body_func->code_count;
                                        temp_program.capacity = body_func->code_capacity;
                                        temp_program.const_count = program ? program->const_count : 0;
                                        temp_program.constants = program ? program->constants : NULL;
                                        temp_program.num_const_count = program ? program->num_const_count : 0;
                                        temp_program.num_constants = program ? program->num_constants : NULL;
                                        temp_program.ast_count = program ? program->ast_count : 0;
                                        temp_program.ast_nodes = program ? program->ast_nodes : NULL;
                                        temp_program.function_count = program ? program->function_count : 0;
                                        temp_program.functions = program ? program->functions : NULL;
                                        temp_program.interpreter = interpreter;
                                        temp_program.local_count = 0;
                                        temp_program.local_names = NULL;
                                        temp_program.local_slot_count = 0;
                                        temp_program.locals = NULL;
                                        temp_program.num_local_count = 0;
                                        temp_program.num_locals = NULL;
                                        
                                        // Save stack state
                                        size_t saved_stack_size = value_stack_size;
                                        
                                        // Execute sub-program in current environment (no new environment created)
                                        Value body_result = bytecode_execute(&temp_program, interpreter, 0);
                                        
                                        // Restore stack size (sub-program may have left values on stack)
                                        while (value_stack_size > saved_stack_size) {
                                            Value val = value_stack_pop();
                                            value_free(&val);
                                        }
                                        
                                    if (interpreter_has_error(interpreter)) {
                                        value_free(&body_result);
                                        interpreter->current_environment = old_env;
                                        environment_free(loop_env);
                                        value_free(&collection);
                                        value_stack_push(value_create_null());
                                        pc++;
                                        break;
                                    }
                                    // Check for break
                                    if (interpreter->break_depth > 0) {
                                        interpreter->break_depth = 0;
                                        value_free(&body_result);
                                        interpreter->current_environment = old_env;
                                        environment_free(loop_env);
                                        value_free(&collection);
                                        value_stack_push(value_create_null());
                                        pc++;
                                        break;
                                    }
                                    // Check for continue
                                    if (interpreter->continue_depth > 0) {
                                        interpreter->continue_depth = 0;
                                        value_free(&body_result);
                                        continue;
                                    }
                                    value_free(&body_result);
                                    }
                                }
                            }
                        }
                        
                        // Sync variables from loop environment back to parent environment (like AST interpreter)
                        // This ensures variables defined/modified in the loop are accessible after
                        // BUT: Loop variables should NOT persist - they shadow outer variables during the loop only
                        // The Environment structure has: names, values, count
                        if (loop_env && old_env && var_name.type == VALUE_STRING) {
                            const char* loop_var_name = var_name.data.string_value;
                            // Iterate through all variables in the loop environment
                            for (size_t i = 0; i < loop_env->count; i++) {
                                if (loop_env->names[i]) {
                                    // Skip the loop variable itself - it should not persist after the loop
                                    if (strcmp(loop_env->names[i], loop_var_name) == 0) {
                                        continue;
                                    }
                                    // Check if variable exists in parent environment (including parent chain)
                                    Value existing = environment_get(old_env, loop_env->names[i]);
                                    if (existing.type != VALUE_NULL) {
                                        // Variable exists in parent, update it - use assign to update properly
                                        value_free(&existing);
                                        environment_assign(old_env, loop_env->names[i], value_clone(&loop_env->values[i]));
                                    } else {
                                        // Variable doesn't exist in parent, define it
                                        environment_define(old_env, loop_env->names[i], value_clone(&loop_env->values[i]));
                                    }
                                }
                            }
                        }
                        
                        // Restore previous environment
                        interpreter->current_environment = old_env;
                        environment_free(loop_env);
                        
                        value_free(&collection);
                    }
                }
                // Push null result (for loops don't return a value)
                value_stack_push(value_create_null());
                pc++;
                break;
            }
            
            case BC_BREAK: {
                // Break statement - set break_depth flag
                if (interpreter) {
                    interpreter->break_depth++;
                }
                // Push null (break statements don't return a value)
                value_stack_push(value_create_null());
                pc++;
                break;
            }
            
            case BC_CONTINUE: {
                // Continue statement - set continue_depth flag
                if (interpreter) {
                    interpreter->continue_depth++;
                }
                // Push null (continue statements don't return a value)
                value_stack_push(value_create_null());
                pc++;
                break;
            }
            
            case BC_ARRAY_GET: {
                // Array access: arr[index]
                // Stack: [arr, index]
                Value index = value_stack_pop();
                Value arr = value_stack_pop();
                
                Value result = value_create_null();
                
                if (arr.type == VALUE_ARRAY && index.type == VALUE_NUMBER) {
                    size_t idx = (size_t)index.data.number_value;
                    if (idx < arr.data.array_value.count) {
                        Value* elem = (Value*)arr.data.array_value.elements[idx];
                        if (elem) {
                            result = value_clone(elem);
                        }
                    }
                }
                
                value_free(&arr);
                value_free(&index);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_ARRAY_SET: {
                // Array assignment: arr[index] = value
                // Stack: [arr, index, value]
                // instr->a = variable name constant index (-1 if complex expression)
                // instr->b = 1 if simple variable, 0 if complex
                Value value = value_stack_pop();
                Value index = value_stack_pop();
                Value arr = value_stack_pop();
                
                if (arr.type == VALUE_ARRAY && index.type == VALUE_NUMBER) {
                    int idx = (int)index.data.number_value;
                    if (idx >= 0 && idx < (int)arr.data.array_value.count) {
                        // Set the array element using value_array_set
                        value_array_set(&arr, idx, value);
                        
                        // If this is a simple variable (instr->b == 1), update it in the environment
                        // environment_assign will clone the value, so we can free arr after
                        if (instr->b == 1 && instr->a >= 0 && instr->a < program->const_count) {
                            Value var_name_val = program->constants[instr->a];
                            if (var_name_val.type == VALUE_STRING && var_name_val.data.string_value) {
                                const char* var_name = var_name_val.data.string_value;
                                // Try to update in current environment first
                                if (interpreter && interpreter->current_environment) {
                                    if (environment_exists(interpreter->current_environment, var_name)) {
                                        environment_assign(interpreter->current_environment, var_name, arr);
                                    } else if (interpreter->global_environment && 
                                               environment_exists(interpreter->global_environment, var_name)) {
                                        environment_assign(interpreter->global_environment, var_name, arr);
                                    }
                                }
                            }
                        }
                        
                        // Free the cloned array (environment_assign clones it)
                        value_free(&arr);
                        value_free(&value); // Free the value we assigned
                    } else {
                        // Index out of bounds - set error
                        if (interpreter) {
                            interpreter_set_error(interpreter, "Array index out of bounds", 0, 0);
                        }
                        value_free(&arr);
                        value_free(&value);
                    }
                } else {
                    // Invalid array or index - set error
                    if (interpreter) {
                        if (arr.type != VALUE_ARRAY) {
                            interpreter_set_error(interpreter, "Cannot assign to non-array element", 0, 0);
                        } else {
                            interpreter_set_error(interpreter, "Array index must be a number", 0, 0);
                        }
                    }
                    value_free(&arr);
                    value_free(&value);
                }
                
                value_free(&index);
                // Push null (assignment doesn't return a value)
                value_stack_push(value_create_null());
                pc++;
                break;
            }
            
            case BC_THROW: {
                // Throw statement: throw expression
                // Stack: [exception_value]
                Value throw_value = value_stack_pop();
                
                // Convert the value to a string for the error message
                Value error_string = value_to_string(&throw_value);
                const char* error_message = error_string.type == VALUE_STRING ? error_string.data.string_value : "Unknown exception";
                
                // Set error with stack trace
                if (interpreter) {
                    interpreter_throw_exception(interpreter, error_message, 0, 0);
                }
                
                // Clean up
                value_free(&throw_value);
                value_free(&error_string);
                
                // If we're in a try block, continue execution to let catch handle it
                // Otherwise, the error will cause the VM loop to exit
                // Push null (throw doesn't return a value)
                value_stack_push(value_create_null());
                pc++;
                break;
            }
            
            case BC_TRY_START: {
                // Start try block - increment try depth
                if (interpreter) {
                    interpreter->try_depth++;
                }
                // Push null (try_start doesn't return a value)
                value_stack_push(value_create_null());
                pc++;
                break;
            }
            
            case BC_TRY_END: {
                // End try block - check if error occurred
                // If no error, decrement try depth and continue
                // If error, keep try_depth set so BC_CATCH can handle it
                if (interpreter && !interpreter_has_error(interpreter)) {
                    interpreter->try_depth--;
                    // Push null (try_end doesn't return a value when no error)
                    value_stack_push(value_create_null());
                } else {
                    // Error occurred - keep try_depth set, BC_CATCH will handle it
                    // Don't decrement try_depth here - BC_CATCH will do it after handling the error
                    // Push null (try_end doesn't return a value when error occurred)
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_CATCH: {
                // Catch block handler
                // instr->a = catch variable name constant index (empty string means no variable)
                // instr->b = catch block function ID (bytecode sub-program)
                if (instr->a >= 0 && instr->a < program->const_count && instr->b >= 0 && instr->b < (int)program->function_count) {
                    Value catch_var_name = program->constants[instr->a];
                    int catch_block_func_id = instr->b;
                    
                    // Check if we're in an error state (from try block)
                    if (interpreter && interpreter_has_error(interpreter)) {
                        // Clear error state BEFORE executing catch block
                        // Save the error message first (need to copy it since it might be freed)
                        char* saved_error = NULL;
                        if (interpreter->error_message) {
                            saved_error = shared_malloc_safe(strlen(interpreter->error_message) + 1, "bytecode_vm", "BC_CATCH", 0);
                            if (saved_error) {
                                strcpy(saved_error, interpreter->error_message);
                            }
                        }
                        interpreter->has_error = 0;
                        
                        // Create catch environment that can access outer variables
                        Environment* catch_env = environment_create(interpreter->current_environment);
                        Environment* old_env = interpreter->current_environment;
                        interpreter->current_environment = catch_env;
                        
                        // Bind error variable if specified (check for non-empty string)
                        if (catch_var_name.type == VALUE_STRING && catch_var_name.data.string_value && 
                            strlen(catch_var_name.data.string_value) > 0) {
                            Value error_value = value_create_string(saved_error ? saved_error : "Unknown error");
                            environment_define(catch_env, catch_var_name.data.string_value, error_value);
                            value_free(&error_value);
                        }
                        
                        // Execute catch block (bytecode sub-program)
                        // Catch blocks execute in the current environment (catch_env), not a new function environment
                        Value catch_result = value_create_null();
                        if (catch_block_func_id >= 0 && catch_block_func_id < (int)program->function_count && program->functions) {
                            BytecodeFunction* catch_block_func = &program->functions[catch_block_func_id];
                            if (catch_block_func && catch_block_func->code && catch_block_func->code_count > 0 && catch_block_func->code_count <= 1000000) {
                                // Create temporary program for sub-program execution
                                BytecodeProgram temp_program = {0};
                                temp_program.code = catch_block_func->code;
                                temp_program.count = catch_block_func->code_count;
                                temp_program.capacity = catch_block_func->code_capacity;
                                temp_program.const_count = program ? program->const_count : 0;
                                temp_program.constants = program ? program->constants : NULL;
                                temp_program.num_const_count = program ? program->num_const_count : 0;
                                temp_program.num_constants = program ? program->num_constants : NULL;
                                temp_program.ast_count = program ? program->ast_count : 0;
                                temp_program.ast_nodes = program ? program->ast_nodes : NULL;
                                temp_program.function_count = program ? program->function_count : 0;
                                temp_program.functions = program ? program->functions : NULL;
                                temp_program.interpreter = interpreter;
                                temp_program.local_count = 0;
                                temp_program.local_names = NULL;
                                temp_program.local_slot_count = 0;
                                temp_program.locals = NULL;
                                temp_program.num_local_count = 0;
                                temp_program.num_locals = NULL;
                                
                                // Save stack state before sub-program execution
                                size_t saved_stack_size = value_stack_size;
                                Value* saved_stack = NULL;
                                if (saved_stack_size > 0) {
                                    saved_stack = shared_malloc_safe(saved_stack_size * sizeof(Value), "bytecode_vm", "BC_CATCH", 1);
                                    if (saved_stack) {
                                        for (size_t i = 0; i < saved_stack_size; i++) {
                                            saved_stack[i] = value_clone(&value_stack[i]);
                                        }
                                    }
                                }
                                
                                // Execute sub-program in current environment (no new environment created)
                                // bytecode_execute will reset the stack, so we need to restore it after
                                catch_result = bytecode_execute(&temp_program, interpreter, 0);
                                
                                // Restore stack state after sub-program execution
                                // Clear any values left by sub-program
                                while (value_stack_size > 0) {
                                    Value val = value_stack_pop();
                                    value_free(&val);
                                }
                                
                                // Restore saved stack
                                if (saved_stack && saved_stack_size > 0) {
                                    for (size_t i = 0; i < saved_stack_size; i++) {
                                        value_stack_push(saved_stack[i]);
                                    }
                                    // Free saved stack copies
                                    for (size_t i = 0; i < saved_stack_size; i++) {
                                        value_free(&saved_stack[i]);
                                    }
                                    shared_free_safe(saved_stack, "bytecode_vm", "BC_CATCH", 2);
                                }
                            }
                        }
                        
                        // Restore environment
                        interpreter->current_environment = old_env;
                        environment_free(catch_env);
                        
                        // Decrement try depth
                        interpreter->try_depth--;
                        
                        // Free saved error message
                        if (saved_error) {
                            shared_free_safe(saved_error, "bytecode_vm", "BC_CATCH", 1);
                        }
                        
                        // Push catch result
                        value_stack_push(catch_result);
                    } else {
                        // No error occurred - skip catch block, push null
                        value_stack_push(value_create_null());
                    }
                } else {
                    // Invalid catch instruction - push null
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_SWITCH_CASE: {
                // Switch case: compare expression with case value, jump if not equal
                // Stack: [expression_value]
                // instr->a = case value function ID (bytecode sub-program)
                // instr->b = case body function ID (bytecode sub-program)
                if (instr->a >= 0 && instr->a < (int)program->function_count && instr->b >= 0 && instr->b < (int)program->function_count) {
                    Value expr_value = value_stack_pop();
                    int case_value_func_id = instr->a;
                    int case_body_func_id = instr->b;
                    
                    // Evaluate case value using bytecode sub-program
                    Value case_value = value_create_null();
                    if (case_value_func_id >= 0 && case_value_func_id < (int)program->function_count && program->functions) {
                        BytecodeFunction* case_value_func = &program->functions[case_value_func_id];
                        if (case_value_func && case_value_func->code && case_value_func->code_count > 0) {
                            // Create temporary program for case value evaluation
                            BytecodeProgram temp_program = {0};
                            temp_program.code = case_value_func->code;
                            temp_program.count = case_value_func->code_count;
                            temp_program.capacity = case_value_func->code_capacity;
                            temp_program.const_count = program ? program->const_count : 0;
                            temp_program.constants = program ? program->constants : NULL;
                            temp_program.num_const_count = program ? program->num_const_count : 0;
                            temp_program.num_constants = program ? program->num_constants : NULL;
                            temp_program.ast_count = program ? program->ast_count : 0;
                            temp_program.ast_nodes = program ? program->ast_nodes : NULL;
                            temp_program.function_count = program ? program->function_count : 0;
                            temp_program.functions = program ? program->functions : NULL;
                            temp_program.interpreter = interpreter;
                            temp_program.local_count = 0;
                            temp_program.local_names = NULL;
                            temp_program.local_slot_count = 0;
                            temp_program.locals = NULL;
                            temp_program.num_local_count = 0;
                            temp_program.num_locals = NULL;
                            
                            // Save stack state
                            size_t saved_stack_size = value_stack_size;
                            Value* saved_stack = NULL;
                            if (saved_stack_size > 0) {
                                saved_stack = shared_malloc_safe(saved_stack_size * sizeof(Value), "bytecode_vm", "BC_SWITCH_CASE", 1);
                                if (saved_stack) {
                                    for (size_t i = 0; i < saved_stack_size; i++) {
                                        saved_stack[i] = value_clone(&value_stack[i]);
                                    }
                                }
                            }
                            
                            // Execute case value sub-program
                            case_value = bytecode_execute(&temp_program, interpreter, 0);
                            
                            // Restore stack state
                            while (value_stack_size > 0) {
                                Value val = value_stack_pop();
                                value_free(&val);
                            }
                            
                            if (saved_stack && saved_stack_size > 0) {
                                for (size_t i = 0; i < saved_stack_size; i++) {
                                    value_stack_push(saved_stack[i]);
                                }
                                for (size_t i = 0; i < saved_stack_size; i++) {
                                    value_free(&saved_stack[i]);
                                }
                                shared_free_safe(saved_stack, "bytecode_vm", "BC_SWITCH_CASE", 2);
                            }
                            
                            // Get case value from stack if not returned directly
                            if (value_stack_size > 0 && case_value.type == VALUE_NULL) {
                                Value stack_val = value_stack_pop();
                                value_free(&case_value);
                                case_value = stack_val;
                            }
                        }
                    }
                    
                    // Compare expression with case value
                    int matches = 0;
                    if (expr_value.type == case_value.type) {
                        if (expr_value.type == VALUE_NUMBER) {
                            matches = (expr_value.data.number_value == case_value.data.number_value);
                        } else if (expr_value.type == VALUE_STRING) {
                            matches = (strcmp(expr_value.data.string_value, case_value.data.string_value) == 0);
                        } else if (expr_value.type == VALUE_BOOLEAN) {
                            matches = (expr_value.data.boolean_value == case_value.data.boolean_value);
                        } else if (expr_value.type == VALUE_NULL) {
                            matches = 1; // Both are null
                        } else {
                            matches = 0; // Different types or complex types
                        }
                    }
                    
                    value_free(&case_value);
                    
                    if (matches) {
                        // Case matches - execute body (bytecode sub-program)
                        // Case bodies execute in the current environment, not a new function environment
                        Value body_result = value_create_null();
                        if (case_body_func_id >= 0 && case_body_func_id < (int)program->function_count && program->functions) {
                            BytecodeFunction* case_body_func = &program->functions[case_body_func_id];
                            if (case_body_func && case_body_func->code && case_body_func->code_count > 0 && case_body_func->code_count <= 1000000) {
                                // Create temporary program for sub-program execution
                                BytecodeProgram temp_program = {0};
                                temp_program.code = case_body_func->code;
                                temp_program.count = case_body_func->code_count;
                                temp_program.capacity = case_body_func->code_capacity;
                                temp_program.const_count = program ? program->const_count : 0;
                                temp_program.constants = program ? program->constants : NULL;
                                temp_program.num_const_count = program ? program->num_const_count : 0;
                                temp_program.num_constants = program ? program->num_constants : NULL;
                                temp_program.ast_count = program ? program->ast_count : 0;
                                temp_program.ast_nodes = program ? program->ast_nodes : NULL;
                                temp_program.function_count = program ? program->function_count : 0;
                                temp_program.functions = program ? program->functions : NULL;
                                temp_program.interpreter = interpreter;
                                temp_program.local_count = 0;
                                temp_program.local_names = NULL;
                                temp_program.local_slot_count = 0;
                                temp_program.locals = NULL;
                                temp_program.num_local_count = 0;
                                temp_program.num_locals = NULL;
                                
                                // Save stack state before sub-program execution
                                size_t saved_stack_size = value_stack_size;
                                Value* saved_stack = NULL;
                                if (saved_stack_size > 0) {
                                    saved_stack = shared_malloc_safe(saved_stack_size * sizeof(Value), "bytecode_vm", "BC_SWITCH_CASE", 1);
                                    if (saved_stack) {
                                        for (size_t i = 0; i < saved_stack_size; i++) {
                                            saved_stack[i] = value_clone(&value_stack[i]);
                                        }
                                    }
                                }
                                
                                // Execute sub-program in current environment (no new environment created)
                                // bytecode_execute will reset the stack, so we need to restore it after
                                body_result = bytecode_execute(&temp_program, interpreter, 0);
                                
                                // Restore stack state after sub-program execution
                                // Clear any values left by sub-program
                                while (value_stack_size > 0) {
                                    Value val = value_stack_pop();
                                    value_free(&val);
                                }
                                
                                // Restore saved stack
                                if (saved_stack && saved_stack_size > 0) {
                                    for (size_t i = 0; i < saved_stack_size; i++) {
                                        value_stack_push(saved_stack[i]);
                                    }
                                    // Free saved stack copies
                                    for (size_t i = 0; i < saved_stack_size; i++) {
                                        value_free(&saved_stack[i]);
                                    }
                                    shared_free_safe(saved_stack, "bytecode_vm", "BC_SWITCH_CASE", 2);
                                }
                            }
                        }
                        
                        // Skip remaining cases (we'll need to jump to BC_SWITCH)
                        // For now, we'll push the result and let BC_SWITCH handle cleanup
                        value_stack_push(body_result);
                        
                        // Push a flag to indicate we matched
                        value_stack_push(value_create_boolean(1));
                        
                        // Free the expression value (we've consumed it)
                        value_free(&expr_value);
                    } else {
                        // Case doesn't match - push expression back and continue
                        value_stack_push(expr_value);
                        value_stack_push(value_create_boolean(0)); // Not matched
                    }
                } else {
                    // Invalid case instruction
                    Value expr_value = value_stack_pop();
                    value_free(&expr_value);
                    value_stack_push(value_create_null());
                    value_stack_push(value_create_boolean(0));
                }
                pc++;
                break;
            }
            
            case BC_SWITCH_DEFAULT: {
                // Switch default case: execute if no case matched
                // Stack: [expression_value, matched_flag]
                // instr->a = default body function ID (bytecode sub-program)
                if (instr->a >= 0 && instr->a < (int)program->function_count) {
                    Value matched_flag = value_stack_pop();
                    Value expr_value = value_stack_pop();
                    
                    int matched = value_is_truthy(&matched_flag);
                    value_free(&matched_flag);
                    
                    if (!matched) {
                        // No case matched - execute default (bytecode sub-program)
                        // Default bodies execute in the current environment, not a new function environment
                        Value default_result = value_create_null();
                        if (instr->a >= 0 && instr->a < (int)program->function_count && program->functions) {
                            BytecodeFunction* default_body_func = &program->functions[instr->a];
                            if (default_body_func && default_body_func->code && default_body_func->code_count > 0 && default_body_func->code_count <= 1000000) {
                                // Create temporary program for sub-program execution
                                BytecodeProgram temp_program = {0};
                                temp_program.code = default_body_func->code;
                                temp_program.count = default_body_func->code_count;
                                temp_program.capacity = default_body_func->code_capacity;
                                temp_program.const_count = program ? program->const_count : 0;
                                temp_program.constants = program ? program->constants : NULL;
                                temp_program.num_const_count = program ? program->num_const_count : 0;
                                temp_program.num_constants = program ? program->num_constants : NULL;
                                temp_program.ast_count = program ? program->ast_count : 0;
                                temp_program.ast_nodes = program ? program->ast_nodes : NULL;
                                temp_program.function_count = program ? program->function_count : 0;
                                temp_program.functions = program ? program->functions : NULL;
                                temp_program.interpreter = interpreter;
                                temp_program.local_count = 0;
                                temp_program.local_names = NULL;
                                temp_program.local_slot_count = 0;
                                temp_program.locals = NULL;
                                temp_program.num_local_count = 0;
                                temp_program.num_locals = NULL;
                                
                                // Save stack state before sub-program execution
                                size_t saved_stack_size = value_stack_size;
                                Value* saved_stack = NULL;
                                if (saved_stack_size > 0) {
                                    saved_stack = shared_malloc_safe(saved_stack_size * sizeof(Value), "bytecode_vm", "BC_SWITCH_DEFAULT", 1);
                                    if (saved_stack) {
                                        for (size_t i = 0; i < saved_stack_size; i++) {
                                            saved_stack[i] = value_clone(&value_stack[i]);
                                        }
                                    }
                                }
                                
                                // Execute sub-program in current environment (no new environment created)
                                // bytecode_execute will reset the stack, so we need to restore it after
                                default_result = bytecode_execute(&temp_program, interpreter, 0);
                                
                                // Restore stack state after sub-program execution
                                // Clear any values left by sub-program
                                while (value_stack_size > 0) {
                                    Value val = value_stack_pop();
                                    value_free(&val);
                                }
                                
                                // Restore saved stack
                                if (saved_stack && saved_stack_size > 0) {
                                    for (size_t i = 0; i < saved_stack_size; i++) {
                                        value_stack_push(saved_stack[i]);
                                    }
                                    // Free saved stack copies
                                    for (size_t i = 0; i < saved_stack_size; i++) {
                                        value_free(&saved_stack[i]);
                                    }
                                    shared_free_safe(saved_stack, "bytecode_vm", "BC_SWITCH_DEFAULT", 2);
                                }
                            }
                        }
                        value_stack_push(default_result);
                    } else {
                        // A case already matched - skip default
                        value_stack_push(value_create_null());
                    }
                    
                    value_free(&expr_value);
                } else {
                    // Invalid default instruction
                    Value matched_flag = value_stack_pop();
                    Value expr_value = value_stack_pop();
                    value_free(&matched_flag);
                    value_free(&expr_value);
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_SWITCH: {
                // Switch statement end: clean up and return result
                // Stack should have the result from the matched case or default
                // If there are extra values (flags), clean them up
                // The result should be on top of the stack
                if (value_stack_size > 1) {
                    // Multiple values - keep the last one (result), free the rest
                    Value* values = (Value*)shared_malloc_safe(value_stack_size * sizeof(Value), "bytecode_vm", "BC_SWITCH", 0);
                    if (values) {
                        // Pop all values
                        for (size_t i = 0; i < value_stack_size; i++) {
                            values[i] = value_stack_pop();
                        }
                        // Push the last value (result)
                        value_stack_push(values[value_stack_size - 1]);
                        // Free the rest
                        for (size_t i = 0; i < value_stack_size - 1; i++) {
                            value_free(&values[i]);
                        }
                        shared_free_safe(values, "bytecode_vm", "BC_SWITCH", 1);
                    }
                } else if (value_stack_size == 0) {
                    // No result - push null
                    value_stack_push(value_create_null());
                }
                // Otherwise, stack has exactly one value (the result), which is correct
                pc++;
                break;
            }
            
            case BC_MATCH_PATTERN: {
                // Match pattern: check if pattern matches expression
                // instr->a = pattern function ID (bytecode sub-program)
                // instr->b = match expression function ID (bytecode sub-program)
                // Stack: [match_value] (from expression evaluation, if any)
                
                if (instr->a >= 0 && instr->a < (int)program->function_count && 
                    instr->b >= 0 && instr->b < (int)program->function_count &&
                    program->functions) {
                    // Evaluate match expression using bytecode sub-program
                    Value match_value = value_create_null();
                    if (instr->b >= 0 && instr->b < (int)program->function_count) {
                        BytecodeFunction* expr_func = &program->functions[instr->b];
                        if (expr_func && expr_func->code && expr_func->code_count > 0) {
                            // Create temporary program for expression evaluation
                            BytecodeProgram temp_program = {0};
                            temp_program.code = expr_func->code;
                            temp_program.count = expr_func->code_count;
                            temp_program.capacity = expr_func->code_capacity;
                            temp_program.const_count = program ? program->const_count : 0;
                            temp_program.constants = program ? program->constants : NULL;
                            temp_program.num_const_count = program ? program->num_const_count : 0;
                            temp_program.num_constants = program ? program->num_constants : NULL;
                            temp_program.ast_count = program ? program->ast_count : 0;
                            temp_program.ast_nodes = program ? program->ast_nodes : NULL;
                            temp_program.function_count = program ? program->function_count : 0;
                            temp_program.functions = program ? program->functions : NULL;
                            temp_program.interpreter = interpreter;
                            temp_program.local_count = 0;
                            temp_program.local_names = NULL;
                            temp_program.local_slot_count = 0;
                            temp_program.locals = NULL;
                            temp_program.num_local_count = 0;
                            temp_program.num_locals = NULL;
                            
                            // Save stack state
                            size_t saved_stack_size = value_stack_size;
                            Value* saved_stack = NULL;
                            if (saved_stack_size > 0) {
                                saved_stack = shared_malloc_safe(saved_stack_size * sizeof(Value), "bytecode_vm", "BC_MATCH_PATTERN", 1);
                                if (saved_stack) {
                                    for (size_t i = 0; i < saved_stack_size; i++) {
                                        saved_stack[i] = value_clone(&value_stack[i]);
                                    }
                                }
                            }
                            
                            // Execute expression sub-program
                            match_value = bytecode_execute(&temp_program, interpreter, 0);
                            
                            // Restore stack state
                            while (value_stack_size > 0) {
                                Value val = value_stack_pop();
                                value_free(&val);
                            }
                            
                            if (saved_stack && saved_stack_size > 0) {
                                for (size_t i = 0; i < saved_stack_size; i++) {
                                    value_stack_push(saved_stack[i]);
                                }
                                for (size_t i = 0; i < saved_stack_size; i++) {
                                    value_free(&saved_stack[i]);
                                }
                                shared_free_safe(saved_stack, "bytecode_vm", "BC_MATCH_PATTERN", 2);
                            }
                            
                            // Get match value from stack if not returned directly
                            if (value_stack_size > 0 && match_value.type == VALUE_NULL) {
                                Value stack_val = value_stack_pop();
                                value_free(&match_value);
                                match_value = stack_val;
                            }
                        }
                    }
                    
                    // If match_value is still null and stack has a value, use it
                    if (match_value.type == VALUE_NULL && value_stack_size > 0) {
                        match_value = value_stack_pop();
                    }
                    
                    // Evaluate pattern using bytecode sub-program
                    Value pattern_val = value_create_null();
                    int matches = 0;
                    if (instr->a >= 0 && instr->a < (int)program->function_count) {
                        BytecodeFunction* pattern_func = &program->functions[instr->a];
                        if (pattern_func && pattern_func->code && pattern_func->code_count > 0) {
                            // Create temporary program for pattern evaluation
                            BytecodeProgram temp_program = {0};
                            temp_program.code = pattern_func->code;
                            temp_program.count = pattern_func->code_count;
                            temp_program.capacity = pattern_func->code_capacity;
                            temp_program.const_count = program ? program->const_count : 0;
                            temp_program.constants = program ? program->constants : NULL;
                            temp_program.num_const_count = program ? program->num_const_count : 0;
                            temp_program.num_constants = program ? program->num_constants : NULL;
                            temp_program.ast_count = program ? program->ast_count : 0;
                            temp_program.ast_nodes = program ? program->ast_nodes : NULL;
                            temp_program.function_count = program ? program->function_count : 0;
                            temp_program.functions = program ? program->functions : NULL;
                            temp_program.interpreter = interpreter;
                            temp_program.local_count = 0;
                            temp_program.local_names = NULL;
                            temp_program.local_slot_count = 0;
                            temp_program.locals = NULL;
                            temp_program.num_local_count = 0;
                            temp_program.num_locals = NULL;
                            
                            // Save stack state
                            size_t saved_stack_size = value_stack_size;
                            Value* saved_stack = NULL;
                            if (saved_stack_size > 0) {
                                saved_stack = shared_malloc_safe(saved_stack_size * sizeof(Value), "bytecode_vm", "BC_MATCH_PATTERN", 3);
                                if (saved_stack) {
                                    for (size_t i = 0; i < saved_stack_size; i++) {
                                        saved_stack[i] = value_clone(&value_stack[i]);
                                    }
                                }
                            }
                            
                            // Execute pattern sub-program
                            pattern_val = bytecode_execute(&temp_program, interpreter, 0);
                            
                            // Restore stack state
                            while (value_stack_size > 0) {
                                Value val = value_stack_pop();
                                value_free(&val);
                            }
                            
                            if (saved_stack && saved_stack_size > 0) {
                                for (size_t i = 0; i < saved_stack_size; i++) {
                                    value_stack_push(saved_stack[i]);
                                }
                                for (size_t i = 0; i < saved_stack_size; i++) {
                                    value_free(&saved_stack[i]);
                                }
                                shared_free_safe(saved_stack, "bytecode_vm", "BC_MATCH_PATTERN", 4);
                            }
                            
                            // Get pattern value from stack if not returned directly
                            if (value_stack_size > 0 && pattern_val.type == VALUE_NULL) {
                                Value stack_val = value_stack_pop();
                        value_free(&pattern_val);
                                pattern_val = stack_val;
                            }
                            
                            // Compare match_value with pattern_val
                        matches = value_equals(&match_value, &pattern_val);
                        }
                    }
                    
                    if (matches) {
                        // Pattern matches - return pattern value
                        value_free(&match_value);
                        value_stack_push(pattern_val);
                        value_stack_push(value_create_boolean(1)); // Matched flag
                    } else {
                        // Pattern doesn't match - push match_value back and continue
                        value_stack_push(match_value);
                        value_stack_push(value_create_boolean(0)); // Not matched flag
                    }
                } else {
                    // Invalid pattern instruction
                    if (value_stack_size > 0) {
                        Value match_value = value_stack_pop();
                        value_free(&match_value);
                    }
                    value_stack_push(value_create_null());
                    value_stack_push(value_create_boolean(0));
                }
                pc++;
                break;
            }
            
            case BC_MATCH_END: {
                // Match expression end: clean up and return result
                // Stack should have: [pattern_result, matched_flag] or [match_value, matched_flag]
                // If matched, return pattern_result; otherwise return null
                // Handle multiple patterns - we need to find the first matched pattern's result
                
                // The stack may have multiple [match_value, matched_flag, pattern_result] triplets
                // We need to find the first one where matched_flag is true
                // For now, simplify: if we have at least 2 values, check the flag
                
                if (value_stack_size >= 2) {
                    // Pop the last matched flag
                    Value matched_flag = value_stack_pop();
                    int matched = (matched_flag.type == VALUE_BOOLEAN && matched_flag.data.boolean_value);
                    value_free(&matched_flag);
                    
                    if (matched && value_stack_size > 0) {
                        // Pattern matched - result is on stack, keep it
                        // Result is already on top of stack, just leave it
                    } else {
                        // No pattern matched - clean up stack and return null
                        // Pop all remaining values (they're from unmatched patterns)
                        while (value_stack_size > 0) {
                            Value val = value_stack_pop();
                            value_free(&val);
                        }
                        value_stack_push(value_create_null());
                    }
                } else if (value_stack_size == 1) {
                    // Only one value - might be a result or match_value
                    // Check if it's a boolean (flag) or result
                    Value top = value_stack_pop();
                    if (top.type == VALUE_BOOLEAN) {
                        // It's a flag, no match
                        value_free(&top);
                        value_stack_push(value_create_null());
                    } else {
                        // It's a result, keep it
                        value_stack_push(top);
                    }
                } else {
                    // Stack is empty - return null
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_CREATE_LAMBDA: {
                // Create lambda function: (params) => body
                // instr->a = lambda body AST index
                // instr->b = function ID (for bytecode execution)
                // Find the lambda AST node by searching for one with this body
                
                fprintf(stderr, "[DEBUG VM] BC_CREATE_LAMBDA: Creating lambda (body_ast_idx=%d, func_id=%d)\n", instr->a, instr->b);
                fflush(stderr);
                
                if (instr->a < program->ast_count && instr->b >= 0) {
                    ASTNode* lambda_body = program->ast_nodes[instr->a];
                    
                    // Get lambda parameters - find the lambda AST node by searching for one with this body
                    ASTNode** lambda_params = NULL;
                    size_t lambda_param_count = 0;
                    
                    // Search for the lambda node that has this body
                    for (size_t i = 0; i < program->ast_count; i++) {
                        ASTNode* node = program->ast_nodes[i];
                        if (node && node->type == AST_NODE_LAMBDA && node->data.lambda.body == lambda_body) {
                            lambda_params = node->data.lambda.parameters;
                            lambda_param_count = node->data.lambda.parameter_count;
                            break;
                        }
                    }
                    
                    // For bytecode functions, pass the function ID directly as the body
                    // This allows value_create_function to detect it as a bytecode function ID
                    Value lambda_value = value_create_function(
                        (ASTNode*)(uintptr_t)instr->b, // Pass function ID as body (will be detected as bytecode function)
                        lambda_params,
                        lambda_param_count,
                        NULL, // No return type for lambdas
                        interpreter ? interpreter->current_environment : NULL
                    );
                    
                    value_stack_push(lambda_value);
                } else {
                    // Invalid lambda instruction - push null
                    value_stack_push(value_create_null());
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
            
            case BC_DUP: {
                // Duplicate top of stack
                if (value_stack_size == 0) {
                    if (interpreter) {
                        interpreter_set_error(interpreter, "Stack underflow in BC_DUP", 0, 0);
                    }
                    goto cleanup;
                }
                Value top = value_stack_peek();
                value_stack_push(value_clone(&top));
                pc++;
                break;
            }
            
            case BC_PROMISE_CREATE: {
                // Create a pending promise
                // Stack: [executor] -> [promise]
                // For now, create a simple pending promise
                // TODO: Execute executor function with resolve/reject callbacks
                Value promise = value_create_pending_promise();
                value_stack_push(promise);
                pc++;
                break;
            }
            
            case BC_AWAIT: {
                // Await promise: await promise -> value
                // Stack: [promise] -> [value]
                // Process event loop until promise is resolved
                if (value_stack_size == 0) {
                    if (interpreter) {
                        interpreter_set_error(interpreter, "Stack underflow in BC_AWAIT", 0, 0);
                    }
                    value_stack_push(value_create_null());
                    pc++;
                    break;
                }
                
                Value promise = value_stack_pop();
                
                if (promise.type != VALUE_PROMISE) {
                    // Not a promise - return as-is (for compatibility)
                    value_stack_push(promise);
                    pc++;
                    break;
                }
                
                // Get the promise from registry so we can check its updated state
                uint64_t promise_id = promise.data.promise_value.promise_id;
                Value* registry_promise = promise_id > 0 ? promise_registry_get(interpreter, promise_id) : NULL;
                
                // Process event loop until promise is resolved or rejected
                // This is a simplified synchronous implementation
                // In a full async implementation, this would suspend execution
                int max_iterations = 1000; // Prevent infinite loops
                int iterations = 0;
                
                // Run event loop at least once to process pending tasks
                async_event_loop_run(interpreter);
                
                while (iterations < max_iterations) {
                    // Check promise state from registry (if available) or local copy
                    // Re-fetch from registry in case it was updated
                    if (promise_id > 0) {
                        registry_promise = promise_registry_get(interpreter, promise_id);
                    }
                    
                    int is_resolved = registry_promise ? registry_promise->data.promise_value.is_resolved : promise.data.promise_value.is_resolved;
                    int is_rejected = registry_promise ? registry_promise->data.promise_value.is_rejected : promise.data.promise_value.is_rejected;
                    
                    if (is_resolved || is_rejected) {
                        break;
                    }
                    
                    // Run event loop again to process more tasks
                    async_event_loop_run(interpreter);
                    iterations++;
                }
                
                // Get final promise state from registry (re-fetch to ensure we have latest state)
                if (promise_id > 0) {
                    registry_promise = promise_registry_get(interpreter, promise_id);
                    if (registry_promise) {
                        promise = *registry_promise;  // Update local copy with registry state
                    }
                }
                
                if (promise.data.promise_value.is_resolved) {
                    // Promise is resolved - return resolved value (or null if no value stored)
                    Value resolved;
                    if (promise.data.promise_value.resolved_value) {
                        resolved = value_clone(promise.data.promise_value.resolved_value);
                    } else {
                        resolved = value_create_null();
                    }
                    // Remove promise from registry (it's been resolved and we're done with it)
                    if (promise_id > 0) {
                        promise_registry_remove(interpreter, promise_id);
                    } else {
                        value_free(&promise);
                    }
                    value_stack_push(resolved);
                } else if (promise.data.promise_value.is_rejected && promise.data.promise_value.rejected_value) {
                    // Promise is rejected - throw error
                    Value error = value_clone(promise.data.promise_value.rejected_value);
                    // Remove promise from registry
                    if (promise_id > 0) {
                        promise_registry_remove(interpreter, promise_id);
                    } else {
                        value_free(&promise);
                    }
                    if (interpreter) {
                        Value error_str = value_to_string(&error);
                        if (error_str.type == VALUE_STRING && error_str.data.string_value) {
                            interpreter_set_error(interpreter, error_str.data.string_value, 0, 0);
                        }
                        value_free(&error_str);
                    }
                    value_free(&error);
                    value_stack_push(value_create_null());
                } else {
                    // Promise still pending after max iterations - return null
                    // Don't remove from registry - it might resolve later
                    value_free(&promise);
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_ASYNC_CALL: {
                // Call async function: async_func(args...) -> Promise
                // Stack: [func, arg1, arg2, ...] -> [promise]
                // Create async task and add to queue
                if (value_stack_size == 0) {
                    if (interpreter) {
                        interpreter_set_error(interpreter, "Stack underflow in BC_ASYNC_CALL", 0, 0);
                    }
                    value_stack_push(value_create_null());
                    pc++;
                    break;
                }
                
                // Get function ID from instruction (instr->a = function ID, instr->b = arg count)
                int func_id = instr->a;
                int arg_count = instr->b;
                
                // Get arguments from stack
                Value* args = NULL;
                if (arg_count > 0) {
                    args = shared_malloc_safe(arg_count * sizeof(Value), "bytecode_vm", "BC_ASYNC_CALL", 0);
                    if (args) {
                        for (int i = 0; i < arg_count; i++) {
                            args[arg_count - 1 - i] = value_stack_pop();
                        }
                    }
                }
                
                // Create pending promise and register it
                Value promise = value_create_pending_promise();
                uint64_t promise_id = promise_registry_add(interpreter, promise);
                
                if (promise_id == 0) {
                    // Registry allocation failed - free args and push null
                    if (args) {
                        for (int i = 0; i < arg_count; i++) {
                            value_free(&args[i]);
                        }
                        shared_free_safe(args, "bytecode_vm", "BC_ASYNC_CALL", 0);
                    }
                    value_stack_push(value_create_null());
                    } else {
                        // Get pointer to promise in registry
                        Value* registry_promise = promise_registry_get(interpreter, promise_id);
                        if (!registry_promise) {
                            // Registry lookup failed - remove from registry, free args, push null
                            promise_registry_remove(interpreter, promise_id);
                            if (args) {
                                for (int i = 0; i < arg_count; i++) {
                                    value_free(&args[i]);
                                }
                                shared_free_safe(args, "bytecode_vm", "BC_ASYNC_CALL", 1);
                            }
                            value_stack_push(value_create_null());
                        } else {
                            // Ensure promise has the ID set (it should already, but be safe)
                            registry_promise->data.promise_value.promise_id = promise_id;
                            
                            // Create async task
                            AsyncTask* task = (AsyncTask*)shared_malloc_safe(sizeof(AsyncTask), "bytecode_vm", "BC_ASYNC_CALL", 2);
                            if (task) {
                                task->promise_ptr = registry_promise;  // Get pointer from registry
                                task->promise_copy = *registry_promise;  // Keep a copy for reference
                                task->program = program;
                                task->function_id = func_id;
                                task->args = args;
                                task->arg_count = arg_count;
                                task->environment = interpreter ? interpreter->current_environment : NULL;
                                task->is_resolved = 0;
                                task->result = value_create_null();
                                
                                // Add to task queue
                                async_task_queue_add(interpreter, task);
                                
                                // Push the promise with ID set (caller will free it, which will remove it from registry)
                                Value promise_to_push = *registry_promise;
                                promise_to_push.data.promise_value.promise_id = promise_id;  // Ensure ID is set
                                value_stack_push(promise_to_push);
                        } else {
                            // Task allocation failed - remove from registry, free args, create rejected promise
                            promise_registry_remove(interpreter, promise_id);
                            if (args) {
                                for (int i = 0; i < arg_count; i++) {
                                    value_free(&args[i]);
                                }
                                shared_free_safe(args, "bytecode_vm", "BC_ASYNC_CALL", 3);
                            }
                            Value error = value_create_string("Failed to create async task");
                            Value rejected_promise = value_create_promise(value_create_null(), 0, error);
                            value_free(&error);
                            value_stack_push(rejected_promise);
                        }
                    }
                }
                pc++;
                break;
            }
            
            case BC_HALT: {
                // Pop result if stack has value, otherwise return null
                if (value_stack_size > 0) {
                    result = value_stack_pop();
                } else {
                    result = value_create_null();
                }
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
                if (LIKELY(num_stack_size >= 2)) {
                    // Pop right operand first (top of stack)
                    double b = num_stack[--num_stack_size];
                    // Pop left operand second
                    double a = num_stack[--num_stack_size];
                    // Push result at current position (where left operand was)
                    num_stack[num_stack_size] = a + b;
                    num_stack_size++;
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
                    num_stack[num_stack_size] = a - b;
                    num_stack_size++;
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
                    num_stack[num_stack_size] = a * b;
                    num_stack_size++;
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
                        num_stack[num_stack_size] = a / b;
                    } else {
                        num_stack[num_stack_size] = 0.0;
                    }
                    num_stack_size++;
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
            
            case BC_MOD_NUM: {
                // Fast path: direct stack access without function calls
                if (num_stack_size >= 2) {
                    double b = num_stack[--num_stack_size];
                    double a = num_stack[--num_stack_size];
                    if (b != 0.0) {
                        num_stack[num_stack_size] = fmod(a, b);
                    } else {
                        num_stack[num_stack_size] = 0.0;
                    }
                    num_stack_size++;
                } else {
                    // Fallback for safety
                    double b = num_stack_pop();
                    double a = num_stack_pop();
                    if (b != 0.0) {
                        num_stack_push(fmod(a, b));
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
                // CRITICAL: Ensure numeric stack is not empty before popping
                // If stack is empty, this means there's a bug in compilation
                if (num_stack_size == 0) {
                    // Numeric stack is empty - this is a bug, but we'll push 0 to avoid crashes
                    value_stack_push(value_create_number(0.0));
                } else {
                    // Pop only the top value - this should be the result of the last operation
                    // CRITICAL: If there are multiple values on the numeric stack, something is wrong
                    // We should only have one value (the result of the last operation)
                    // Clear any leftover values to prevent bugs
                    double num = num_stack[num_stack_size - 1];
                    num_stack_size = 0;  // Clear entire numeric stack after conversion
                    value_stack_push(value_create_number(num));
                }
                pc++;
                break;
            }
            
            
            case BC_INC_LOCAL: {
                if (instr->a < program->num_local_count) {
                    program->num_locals[instr->a] += 1.0;
                    
                    // Also update the value locals array for consistency
                    if (instr->a < program->local_slot_count) {
                        value_free(&program->locals[instr->a]);
                        program->locals[instr->a] = value_create_number(program->num_locals[instr->a]);
                    }
                    
                    // Also update environment so AST-interpreted code can access it
                    if (interpreter && interpreter->current_environment && program->local_names && 
                        instr->a >= 0 && (size_t)instr->a < program->local_count) {
                        const char* var_name = program->local_names[instr->a];
                        if (var_name) {
                            Value updated_val = value_create_number(program->num_locals[instr->a]);
                            if (environment_exists(interpreter->current_environment, var_name)) {
                                environment_assign(interpreter->current_environment, var_name, updated_val);
                            } else {
                                environment_define(interpreter->current_environment, var_name, updated_val);
                            }
                            value_free(&updated_val);
                        }
                    }
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
                    
                    // Also update environment so AST-interpreted code can access it
                    if (interpreter && interpreter->current_environment && program->local_names && 
                        instr->a >= 0 && (size_t)instr->a < program->local_count) {
                        const char* var_name = program->local_names[instr->a];
                        if (var_name) {
                            Value updated_val = value_create_number(program->num_locals[instr->a]);
                            if (environment_exists(interpreter->current_environment, var_name)) {
                                environment_assign(interpreter->current_environment, var_name, updated_val);
                            } else {
                                environment_define(interpreter->current_environment, var_name, updated_val);
                            }
                            value_free(&updated_val);
                        }
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
                // Unknown opcode - report error and fail (like LuaJIT)
                // Log the opcode value for debugging
                if (interpreter) {
                    char error_msg[512];
                    snprintf(error_msg, sizeof(error_msg), 
                             "Unknown bytecode opcode: %d (0x%x) at PC %zu. Program count: %zu, AST count: %zu", 
                             instr->op, instr->op, pc, program->count, program->ast_count);
                    interpreter_set_error(interpreter, error_msg, 0, 0);
                }
                // Don't continue execution - this is a serious error
                goto cleanup;
            }
            }
        }
    }
    
cleanup:
    
    // Process any remaining async tasks before cleanup
    // Temporarily disabled to debug segfault
    // if (interpreter && interpreter->async_enabled && interpreter->task_queue && interpreter->task_queue_size > 0) {
    //     async_event_loop_run(interpreter);
    // }
    
    // Clean up any remaining stack values
    while (value_stack_size > 0) {
        Value val = value_stack_pop();
        value_free(&val);
    }
    
    // CRITICAL: Clear numeric stack to prevent leftover values from affecting next execution
    // Leftover values on numeric stack can cause arithmetic bugs
    num_stack_size = 0;
    
    // Free stack memory if it grew too large (prevent memory bloat)
    // Keep a reasonable capacity (128 entries) to avoid constant reallocation
    // But don't free if capacity is reasonable - just reset size to prevent leaks
    if (value_stack_capacity > 1024) {
        shared_free_safe(value_stack, "bytecode_vm", "cleanup_stack", 1);
        value_stack = NULL;
        value_stack_capacity = 0;
        value_stack_size = 0;
    } else if (value_stack_capacity > 128) {
        // If capacity is between 128 and 1024, shrink it back to 128 to prevent bloat
        Value* new_stack = shared_realloc_safe(value_stack, 128 * sizeof(Value), "bytecode_vm", "shrink_stack", 1);
        if (new_stack) {
            value_stack = new_stack;
            value_stack_capacity = 128;
        }
    }
    if (num_stack_capacity > 1024) {
        shared_free_safe(num_stack, "bytecode_vm", "cleanup_num_stack", 1);
        num_stack = NULL;
        num_stack_capacity = 0;
        num_stack_size = 0;
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
        if (func->param_names && func->param_names[i]) {
            const char* param_name = func->param_names[i];
            environment_define(func_env, param_name, value_clone(&args[i]));
        }
    }
    
    // Save current environment and set function environment
    Environment* old_env = interpreter->current_environment;
    interpreter->current_environment = func_env;
    
    // Execute the function's bytecode
    Value result = value_create_null();
    
    if (func->code_count > 0) {
        // Create a temporary program with just this function's code
        // We need to pass the constants AND functions from the main program
        // so recursive calls can find themselves
        BytecodeProgram temp_program = {0};
        temp_program.code = func->code;
        temp_program.count = func->code_count;
        temp_program.const_count = program ? program->const_count : 0;
        temp_program.constants = program ? program->constants : NULL;
        temp_program.num_const_count = program ? program->num_const_count : 0;
        temp_program.num_constants = program ? program->num_constants : NULL;
        temp_program.ast_count = program ? program->ast_count : 0;
        temp_program.ast_nodes = program ? program->ast_nodes : NULL;
        temp_program.function_count = program ? program->function_count : 0;
        temp_program.functions = program ? program->functions : NULL;
        temp_program.interpreter = interpreter;
        
        
        // Execute the function's bytecode
        // Save stack state before recursive call
        // The stack may contain operands from the calling expression
        size_t saved_stack_size = value_stack_size;
        Value* saved_stack = NULL;
        if (saved_stack_size > 0) {
            saved_stack = shared_malloc_safe(saved_stack_size * sizeof(Value), "bytecode_vm", "bytecode_execute_function_bytecode", 1);
            if (saved_stack) {
                for (size_t i = 0; i < saved_stack_size; i++) {
                    saved_stack[i] = value_clone(&value_stack[i]);
                }
            }
        }
        
        // Reset return flag before execution
        // IMPORTANT: Don't clear interpreter->return_value here - it might contain a value
        // from a previous call. We'll check interpreter->has_return after execution.
        interpreter->has_return = 0;
        result = bytecode_execute(&temp_program, interpreter, 0);
        
        // Save the function's return value (if any) before restoring stack
        // Check has_return first - if set, the return value is in interpreter->return_value
        // This is the authoritative source when BC_RETURN is used
        Value function_return_value = value_create_null();
        int has_function_return = 0;
        if (interpreter->has_return && interpreter->return_value.type != VALUE_NULL) {
            // Function used BC_RETURN - the return value is in interpreter->return_value
            function_return_value = value_clone(&interpreter->return_value);
            has_function_return = 1;
            // DON'T clear the flag or free return_value yet - keep them around
            // until after we've updated result, so callers can check if needed
            // We'll clear them after updating result (see below)
        } else if (result.type != VALUE_NULL) {
            // If bytecode_execute returned a non-null result, use that
            // This handles cases where BC_RETURN set result but interpreter->has_return wasn't set
            function_return_value = value_clone(&result);
            has_function_return = 1;
        } else if (value_stack_size > saved_stack_size) {
            // If function didn't use BC_RETURN, the result might be on the stack
            // Only pop if there are more values on the stack than we saved
            // (This means the function left a value on the stack)
            function_return_value = value_stack_pop();
            has_function_return = 1;
        }
        
        // Restore stack state after recursive call
        if (saved_stack && saved_stack_size > 0) {
            // Clear any remaining values left on stack by function execution
            while (value_stack_size > 0) {
                Value val = value_stack_pop();
                value_free(&val);
            }
            // Restore saved stack
            for (size_t i = 0; i < saved_stack_size; i++) {
                value_stack_push(saved_stack[i]);
            }
            // Free saved stack copies
            for (size_t i = 0; i < saved_stack_size; i++) {
                value_free(&saved_stack[i]);
            }
            shared_free_safe(saved_stack, "bytecode_vm", "bytecode_execute_function_bytecode", 2);
        }
        
        // Push the function's return value onto the stack after restoring the saved stack
        if (has_function_return) {
            value_stack_push(function_return_value);
        }
        
        // Update result to match the return value
        // This is critical - result must contain the function's return value
        if (has_function_return) {
            value_free(&result);
            result = value_clone(&function_return_value);
        }
        
        // Free interpreter->return_value and clear has_return now that we've captured it
        // Do this AFTER we've updated result, so callers can check interpreter->has_return if needed
        // IMPORTANT: Only clear if has_function_return is true AND we got it from interpreter->return_value
        // If has_function_return is false, we didn't capture it, so leave interpreter->has_return set
        // so the caller can check it (the safety net at the end will handle it)
        if (has_function_return) {
            // We captured a return value - check if it was from interpreter->return_value
            if (interpreter->has_return && interpreter->return_value.type != VALUE_NULL) {
                // We captured the return value from interpreter->return_value
                // Now we can free it and clear the flag
                value_free(&interpreter->return_value);
                interpreter->return_value = value_create_null();
                interpreter->has_return = 0;
            }
        }
        // If has_function_return is false, we didn't capture a return value
        // Leave interpreter->has_return and interpreter->return_value alone
        // so the caller can check them (the safety net at the end will handle it)
        
        // Note: The return value is pushed onto the stack at line 4159 above
        // so it's available for the caller (BC_CALL_USER_FUNCTION or BC_CALL_FUNCTION_VALUE)
    }
    
    // Restore old environment
    interpreter->current_environment = old_env;
    
    // Clean up function environment
    environment_free(func_env);
    
    // Final check: if result is NULL but interpreter->has_return is still set,
    // use interpreter->return_value as the return value
    // This shouldn't happen if everything worked correctly, but it's a safety net
    if (result.type == VALUE_NULL && interpreter->has_return && interpreter->return_value.type != VALUE_NULL) {
        value_free(&result);
        result = value_clone(&interpreter->return_value);
        interpreter->has_return = 0;
        value_free(&interpreter->return_value);
        interpreter->return_value = value_create_null();
    }
    
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

// ============================================================================
// Phase 4: Module Cache Helper Functions
// ============================================================================

// Normalize file path (resolve relative paths, remove redundant separators)
static char* normalize_file_path(const char* path) {
    if (!path) return NULL;
    
    // For now, just return a copy - full normalization can be added later
    return shared_strdup(path);
}

// Compute simple hash of file content (for cache invalidation)
static char* compute_file_hash(const char* file_path) {
    if (!file_path) return NULL;
    
    FILE* file = fopen(file_path, "r");
    if (!file) return NULL;
    
    // Simple hash: use file size + mtime as hash
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    
    struct stat st;
    if (stat(file_path, &st) != 0) return NULL;
    
    char* hash = shared_malloc_safe(64, "bytecode_vm", "compute_file_hash", 0);
    if (hash) {
        snprintf(hash, 64, "%ld_%ld", (long)st.st_mtime, size);
    }
    return hash;
}

// Get file modification time
static time_t get_file_mtime(const char* file_path) {
    if (!file_path) return 0;
    
    struct stat st;
    if (stat(file_path, &st) == 0) {
        return st.st_mtime;
    }
    return 0;
}

// Find cached module entry
static ModuleCacheEntry* find_cached_module(Interpreter* interpreter, const char* file_path) {
    if (!interpreter || !file_path || !interpreter->module_cache) {
        return NULL;
    }
    
    for (size_t i = 0; i < interpreter->module_cache_count; i++) {
        if (interpreter->module_cache[i].file_path && 
            strcmp(interpreter->module_cache[i].file_path, file_path) == 0) {
            return &interpreter->module_cache[i];
        }
    }
    return NULL;
}

// Cache a module
static void cache_module(Interpreter* interpreter, const char* file_path, Environment* module_env, Value module_value, BytecodeProgram* module_bytecode) {
    if (!interpreter || !file_path) return;
    
    // Allocate storage for module value
    Value* value_storage = shared_malloc_safe(sizeof(Value), "bytecode_vm", "cache_module", 0);
    if (!value_storage) return;
    // Clone the module value to ensure all nested data is properly stored
    *value_storage = value_clone(&module_value);
    
    // Check if already cached
    ModuleCacheEntry* existing = find_cached_module(interpreter, file_path);
    if (existing) {
        // Update existing entry
        existing->file_mtime = get_file_mtime(file_path);
        if (existing->file_hash) {
            shared_free_safe(existing->file_hash, "bytecode_vm", "cache_module", 0);
        }
        existing->file_hash = compute_file_hash(file_path);
        existing->module_env = module_env;
        existing->module_bytecode_program = module_bytecode; // Store bytecode program for function calls
        if (existing->module_value_storage) {
            value_free((Value*)existing->module_value_storage);
            shared_free_safe(existing->module_value_storage, "bytecode_vm", "cache_module", 0);
        }
        existing->module_value_storage = value_storage;
        existing->is_valid = 1;
        return;
    }
    
    // Expand cache if needed
    if (interpreter->module_cache_count >= interpreter->module_cache_capacity) {
        size_t new_capacity = interpreter->module_cache_capacity == 0 ? 8 : interpreter->module_cache_capacity * 2;
        ModuleCacheEntry* new_cache = shared_realloc_safe(interpreter->module_cache, 
                                                          new_capacity * sizeof(ModuleCacheEntry),
                                                          "bytecode_vm", "cache_module", 0);
        if (!new_cache) {
            shared_free_safe(value_storage, "bytecode_vm", "cache_module", 0);
            return;
        }
        interpreter->module_cache = new_cache;
        interpreter->module_cache_capacity = new_capacity;
    }
    
    // Add new entry
    ModuleCacheEntry* entry = &interpreter->module_cache[interpreter->module_cache_count++];
    entry->file_path = shared_strdup(file_path);
    entry->file_hash = compute_file_hash(file_path);
    entry->file_mtime = get_file_mtime(file_path);
    entry->module_env = module_env;
    entry->module_value_storage = value_storage;
    entry->module_bytecode_program = module_bytecode; // Store bytecode program for function calls
    entry->is_valid = 1;
}

// Check for circular import
static int check_circular_import(Interpreter* interpreter, const char* module_path) {
    if (!interpreter || !module_path) return 0;
    
    ImportChain* chain = interpreter->import_chain;
    while (chain) {
        if (chain->module_path && strcmp(chain->module_path, module_path) == 0) {
            return 1; // Circular import detected
        }
        chain = chain->next;
    }
    return 0;
}

// Push module to import chain
static void push_import_chain(Interpreter* interpreter, const char* module_path) {
    if (!interpreter || !module_path) return;
    
    ImportChain* new_link = shared_malloc_safe(sizeof(ImportChain), "bytecode_vm", "push_import_chain", 0);
    if (new_link) {
        new_link->module_path = shared_strdup(module_path);
        new_link->next = interpreter->import_chain;
        interpreter->import_chain = new_link;
    }
}

// Pop module from import chain
static void pop_import_chain(Interpreter* interpreter) {
    if (!interpreter || !interpreter->import_chain) return;
    
    ImportChain* top = interpreter->import_chain;
    interpreter->import_chain = top->next;
    
    if (top->module_path) {
        shared_free_safe(top->module_path, "bytecode_vm", "pop_import_chain", 0);
    }
    shared_free_safe(top, "bytecode_vm", "pop_import_chain", 0);
}


