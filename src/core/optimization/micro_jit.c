#include "../../include/core/optimization/micro_jit.h"
#include "../../include/core/interpreter/interpreter_core.h"
#include "../../include/core/ast.h"
#include "../../include/core/optimization/bytecode_engine.h"
#include "../../include/core/optimization/hot_spot_tracker.h"
#include "../../include/core/interpreter/value_operations.h"
#include "../../include/utils/shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>

// ============================================================================
// PLATFORM DETECTION
// ============================================================================

JitTarget micro_jit_detect_platform(void) {
#if defined(__aarch64__) || defined(__arm64__)
    return JIT_TARGET_ARM64;
#elif defined(__x86_64__)
    return JIT_TARGET_X86_64;
#else
    return JIT_TARGET_ARM64; // Default to ARM64
#endif
}

int micro_jit_is_arm64(void) {
    return micro_jit_detect_platform() == JIT_TARGET_ARM64;
}

int micro_jit_is_x86_64(void) {
    return micro_jit_detect_platform() == JIT_TARGET_X86_64;
}

int micro_jit_has_executable_memory_support(void) {
    // Check if we can allocate executable memory
    void* test_mem = mmap(NULL, 4096, PROT_READ | PROT_WRITE | PROT_EXEC, 
                         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (test_mem == MAP_FAILED) {
        return 0;
    }
    munmap(test_mem, 4096);
    return 1;
}

// ============================================================================
// CONTEXT MANAGEMENT
// ============================================================================

MicroJitContext* micro_jit_create(JitTarget target, MicroJitMode mode) {
    MicroJitContext* context = (MicroJitContext*)shared_malloc_safe(
        sizeof(MicroJitContext), "micro_jit", "context_create", 0);
    if (!context) return NULL;
    
    // Initialize context
    context->target = target;
    context->mode = mode;
    context->code_cache = NULL;
    
    // Platform detection
    context->is_arm64 = micro_jit_is_arm64();
    context->is_x86_64 = micro_jit_is_x86_64();
    context->has_executable_memory = micro_jit_has_executable_memory_support();
    
    // Compilation settings
    context->max_function_size = 4096; // 4KB default
    context->enable_inlining = 1;
    context->enable_specialization = 1;
    context->enable_guards = 1;
    
    // Performance tracking
    context->total_compilation_time_ns = 0;
    context->total_execution_time_ns = 0;
    context->overall_speedup = 1.0;
    
    // Create code cache
    context->code_cache = (JitCodeCache*)shared_malloc_safe(
        sizeof(JitCodeCache), "micro_jit", "cache_create", 0);
    if (!context->code_cache) {
        micro_jit_free(context);
        return NULL;
    }
    
    // Initialize code cache
    context->code_cache->functions = NULL;
    context->code_cache->function_count = 0;
    context->code_cache->capacity = 0;
    context->code_cache->total_code_size = 0;
    context->code_cache->max_code_size = 1024 * 1024; // 1MB
    context->code_cache->lru_enabled = 1;
    context->code_cache->last_access_times = NULL;
    context->code_cache->eviction_count = 0;
    context->code_cache->compilation_count = 0;
    context->code_cache->success_count = 0;
    context->code_cache->deoptimization_count = 0;
    context->code_cache->cache_hit_count = 0;
    context->code_cache->cache_miss_count = 0;
    
    return context;
}

void micro_jit_free(MicroJitContext* context) {
    if (!context) return;
    
    // Free code cache
    if (context->code_cache) {
        // Free all compiled functions
        for (size_t i = 0; i < context->code_cache->function_count; i++) {
            JitCompiledFunction* func = &context->code_cache->functions[i];
            if (func->code_buffer) {
                micro_jit_free_executable_memory(func->code_buffer, func->code_size);
            }
            if (func->parameter_types) {
                shared_free_safe(func->parameter_types, "micro_jit", "free_func", 0);
            }
        }
        
        if (context->code_cache->functions) {
            shared_free_safe(context->code_cache->functions, "micro_jit", "free_cache", 0);
        }
        if (context->code_cache->last_access_times) {
            shared_free_safe(context->code_cache->last_access_times, "micro_jit", "free_cache", 0);
        }
        
        shared_free_safe(context->code_cache, "micro_jit", "free_cache", 0);
    }
    
    shared_free_safe(context, "micro_jit", "free_context", 0);
}

void micro_jit_reset(MicroJitContext* context) {
    if (!context) return;
    
    // Clear code cache
    micro_jit_clear_cache(context);
    
    // Reset statistics
    context->total_compilation_time_ns = 0;
    context->total_execution_time_ns = 0;
    context->overall_speedup = 1.0;
}

// ============================================================================
// CONFIGURATION
// ============================================================================

void micro_jit_set_mode(MicroJitContext* context, MicroJitMode mode) {
    if (context) {
        context->mode = mode;
    }
}

void micro_jit_set_max_function_size(MicroJitContext* context, size_t max_size) {
    if (context) {
        context->max_function_size = max_size;
    }
}

void micro_jit_set_inlining(MicroJitContext* context, int enabled) {
    if (context) {
        context->enable_inlining = enabled ? 1 : 0;
    }
}

void micro_jit_set_specialization(MicroJitContext* context, int enabled) {
    if (context) {
        context->enable_specialization = enabled ? 1 : 0;
    }
}

void micro_jit_set_guards(MicroJitContext* context, int enabled) {
    if (context) {
        context->enable_guards = enabled ? 1 : 0;
    }
}

// ============================================================================
// MEMORY MANAGEMENT
// ============================================================================

uint8_t* micro_jit_allocate_executable_memory(size_t size) {
    // Align size to page boundary
    size_t page_size = micro_jit_get_page_size();
    size_t aligned_size = (size + page_size - 1) & ~(page_size - 1);
    
    // Allocate executable memory
    void* memory = mmap(NULL, aligned_size, 
                       PROT_READ | PROT_WRITE | PROT_EXEC,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    if (memory == MAP_FAILED) {
        return NULL;
    }
    
    return (uint8_t*)memory;
}

void micro_jit_free_executable_memory(uint8_t* memory, size_t size) {
    if (memory) {
        size_t page_size = micro_jit_get_page_size();
        size_t aligned_size = (size + page_size - 1) & ~(page_size - 1);
        munmap(memory, aligned_size);
    }
}

size_t micro_jit_get_page_size(void) {
    static size_t page_size = 0;
    if (page_size == 0) {
        page_size = getpagesize();
    }
    return page_size;
}

int micro_jit_make_executable(void* memory, size_t size) {
    return mprotect(memory, size, PROT_READ | PROT_EXEC) == 0;
}

void* micro_jit_align_memory(void* ptr, size_t alignment) {
    uintptr_t addr = (uintptr_t)ptr;
    uintptr_t aligned = (addr + alignment - 1) & ~(alignment - 1);
    return (void*)aligned;
}

// ============================================================================
// CODE COMPILATION
// ============================================================================

JitStatus micro_jit_compile_function(MicroJitContext* context, 
                                    BytecodeProgram* bytecode, 
                                    ASTNode* ast_node,
                                    HotSpotInfo* hot_spot_info,
                                    JitCompiledFunction** compiled_func) {
    if (!context || !bytecode || !ast_node || !compiled_func) {
        return JIT_STATUS_FAILED;
    }
    
    // Check if compilation is enabled
    if (context->mode == MICRO_JIT_MODE_DISABLED || context->mode == MICRO_JIT_MODE_BYTECODE) {
        return JIT_STATUS_UNSUPPORTED;
    }
    
    // Check if we have executable memory support
    if (!context->has_executable_memory) {
        return JIT_STATUS_UNSUPPORTED;
    }
    
    // Check function size
    if (bytecode->instruction_count > context->max_function_size / 4) {
        return JIT_STATUS_UNSUPPORTED; // Too large to compile
    }
    
    uint64_t start_time = get_current_time_ns();
    
    // Generate native code
    uint8_t* native_code = NULL;
    size_t code_size = 0;
    JitStatus status = micro_jit_compile_bytecode(context, bytecode, &native_code, &code_size);
    
    if (status != JIT_STATUS_SUCCESS) {
        return status;
    }
    
    // Create compiled function metadata
    JitCompiledFunction* func = (JitCompiledFunction*)shared_malloc_safe(
        sizeof(JitCompiledFunction), "micro_jit", "compile_func", 0);
    if (!func) {
        micro_jit_free_executable_memory(native_code, code_size);
        return JIT_STATUS_FAILED;
    }
    
    // Initialize function metadata
    func->native_code = (NativeFunction)native_code;
    func->code_size = code_size;
    func->code_buffer = native_code;
    func->source_bytecode = bytecode;
    func->source_ast = ast_node;
    func->hot_spot_info = hot_spot_info;
    func->parameter_types = NULL;
    func->parameter_count = 0;
    func->guard_count = 0;
    func->deoptimization_count = 0;
    func->execution_count = 0;
    func->total_time_ns = 0;
    func->avg_time_ns = 0;
    func->speedup_factor = 1.0;
    func->created_at_ns = get_current_time_ns();
    func->last_used_ns = func->created_at_ns;
    func->is_valid = 1;
    
    // Set parameter types if available
    if (hot_spot_info && hot_spot_info->parameter_types) {
        func->parameter_types = (ValueType*)shared_malloc_safe(
            hot_spot_info->parameter_count * sizeof(ValueType), 
            "micro_jit", "compile_func", 0);
        if (func->parameter_types) {
            memcpy(func->parameter_types, hot_spot_info->parameter_types,
                   hot_spot_info->parameter_count * sizeof(ValueType));
            func->parameter_count = hot_spot_info->parameter_count;
        }
    }
    
    // Add to code cache
    if (context->code_cache) {
        // TODO: Implement code cache management
        // For now, just store the function
    }
    
    *compiled_func = func;
    
    // Update statistics
    uint64_t end_time = get_current_time_ns();
    context->total_compilation_time_ns += (end_time - start_time);
    context->code_cache->compilation_count++;
    context->code_cache->success_count++;
    
    return JIT_STATUS_SUCCESS;
}

JitStatus micro_jit_compile_bytecode(MicroJitContext* context,
                                    BytecodeProgram* bytecode,
                                    uint8_t** native_code,
                                    size_t* code_size) {
    if (!context || !bytecode || !native_code || !code_size) {
        return JIT_STATUS_FAILED;
    }
    
    // Choose target architecture
    JitTarget target = context->target;
    if (target == JIT_TARGET_AUTO) {
        target = micro_jit_detect_platform();
    }
    
    // Generate platform-specific code
    switch (target) {
        case JIT_TARGET_ARM64:
            return micro_jit_generate_arm64(bytecode, native_code, code_size);
        case JIT_TARGET_X86_64:
            return micro_jit_generate_x86_64(bytecode, native_code, code_size);
        default:
            return JIT_STATUS_UNSUPPORTED;
    }
}

// ============================================================================
// PLATFORM-SPECIFIC CODE GENERATION
// ============================================================================

JitStatus micro_jit_generate_arm64(BytecodeProgram* bytecode,
                                  uint8_t** native_code,
                                  size_t* code_size) {
    if (!bytecode || !native_code || !code_size) {
        return JIT_STATUS_FAILED;
    }
    
    // Estimate code size (rough approximation)
    size_t estimated_size = bytecode->instruction_count * 16; // ~16 bytes per instruction
    if (estimated_size < 64) estimated_size = 64; // Minimum size
    
    // Allocate executable memory
    uint8_t* code = micro_jit_allocate_executable_memory(estimated_size);
    if (!code) {
        return JIT_STATUS_FAILED;
    }
    
    size_t offset = 0;
    
    // Generate ARM64 prologue
    // TODO: Implement proper ARM64 code generation
    // For now, generate a simple function that calls back to interpreter
    
    // Generate epilogue
    micro_jit_emit_ret(code, &offset);
    
    *native_code = code;
    *code_size = offset;
    
    return JIT_STATUS_SUCCESS;
}

JitStatus micro_jit_generate_x86_64(BytecodeProgram* bytecode,
                                   uint8_t** native_code,
                                   size_t* code_size) {
    if (!bytecode || !native_code || !code_size) {
        return JIT_STATUS_FAILED;
    }
    
    // Estimate code size (rough approximation)
    size_t estimated_size = bytecode->instruction_count * 12; // ~12 bytes per instruction
    if (estimated_size < 64) estimated_size = 64; // Minimum size
    
    // Allocate executable memory
    uint8_t* code = micro_jit_allocate_executable_memory(estimated_size);
    if (!code) {
        return JIT_STATUS_FAILED;
    }
    
    size_t offset = 0;
    
    // Generate x86_64 prologue
    // TODO: Implement proper x86_64 code generation
    // For now, generate a simple function that calls back to interpreter
    
    // Generate epilogue
    micro_jit_emit_ret(code, &offset);
    
    *native_code = code;
    *code_size = offset;
    
    return JIT_STATUS_SUCCESS;
}

// ============================================================================
// ASSEMBLY UTILITIES
// ============================================================================

void micro_jit_emit_nop(uint8_t* code, size_t* offset) {
    if (code && offset) {
        code[*offset] = 0x90; // x86_64 NOP
        (*offset)++;
    }
}

void micro_jit_emit_ret(uint8_t* code, size_t* offset) {
    if (code && offset) {
        code[*offset] = 0xC3; // x86_64 RET
        (*offset)++;
    }
}

void micro_jit_emit_call(uint8_t* code, size_t* offset, void* target) {
    if (code && offset) {
        // TODO: Implement proper call instruction
        // For now, just emit a placeholder
        micro_jit_emit_nop(code, offset);
    }
}

void micro_jit_emit_jump(uint8_t* code, size_t* offset, void* target) {
    if (code && offset) {
        // TODO: Implement proper jump instruction
        // For now, just emit a placeholder
        micro_jit_emit_nop(code, offset);
    }
}

// ============================================================================
// CODE EXECUTION
// ============================================================================

Value micro_jit_execute_function(JitCompiledFunction* compiled_func,
                                Value* args,
                                size_t arg_count,
                                Interpreter* interpreter) {
    if (!compiled_func || !compiled_func->is_valid) {
        return value_create_null();
    }
    
    // Check guards if enabled
    if (compiled_func->guard_count > 0) {
        if (!micro_jit_check_guards(compiled_func, args, arg_count)) {
            // Guard failed, deoptimize
            micro_jit_deoptimize_function(compiled_func);
            return value_create_null();
        }
    }
    
    uint64_t start_time = get_current_time_ns();
    
    // Execute native code
    Value result = compiled_func->native_code(args, arg_count, interpreter);
    
    uint64_t end_time = get_current_time_ns();
    
    // Update statistics
    compiled_func->execution_count++;
    compiled_func->total_time_ns += (end_time - start_time);
    compiled_func->avg_time_ns = compiled_func->total_time_ns / compiled_func->execution_count;
    compiled_func->last_used_ns = end_time;
    
    return result;
}

// ============================================================================
// GUARD CHECKING
// ============================================================================

int micro_jit_check_guards(JitCompiledFunction* compiled_func,
                          Value* args,
                          size_t arg_count) {
    if (!compiled_func || !args) {
        return 0;
    }
    
    // Check parameter types
    if (compiled_func->parameter_types && compiled_func->parameter_count > 0) {
        for (size_t i = 0; i < compiled_func->parameter_count && i < arg_count; i++) {
            if (args[i].type != compiled_func->parameter_types[i]) {
                return 0; // Type mismatch
            }
        }
    }
    
    return 1; // All guards passed
}

void micro_jit_deoptimize_function(JitCompiledFunction* compiled_func) {
    if (compiled_func) {
        compiled_func->is_valid = 0;
        compiled_func->deoptimization_count++;
    }
}

// ============================================================================
// CODE CACHE MANAGEMENT
// ============================================================================

JitCompiledFunction* micro_jit_find_function(MicroJitContext* context,
                                            ASTNode* ast_node) {
    if (!context || !context->code_cache || !ast_node) {
        return NULL;
    }
    
    // TODO: Implement proper function lookup
    // For now, return NULL (no caching implemented yet)
    return NULL;
}

void micro_jit_evict_cold_functions(MicroJitContext* context) {
    if (!context || !context->code_cache) {
        return;
    }
    
    // TODO: Implement LRU eviction
    // For now, do nothing
}

void micro_jit_clear_cache(MicroJitContext* context) {
    if (!context || !context->code_cache) {
        return;
    }
    
    // Free all compiled functions
    for (size_t i = 0; i < context->code_cache->function_count; i++) {
        JitCompiledFunction* func = &context->code_cache->functions[i];
        if (func->code_buffer) {
            micro_jit_free_executable_memory(func->code_buffer, func->code_size);
        }
        if (func->parameter_types) {
            shared_free_safe(func->parameter_types, "micro_jit", "clear_cache", 0);
        }
    }
    
    // Reset cache
    context->code_cache->function_count = 0;
    context->code_cache->total_code_size = 0;
    context->code_cache->eviction_count = 0;
}

// ============================================================================
// OPTIMIZATION PASSES
// ============================================================================

int micro_jit_optimize_bytecode(BytecodeProgram* bytecode) {
    if (!bytecode) return 0;
    
    // TODO: Implement bytecode optimizations
    // - Constant folding
    // - Dead code elimination
    // - Peephole optimizations
    
    return 1;
}

int micro_jit_inline_functions(BytecodeProgram* bytecode) {
    if (!bytecode) return 0;
    
    // TODO: Implement function inlining
    // - Identify small functions
    // - Inline them at call sites
    
    return 1;
}

int micro_jit_specialize_types(BytecodeProgram* bytecode, ValueType* types, size_t count) {
    if (!bytecode || !types) return 0;
    
    // TODO: Implement type specialization
    // - Generate specialized bytecode for known types
    // - Add guard checks
    
    return 1;
}

// ============================================================================
// SAFETY AND VALIDATION
// ============================================================================

int micro_jit_validate_generated_code(uint8_t* code, size_t size) {
    if (!code || size == 0) return 0;
    
    // TODO: Implement code validation
    // - Check for valid instructions
    // - Verify control flow
    // - Test execution
    
    return 1;
}

int micro_jit_test_generated_code(JitCompiledFunction* compiled_func) {
    if (!compiled_func || !compiled_func->is_valid) return 0;
    
    // TODO: Implement code testing
    // - Run with test inputs
    // - Verify output correctness
    
    return 1;
}

// ============================================================================
// STATISTICS AND REPORTING
// ============================================================================

void micro_jit_print_statistics(MicroJitContext* context) {
    if (!context) return;
    
    printf("Micro-JIT Statistics:\n");
    printf("  Target: %s\n", context->is_arm64 ? "ARM64" : "x86_64");
    printf("  Mode: %d\n", context->mode);
    printf("  Executable memory: %s\n", context->has_executable_memory ? "Yes" : "No");
    printf("  Total compilation time: %llu ns\n", 
           (unsigned long long)context->total_compilation_time_ns);
    printf("  Total execution time: %llu ns\n", 
           (unsigned long long)context->total_execution_time_ns);
    printf("  Overall speedup: %.2fx\n", context->overall_speedup);
    
    if (context->code_cache) {
        printf("  Code cache:\n");
        printf("    Functions: %zu\n", context->code_cache->function_count);
        printf("    Total code size: %zu bytes\n", context->code_cache->total_code_size);
        printf("    Compilations: %llu\n", (unsigned long long)context->code_cache->compilation_count);
        printf("    Successes: %llu\n", (unsigned long long)context->code_cache->success_count);
        printf("    Deoptimizations: %llu\n", (unsigned long long)context->code_cache->deoptimization_count);
    }
}

void micro_jit_print_cache_status(MicroJitContext* context) {
    if (!context || !context->code_cache) return;
    
    printf("Code Cache Status:\n");
    printf("  Capacity: %zu functions\n", context->code_cache->capacity);
    printf("  Used: %zu functions\n", context->code_cache->function_count);
    printf("  Code size: %zu / %zu bytes\n", 
           context->code_cache->total_code_size, 
           context->code_cache->max_code_size);
    printf("  Cache hits: %llu\n", (unsigned long long)context->code_cache->cache_hit_count);
    printf("  Cache misses: %llu\n", (unsigned long long)context->code_cache->cache_miss_count);
}

double micro_jit_get_speedup_factor(MicroJitContext* context) {
    return context ? context->overall_speedup : 1.0;
}

uint64_t micro_jit_get_total_compilation_time(MicroJitContext* context) {
    return context ? context->total_compilation_time_ns : 0;
}

// ============================================================================
// INTERPRETER INTEGRATION
// ============================================================================

void micro_jit_initialize_for_interpreter(Interpreter* interpreter) {
    if (!interpreter) return;
    
    // Initialize Micro-JIT context
    // This will be implemented when we modify the Interpreter struct
}

void micro_jit_cleanup_for_interpreter(Interpreter* interpreter) {
    if (!interpreter) return;
    
    // Cleanup Micro-JIT context
    // This will be implemented when we modify the Interpreter struct
}

MicroJitContext* micro_jit_get_from_interpreter(Interpreter* interpreter) {
    if (!interpreter) return NULL;
    
    // Return Micro-JIT context from interpreter
    // This will be implemented when we modify the Interpreter struct
    return NULL;
}

// ============================================================================
// ERROR HANDLING
// ============================================================================

const char* micro_jit_error_string(MicroJitError error) {
    switch (error) {
        case MICRO_JIT_ERROR_NONE: return "No error";
        case MICRO_JIT_ERROR_MEMORY_ALLOCATION: return "Memory allocation failed";
        case MICRO_JIT_ERROR_UNSUPPORTED_PLATFORM: return "Unsupported platform";
        case MICRO_JIT_ERROR_INVALID_BYTECODE: return "Invalid bytecode";
        case MICRO_JIT_ERROR_COMPILATION_FAILED: return "Compilation failed";
        case MICRO_JIT_ERROR_CACHE_FULL: return "Code cache full";
        case MICRO_JIT_ERROR_GUARD_FAILED: return "Guard check failed";
        case MICRO_JIT_ERROR_EXECUTION_FAILED: return "Execution failed";
        default: return "Unknown error";
    }
}

void micro_jit_set_error(MicroJitContext* context, MicroJitError error) {
    // TODO: Implement error tracking
    (void)context;
    (void)error;
}

MicroJitError micro_jit_get_last_error(MicroJitContext* context) {
    // TODO: Implement error tracking
    (void)context;
    return MICRO_JIT_ERROR_NONE;
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

// get_current_time_ns is defined in hot_spot_tracker.c
