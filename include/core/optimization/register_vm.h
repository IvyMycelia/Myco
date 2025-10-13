/**
 * @file register_vm.h
 * @brief Register-based virtual machine implementation
 * 
 * 256 virtual registers per frame, 128-instruction set.
 * Direct register operations, SSA-form IR, linear scan allocation.
 */

#ifndef REGISTER_VM_H
#define REGISTER_VM_H

#include "../interpreter/interpreter_core.h"
#include "../ast.h"
#include <stdint.h>
#include <stddef.h>

// ============================================================================
// REGISTER-BASED INSTRUCTION SET (128 Instructions)
// ============================================================================

/**
 * @brief Register-based instruction opcodes
 * 
 * 128 instructions: Load/Store (0-31), Arithmetic (32-63), Comparison (64-79),
 * Control Flow (80-95), Vector (96-111), Special (112-127).
 */
typedef enum {
    // Load/Store Operations (0-31)
    REG_LOAD_CONST = 0,        // Load constant into register
    REG_LOAD_VAR = 1,          // Load variable into register
    REG_STORE_VAR = 2,         // Store register to variable
    REG_LOAD_GLOBAL = 3,       // Load global variable into register
    REG_STORE_GLOBAL = 4,      // Store register to global variable
    REG_LOAD_LOCAL = 5,        // Load local variable (optimized)
    REG_STORE_LOCAL = 6,       // Store to local variable (optimized)
    REG_LOAD_UPVALUE = 7,      // Load upvalue into register
    REG_STORE_UPVALUE = 8,     // Store register to upvalue
    REG_LOAD_NULL = 9,         // Load null into register
    REG_LOAD_TRUE = 10,        // Load true into register
    REG_LOAD_FALSE = 11,       // Load false into register
    REG_LOAD_ZERO = 12,        // Load zero into register
    REG_LOAD_ONE = 13,         // Load one into register
    REG_MOV_RR = 14,           // Move register to register (zero-copy)
    REG_COPY_RR = 15,          // Copy register to register
    REG_LOAD_IMM8 = 16,        // Load 8-bit immediate into register
    REG_LOAD_IMM16 = 17,       // Load 16-bit immediate into register
    REG_LOAD_IMM32 = 18,       // Load 32-bit immediate into register
    REG_LOAD_IMM64 = 19,       // Load 64-bit immediate into register
    REG_LOAD_FLOAT = 20,       // Load float constant into register
    REG_LOAD_DOUBLE = 21,      // Load double constant into register
    REG_LOAD_STRING = 22,      // Load string constant into register
    REG_LOAD_ARRAY = 23,       // Load array element into register
    REG_STORE_ARRAY = 24,      // Store register to array element
    REG_LOAD_OBJECT = 25,      // Load object property into register
    REG_STORE_OBJECT = 26,     // Store register to object property
    REG_LOAD_INDEX = 27,       // Load indexed value into register
    REG_STORE_INDEX = 28,      // Store register to indexed value
    REG_LOAD_REF = 29,         // Load reference into register
    REG_STORE_REF = 30,        // Store register to reference
    REG_DUP_R = 31,            // Duplicate register
    
    // Arithmetic Operations (32-63)
    REG_ADDI_RR = 32,          // Add two integer registers
    REG_ADDF_RR = 33,          // Add two float registers
    REG_SUBI_RR = 34,          // Subtract two integer registers
    REG_SUBF_RR = 35,          // Subtract two float registers
    REG_MULI_RR = 36,          // Multiply two integer registers
    REG_MULF_RR = 37,          // Multiply two float registers
    REG_DIVI_RR = 38,          // Divide two integer registers
    REG_DIVF_RR = 39,          // Divide two float registers
    REG_MODI_RR = 40,          // Modulo two integer registers
    REG_MODF_RR = 41,          // Modulo two float registers
    REG_POWI_RR = 42,          // Power two integer registers
    REG_POWF_RR = 43,          // Power two float registers
    REG_NEGI_R = 44,           // Negate integer register
    REG_NEGF_R = 45,           // Negate float register
    REG_INCI_R = 46,           // Increment integer register
    REG_DECI_R = 47,           // Decrement integer register
    REG_INCF_R = 48,           // Increment float register
    REG_DECF_R = 49,           // Decrement float register
    REG_ABSI_R = 50,           // Absolute value of integer register
    REG_ABSF_R = 51,           // Absolute value of float register
    REG_SQRT_R = 52,           // Square root of float register
    REG_SIN_R = 53,            // Sine of float register
    REG_COS_R = 54,            // Cosine of float register
    REG_TAN_R = 55,            // Tangent of float register
    REG_LOG_R = 56,            // Natural logarithm of float register
    REG_EXP_R = 57,            // Exponential of float register
    REG_ROUND_R = 58,          // Round float register to integer
    REG_FLOOR_R = 59,          // Floor float register to integer
    REG_CEIL_R = 60,           // Ceiling float register to integer
    REG_TRUNC_R = 61,          // Truncate float register to integer
    REG_CAST_IF = 62,          // Cast integer to float
    REG_CAST_FI = 63,          // Cast float to integer
    
    // Comparison Operations (64-79)
    REG_EQ_RR = 64,            // Equal (any type)
    REG_NE_RR = 65,            // Not equal (any type)
    REG_LT_RR = 66,            // Less than (any type)
    REG_LE_RR = 67,            // Less or equal (any type)
    REG_GT_RR = 68,            // Greater than (any type)
    REG_GE_RR = 69,            // Greater or equal (any type)
    REG_EQI_RR = 70,           // Equal (integers)
    REG_NEI_RR = 71,           // Not equal (integers)
    REG_LTI_RR = 72,           // Less than (integers)
    REG_LEI_RR = 73,           // Less or equal (integers)
    REG_GTI_RR = 74,           // Greater than (integers)
    REG_GEI_RR = 75,           // Greater or equal (integers)
    REG_EQF_RR = 76,           // Equal (floats)
    REG_NEF_RR = 77,           // Not equal (floats)
    REG_LTF_RR = 78,           // Less than (floats)
    REG_LEF_RR = 79,           // Less or equal (floats)
    
    // Control Flow (80-95)
    REG_JUMP = 80,             // Unconditional jump
    REG_JUMP_IF_FALSE = 81,    // Jump if register is false
    REG_JUMP_IF_TRUE = 82,     // Jump if register is true
    REG_JUMP_IF_NULL = 83,     // Jump if register is null
    REG_JUMP_IF_NOT_NULL = 84, // Jump if register is not null
    REG_JUMP_IF_ZERO = 85,     // Jump if register is zero
    REG_JUMP_IF_NOT_ZERO = 86, // Jump if register is not zero
    REG_CALL = 87,             // Call function
    REG_CALL_METHOD = 88,      // Call method on object
    REG_RETURN = 89,           // Return from function
    REG_RETURN_NULL = 90,      // Return null
    REG_TAIL_CALL = 91,        // Tail call optimization
    REG_CALL_NATIVE = 92,      // Call native function
    REG_CALL_BUILTIN = 93,     // Call builtin function
    REG_THROW = 94,            // Throw exception
    REG_RETHROW = 95,          // Rethrow exception
    
    // Vector Operations (96-111)
    REG_ADDV_RR = 96,          // Vector add (SIMD)
    REG_SUBV_RR = 97,          // Vector subtract (SIMD)
    REG_MULV_RR = 98,          // Vector multiply (SIMD)
    REG_DIVV_RR = 99,          // Vector divide (SIMD)
    REG_DOTV_RR = 100,         // Vector dot product
    REG_CROSSV_RR = 101,       // Vector cross product
    REG_LENV_R = 102,          // Vector length
    REG_NORMV_R = 103,         // Vector normalize
    REG_MINV_RR = 104,         // Vector minimum
    REG_MAXV_RR = 105,         // Vector maximum
    REG_SUMV_R = 106,          // Vector sum
    REG_MEANV_R = 107,         // Vector mean
    REG_SORTV_R = 108,         // Vector sort
    REG_REVERSEV_R = 109,      // Vector reverse
    REG_SHUFFLEV_RR = 110,     // Vector shuffle
    REG_BLENDV_RRR = 111,      // Vector blend
    
    // Special Operations (112-127)
    REG_MULADD_RRR = 112,      // Fused multiply-add (a*b+c)
    REG_MULSUB_RRR = 113,      // Fused multiply-subtract (a*b-c)
    REG_FMA_RRR = 114,         // Fused multiply-add (float)
    REG_FMS_RRR = 115,         // Fused multiply-subtract (float)
    REG_CMOV_RRR = 116,        // Conditional move
    REG_SELECT_RRR = 117,      // Select between two values
    REG_CLAMP_RRR = 118,       // Clamp value between min/max
    REG_LERP_RRR = 119,        // Linear interpolation
    REG_SMOOTHSTEP_RRR = 120,  // Smooth step interpolation
    REG_SIGN_R = 121,          // Sign of value
    REG_MIN_RRR = 122,         // Minimum of three values
    REG_MAX_RRR = 123,         // Maximum of three values
    REG_AVG_RRR = 124,         // Average of three values
    REG_MED_RRR = 125,         // Median of three values
    REG_HALT = 126,            // Halt execution
    REG_NOP = 127              // No operation
} RegisterOpcode;

