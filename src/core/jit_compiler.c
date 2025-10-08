#include "jit_compiler.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/mman.h>
#include <unistd.h>

// Compatibility for MAP_ANONYMOUS
#ifndef MAP_ANONYMOUS
#ifdef MAP_ANON
#define MAP_ANONYMOUS MAP_ANON
#else
// Fallback for systems that don't have either
#define MAP_ANONYMOUS 0x20
#endif
#endif

// Compatibility for MAP_FAILED
#ifndef MAP_FAILED
#define MAP_FAILED ((void *) -1)
#endif

// JIT context management
JitContext* jit_context_create(JitTargetArchitecture target, JitCompilationMode mode) {
    JitContext* context = malloc(sizeof(JitContext));
    if (!context) return NULL;
    
    context->target = target;
    context->mode = mode;
    context->code_buffer = NULL;
    context->code_size = 0;
    context->code_capacity = 0;
    context->code_position = 0;
    
    context->function_table = NULL;
    context->compiled_functions = NULL;
    context->function_count = 0;
    context->function_capacity = 0;
    
    context->function_types = NULL;
    context->function_names = NULL;
    
    context->runtime_data = NULL;
    context->runtime_size = 0;
    
    context->enable_optimizations = 1;
    context->enable_inlining = 1;
    context->enable_constant_folding = 1;
    
    context->debug_mode = 0;
    context->debug_symbols = NULL;
    context->debug_symbol_count = 0;
    
    // Auto-detect target if needed
    if (target == JIT_TARGET_AUTO) {
        context->target = jit_detect_target_architecture();
    }
    
    return context;
}

void jit_context_free(JitContext* context) {
    if (!context) return;
    
    // Free code buffer
    if (context->code_buffer) {
        munmap(context->code_buffer, context->code_capacity);
    }
    
    // Free function table
    for (size_t i = 0; i < context->function_count; i++) {
        free(context->function_names[i]);
        type_free(context->function_types[i]);
    }
    free(context->function_table);
    free(context->function_types);
    free(context->function_names);
    
    // Free runtime data
    if (context->runtime_data) {
        free(context->runtime_data);
    }
    
    // Free debug symbols
    for (size_t i = 0; i < context->debug_symbol_count; i++) {
        free(context->debug_symbols[i]);
    }
    free(context->debug_symbols);
    
    free(context);
}

// Main JIT compilation
JitResult* jit_compile_ast(JitContext* context, ASTNode* ast) {
    if (!context || !ast) return NULL;
    
    clock_t start_time = clock();
    JitResult* result = malloc(sizeof(JitResult));
    if (!result) return NULL;
    
    result->success = 0;
    result->compiled_functions = NULL;
    result->function_count = 0;
    result->error_message = NULL;
    result->total_code_size = 0;
    result->compilation_time = 0.0;
    
    // Allocate initial code buffer
    if (!jit_allocate_code_buffer(context, 1024 * 1024)) { // 1MB initial buffer
        jit_set_error(context, "Failed to allocate code buffer");
        result->error_message = strdup(jit_get_last_error(context));
        return result;
    }
    
    // Generate prologue
    if (!jit_generate_prologue(context)) {
        jit_set_error(context, "Failed to generate prologue");
        result->error_message = strdup(jit_get_last_error(context));
        return result;
    }
    
    // Compile main program
    if (ast->type == AST_NODE_BLOCK) {
        // Compile each statement in the block
        for (size_t i = 0; i < ast->data.block.statement_count; i++) {
            ASTNode* stmt = ast->data.block.statements[i];
            if (stmt->type == AST_NODE_FUNCTION) {
                JitResult* func_result = jit_compile_function(context, stmt);
                if (!func_result || !func_result->success) {
                    if (func_result) {
                        result->error_message = strdup(func_result->error_message);
                        free(func_result);
                    }
                    return result;
                }
                free(func_result);
            }
        }
    }
    
    // Generate epilogue
    if (!jit_generate_epilogue(context)) {
        jit_set_error(context, "Failed to generate epilogue");
        result->error_message = strdup(jit_get_last_error(context));
        return result;
    }
    
    // Apply optimizations if enabled
    if (context->enable_optimizations) {
        jit_optimize_code(context);
    }
    
    // Set up result
    result->success = 1;
    result->total_code_size = context->code_size;
    result->compilation_time = ((double)(clock() - start_time)) / CLOCKS_PER_SEC;
    
    return result;
}

