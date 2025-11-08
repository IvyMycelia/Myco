#ifndef BYTECODE_H
#define BYTECODE_H

#include "ast.h"
#include "interpreter/interpreter_core.h"
#include "interpreter/value_operations.h"
#include <stddef.h>

// Minimal stack-based bytecode for safe, incremental speedups.

typedef enum {
    BC_LOAD_CONST = 1,
    BC_LOAD_LOCAL,
    BC_LOAD_VAR,        // Load variable from environment
    BC_STORE_LOCAL,
    BC_LOAD_GLOBAL,
    BC_STORE_GLOBAL,
    BC_ADD,
    BC_SUB,
    BC_MUL,
    BC_DIV,
    BC_EQ,
    BC_NE,
    BC_LT,
    BC_LE,
    BC_GT,
    BC_GE,
    BC_AND,         // Logical AND: a && b
    BC_OR,          // Logical OR: a || b
    BC_JUMP,
    BC_JUMP_IF_FALSE,
    BC_LOOP_START,     // Start while loop
    BC_LOOP_END,       // End while loop iteration
    BC_FOR_LOOP_START, // Start for loop: for i in range/array
    BC_FOR_LOOP_END,   // End for loop iteration
    BC_PRINT,         // Print top of stack
    BC_PRINT_MULTIPLE,// Print multiple values on one line
    BC_METHOD_CALL,   // Call method on object: obj.method(args...)
    BC_PROPERTY_ACCESS, // Access object property: obj.name
    BC_CALL_BUILTIN,  // Call built-in function by name
    BC_CALL_USER_FUNCTION, // Call user-defined function: func(args...)
    BC_CALL_FUNCTION_VALUE, // Call function value from stack: func(args...) where func is on stack
    BC_DEFINE_FUNCTION, // Define function in environment: func_name -> function_value
    BC_TO_STRING,     // Convert value to string
    BC_GET_TYPE,      // Get value type
    BC_GET_LENGTH,    // Get value length
    BC_IS_STRING,     // Check if value is string
    BC_IS_NUMBER,     // Check if value is number
    BC_IS_INT,        // Check if value is int
    BC_IS_FLOAT,      // Check if value is float
    BC_IS_BOOL,       // Check if value is bool
    BC_IS_ARRAY,      // Check if value is array
    BC_IS_NULL,       // Check if value is null
    BC_IS_OBJECT,     // Check if value is object
    BC_IS_FUNCTION,   // Check if value is function
    BC_ARRAY_GET,     // Get array element: arr[index]
    BC_ARRAY_SET,     // Set array element: arr[index] = value
    BC_ARRAY_PUSH,    // Push value to array
    BC_ARRAY_POP,     // Pop value from array
    BC_ARRAY_CONTAINS, // Check if array contains value
    BC_ARRAY_INDEX_OF, // Get index of value in array
    BC_ARRAY_JOIN,     // Join array elements with separator
    BC_ARRAY_UNIQUE,   // Remove duplicates from array
    BC_ARRAY_SLICE,    // Slice array from start to end
    BC_ARRAY_CONCAT_METHOD, // Array concat method: arr.concat(other)
    BC_CREATE_ARRAY,   // Create array from stack elements
    BC_ARRAY_CONCAT,   // Concatenate arrays: arr1 + arr2
    BC_CREATE_RANGE,   // Create range: pops end, start from stack, creates range(start..end)
    BC_CREATE_RANGE_STEP, // Create range with step: pops step, end, start from stack
    BC_CREATE_OBJECT,  // Create object from key-value pairs on stack
    BC_CREATE_MAP,     // Create hash map from key-value pairs on stack
    BC_CREATE_SET,     // Create set from elements on stack
    BC_IMPORT_LIB,     // Import library: use library_name
    BC_STRING_UPPER,  // Convert string to uppercase
    BC_STRING_LOWER,  // Convert string to lowercase
    BC_STRING_TRIM,   // Trim string whitespace
    BC_STRING_SPLIT,  // Split string by delimiter
    BC_STRING_REPLACE, // Replace substring in string
    BC_MATH_ABS,      // Math absolute value
    BC_MATH_SQRT,     // Math square root
    BC_MATH_POW,      // Math power
    BC_MATH_SIN,      // Math sine
    BC_MATH_COS,      // Math cosine
    BC_MATH_TAN,      // Math tangent
    BC_MATH_FLOOR,    // Math floor
    BC_MATH_CEIL,     // Math ceiling
    BC_MATH_ROUND,    // Math round
    BC_MAP_HAS,       // Map has key check
    BC_MAP_SIZE,      // Map size property
    BC_MAP_KEYS,      // Map keys method
    BC_MAP_DELETE,    // Map delete method
    BC_MAP_CLEAR,     // Map clear method
    BC_MAP_UPDATE,    // Map update method
    BC_SET_ADD,       // Set add method
    BC_SET_HAS,       // Set has element check
    BC_SET_REMOVE,    // Set remove method
    BC_SET_SIZE,      // Set size property
    BC_SET_CLEAR,     // Set clear method
    BC_SET_TO_ARRAY,  // Set toArray method
    BC_SET_UNION,     // Set union method
    BC_SET_INTERSECTION, // Set intersection method
    BC_EVAL_AST,      // Fallback: evaluate referenced AST subtree via eval_node()
    BC_MATCH,         // Pattern matching: match expr with cases (spore)
    BC_MATCH_CASE,    // Match case: case pattern => body (spore)
    BC_MATCH_PATTERN, // Match pattern: check if pattern matches expression
    BC_MATCH_END,     // Match expression end: clean up and return result
    BC_PATTERN_LITERAL, // Pattern: literal pattern (string, number, etc.)
    BC_PATTERN_WILDCARD, // Pattern: wildcard pattern (_)
    BC_PATTERN_TYPE,  // Pattern: type pattern (e.g., String, Int)
    BC_CREATE_CLASS,  // Create class definition
    BC_INSTANTIATE_CLASS, // Instantiate class: ClassName(args...)
    BC_FOR_LOOP,      // For loop: for i in collection
    BC_BREAK,         // Break statement - exit loop
    BC_CONTINUE,      // Continue statement - next iteration
    BC_THROW,         // Throw statement - throw exception
    BC_TRY_START,     // Start try block
    BC_TRY_END,       // End try block
    BC_CATCH,         // Catch block handler
    BC_SWITCH,        // Switch statement: switch expr { cases }
    BC_SWITCH_CASE,   // Switch case: case value => body
    BC_SWITCH_DEFAULT,// Switch default case
    BC_CREATE_LAMBDA, // Create lambda function: (params) => body
    BC_POP,
    BC_HALT,
    // Specialized numeric operations (bypass Value boxing)
    BC_LOAD_NUM,      // Load numeric constant directly
    BC_LOAD_NUM_LOCAL,// Load numeric local directly
    BC_STORE_NUM_LOCAL,// Store numeric local directly
    BC_ADD_NUM,       // Direct numeric addition
    BC_SUB_NUM,       // Direct numeric subtraction
    BC_MUL_NUM,       // Direct numeric multiplication
    BC_DIV_NUM,       // Direct numeric division
    BC_LT_NUM,        // Direct numeric less than
    BC_LE_NUM,        // Direct numeric less equal
    BC_GT_NUM,        // Direct numeric greater than
    BC_GE_NUM,        // Direct numeric greater equal
    BC_EQ_NUM,        // Direct numeric equality
    BC_NE_NUM,        // Direct numeric inequality
    BC_VALUE_TO_NUM,  // Convert Value to numeric
    BC_NOT,           // Logical NOT operation
    // Superinstructions (register-like, numeric locals only)
    BC_INC_LOCAL,     // a: num_locals[a] += 1
    BC_ADD_LLL,       // a,b,c: a = b + c
    BC_ADD_LOCAL_IMM, // a,b:   a += num_constants[b]
    BC_CMP_LOCAL_IMM_JUMP_FALSE, // a,b,c: if (!(a < num_constants[b])) pc=c
    BC_MUL_LOCAL_IMM, // a,b:   a *= num_constants[b]
    BC_NUM_TO_VALUE,  // a:     convert num_stack to value_stack
    BC_CALL_FUNCTION, // a:     call function at AST index a
    BC_RETURN,        // a:     return (a=0: void, a=1: value on stack)
    BC_PUSH_FRAME,    // a:     push new call frame (a = param count)
    BC_POP_FRAME      // a:     pop call frame (a = return value count)
} BytecodeOp;