/**
 * @brief Register-based instruction structure
 * 
 * @param opcode Instruction opcode (0-127)
 * @param dst Destination register (8-bit)
 * @param src1 First source register (8-bit)
 * @param src2 Second source register (8-bit)
 * @param src3 Third source register (8-bit)
 * @param immediate 32-bit immediate value
 * @param offset 16-bit jump offset or array index
 */
typedef struct {
    uint8_t opcode;            // Instruction opcode (0-127)
    uint8_t dst;               // Destination register
    uint8_t src1;              // First source register
    uint8_t src2;              // Second source register
    uint8_t src3;              // Third source register
    uint32_t immediate;        // Immediate value
    uint16_t offset;           // Jump offset or array index
} RegisterInstruction;

/**
 * @brief Register-based program structure
 * 
 * Contains instructions, constants, and metadata for register VM execution.
 */
typedef struct {
    RegisterInstruction* instructions;  // Array of instructions
    size_t instruction_count;           // Number of instructions
    size_t capacity;                    // Allocated capacity
    
    Value* constants;                   // Constant pool
    size_t constant_count;              // Number of constants
    size_t constant_capacity;           // Constant pool capacity
    
    char** string_pool;                 // String constant pool
    size_t string_count;                // Number of strings
    size_t string_capacity;             // String pool capacity
    
    // Register allocation metadata
    size_t max_registers;               // Maximum registers used
    size_t register_count;              // Number of registers allocated
    uint8_t* register_types;            // Type of each register
    
    // Function metadata
    int is_function;                    // Is this a function body?
    char* function_name;                // Function name (if applicable)
    size_t parameter_count;             // Number of parameters
    size_t local_count;                 // Number of local variables
    size_t upvalue_count;               // Number of upvalues
    
    // Optimization metadata
    int optimized;                      // Has this program been optimized?
    int vectorized;                     // Has this program been vectorized?
    int traceable;                      // Is this program traceable?
    double hotness_score;               // Hotness score (0.0-1.0)
} RegisterProgram;

