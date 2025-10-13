#ifndef MICRO_JIT_H
#define MICRO_JIT_H

#include "../interpreter/interpreter_core.h"
#include "../ast.h"
#include "bytecode_engine.h"
#include "hot_spot_tracker.h"
#include <stdint.h>
#include <stddef.h>

// ============================================================================
// MICRO-JIT COMPILER FOR HOT PATHS
// ============================================================================

// JIT compilation targets (reuse existing JIT enums)
typedef JitTargetArchitecture JitTarget;

// JIT compilation modes
typedef enum {
    MICRO_JIT_MODE_DISABLED = 0,    // No JIT compilation
    MICRO_JIT_MODE_BYTECODE = 1,    // Bytecode only
    MICRO_JIT_MODE_MICRO = 2,       // Micro-JIT for hot paths
    MICRO_JIT_MODE_FULL = 3         // Full JIT compilation
} MicroJitMode;

// JIT compilation status
typedef enum {
    JIT_STATUS_SUCCESS = 0,   // Compilation successful
    JIT_STATUS_FAILED = 1,    // Compilation failed
    JIT_STATUS_DEOPTIMIZED = 2, // Code deoptimized due to guard failure
    JIT_STATUS_CACHE_FULL = 3,  // Code cache full
    JIT_STATUS_UNSUPPORTED = 4  // Unsupported operation
} JitStatus;

// Native code function signature
typedef Value (*NativeFunction)(Value* args, size_t arg_count, Interpreter* interpreter);

// JIT compiled function metadata
typedef struct {
    NativeFunction native_code;    // Pointer to generated native code
    size_t code_size;              // Size of generated code in bytes
    uint8_t* code_buffer;          // Executable memory buffer
    BytecodeProgram* source_bytecode; // Source bytecode for deoptimization
    ASTNode* source_ast;           // Source AST node
    HotSpotInfo* hot_spot_info;    // Associated hot spot information
    
    // Guard information
    ValueType* parameter_types;    // Expected parameter types
    size_t parameter_count;        // Number of parameters
    int guard_count;               // Number of guard checks
    int deoptimization_count;      // Number of times deoptimized
    
    // Performance metrics
    uint64_t execution_count;      // Number of times executed
    uint64_t total_time_ns;        // Total execution time
    uint64_t avg_time_ns;          // Average execution time
    double speedup_factor;         // Measured speedup over bytecode
    
    // Memory management
    uint64_t created_at_ns;        // Creation timestamp
    uint64_t last_used_ns;         // Last execution timestamp
    int is_valid;                  // Is this compiled function still valid
} JitCompiledFunction;

// JIT code cache
typedef struct {
    JitCompiledFunction* functions; // Array of compiled functions
    size_t function_count;          // Number of compiled functions
    size_t capacity;                // Maximum number of functions
    size_t total_code_size;         // Total size of generated code
    size_t max_code_size;           // Maximum allowed code size (1MB)
    
    // Cache management
    int lru_enabled;                // Enable LRU eviction
    uint64_t* last_access_times;    // Last access time for each function
    size_t eviction_count;          // Number of functions evicted
    
    // Statistics
    uint64_t compilation_count;     // Total compilations attempted
    uint64_t success_count;         // Successful compilations
    uint64_t deoptimization_count;  // Total deoptimizations
    uint64_t cache_hit_count;       // Cache hits
    uint64_t cache_miss_count;      // Cache misses
} JitCodeCache;

// Micro-JIT compiler context
typedef struct {
    JitTarget target;               // Target architecture
    MicroJitMode mode;              // Compilation mode
    JitCodeCache* code_cache;       // Code cache
    
    // Platform-specific information
    int is_arm64;                   // ARM64 platform flag
    int is_x86_64;                  // x86_64 platform flag
    int has_executable_memory;      // Can allocate executable memory
    
    // Compilation settings
    size_t max_function_size;       // Maximum function size to compile (4KB)
    int enable_inlining;            // Enable function inlining
    int enable_specialization;      // Enable type specialization
    int enable_guards;              // Enable guard checks
    
    // Performance tracking
    uint64_t total_compilation_time_ns; // Total time spent compiling
    uint64_t total_execution_time_ns;   // Total time spent in JIT code
    double overall_speedup;         // Overall speedup factor
} MicroJitContext;

// ============================================================================
// MICRO-JIT COMPILER FUNCTIONS
// ============================================================================

// Context management
MicroJitContext* micro_jit_create(JitTarget target, MicroJitMode mode);
void micro_jit_free(MicroJitContext* context);
void micro_jit_reset(MicroJitContext* context);

