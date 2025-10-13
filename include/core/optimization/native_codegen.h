/**
 * @file native_codegen.h
 * @brief Native code generation
 * 
 * Platform-specific code generation (x86_64, ARM64), CPU optimizations (SSE, AVX, NEON),
 * peephole optimization, branch prediction hints.
 */

#ifndef NATIVE_CODEGEN_H
#define NATIVE_CODEGEN_H

#include "trace_optimizer.h"
#include "register_vm.h"
#include <stdint.h>
#include <stddef.h>

// ============================================================================
// NATIVE CODE GENERATION DATA STRUCTURES
// ============================================================================

/**
 * @brief Target architecture
 * 
 * Defines the target architecture for native code generation.
 */
typedef enum {
    TARGET_ARCH_X86_64 = 0,        // x86-64 architecture
    TARGET_ARCH_ARM64 = 1,         // ARM64 architecture
    TARGET_ARCH_AUTO = 2           // Auto-detect architecture
} TargetArchitecture;

/**
 * @brief CPU feature flags
 * 
 * Defines CPU features that can be used for optimization.
 */
typedef enum {
    CPU_FEATURE_SSE = (1 << 0),    // SSE instructions
    CPU_FEATURE_SSE2 = (1 << 1),   // SSE2 instructions
    CPU_FEATURE_SSE3 = (1 << 2),   // SSE3 instructions
    CPU_FEATURE_SSE4_1 = (1 << 3), // SSE4.1 instructions
    CPU_FEATURE_SSE4_2 = (1 << 4), // SSE4.2 instructions
    CPU_FEATURE_AVX = (1 << 5),    // AVX instructions
    CPU_FEATURE_AVX2 = (1 << 6),   // AVX2 instructions
    CPU_FEATURE_AVX512 = (1 << 7), // AVX-512 instructions
    CPU_FEATURE_NEON = (1 << 8),   // NEON instructions (ARM)
    CPU_FEATURE_FMA = (1 << 9),    // Fused multiply-add
    CPU_FEATURE_BMI = (1 << 10),   // Bit manipulation instructions
    CPU_FEATURE_LZCNT = (1 << 11), // Leading zero count
    CPU_FEATURE_POPCNT = (1 << 12) // Population count
} CPUFeatureFlags;

/**
 * @brief Code generation mode
 * 
 * Defines the mode of code generation.
 */
typedef enum {
    CODEGEN_MODE_DEBUG = 0,        // Debug mode (with debug info)
    CODEGEN_MODE_RELEASE = 1,      // Release mode (optimized)
    CODEGEN_MODE_SIZE = 2,         // Size-optimized mode
    CODEGEN_MODE_SPEED = 3         // Speed-optimized mode
} CodeGenerationMode;

/**
 * @brief Native instruction
 * 
 * Represents a single native machine instruction.
 */
typedef struct {
    uint8_t* code;                 // Machine code bytes
    size_t code_size;              // Size of machine code
    uint32_t instruction_id;       // Original instruction ID
    uint32_t opcode;               // Native opcode
    uint32_t operands[4];          // Operands
    uint8_t operand_count;         // Number of operands
    uint8_t operand_sizes[4];      // Size of each operand
    uint8_t operand_types[4];      // Type of each operand
    
    // Addressing information
    uint8_t addressing_mode;       // Addressing mode
    int32_t displacement;          // Displacement value
    uint8_t scale;                 // Scale factor
    uint8_t index_reg;             // Index register
    uint8_t base_reg;              // Base register
    
    // Optimization hints
    uint8_t latency;               // Instruction latency
    uint8_t throughput;            // Instruction throughput
    uint8_t port_usage;            // Port usage mask
    uint8_t branch_prediction;     // Branch prediction hint
    
    // Debug information
    uint32_t source_line;          // Source line number
    uint32_t source_column;        // Source column number
    char* source_file;             // Source file name
} NativeInstruction;

/**
 * @brief Native function
 * 
 * Contains a complete native function with machine code and metadata.
 */