/**
 * @brief Register VM execution context
 * 
 * Contains registers, call stack, and execution state.
 */
typedef struct {
    RegisterProgram* program;           // Program to execute
    Value* registers;                   // Register array (256 registers)
    size_t register_count;              // Number of registers allocated
    
    Value* locals;                      // Local variables
    size_t local_count;                 // Number of locals
    
    Value* upvalues;                    // Upvalues (closure variables)
    size_t upvalue_count;               // Number of upvalues
    
    size_t pc;                          // Program counter
    int halted;                         // Execution halted flag
    
    // Call stack for function calls
    struct RegisterContext* caller;     // Calling context
    size_t return_pc;                   // Return program counter
    
    // Performance tracking
    uint64_t execution_count;           // Number of instructions executed
    uint64_t hot_spot_threshold;        // Hot spot detection threshold
    int is_hot;                         // Is this context hot?
    
    // Optimization state
    int optimization_level;             // Current optimization level
    int jit_eligible;                   // Is this context JIT eligible?
    int trace_eligible;                 // Is this context trace eligible?
} RegisterContext;

// ============================================================================
// REGISTER VM FUNCTIONS
// ============================================================================

/**
 * @brief Program management functions
 */

/**
 * @brief Create a new register-based program
 * @return New RegisterProgram instance, or NULL on failure
 */
RegisterProgram* register_program_create(void);

/**
 * @brief Free a register-based program and all associated memory
 * @param program Program to free (can be NULL)
 */
void register_program_free(RegisterProgram* program);

/**
 * @brief Add an instruction to a register-based program
 * @param program Target program
 * @param instr Instruction to add
 * @return 1 on success, 0 on failure
 */
int register_program_add_instruction(RegisterProgram* program, RegisterInstruction instr);

/**
 * @brief Add a constant value to the constant pool
 * @param program Target program
 * @param value Constant value to add
 * @return Index of the constant in the pool, or -1 on failure
 */
int register_program_add_constant(RegisterProgram* program, Value value);