// Legacy superinstruction enum (kept for compatibility)
#define BC_SUPER_START 1000
typedef enum {
    BC_INC_LOCAL_LEGACY = BC_SUPER_START,          // Legacy: use BC_INC_LOCAL instead
    BC_ADD_LLL_LEGACY,                             // Legacy: use BC_ADD_LLL instead
    BC_ADD_LOCAL_IMM_LEGACY,                       // Legacy: use BC_ADD_LOCAL_IMM instead
    BC_CMP_LOCAL_IMM_JUMP_FALSE_LEGACY,            // Legacy: use BC_CMP_LOCAL_IMM_JUMP_FALSE instead
    BC_MUL_LOCAL_IMM_LEGACY,                       // Legacy: use BC_MUL_LOCAL_IMM instead
    BC_NUM_TO_VALUE_LEGACY,                        // Legacy: use BC_NUM_TO_VALUE instead
    BC_CALL_FUNCTION_LEGACY,                       // Legacy: use BC_CALL_FUNCTION instead
    BC_RETURN_LEGACY,                              // Legacy: use BC_RETURN instead
    BC_PUSH_FRAME_LEGACY,                          // Legacy: use BC_PUSH_FRAME instead
    BC_POP_FRAME_LEGACY                            // Legacy: use BC_POP_FRAME instead
} BytecodeSuperOp;