// Configuration
void micro_jit_set_mode(MicroJitContext* context, MicroJitMode mode);
void micro_jit_set_max_function_size(MicroJitContext* context, size_t max_size);
void micro_jit_set_inlining(MicroJitContext* context, int enabled);
void micro_jit_set_specialization(MicroJitContext* context, int enabled);
void micro_jit_set_guards(MicroJitContext* context, int enabled);

// Code compilation
JitStatus micro_jit_compile_function(MicroJitContext* context, 
                                    BytecodeProgram* bytecode, 
                                    ASTNode* ast_node,
                                    HotSpotInfo* hot_spot_info,
                                    JitCompiledFunction** compiled_func);

JitStatus micro_jit_compile_bytecode(MicroJitContext* context,
                                    BytecodeProgram* bytecode,
                                    uint8_t** native_code,
                                    size_t* code_size);

// Code execution
Value micro_jit_execute_function(JitCompiledFunction* compiled_func,
                                Value* args,
                                size_t arg_count,
                                Interpreter* interpreter);

// Guard checking
int micro_jit_check_guards(JitCompiledFunction* compiled_func,
                          Value* args,
                          size_t arg_count);

void micro_jit_deoptimize_function(JitCompiledFunction* compiled_func);

// Code cache management
JitCompiledFunction* micro_jit_find_function(MicroJitContext* context,
                                            ASTNode* ast_node);

void micro_jit_evict_cold_functions(MicroJitContext* context);
void micro_jit_clear_cache(MicroJitContext* context);

// Memory management
uint8_t* micro_jit_allocate_executable_memory(size_t size);
void micro_jit_free_executable_memory(uint8_t* memory, size_t size);

// Platform-specific code generation
JitStatus micro_jit_generate_arm64(BytecodeProgram* bytecode,
                                  uint8_t** native_code,
                                  size_t* code_size);

JitStatus micro_jit_generate_x86_64(BytecodeProgram* bytecode,
                                   uint8_t** native_code,
                                   size_t* code_size);

// Optimization passes
int micro_jit_optimize_bytecode(BytecodeProgram* bytecode);
int micro_jit_inline_functions(BytecodeProgram* bytecode);
int micro_jit_specialize_types(BytecodeProgram* bytecode, ValueType* types, size_t count);

// Safety and validation
int micro_jit_validate_generated_code(uint8_t* code, size_t size);
int micro_jit_test_generated_code(JitCompiledFunction* compiled_func);

// Statistics and reporting
void micro_jit_print_statistics(MicroJitContext* context);
void micro_jit_print_cache_status(MicroJitContext* context);
double micro_jit_get_speedup_factor(MicroJitContext* context);
uint64_t micro_jit_get_total_compilation_time(MicroJitContext* context);

// Integration with interpreter
void micro_jit_initialize_for_interpreter(Interpreter* interpreter);
void micro_jit_cleanup_for_interpreter(Interpreter* interpreter);
MicroJitContext* micro_jit_get_from_interpreter(Interpreter* interpreter);

// ============================================================================
// PLATFORM DETECTION AND UTILITIES
// ============================================================================

// Platform detection
JitTarget micro_jit_detect_platform(void);
int micro_jit_is_arm64(void);
int micro_jit_is_x86_64(void);
int micro_jit_has_executable_memory_support(void);

// Memory utilities
void* micro_jit_align_memory(void* ptr, size_t alignment);
size_t micro_jit_get_page_size(void);
int micro_jit_make_executable(void* memory, size_t size);

// Assembly utilities
void micro_jit_emit_nop(uint8_t* code, size_t* offset);
void micro_jit_emit_ret(uint8_t* code, size_t* offset);
void micro_jit_emit_call(uint8_t* code, size_t* offset, void* target);
void micro_jit_emit_jump(uint8_t* code, size_t* offset, void* target);

// ============================================================================
// ERROR HANDLING
// ============================================================================

typedef enum {
    MICRO_JIT_ERROR_NONE = 0,
    MICRO_JIT_ERROR_MEMORY_ALLOCATION,
    MICRO_JIT_ERROR_UNSUPPORTED_PLATFORM,
    MICRO_JIT_ERROR_INVALID_BYTECODE,
    MICRO_JIT_ERROR_COMPILATION_FAILED,
    MICRO_JIT_ERROR_CACHE_FULL,
    MICRO_JIT_ERROR_GUARD_FAILED,
    MICRO_JIT_ERROR_EXECUTION_FAILED
} MicroJitError;

const char* micro_jit_error_string(MicroJitError error);
void micro_jit_set_error(MicroJitContext* context, MicroJitError error);
MicroJitError micro_jit_get_last_error(MicroJitContext* context);

#endif // MICRO_JIT_H