JitResult* jit_compile_function(JitContext* context, ASTNode* function_node) {
    if (!context || !function_node || function_node->type != AST_NODE_FUNCTION) {
        return NULL;
    }
    
    JitResult* result = malloc(sizeof(JitResult));
    if (!result) return NULL;
    
    result->success = 0;
    result->compiled_functions = NULL;
    result->function_count = 0;
    result->error_message = NULL;
    result->total_code_size = 0;
    result->compilation_time = 0.0;
    
    // Check if function is compilable
    if (!jit_is_function_compilable(function_node)) {
        result->error_message = strdup("Function is not suitable for JIT compilation");
        return result;
    }
    
    // Allocate space for compiled function
    JitFunction* func = malloc(sizeof(JitFunction));
    if (!func) {
        result->error_message = strdup("Memory allocation failed");
        return result;
    }
    
    func->name = strdup(function_node->data.function_definition.function_name);
    func->signature = type_create_function(NULL, 0, NULL, 0, 0);
    func->native_code = NULL;
    func->code_size = 0;
    func->is_compiled = 0;
    
    // Generate function prologue
    size_t func_start = context->code_position;
    
    // Generate function body
    if (function_node->data.function_definition.body) {
        if (!jit_generate_expression(context, function_node->data.function_definition.body)) {
            result->error_message = strdup("Failed to generate function body");
            free(func->name);
            type_free(func->signature);
            free(func);
            return result;
        }
    }
    
    // Generate function epilogue
    size_t func_end = context->code_position;
    func->code_size = func_end - func_start;
    func->native_code = context->code_buffer + func_start;
    func->is_compiled = 1;
    
    // Add to function table
    if (context->function_count >= context->function_capacity) {
        size_t new_capacity = context->function_capacity == 0 ? 4 : context->function_capacity * 2;
        JitFunction** new_table = realloc(context->compiled_functions, new_capacity * sizeof(JitFunction*));
        if (!new_table) {
            result->error_message = strdup("Failed to expand function table");
            free(func->name);
            type_free(func->signature);
            free(func);
            return result;
        }
        context->compiled_functions = new_table;
        context->function_capacity = new_capacity;
    }
    
    context->compiled_functions[context->function_count] = func;
    context->function_count++;
    
    result->success = 1;
    result->compiled_functions = context->compiled_functions[context->function_count - 1];
    result->function_count = context->function_count;
    result->total_code_size = func->code_size;
    
    return result;
}

JitResult* jit_compile_expression(JitContext* context, ASTNode* expression) {
    if (!context || !expression) return NULL;
    
    JitResult* result = malloc(sizeof(JitResult));
    if (!result) return NULL;
    
    result->success = 0;
    result->compiled_functions = NULL;
    result->function_count = 0;
    result->error_message = NULL;
    result->total_code_size = 0;
    result->compilation_time = 0.0;
    
    // Generate code based on expression type
    int success = 0;
    switch (expression->type) {
        case AST_NODE_BINARY_OP:
            success = jit_generate_binary_op(context, expression);
            break;
        case AST_NODE_UNARY_OP:
            success = jit_generate_unary_op(context, expression);
            break;
        case AST_NODE_FUNCTION_CALL:
            success = jit_generate_function_call(context, expression);
            break;
        case AST_NODE_IDENTIFIER:
            success = jit_generate_variable_access(context, expression);
            break;
        case AST_NODE_ASSIGNMENT:
            success = jit_generate_variable_assignment(context, expression);
            break;
        default:
            result->error_message = strdup("Unsupported expression type for JIT compilation");
            return result;
    }
    
    if (!success) {
        result->error_message = strdup("Failed to generate code for expression");
        return result;
    }
    
    result->success = 1;
    result->total_code_size = context->code_size;
    
    return result;
}

