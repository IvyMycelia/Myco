#ifndef JIT_COMPILER_H
#define JIT_COMPILER_H

#include "ast.h"
#include "type_checker.h"
#include <stddef.h>
#include <stdint.h>

// JIT compilation target architectures
typedef enum {
    JIT_TARGET_X86_64,
    JIT_TARGET_ARM64,
    JIT_TARGET_ARM,
    JIT_TARGET_AUTO
} JitTargetArchitecture;

// JIT compilation modes
typedef enum {
    JIT_MODE_INTERPRETED,    // Pure interpretation
    JIT_MODE_HYBRID,         // Mix of interpretation and compilation
    JIT_MODE_COMPILED        // Full compilation where possible
} JitCompilationMode;

// JIT function signature
typedef struct JitFunction {
    void* native_code;
    MycoType* signature;
    char* name;
    size_t code_size;
    int is_compiled;
} JitFunction;

// Native code generation context
typedef struct JitContext {
    JitTargetArchitecture target;
    JitCompilationMode mode;
    uint8_t* code_buffer;
    size_t code_size;
    size_t code_capacity;
    size_t code_position;
    
    // Function table for compiled functions
    void** function_table;
    JitFunction** compiled_functions;
    size_t function_count;
    size_t function_capacity;
    
    // Type information for compiled code
    MycoType** function_types;
    char** function_names;
    
    // Runtime data
    void* runtime_data;
    size_t runtime_size;
    
    // Optimization flags
    int enable_optimizations;
    int enable_inlining;
    int enable_constant_folding;
    
    // Debug information
    int debug_mode;
    char** debug_symbols;
    size_t debug_symbol_count;
} JitContext;

// JIT compilation results
typedef struct JitResult {
    int success;
    JitFunction* compiled_functions;
    size_t function_count;
    char* error_message;
    size_t total_code_size;
    double compilation_time;
} JitResult;

// JIT compiler functions
JitContext* jit_context_create(JitTargetArchitecture target, JitCompilationMode mode);
void jit_context_free(JitContext* context);

// Main JIT compilation
JitResult* jit_compile_ast(JitContext* context, ASTNode* ast);
JitResult* jit_compile_function(JitContext* context, ASTNode* function_node);
JitResult* jit_compile_expression(JitContext* context, ASTNode* expression);

// Code generation
int jit_generate_prologue(JitContext* context);
int jit_generate_epilogue(JitContext* context);
int jit_generate_expression(JitContext* context, ASTNode* expression);
int jit_generate_function_call(JitContext* context, ASTNode* call_node);
int jit_generate_binary_op(JitContext* context, ASTNode* op_node);
int jit_generate_unary_op(JitContext* context, ASTNode* op_node);
int jit_generate_variable_access(JitContext* context, ASTNode* var_node);
int jit_generate_variable_assignment(JitContext* context, ASTNode* assign_node);
int jit_generate_control_flow(JitContext* context, ASTNode* control_node);
int jit_generate_if_statement(JitContext* context, ASTNode* if_node);
int jit_generate_while_loop(JitContext* context, ASTNode* while_node);
int jit_generate_for_loop(JitContext* context, ASTNode* for_node);

// Native code execution
void* jit_execute_function(JitContext* context, const char* function_name, void** args);
void* jit_execute_compiled_code(JitContext* context, void* code_ptr, void** args);

// Memory management
int jit_allocate_code_buffer(JitContext* context, size_t size);
int jit_allocate_runtime_data(JitContext* context, size_t size);
void jit_free_compiled_code(JitContext* context);

// Optimization passes
int jit_optimize_code(JitContext* context);
int jit_inline_functions(JitContext* context);
int jit_constant_fold(JitContext* context);
int jit_eliminate_dead_code(JitContext* context);

// Platform-specific code generation
#ifdef __x86_64__
int jit_generate_x86_64_prologue(JitContext* context);
int jit_generate_x86_64_epilogue(JitContext* context);
int jit_emit_x86_64_call(JitContext* context, void* target);
int jit_generate_x86_64_binary_op(JitContext* context, ASTNode* op_node);
int jit_generate_x86_64_unary_op(JitContext* context, ASTNode* op_node);
int jit_emit_x86_64_load(JitContext* context, const char* var_name);
int jit_emit_x86_64_store(JitContext* context, const char* var_name);
int jit_emit_x86_64_instruction(JitContext* context, const char* mnemonic, ...);
#endif

#ifdef __aarch64__
int jit_generate_arm64_prologue(JitContext* context);
int jit_generate_arm64_epilogue(JitContext* context);
int jit_emit_arm64_call(JitContext* context, void* target);
int jit_generate_arm64_binary_op(JitContext* context, ASTNode* op_node);
int jit_generate_arm64_unary_op(JitContext* context, ASTNode* op_node);
int jit_emit_arm64_load(JitContext* context, const char* var_name);
int jit_emit_arm64_store(JitContext* context, const char* var_name);
int jit_emit_arm64_instruction(JitContext* context, const char* mnemonic, ...);
#endif

// Debug and profiling
void jit_print_compiled_code(JitContext* context);
void jit_print_function_table(JitContext* context);
void jit_profile_execution(JitContext* context, const char* function_name);

// Utility functions
JitTargetArchitecture jit_detect_target_architecture(void);
const char* jit_target_to_string(JitTargetArchitecture target);
const char* jit_mode_to_string(JitCompilationMode mode);
int jit_is_function_compilable(ASTNode* function_node);
int jit_estimate_compilation_cost(ASTNode* node);

// Error handling
void jit_set_error(JitContext* context, const char* error_message);
const char* jit_get_last_error(JitContext* context);
void jit_clear_error(JitContext* context);

// Configuration
void jit_set_optimization_level(JitContext* context, int level);
void jit_enable_debug_mode(JitContext* context, int enable);
void jit_set_code_buffer_size(JitContext* context, size_t size);

#endif // JIT_COMPILER_H