typedef struct {
    uint32_t function_id;          // Function identifier
    uint8_t* code;                 // Machine code
    size_t code_size;              // Size of machine code
    size_t code_capacity;          // Capacity of code buffer
    
    // Function metadata
    uint32_t parameter_count;      // Number of parameters
    uint32_t local_count;          // Number of local variables
    uint32_t register_count;       // Number of registers used
    uint32_t stack_size;           // Stack frame size
    
    // Entry and exit points
    uint8_t* entry_point;          // Function entry point
    uint8_t* exit_point;           // Function exit point
    uint8_t* return_point;         // Return point
    
    // Calling convention
    uint8_t calling_convention;    // Calling convention used
    uint8_t* prologue;             // Function prologue
    uint8_t* epilogue;             // Function epilogue
    
    // Optimization metadata
    uint32_t optimization_level;   // Optimization level used
    uint32_t instruction_count;    // Number of instructions
    uint32_t cycle_count;          // Estimated cycle count
    double performance_score;      // Performance score (0.0-1.0)
    
    // Debug information
    uint32_t* line_numbers;        // Line number mapping
    uint32_t* column_numbers;      // Column number mapping
    char** source_files;           // Source file names
    uint32_t debug_info_size;      // Size of debug information
    
    // Memory management
    int is_owned;                  // Does this function own its memory?
} NativeFunction;

/**
 * @brief Code generation context
 * 
 * Contains the state and configuration for native code generation.
 */
typedef struct {
    // Configuration
    TargetArchitecture target_arch; // Target architecture
    CodeGenerationMode mode;       // Code generation mode
    CPUFeatureFlags cpu_features;  // Available CPU features
    uint32_t optimization_level;   // Optimization level
    
    // Code generation state
    uint32_t current_function_id;  // Current function being generated
    uint8_t* current_code;         // Current code buffer
    size_t current_code_size;      // Current code size
    size_t current_code_capacity;  // Current code capacity
    
    // Generated functions
    NativeFunction* functions;     // Array of generated functions
    uint32_t function_count;       // Number of generated functions
    uint32_t max_functions;        // Maximum number of functions
    
    // Code cache
    uint8_t* code_cache;           // Code cache buffer
    size_t code_cache_size;        // Size of code cache
    size_t code_cache_capacity;    // Capacity of code cache
    size_t code_cache_used;        // Used code cache space
    
    // Register allocation
    uint8_t* register_map;         // Register mapping
    uint32_t register_count;       // Number of registers
    uint8_t* spill_slots;          // Spill slot mapping
    uint32_t spill_slot_count;     // Number of spill slots
    
    // Statistics
    uint64_t total_functions_generated; // Total functions generated
    uint64_t total_code_generated; // Total code generated
    uint64_t total_generation_time; // Total generation time (nanoseconds)
    double average_generation_time; // Average generation time per function
    double average_code_size;      // Average code size per function
    double average_performance_score; // Average performance score
    
    // Performance tracking
    uint64_t generation_start_time; // Generation start time
    uint64_t generation_end_time;   // Generation end time
    double total_generation_time_ms; // Total generation time (milliseconds)
    double generation_overhead;     // Generation overhead percentage
} NativeCodegenContext;

// ============================================================================
// NATIVE CODE GENERATION FUNCTIONS
// ============================================================================

/**
 * @brief Core generation functions
 * 
 * These functions handle the main native code generation process,
 * from initialization to function completion.
 */

/**
 * @brief Create a new native code generator context
 * @param target_arch Target architecture
 * @param mode Code generation mode
 * @return New NativeCodegenContext, or NULL on failure
 * @note The context is initialized with default settings
 */
NativeCodegenContext* native_codegen_create(TargetArchitecture target_arch, CodeGenerationMode mode);

/**
 * @brief Free a native code generator context
 * @param context Context to free (can be NULL)
 * @note This function is safe to call with NULL
 */
void native_codegen_free(NativeCodegenContext* context);

/**
 * @brief Generate native code for an optimized trace
 * @param context Code generator context
 * @param trace Optimized trace to compile
 * @return Generated native function, or NULL on failure
 * @note This is the main entry point for native code generation
 */
NativeFunction* native_codegen_generate_function(NativeCodegenContext* context, OptimizedTrace* trace);

/**
 * @brief Generate native code for a register program
 * @param context Code generator context
 * @param program Register program to compile
 * @return Generated native function, or NULL on failure
 * @note This function generates native code from a register program
 */
NativeFunction* native_codegen_generate_from_program(NativeCodegenContext* context, RegisterProgram* program);

/**
 * @brief Finalize a native function
 * @param context Code generator context
 * @param function Function to finalize
 * @return 1 on success, 0 on failure
 * @note This function finalizes a native function and makes it executable
 */
int native_codegen_finalize_function(NativeCodegenContext* context, NativeFunction* function);