// Code generation functions
int jit_generate_prologue(JitContext* context) {
    if (!context) return 0;
    
    // Platform-specific prologue generation
    switch (context->target) {
#ifdef __x86_64__
        case JIT_TARGET_X86_64:
            return jit_generate_x86_64_prologue(context);
#endif
#ifdef __aarch64__
        case JIT_TARGET_ARM64:
            return jit_generate_arm64_prologue(context);
#endif
        default:
            // Fallback to interpreted mode
            return 1;
    }
}

int jit_generate_epilogue(JitContext* context) {
    if (!context) return 0;
    
    // Platform-specific epilogue generation
    switch (context->target) {
#ifdef __x86_64__
        case JIT_TARGET_X86_64:
            return jit_generate_x86_64_epilogue(context);
#endif
#ifdef __aarch64__
        case JIT_TARGET_ARM64:
            return jit_generate_arm64_epilogue(context);
#endif
        default:
            // Fallback to interpreted mode
            return 1;
    }
}

int jit_generate_function_call(JitContext* context, ASTNode* call_node) {
    if (!context || !call_node || call_node->type != AST_NODE_FUNCTION_CALL) return 0;
    
    // Look up function in function table
    const char* func_name = call_node->data.function_call.function_name;
    JitFunction* func = NULL;
    
    for (size_t i = 0; i < context->function_count; i++) {
        if (strcmp(context->compiled_functions[i]->name, func_name) == 0) {
            func = context->compiled_functions[i];
            break;
        }
    }
    
    if (!func) {
        // Function not compiled yet, fall back to interpretation
        return 1;
    }
    
    // Generate call instruction
    switch (context->target) {
#ifdef __x86_64__
        case JIT_TARGET_X86_64:
            return jit_emit_x86_64_call(context, func->native_code);
#endif
#ifdef __aarch64__
        case JIT_TARGET_ARM64:
            return jit_emit_arm64_call(context, func->native_code);
#endif
        default:
            return 1;
    }
}

int jit_generate_binary_op(JitContext* context, ASTNode* op_node) {
    if (!context || !op_node || op_node->type != AST_NODE_BINARY_OP) return 0;
    
    // Generate code for operands
    if (!jit_generate_expression(context, op_node->data.binary.left)) return 0;
    if (!jit_generate_expression(context, op_node->data.binary.right)) return 0;
    
    // Update statistics based on operation type
    switch (op_node->data.binary.op) {
        case OP_ADD:
        case OP_SUBTRACT:
        case OP_MULTIPLY:
        case OP_DIVIDE:
            // TODO: Add statistics tracking
            break;
        case OP_EQUAL:
        case OP_NOT_EQUAL:
        case OP_LESS_THAN:
        case OP_GREATER_THAN:
        case OP_LESS_EQUAL:
        case OP_GREATER_EQUAL:
            // TODO: Add statistics tracking
            break;
        case OP_LOGICAL_AND:
        case OP_LOGICAL_OR:
            // TODO: Add statistics tracking
            break;
        default:
            break;
    }
    
    // Generate operation instruction
    switch (context->target) {
#ifdef __x86_64__
        case JIT_TARGET_X86_64:
            return jit_generate_x86_64_binary_op(context, op_node);
#endif
#ifdef __aarch64__
        case JIT_TARGET_ARM64:
            return jit_generate_arm64_binary_op(context, op_node);
#endif
        default:
            return 1;
    }
}

int jit_generate_unary_op(JitContext* context, ASTNode* op_node) {
    if (!context || !op_node || op_node->type != AST_NODE_UNARY_OP) return 0;
    
    // Generate code for operand
    if (!jit_generate_expression(context, op_node->data.unary.operand)) return 0;
    
    // Generate operation instruction
    switch (context->target) {
#ifdef __x86_64__
        case JIT_TARGET_X86_64:
            return jit_generate_x86_64_unary_op(context, op_node);
#endif
#ifdef __aarch64__
        case JIT_TARGET_ARM64:
            return jit_generate_arm64_unary_op(context, op_node);
#endif
        default:
            return 1;
    }
}