/**
 * @brief Add a string constant to the string pool
 * @param program Target program
 * @param str String to add (copied)
 * @return Index of the string in the pool, or -1 on failure
 */
int register_program_add_string(RegisterProgram* program, const char* str);

/**
 * @brief Compilation functions
 */

/**
 * @brief Compile an AST node to register-based program
 * @param ast AST node to compile
 * @param interpreter Interpreter context for compilation
 * @return Compiled register program, or NULL on failure
 */
RegisterProgram* register_compile_ast(ASTNode* ast, Interpreter* interpreter);

/**
 * @brief Compile an expression AST node to register-based program
 * @param expr Expression AST node
 * @param interpreter Interpreter context
 * @return Compiled register program, or NULL on failure
 */
RegisterProgram* register_compile_expression(ASTNode* expr, Interpreter* interpreter);

/**
 * @brief Compile a statement AST node to register-based program
 * @param stmt Statement AST node
 * @param interpreter Interpreter context
 * @return Compiled register program, or NULL on failure
 */
RegisterProgram* register_compile_statement(ASTNode* stmt, Interpreter* interpreter);

/**
 * @brief Compile a function AST node to register-based program
 * @param func Function AST node
 * @param interpreter Interpreter context
 * @return Compiled register program, or NULL on failure
 */
RegisterProgram* register_compile_function(ASTNode* func, Interpreter* interpreter);

/**
 * @brief Execution functions
 */

/**
 * @brief Create a new register VM execution context
 * @param program Program to execute
 * @return New RegisterContext instance, or NULL on failure
 */
RegisterContext* register_context_create(RegisterProgram* program);

/**
 * @brief Free a register VM execution context
 * @param context Context to free (can be NULL)
 */
void register_context_free(RegisterContext* context);

/**
 * @brief Execute a register-based program
 * @param context Execution context
 * @param interpreter Interpreter for builtin functions
 * @return Result value of execution
 */
Value register_execute(RegisterContext* context, Interpreter* interpreter);

/**
 * @brief Execute a single register instruction
 * @param context Execution context
 * @param interpreter Interpreter for builtin functions
 * @param instr Instruction to execute
 * @return Result value of instruction execution
 */
Value register_execute_instruction(RegisterContext* context, Interpreter* interpreter, RegisterInstruction instr);

/**
 * @brief Optimization functions
 */

/**
 * @brief Optimize a register-based program
 * @param program Program to optimize
 * @return 1 on success, 0 on failure
 */
int register_optimize_program(RegisterProgram* program);

/**
 * @brief Apply peephole optimizations to a program
 * @param program Program to optimize
 * @return Number of optimizations applied
 */
int register_peephole_optimize(RegisterProgram* program);

/**
 * @brief Apply constant folding to a program
 * @param program Program to optimize
 * @return Number of optimizations applied
 */
int register_constant_fold(RegisterProgram* program);

/**
 * @brief Apply dead code elimination to a program
 * @param program Program to optimize
 * @return Number of optimizations applied
 */
int register_dead_code_elimination(RegisterProgram* program);

/**
 * @brief Apply register allocation optimization
 * @param program Program to optimize
 * @return 1 on success, 0 on failure
 */
int register_allocate_optimize(RegisterProgram* program);

/**
 * @brief Utility functions
 */

/**
 * @brief Get the number of registers used by a program
 * @param program Program to analyze
 * @return Number of registers used
 */
size_t register_program_get_register_count(RegisterProgram* program);

/**
 * @brief Get the hotness score of a program
 * @param program Program to analyze
 * @return Hotness score (0.0-1.0)
 */
double register_program_get_hotness_score(RegisterProgram* program);

/**
 * @brief Set the hotness score of a program
 * @param program Program to modify
 * @param score Hotness score (0.0-1.0)
 */
void register_program_set_hotness_score(RegisterProgram* program, double score);

/**
 * @brief Check if a program is eligible for JIT compilation
 * @param program Program to check
 * @return 1 if eligible, 0 otherwise
 */
int register_program_is_jit_eligible(RegisterProgram* program);

/**
 * @brief Check if a program is eligible for trace recording
 * @param program Program to check
 * @return 1 if eligible, 0 otherwise
 */
int register_program_is_trace_eligible(RegisterProgram* program);

/**
 * @brief Print a register-based program (for debugging)
 * @param program Program to print
 */
void register_program_print(RegisterProgram* program);

/**
 * @brief Print a register instruction (for debugging)
 * @param instr Instruction to print
 */
void register_instruction_print(RegisterInstruction instr);

#endif // REGISTER_VM_H