/**
 * @brief Platform-specific generation functions
 * 
 * These functions handle platform-specific code generation.
 */

/**
 * @brief Generate x86-64 native code
 * @param context Code generator context
 * @param trace Optimized trace to compile
 * @return Generated native function, or NULL on failure
 * @note This function generates x86-64 specific machine code
 */
NativeFunction* native_codegen_generate_x86_64(NativeCodegenContext* context, OptimizedTrace* trace);

/**
 * @brief Generate ARM64 native code
 * @param context Code generator context
 * @param trace Optimized trace to compile
 * @return Generated native function, or NULL on failure
 * @note This function generates ARM64 specific machine code
 */
NativeFunction* native_codegen_generate_arm64(NativeCodegenContext* context, OptimizedTrace* trace);

/**
 * @brief Auto-detect target architecture
 * @return Detected target architecture
 * @note This function detects the current system architecture
 */
TargetArchitecture native_codegen_detect_architecture(void);

/**
 * @brief Detect available CPU features
 * @return Available CPU features
 * @note This function detects CPU features available on the current system
 */
CPUFeatureFlags native_codegen_detect_cpu_features(void);

/**
 * @brief Instruction generation functions
 * 
 * These functions generate specific types of native instructions.
 */

/**
 * @brief Generate arithmetic instruction
 * @param context Code generator context
 * @param opcode Arithmetic opcode
 * @param dst Destination register
 * @param src1 First source register
 * @param src2 Second source register
 * @return Generated instruction, or NULL on failure
 * @note This function generates arithmetic instructions
 */
NativeInstruction* native_codegen_generate_arithmetic(NativeCodegenContext* context, uint32_t opcode, 
                                                     uint32_t dst, uint32_t src1, uint32_t src2);

/**
 * @brief Generate memory instruction
 * @param context Code generator context
 * @param opcode Memory opcode
 * @param reg Register operand
 * @param addr Memory address
 * @return Generated instruction, or NULL on failure
 * @note This function generates memory access instructions
 */
NativeInstruction* native_codegen_generate_memory(NativeCodegenContext* context, uint32_t opcode, 
                                                 uint32_t reg, uint64_t addr);

/**
 * @brief Generate control flow instruction
 * @param context Code generator context
 * @param opcode Control flow opcode
 * @param target Target address
 * @return Generated instruction, or NULL on failure
 * @note This function generates control flow instructions
 */
NativeInstruction* native_codegen_generate_control_flow(NativeCodegenContext* context, uint32_t opcode, 
                                                       uint64_t target);

/**
 * @brief Generate SIMD instruction
 * @param context Code generator context
 * @param opcode SIMD opcode
 * @param dst Destination register
 * @param src1 First source register
 * @param src2 Second source register
 * @return Generated instruction, or NULL on failure
 * @note This function generates SIMD vector instructions
 */
NativeInstruction* native_codegen_generate_simd(NativeCodegenContext* context, uint32_t opcode, 
                                               uint32_t dst, uint32_t src1, uint32_t src2);

/**
 * @brief Optimization functions
 * 
 * These functions optimize generated native code.
 */

/**
 * @brief Apply peephole optimization to native code
 * @param context Code generator context
 * @param function Function to optimize
 * @return 1 on success, 0 on failure
 * @note This function applies peephole optimizations to native code
 */
int native_codegen_peephole_optimize(NativeCodegenContext* context, NativeFunction* function);

/**
 * @brief Apply instruction scheduling optimization
 * @param context Code generator context
 * @param function Function to optimize
 * @return 1 on success, 0 on failure
 * @note This function optimizes instruction scheduling for better performance
 */
int native_codegen_schedule_instructions(NativeCodegenContext* context, NativeFunction* function);

/**
 * @brief Apply register allocation optimization
 * @param context Code generator context
 * @param function Function to optimize
 * @return 1 on success, 0 on failure
 * @note This function optimizes register allocation in native code
 */
int native_codegen_optimize_register_allocation(NativeCodegenContext* context, NativeFunction* function);

/**
 * @brief Apply branch prediction optimization
 * @param context Code generator context
 * @param function Function to optimize
 * @return 1 on success, 0 on failure
 * @note This function optimizes branch prediction in native code
 */
int native_codegen_optimize_branch_prediction(NativeCodegenContext* context, NativeFunction* function);

/**
 * @brief Management functions
 * 
 * These functions manage generated native functions.
 */