int jit_generate_variable_access(JitContext* context, ASTNode* var_node) {
    if (!context || !var_node || var_node->type != AST_NODE_IDENTIFIER) return 0;
    
    // Generate load instruction
    switch (context->target) {
#ifdef __x86_64__
        case JIT_TARGET_X86_64:
            return jit_emit_x86_64_load(context, var_node->data.identifier_value);
#endif
#ifdef __aarch64__
        case JIT_TARGET_ARM64:
            return jit_emit_arm64_load(context, var_node->data.identifier_value);
#endif
        default:
            return 1;
    }
}

int jit_generate_variable_assignment(JitContext* context, ASTNode* assign_node) {
    if (!context || !assign_node || assign_node->type != AST_NODE_ASSIGNMENT) return 0;
    
    // Generate code for value
    if (!jit_generate_expression(context, assign_node->data.assignment.value)) return 0;
    
    // Generate store instruction
    switch (context->target) {
#ifdef __x86_64__
        case JIT_TARGET_X86_64:
            return jit_emit_x86_64_store(context, assign_node->data.assignment.variable_name);
#endif
#ifdef __aarch64__
        case JIT_TARGET_ARM64:
            return jit_emit_arm64_store(context, assign_node->data.assignment.variable_name);
#endif
        default:
            return 1;
    }
}

int jit_generate_control_flow(JitContext* context, ASTNode* control_node) {
    if (!context || !control_node) return 0;
    
    switch (control_node->type) {
        case AST_NODE_IF_STATEMENT:
            return jit_generate_if_statement(context, control_node);
        case AST_NODE_WHILE_LOOP:
            return jit_generate_while_loop(context, control_node);
        case AST_NODE_FOR_LOOP:
            return jit_generate_for_loop(context, control_node);
        default:
            return 1;
    }
}

// Memory management
int jit_allocate_code_buffer(JitContext* context, size_t size) {
    if (!context) return 0;
    
    // Allocate executable memory
    context->code_buffer = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, 
                               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (context->code_buffer == MAP_FAILED) {
        return 0;
    }
    
    context->code_capacity = size;
    context->code_size = 0;
    context->code_position = 0;
    
    return 1;
}

int jit_allocate_runtime_data(JitContext* context, size_t size) {
    if (!context) return 0;
    
    context->runtime_data = malloc(size);
    if (!context->runtime_data) return 0;
    
    context->runtime_size = size;
    return 1;
}

void jit_free_compiled_code(JitContext* context) {
    if (!context) return;
    
    if (context->code_buffer) {
        munmap(context->code_buffer, context->code_capacity);
        context->code_buffer = NULL;
        context->code_capacity = 0;
        context->code_size = 0;
        context->code_position = 0;
    }
}

// Optimization passes
int jit_optimize_code(JitContext* context) {
    if (!context) return 0;
    
    // Apply various optimization passes
    if (context->enable_inlining) {
        jit_inline_functions(context);
    }
    
    if (context->enable_constant_folding) {
        jit_constant_fold(context);
    }
    
    jit_eliminate_dead_code(context);
    
    return 1;
}

int jit_inline_functions(JitContext* context) {
    // TODO: Implement function inlining
    return 1;
}

int jit_constant_fold(JitContext* context) {
    // TODO: Implement constant folding
    return 1;
}

int jit_eliminate_dead_code(JitContext* context) {
    // TODO: Implement dead code elimination
    return 1;
}

// Native code execution
void* jit_execute_function(JitContext* context, const char* function_name, void** args) {
    if (!context || !function_name) return NULL;
    
    // Find function in function table
    for (size_t i = 0; i < context->function_count; i++) {
        if (strcmp(context->compiled_functions[i]->name, function_name) == 0) {
            JitFunction* func = context->compiled_functions[i];
            if (func->is_compiled) {
                return jit_execute_compiled_code(context, func->native_code, args);
            }
        }
    }
    
    return NULL;
}