typedef struct {
    BytecodeOp op;
    int a;   // Generic operand A (e.g., const/local index or jump target)
    int b;   // Generic operand B (varies by opcode)
    int c;   // Generic operand C (second index or jump target)
} BytecodeInstruction;

typedef struct {
    size_t return_pc;           // Program counter to return to
    size_t local_start;         // Start of local variables in locals array
    size_t local_count;         // Number of local variables
    size_t num_local_start;     // Start of numeric locals in num_locals array
    size_t num_local_count;     // Number of numeric locals
} BytecodeCallFrame;

typedef struct {
    char* name;                 // Function name
    BytecodeInstruction* code;  // Function bytecode
    size_t code_count;          // Number of instructions
    size_t code_capacity;       // Capacity for instructions
    char** param_names;         // Parameter names
    size_t param_count;         // Number of parameters
    size_t param_capacity;      // Capacity for parameters
    size_t local_start;         // Start of local variables for this function
    size_t local_count;         // Number of local variables for this function
    size_t num_local_start;     // Start of numeric locals for this function
    size_t num_local_count;     // Number of numeric locals for this function
} BytecodeFunction;

typedef struct {
    // Program buffer
    BytecodeInstruction* code;
    size_t count;
    size_t capacity;

    // Constants pool
    Value* constants;
    size_t const_count;
    size_t const_capacity;

    // AST references for fallback execution
    ASTNode** ast_nodes;
    size_t ast_count;
    size_t ast_capacity;

    // Local names mapping (simple linear map for safety and clarity)
    char** local_names;
    size_t local_count;
    size_t local_capacity;
    
    // Local variables storage for fast access
    Value* locals;
    size_t local_slot_count;
    size_t local_slot_capacity;
    
    // Numeric constants and locals for fast arithmetic
    double* num_constants;
    size_t num_const_count;
    size_t num_const_capacity;
    
    double* num_locals;
    size_t num_local_count;
    size_t num_local_capacity;
    
    // Function definitions (compiled to bytecode)
    BytecodeFunction* functions;
    size_t function_count;
    size_t function_capacity;
    
    // Call stack for function execution
    BytecodeCallFrame* call_stack;
    size_t call_stack_size;
    size_t call_stack_capacity;
    
    // Value pool for fast allocation (performance optimization)
    Value* value_pool;
    size_t value_pool_size;
    size_t value_pool_next;
    bool value_pool_initialized;
    
    // Interpreter reference for global variable access
    Interpreter* interpreter;
} BytecodeProgram;

// Compilation
BytecodeProgram* bytecode_program_create(void);
void bytecode_program_free(BytecodeProgram* program);
int bytecode_compile_program(BytecodeProgram* program, ASTNode* root, Interpreter* interpreter);

// Execution
Value bytecode_execute(BytecodeProgram* program, Interpreter* interpreter, int debug);
Value bytecode_execute_function_bytecode(Interpreter* interpreter, BytecodeFunction* func, Value* args, int arg_count, BytecodeProgram* program);

#endif // BYTECODE_H