/**
 * @brief Get a generated native function by ID
 * @param context Code generator context
 * @param function_id Function ID
 * @return Native function, or NULL if not found
 * @note This function retrieves a native function by its ID
 */
NativeFunction* native_codegen_get_function(NativeCodegenContext* context, uint32_t function_id);

/**
 * @brief Get all generated native functions
 * @param context Code generator context
 * @param functions Array to store functions
 * @param max_functions Maximum number of functions to return
 * @return Number of functions returned
 * @note This function retrieves all generated native functions
 */
uint32_t native_codegen_get_all_functions(NativeCodegenContext* context, NativeFunction** functions, uint32_t max_functions);

/**
 * @brief Remove a native function
 * @param context Code generator context
 * @param function_id Function ID to remove
 * @return 1 on success, 0 on failure
 * @note This function removes a native function
 */
int native_codegen_remove_function(NativeCodegenContext* context, uint32_t function_id);

/**
 * @brief Clear all native functions
 * @param context Code generator context
 * @note This function removes all native functions
 */
void native_codegen_clear_functions(NativeCodegenContext* context);

/**
 * @brief Execute a native function
 * @param context Code generator context
 * @param function Function to execute
 * @param args Function arguments
 * @param arg_count Number of arguments
 * @return Function result
 * @note This function executes a native function with the given arguments
 */
uint64_t native_codegen_execute_function(NativeCodegenContext* context, NativeFunction* function, 
                                        uint64_t* args, uint32_t arg_count);

/**
 * @brief Configuration functions
 * 
 * These functions configure the native code generator behavior.
 */

/**
 * @brief Set target architecture
 * @param context Code generator context
 * @param target_arch New target architecture
 * @note This function changes the target architecture
 */
void native_codegen_set_target_architecture(NativeCodegenContext* context, TargetArchitecture target_arch);

/**
 * @brief Set code generation mode
 * @param context Code generator context
 * @param mode New code generation mode
 * @note This function changes the code generation mode
 */
void native_codegen_set_mode(NativeCodegenContext* context, CodeGenerationMode mode);

/**
 * @brief Set CPU features
 * @param context Code generator context
 * @param features Available CPU features
 * @note This function sets the available CPU features
 */
void native_codegen_set_cpu_features(NativeCodegenContext* context, CPUFeatureFlags features);

/**
 * @brief Set optimization level
 * @param context Code generator context
 * @param level Optimization level
 * @note This function sets the optimization level
 */
void native_codegen_set_optimization_level(NativeCodegenContext* context, uint32_t level);

/**
 * @brief Utility functions
 * 
 * These functions provide utility functionality for working with
 * native code generator contexts and functions.
 */

/**
 * @brief Get code generation statistics
 * @param context Code generator context
 * @return Statistics string
 * @note Returns a formatted string with code generation statistics
 */
char* native_codegen_get_statistics(NativeCodegenContext* context);

/**
 * @brief Print native function information
 * @param context Code generator context
 * @param function_id Function ID to print
 * @note Prints human-readable representation of a native function
 */
void native_codegen_print_function(NativeCodegenContext* context, uint32_t function_id);

/**
 * @brief Print all native functions
 * @param context Code generator context
 * @note Prints human-readable representation of all native functions
 */
void native_codegen_print_all_functions(NativeCodegenContext* context);

/**
 * @brief Validate native function
 * @param context Code generator context
 * @param function_id Function ID to validate
 * @return 1 if valid, 0 if invalid
 * @note Validates that a native function is correct
 */
int native_codegen_validate_function(NativeCodegenContext* context, uint32_t function_id);

/**
 * @brief Export native function to file
 * @param context Code generator context
 * @param function_id Function ID to export
 * @param filename Output filename
 * @return 1 on success, 0 on failure
 * @note Exports a native function to a file for analysis
 */
int native_codegen_export_function(NativeCodegenContext* context, uint32_t function_id, const char* filename);

/**
 * @brief Import native function from file
 * @param context Code generator context
 * @param filename Input filename
 * @return Function ID, or 0 on failure
 * @note Imports a native function from a file
 */
uint32_t native_codegen_import_function(NativeCodegenContext* context, const char* filename);

/**
 * @brief Disassemble native function
 * @param context Code generator context
 * @param function_id Function ID to disassemble
 * @return Disassembly string
 * @note Returns a disassembly of the native function
 */
char* native_codegen_disassemble_function(NativeCodegenContext* context, uint32_t function_id);

#endif // NATIVE_CODEGEN_H