void* jit_execute_compiled_code(JitContext* context, void* code_ptr, void** args) {
    if (!context || !code_ptr) return NULL;
    
    // Cast to function pointer and execute
    void* (*func_ptr)(void**) = (void*(*)(void**))code_ptr;
    return func_ptr(args);
}

// Platform-specific implementations
#ifdef __x86_64__
int jit_generate_x86_64_prologue(JitContext* context) {
    // TODO: Implement x86_64 prologue
    return 1;
}

int jit_generate_x86_64_epilogue(JitContext* context) {
    // TODO: Implement x86_64 epilogue
    return 1;
}

int jit_emit_x86_64_call(JitContext* context, void* target) {
    // TODO: Implement x86_64 call instruction
    return 1;
}

int jit_generate_x86_64_binary_op(JitContext* context, ASTNode* op_node) {
    // TODO: Implement x86_64 binary operations
    return 1;
}

int jit_generate_x86_64_unary_op(JitContext* context, ASTNode* op_node) {
    // TODO: Implement x86_64 unary operations
    return 1;
}

int jit_emit_x86_64_load(JitContext* context, const char* var_name) {
    // TODO: Implement x86_64 load instruction
    return 1;
}

int jit_emit_x86_64_store(JitContext* context, const char* var_name) {
    // TODO: Implement x86_64 store instruction
    return 1;
}
#endif

#ifdef __aarch64__
int jit_generate_arm64_prologue(JitContext* context) {
    // TODO: Implement ARM64 prologue
    return 1;
}

int jit_generate_arm64_epilogue(JitContext* context) {
    // TODO: Implement ARM64 epilogue
    return 1;
}

int jit_emit_arm64_call(JitContext* context, void* target) {
    // TODO: Implement ARM64 call instruction
    return 1;
}

int jit_generate_arm64_binary_op(JitContext* context, ASTNode* op_node) {
    // TODO: Implement ARM64 binary operations
    return 1;
}

int jit_generate_arm64_unary_op(JitContext* context, ASTNode* op_node) {
    // TODO: Implement ARM64 unary operations
    return 1;
}

int jit_emit_arm64_load(JitContext* context, const char* var_name) {
    // TODO: Implement ARM64 load instruction
    return 1;
}

int jit_emit_arm64_store(JitContext* context, const char* var_name) {
    // TODO: Implement ARM64 store instruction
    return 1;
}
#endif

// Utility functions
JitTargetArchitecture jit_detect_target_architecture(void) {
#ifdef __x86_64__
    return JIT_TARGET_X86_64;
#elif defined(__aarch64__)
    return JIT_TARGET_ARM64;
#elif defined(__arm__)
    return JIT_TARGET_ARM;
#else
    return JIT_TARGET_X86_64; // Default fallback
#endif
}

const char* jit_target_to_string(JitTargetArchitecture target) {
    switch (target) {
        case JIT_TARGET_X86_64: return "x86_64";
        case JIT_TARGET_ARM64: return "arm64";
        case JIT_TARGET_ARM: return "arm";
        case JIT_TARGET_AUTO: return "auto";
        default: return "unknown";
    }
}

const char* jit_mode_to_string(JitCompilationMode mode) {
    switch (mode) {
        case JIT_MODE_INTERPRETED: return "interpreted";
        case JIT_MODE_HYBRID: return "hybrid";
        case JIT_MODE_COMPILED: return "compiled";
        default: return "unknown";
    }
}

int jit_is_function_compilable(ASTNode* function_node) {
    if (!function_node || function_node->type != AST_NODE_FUNCTION) return 0;
    
    // Check if function has a simple enough body for JIT compilation
    // For now, allow all functions
    return 1;
}

int jit_estimate_compilation_cost(ASTNode* node) {
    if (!node) return 0;
    
    // Simple cost estimation based on node type
    switch (node->type) {
        case AST_NODE_NUMBER:
        case AST_NODE_STRING:
        case AST_NODE_BOOL:
        case AST_NODE_NULL:
            return 1;
        case AST_NODE_BINARY_OP:
        case AST_NODE_UNARY_OP:
            return 2;
        case AST_NODE_FUNCTION_CALL:
            return 5;
        case AST_NODE_IF_STATEMENT:
        case AST_NODE_WHILE_LOOP:
        case AST_NODE_FOR_LOOP:
            return 10;
        default:
            return 3;
    }
}

// Error handling
void jit_set_error(JitContext* context, const char* error_message) {
    if (!context) return;
    // Store error in context for later retrieval
}

const char* jit_get_last_error(JitContext* context) {
    if (!context) return "No error";
    return "Unknown error";
}

void jit_clear_error(JitContext* context) {
    if (!context) return;
    // Clear error state
}

// Configuration
void jit_set_optimization_level(JitContext* context, int level) {
    if (!context) return;
    context->enable_optimizations = level > 0;
    context->enable_inlining = level >= 2;
    context->enable_constant_folding = level >= 1;
}

void jit_enable_debug_mode(JitContext* context, int enable) {
    if (!context) return;
    context->debug_mode = enable;
}

void jit_set_code_buffer_size(JitContext* context, size_t size) {
    if (!context) return;
    if (context->code_buffer) {
        munmap(context->code_buffer, context->code_capacity);
    }
    jit_allocate_code_buffer(context, size);
}

// Debug and profiling
void jit_print_compiled_code(JitContext* context) {
    if (!context || !context->code_buffer) return;
    
    printf("JIT Compiled Code (size: %zu bytes):\n", context->code_size);
    for (size_t i = 0; i < context->code_size; i++) {
        printf("%02x ", context->code_buffer[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    if (context->code_size % 16 != 0) printf("\n");
}

void jit_print_function_table(JitContext* context) {
    if (!context) return;
    
    printf("JIT Function Table:\n");
    for (size_t i = 0; i < context->function_count; i++) {
        JitFunction* func = context->compiled_functions[i];
        printf("  %s: %p (size: %zu, compiled: %s)\n", 
               func->name, func->native_code, func->code_size,
               func->is_compiled ? "yes" : "no");
    }
}

void jit_profile_execution(JitContext* context, const char* function_name) {
    if (!context || !function_name) return;
    
    // TODO: Implement execution profiling
    printf("Profiling function: %s\n", function_name);
}

// Additional helper functions for control flow
int jit_generate_if_statement(JitContext* context, ASTNode* if_node) {
    // TODO: Implement if statement code generation
    return 1;
}

int jit_generate_while_loop(JitContext* context, ASTNode* while_node) {
    // TODO: Implement while loop code generation
    return 1;
}

int jit_generate_for_loop(JitContext* context, ASTNode* for_node) {
    // TODO: Implement for loop code generation
    return 1;
}

// Additional missing functions
int jit_generate_expression(JitContext* context, ASTNode* expression) {
    if (!context || !expression) return 0;
    
    switch (expression->type) {
        case AST_NODE_NUMBER:
            // Load immediate number value
            // TODO: Add statistics tracking
            return 1;
            
        case AST_NODE_STRING:
            // Load string literal
            // TODO: Add statistics tracking
            return 1;
            
        case AST_NODE_BOOL:
            // Load boolean value (0 or 1)
            // TODO: Add statistics tracking
            return 1;
            
        case AST_NODE_NULL:
            // Load null value (0)
            // TODO: Add statistics tracking
            return 1;
            
        case AST_NODE_BINARY_OP:
            return jit_generate_binary_op(context, expression);
        case AST_NODE_UNARY_OP:
            return jit_generate_unary_op(context, expression);
        case AST_NODE_FUNCTION_CALL:
            return jit_generate_function_call(context, expression);
        case AST_NODE_IDENTIFIER:
            return jit_generate_variable_access(context, expression);
        case AST_NODE_ASSIGNMENT:
            return jit_generate_variable_assignment(context, expression);
        case AST_NODE_IF_STATEMENT:
        case AST_NODE_WHILE_LOOP:
        case AST_NODE_FOR_LOOP:
            return jit_generate_control_flow(context, expression);
        default:
            return 1; // Skip unsupported expression types
    }
}
