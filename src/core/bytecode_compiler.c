#include "../../include/core/bytecode.h"
#include "../../include/utils/shared_utilities.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Compiler optimization structures
typedef struct {
    int* constant_foldings;
    size_t count;
    size_t capacity;
} ConstantFoldingCache;

typedef struct {
    int* dead_instructions;
    size_t count;
    size_t capacity;
} DeadCodeTracker;

// Compiler optimization globals
static ConstantFoldingCache* const_fold_cache = NULL;
static DeadCodeTracker* dead_code_tracker = NULL;

// Helper function for adding numeric constants
static int bc_add_num_const(BytecodeProgram* p, double val) {
    if (p->num_const_count + 1 > p->num_const_capacity) {
        size_t new_cap = p->num_const_capacity ? p->num_const_capacity * 2 : 64;
        p->num_constants = shared_realloc_safe(p->num_constants, new_cap * sizeof(double), "bytecode", "bc_add_num_const", 1);
        p->num_const_capacity = new_cap;
    }
    p->num_constants[p->num_const_count] = val;
    return (int)p->num_const_count++;
}

// Compiler optimization helper functions
static void init_compiler_optimizations(void) {
    if (!const_fold_cache) {
        const_fold_cache = shared_malloc_safe(sizeof(ConstantFoldingCache), "bytecode_compiler", "init_const_fold", 0);
        if (const_fold_cache) {
            const_fold_cache->constant_foldings = shared_malloc_safe(64 * sizeof(int), "bytecode_compiler", "init_const_fold_array", 0);
            const_fold_cache->count = 0;
            const_fold_cache->capacity = 64;
        }
    }
    
    if (!dead_code_tracker) {
        dead_code_tracker = shared_malloc_safe(sizeof(DeadCodeTracker), "bytecode_compiler", "init_dead_code", 0);
        if (dead_code_tracker) {
            dead_code_tracker->dead_instructions = shared_malloc_safe(128 * sizeof(int), "bytecode_compiler", "init_dead_code_array", 0);
            dead_code_tracker->count = 0;
            dead_code_tracker->capacity = 128;
        }
    }
}

static void cleanup_compiler_optimizations(void) {
    if (const_fold_cache) {
        if (const_fold_cache->constant_foldings) {
            shared_free_safe(const_fold_cache->constant_foldings, "bytecode_compiler", "cleanup_const_fold", 0);
        }
        shared_free_safe(const_fold_cache, "bytecode_compiler", "cleanup_const_fold_struct", 0);
        const_fold_cache = NULL;
    }
    
    if (dead_code_tracker) {
        if (dead_code_tracker->dead_instructions) {
            shared_free_safe(dead_code_tracker->dead_instructions, "bytecode_compiler", "cleanup_dead_code", 0);
        }
        shared_free_safe(dead_code_tracker, "bytecode_compiler", "cleanup_dead_code_struct", 0);
        dead_code_tracker = NULL;
    }
}

// Constant folding optimization
static int try_constant_fold(BytecodeProgram* p, BytecodeOp op, int a, int b) {
    if (!const_fold_cache) return -1;
    
    // CRITICAL: Stack-based numeric operations (BC_ADD_NUM, BC_SUB_NUM, etc.) 
    // do NOT use operands a and b - they pop from the numeric stack.
    // Trying to fold them using a and b operands will cause incorrect results.
    // We should only fold operations that explicitly use operands, not stack-based ones.
    // Stack-based operations are: BC_ADD_NUM, BC_SUB_NUM, BC_MUL_NUM, BC_DIV_NUM
    // These cannot be constant-folded by checking operands - they need stack analysis.
    
    // For now, disable constant folding for stack-based numeric operations
    // to prevent incorrect folding (e.g., 5+3 becoming 10 because it reads constants[0] twice)
    if (op == BC_ADD_NUM || op == BC_SUB_NUM || op == BC_MUL_NUM || op == BC_DIV_NUM) {
        // These are stack-based operations - cannot fold by checking operands
        return -1;
    }
    
    return -1;
}

// Dead code elimination
static void mark_dead_instruction(int instruction_index) {
    if (!dead_code_tracker) return;
    
    if (dead_code_tracker->count >= dead_code_tracker->capacity) {
        size_t new_cap = dead_code_tracker->capacity * 2;
        int* new_array = shared_realloc_safe(
            dead_code_tracker->dead_instructions,
            new_cap * sizeof(int),
            "bytecode_compiler", "dead_code_realloc", 0
        );
        if (!new_array) return;
        dead_code_tracker->dead_instructions = new_array;
        dead_code_tracker->capacity = new_cap;
    }
    
    dead_code_tracker->dead_instructions[dead_code_tracker->count] = instruction_index;
    dead_code_tracker->count++;
}

// Loop unrolling optimization
static int should_unroll_loop(BytecodeProgram* p, int start_idx, int end_idx) {
    if (!p || start_idx < 0 || end_idx >= (int)p->count || start_idx >= end_idx) {
        return 0;
    }
    
    int instruction_count = end_idx - start_idx + 1;
    
    // Unroll loops with <= 5 instructions and <= 3 iterations
    return (instruction_count <= 5);
}

// Local variable slot reuse optimization
static int find_reusable_slot(BytecodeProgram* p, int current_slot) {
    if (!p || current_slot < 0 || current_slot >= (int)p->local_slot_count) {
        return current_slot;
    }
    
    // Simple strategy: reuse slots that are no longer needed
    // This is a basic implementation - could be enhanced with liveness analysis
    return current_slot;
}

// Forward declarations
static void apply_constant_folding(BytecodeProgram* program);
static void apply_dead_code_elimination(BytecodeProgram* program);
static void apply_loop_unrolling(BytecodeProgram* program);
static void apply_slot_reuse(BytecodeProgram* program);

// Apply all compiler optimizations
static void apply_compiler_optimizations(BytecodeProgram* program) {
    if (!program || !program->code) return;
    
    // Phase 1: Constant folding
    apply_constant_folding(program);
    
    // Phase 2: Dead code elimination
    apply_dead_code_elimination(program);
    
    // Phase 3: Loop unrolling
    apply_loop_unrolling(program);
    
    // Phase 4: Local variable slot reuse
    apply_slot_reuse(program);
}

// Apply constant folding optimization
static void apply_constant_folding(BytecodeProgram* program) {
    if (!program || !program->code) return;
    
    // DISABLED: Constant folding for stack-based numeric operations is broken
    // because it tries to use instruction operands (a, b) which are always 0
    // for stack-based operations. This causes incorrect folding (e.g., 5+3 becomes 10).
    // TODO: Implement proper constant folding that analyzes the stack state
    // instead of trying to use non-existent operands.
    
    // For now, skip constant folding entirely to prevent arithmetic bugs
    (void)program;  // Suppress unused parameter warning
    return;
    
    /*
    for (size_t i = 0; i < program->count; i++) {
        BytecodeInstruction* instr = &program->code[i];
        
        // Try to fold numeric operations
        if (instr->op == BC_ADD_NUM || instr->op == BC_SUB_NUM || 
            instr->op == BC_MUL_NUM || instr->op == BC_DIV_NUM) {
            
            int folded_result = try_constant_fold(program, instr->op, instr->a, instr->b);
            if (folded_result >= 0) {
                // Replace with load constant
                instr->op = BC_LOAD_NUM;
                instr->a = folded_result;
                instr->b = 0;
            }
        }
    }
    */
}

// Apply dead code elimination
static void apply_dead_code_elimination(BytecodeProgram* program) {
    // DISABLED: This optimization has a bug where it marks jump targets as dead code
    // TODO: Fix this to properly track reachable instructions via jump target analysis
    (void)program;
    return;
    
    /*
    if (!program || !program->code || !dead_code_tracker) return;
    
    // Mark unreachable code after unconditional jumps
    for (size_t i = 0; i < program->count; i++) {
        BytecodeInstruction* instr = &program->code[i];
        
        if (instr->op == BC_JUMP || instr->op == BC_RETURN) {
            // BUG: This marks ALL instructions after the jump as dead,
            // even if they are jump targets!
            // Mark instructions after unconditional jumps as dead
            for (size_t j = i + 1; j < program->count; j++) {
                mark_dead_instruction((int)j);
            }
            break;
        }
    }
    
    // Remove dead instructions (simplified - in practice would compact array)
    // For now, just mark them as NOP
    for (size_t i = 0; i < dead_code_tracker->count; i++) {
        int dead_idx = dead_code_tracker->dead_instructions[i];
        if (dead_idx >= 0 && dead_idx < (int)program->count) {
            program->code[dead_idx].op = BC_POP; // Use BC_POP as NOP equivalent
            program->code[dead_idx].a = 0;
            program->code[dead_idx].b = 0;
        }
    }
    */
}

// Apply loop unrolling optimization
static void apply_loop_unrolling(BytecodeProgram* program) {
    if (!program || !program->code) return;
    
    // Simple loop detection and unrolling
    for (size_t i = 0; i < program->count - 1; i++) {
        BytecodeInstruction* instr = &program->code[i];
        
        // Look for loop patterns (simplified detection)
        if (instr->op == BC_JUMP_IF_FALSE) {
            int target = instr->a;
            if (target > (int)i && target < (int)program->count) {
                // Check if this is a small loop that should be unrolled
                if (should_unroll_loop(program, (int)i, target)) {
                    // Mark for unrolling (simplified - would duplicate instructions)
                    // For now, just optimize the loop condition by inverting it
                    // This is a placeholder - actual unrolling would duplicate instructions
                }
            }
        }
    }
}

// Apply local variable slot reuse optimization
static void apply_slot_reuse(BytecodeProgram* program) {
    if (!program || !program->code) return;
    
    // Optimize local variable assignments
    for (size_t i = 0; i < program->count; i++) {
        BytecodeInstruction* instr = &program->code[i];
        
        if (instr->op == BC_STORE_LOCAL) {
            // Try to reuse a slot if possible
            int new_slot = find_reusable_slot(program, instr->a);
            if (new_slot != instr->a) {
                instr->a = new_slot;
            }
        }
    }
}

static void bc_emit(BytecodeProgram* p, BytecodeOp op, int a, int b) {
    if (p->count + 1 > p->capacity) {
        size_t new_cap = p->capacity ? p->capacity * 2 : 128;
        // Cap maximum bytecode size to prevent excessive memory usage
        if (new_cap > 500000) {
            new_cap = 500000;
        }
        p->code = shared_realloc_safe(p->code, new_cap * sizeof(BytecodeInstruction), "bytecode", "bc_emit", 1);
        p->capacity = new_cap;
    }
    p->code[p->count].op = op;
    p->code[p->count].a = a;
    p->code[p->count].b = b;
    p->code[p->count].c = 0;
    p->count++;
}

static void bc_emit_super(BytecodeProgram* p, BytecodeOp op, int a, int b, int c) {
    if (p->count + 1 > p->capacity) {
        size_t new_cap = p->capacity ? p->capacity * 2 : 128;
        p->code = shared_realloc_safe(p->code, new_cap * sizeof(BytecodeInstruction), "bytecode", "bc_emit_super", 1);
        p->capacity = new_cap;
    }
    p->code[p->count].op = op;
    p->code[p->count].a = a;
    p->code[p->count].b = b;
    p->code[p->count].c = c;
    p->count++;
}

static int bc_add_const(BytecodeProgram* p, Value v) {
    if (p->const_count + 1 > p->const_capacity) {
        size_t new_cap = p->const_capacity ? p->const_capacity * 2 : 64;
        // Cap maximum constant pool size to prevent excessive memory usage
        if (new_cap > 100000) {
            new_cap = 100000;
        }
        p->constants = shared_realloc_safe(p->constants, new_cap * sizeof(Value), "bytecode", "bc_add_const", 1);
        p->const_capacity = new_cap;
    }
    // Store the value in the constant pool
    // For strings and arrays, we need to ensure they are duplicated so the constant pool owns them
    // This prevents issues where freeing a temporary Value frees the data that the constant uses
    int idx = (int)p->const_count;
    if (v.type == VALUE_STRING && v.data.string_value) {
        // For strings, create a new copy so the constant pool owns it
        // This prevents the string from being freed when temporary Values are freed
        p->constants[p->const_count] = value_create_string(v.data.string_value);
        // Free the original value (it was a temporary)
        value_free(&v);
    } else if (v.type == VALUE_ARRAY) {
        // For arrays, clone the array so the constant pool owns it
        // This prevents the array elements from being freed when temporary Values are freed
        p->constants[p->const_count] = value_clone(&v);
        // Free the original value (it was a temporary)
        value_free(&v);
    } else {
        // For other types, store directly
        p->constants[p->const_count] = v;
    }
    p->const_count++;
    return idx;
}

static int bc_add_ast(BytecodeProgram* p, ASTNode* n) {
    if (!p || !n) return -1;  // Validate inputs
    
    // Validate node pointer is in reasonable memory range before storing
    uintptr_t node_addr = (uintptr_t)n;
    if (node_addr < 0x1000 || node_addr > 0x7fffffffffffULL || 
        (node_addr >> 48) == 0xffffULL) {
        // Invalid node pointer - don't store it
        return -1;
    }
    
    // Validate node structure appears valid by checking type field
    // This is a heuristic - we can't fully validate without risking a segfault
    // But we can at least check if the type looks reasonable
    if (n->type > 100) {  // AST node types should be small enum values
        // Suspicious type value - don't store
        return -1;
    }
    
    if (p->ast_count + 1 > p->ast_capacity) {
        size_t new_cap = p->ast_capacity ? p->ast_capacity * 2 : 64;
        p->ast_nodes = shared_realloc_safe(p->ast_nodes, new_cap * sizeof(ASTNode*), "bytecode", "bc_add_ast", 1);
        p->ast_capacity = new_cap;
    }
    p->ast_nodes[p->ast_count] = n;
    return (int)p->ast_count++;
}

static void bc_emit_to_function(BytecodeFunction* func, BytecodeOp op, int a, int b, int c) {
    if (func->code_count + 1 > func->code_capacity) {
        size_t new_cap = func->code_capacity * 2;
        func->code = shared_realloc_safe(func->code, new_cap * sizeof(BytecodeInstruction), "bytecode", "bc_emit_to_function", 1);
        func->code_capacity = new_cap;
    }
    func->code[func->code_count] = (BytecodeInstruction){op, a, b, c};
    func->code_count++;
}

// Forward declaration
int bc_compile_ast_to_subprogram(BytecodeProgram* p, ASTNode* node, const char* name);

static void compile_node_to_function(BytecodeProgram* p, BytecodeFunction* func, ASTNode* n) {
    if (!n || !p || !func) return;
    
    // Validate node pointer is in reasonable memory range
    uintptr_t node_addr = (uintptr_t)n;
    if (node_addr < 0x1000 || node_addr > 0x7fffffffffffULL || (node_addr >> 48) == 0xffffULL) {
        if (p->interpreter) {
            interpreter_set_error(p->interpreter, "Invalid AST node pointer in compile_node_to_function", 0, 0);
        }
        return;
    }
    
    // Validate node type is reasonable before accessing
    if (n->type > 100 || n->type < 0) {
        if (p->interpreter) {
            char error_msg[256];
            snprintf(error_msg, sizeof(error_msg), "Invalid AST node type %d in compile_node_to_function", n->type);
            interpreter_set_error(p->interpreter, error_msg, 0, 0);
        }
        return;
    }
    
    switch (n->type) {
        case AST_NODE_NUMBER: {
            // For functions, we need to load numbers as Value types, not numeric stack
            // because return statements and function calls expect Value types
            int const_idx = bc_add_const(p, value_create_number(n->data.number_value));
            bc_emit_to_function(func, BC_LOAD_CONST, const_idx, 0, 0);
        } break;
        case AST_NODE_STRING: {
            // String literals should be loaded as constants onto the value stack
            int const_idx = bc_add_const(p, value_create_string(n->data.string_value));
            bc_emit_to_function(func, BC_LOAD_CONST, const_idx, 0, 0);
        } break;
        case AST_NODE_BOOL: {
            int const_idx = bc_add_const(p, value_create_boolean(n->data.bool_value));
            bc_emit_to_function(func, BC_LOAD_CONST, const_idx, 0, 0);
        } break;
        case AST_NODE_NULL: {
            int const_idx = bc_add_const(p, value_create_null());
            bc_emit_to_function(func, BC_LOAD_CONST, const_idx, 0, 0);
        } break;
        case AST_NODE_IDENTIFIER: {
            // Check if this is a function parameter
            int param_idx = -1;
            for (size_t i = 0; i < func->param_count; i++) {
                if (func->param_names[i] && strcmp(func->param_names[i], n->data.identifier_value) == 0) {
                    param_idx = (int)i;
                    break;
                }
            }
            
            if (param_idx >= 0) {
                // This is a function parameter - load from environment
                int const_idx = bc_add_const(p, value_create_string(n->data.identifier_value));
                bc_emit_to_function(func, BC_LOAD_VAR, const_idx, 0, 0);
            } else {
                // This is a global variable or function name - load from global environment
                int const_idx = bc_add_const(p, value_create_string(n->data.identifier_value));
                bc_emit_to_function(func, BC_LOAD_GLOBAL, const_idx, 0, 0);
            }
        } break;
        case AST_NODE_BINARY_OP: {
            // Compile left and right operands
            compile_node_to_function(p, func, n->data.binary.left);
            compile_node_to_function(p, func, n->data.binary.right);
            
            // Emit the operation
            switch (n->data.binary.op) {
                case OP_ADD:
                    bc_emit_to_function(func, BC_ADD, 0, 0, 0);
                    break;
                case OP_SUBTRACT:
                    bc_emit_to_function(func, BC_SUB, 0, 0, 0);
                    break;
                case OP_MULTIPLY:
                    bc_emit_to_function(func, BC_MUL, 0, 0, 0);
                    break;
                case OP_DIVIDE:
                    bc_emit_to_function(func, BC_DIV, 0, 0, 0);
                    break;
                case OP_MODULO:
                    bc_emit_to_function(func, BC_MOD, 0, 0, 0);
                    break;
                case OP_EQUAL:
                    bc_emit_to_function(func, BC_EQ, 0, 0, 0);
                    break;
                case OP_NOT_EQUAL:
                    bc_emit_to_function(func, BC_NE, 0, 0, 0);
                    break;
                case OP_LESS_THAN:
                    bc_emit_to_function(func, BC_LT, 0, 0, 0);
                    break;
                case OP_LESS_EQUAL:
                    bc_emit_to_function(func, BC_LE, 0, 0, 0);
                    break;
                case OP_GREATER_THAN:
                    bc_emit_to_function(func, BC_GT, 0, 0, 0);
                    break;
                case OP_GREATER_EQUAL:
                    bc_emit_to_function(func, BC_GE, 0, 0, 0);
                    break;
                case OP_LOGICAL_AND:
                    bc_emit_to_function(func, BC_AND, 0, 0, 0);
                    break;
                case OP_LOGICAL_OR:
                    bc_emit_to_function(func, BC_OR, 0, 0, 0);
                    break;
                case OP_RANGE:
                    // Range operator: start..end (exclusive)
                    // Stack: [end, start] -> [range]
                    bc_emit_to_function(func, BC_CREATE_RANGE, 0, 0, 0);
                    break;
                case OP_RANGE_INCLUSIVE:
                    // Range operator: start..=end (inclusive)
                    // For now, treat as exclusive range (BC_CREATE_RANGE doesn't support inclusive yet)
                    bc_emit_to_function(func, BC_CREATE_RANGE, 0, 0, 0);
                    break;
                case OP_RANGE_STEP:
                    // Range operator with step: start..end:step
                    // Stack: [step, end, start] -> [range]
                    if (n->data.binary.step) {
                        compile_node_to_function(p, func, n->data.binary.step);
                    } else {
                        // No step provided - use default step of 1.0
                        int step_idx = bc_add_const(p, value_create_number(1.0));
                        bc_emit_to_function(func, BC_LOAD_CONST, step_idx, 0, 0);
                    }
                    bc_emit_to_function(func, BC_CREATE_RANGE_STEP, 0, 0, 0);
                    break;
                case OP_POWER:
                case OP_LOGICAL_XOR:
                case OP_BITWISE_AND:
                case OP_BITWISE_OR:
                case OP_BITWISE_XOR:
                case OP_LEFT_SHIFT:
                case OP_RIGHT_SHIFT:
                    // Unsupported binary operators - report error
                    if (p->interpreter) {
                        char error_msg[256];
                        snprintf(error_msg, sizeof(error_msg), "Binary operator %d not supported in function bytecode compilation", n->data.binary.op);
                        interpreter_set_error(p->interpreter, error_msg, 0, 0);
                    }
                    break;
                default:
                    // Unknown binary operator - report error
                    if (p->interpreter) {
                        char error_msg[256];
                        snprintf(error_msg, sizeof(error_msg), "Binary operator %d not supported in function bytecode compilation", n->data.binary.op);
                        interpreter_set_error(p->interpreter, error_msg, 0, 0);
                    }
                    break;
            }
        } break;
        case AST_NODE_UNARY_OP: {
            if (!n || !n->data.unary.operand) {
                if (p->interpreter) {
                    interpreter_set_error(p->interpreter, "Invalid unary operation operand in function bytecode compilation", 0, 0);
                }
                break;
            }
            
            compile_node_to_function(p, func, n->data.unary.operand);
            
            switch (n->data.unary.op) {
                case OP_NEGATIVE:
                    // Negate: multiply by -1
                    bc_emit_to_function(func, BC_LOAD_CONST, bc_add_const(p, value_create_number(-1.0)), 0, 0);
                    bc_emit_to_function(func, BC_MUL, 0, 0, 0);
                    break;
                case OP_LOGICAL_NOT:
                    bc_emit_to_function(func, BC_NOT, 0, 0, 0);
                    break;
                default:
                    if (p->interpreter) {
                        char error_msg[256];
                        snprintf(error_msg, sizeof(error_msg), "Unary operator %d not supported in function bytecode compilation", n->data.unary.op);
                        interpreter_set_error(p->interpreter, error_msg, 0, 0);
                    }
                    break;
            }
        } break;
        case AST_NODE_IF_STATEMENT: {
            // Compile condition
            compile_node_to_function(p, func, n->data.if_statement.condition);
            
            // Jump if false to else-if/else/end
            int jmp_false_pos = (int)func->code_count;
            bc_emit_to_function(func, BC_JUMP_IF_FALSE, 0, 0, 0); // Placeholder
            
            // Compile then block
            if (n->data.if_statement.then_block) {
                compile_node_to_function(p, func, n->data.if_statement.then_block);
            }
            
            // Jump to end (skip else-if/else block)
            int jmp_end_pos = (int)func->code_count;
            bc_emit_to_function(func, BC_JUMP, 0, 0, 0); // Placeholder
            
            // Patch the false jump to point to else-if chain, else block, or end
            int else_start = (int)func->code_count;
            
            // Handle else-if chain first (if present)
            if (n->data.if_statement.else_if_chain) {
                // Compile the else-if chain (which is itself an if statement)
                compile_node_to_function(p, func, n->data.if_statement.else_if_chain);
            } else if (n->data.if_statement.else_block) {
                // Compile else block
                compile_node_to_function(p, func, n->data.if_statement.else_block);
            }
            int end_pos = (int)func->code_count;
            
            // Patch jumps
            func->code[jmp_false_pos].a = else_start;
            func->code[jmp_end_pos].a = end_pos;
        } break;
        case AST_NODE_RETURN: {
            if (n->data.return_statement.value) {
                // Compile the return value
                compile_node_to_function(p, func, n->data.return_statement.value);
                // Return value is now on the stack
                bc_emit_to_function(func, (BytecodeOp)BC_RETURN, 1, 0, 0); // Return with value
            } else {
                // Return without value
                bc_emit_to_function(func, (BytecodeOp)BC_RETURN, 0, 0, 0); // Return without value
            }
        } break;
        case AST_NODE_BLOCK: {
            // Compile all statements in the block
            if (n->data.block.statements && n->data.block.statement_count > 0) {
                for (size_t i = 0; i < n->data.block.statement_count; i++) {
                    ASTNode* stmt = n->data.block.statements[i];
                    if (stmt) {
                        compile_node_to_function(p, func, stmt);
                    }
                }
            } else {
                // Fallback to linked list traversal if array is empty
                ASTNode* stmt = n->data.block.statements ? *n->data.block.statements : NULL;
                while (stmt) {
                    compile_node_to_function(p, func, stmt);
                    stmt = stmt->next;
                }
            }
        } break;
        case AST_NODE_FUNCTION_CALL: {
            // For print statements, compile directly
            if (strcmp(n->data.function_call.function_name, "print") == 0) {
                // Compile all arguments first
                for (size_t i = 0; i < n->data.function_call.argument_count; i++) {
                    compile_node_to_function(p, func, n->data.function_call.arguments[i]);
                }
                // Then print all arguments on one line
                bc_emit_to_function(func, BC_PRINT_MULTIPLE, (int)n->data.function_call.argument_count, 0, 0);
            } else {
                // Look up existing function and call it
                int func_id = -1;
                
                // Search in function table (includes functions registered before this one)
                for (size_t i = 0; i < p->function_count; i++) {
                    if (p->functions[i].name) {
                        if (strcmp(p->functions[i].name, n->data.function_call.function_name) == 0) {
                            func_id = (int)i;
                            break;
                        }
                    }
                }
                
                if (func_id >= 0) {
                    // Found function in bytecode table - compile call
                    // Compile arguments
                    for (size_t i = 0; i < n->data.function_call.argument_count; i++) {
                        compile_node_to_function(p, func, n->data.function_call.arguments[i]);
                    }
                    // Emit user function call
                    bc_emit_to_function(func, BC_CALL_USER_FUNCTION, func_id, (int)n->data.function_call.argument_count, 0);
                } else {
                    // Function not found in bytecode table - try loading from environment
                    // This handles function variables and recursive calls
                    // Compile arguments first
                    for (size_t i = 0; i < n->data.function_call.argument_count; i++) {
                        compile_node_to_function(p, func, n->data.function_call.arguments[i]);
                    }
                    // Load function from global environment
                    int name_idx = bc_add_const(p, value_create_string(n->data.function_call.function_name));
                    bc_emit_to_function(func, BC_LOAD_GLOBAL, name_idx, 0, 0);
                    // Call the function value
                    bc_emit_to_function(func, BC_CALL_FUNCTION_VALUE, (int)n->data.function_call.argument_count, 0, 0);
                }
            }
        } break;
        case AST_NODE_FUNCTION_CALL_EXPR: {
            // Method call expression: obj.method(args)
            if (n->data.function_call_expr.function && 
                n->data.function_call_expr.function->type == AST_NODE_MEMBER_ACCESS) {
                ASTNode* member_access = n->data.function_call_expr.function;
                const char* method_name = member_access->data.member_access.member_name;
                
                // Compile the object
                compile_node_to_function(p, func, member_access->data.member_access.object);
                
                // Check for array methods that modify in place
                if (method_name && strcmp(method_name, "push") == 0 && n->data.function_call_expr.argument_count == 1) {
                    // For arr.push(val), we need to:
                    // 1. Array is already on stack from compile_node_to_function above
                    // 2. Load val onto stack
                    // 3. Call BC_ARRAY_PUSH (modifies arr and returns it)
                    // 4. Store the result back to arr variable
                    // Compile argument (array is already on stack)
                    compile_node_to_function(p, func, n->data.function_call_expr.arguments[0]);
                    // Now stack has: [array, value]
                    bc_emit_to_function(func, BC_ARRAY_PUSH, 0, 0, 0);
                    // Stack now has: [modified_array]
                    // Store result back to variable if member access is a simple identifier
                    if (member_access->data.member_access.object->type == AST_NODE_IDENTIFIER) {
                        const char* var_name = member_access->data.member_access.object->data.identifier_value;
                        int var_name_idx = bc_add_const(p, value_create_string(var_name));
                        bc_emit_to_function(func, BC_STORE_GLOBAL, var_name_idx, 0, 0);
                    }
                } else {
                    // Compile arguments
                    for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                        compile_node_to_function(p, func, n->data.function_call_expr.arguments[i]);
                    }
                    
                    // Handle common property access patterns (like .type, .toString)
                    if (method_name && strcmp(method_name, "toString") == 0 && n->data.function_call_expr.argument_count == 0) {
                        bc_emit_to_function(func, BC_TO_STRING, 0, 0, 0);
                    } else if (method_name && strcmp(method_name, "type") == 0 && n->data.function_call_expr.argument_count == 0) {
                        bc_emit_to_function(func, BC_GET_TYPE, 0, 0, 0);
                    } else {
                        // General method call - use BC_METHOD_CALL
                        int method_name_idx = bc_add_const(p, value_create_string(method_name ? method_name : ""));
                        bc_emit_to_function(func, BC_METHOD_CALL, method_name_idx, (int)n->data.function_call_expr.argument_count, 0);
                    }
                }
            } else if (n->data.function_call_expr.function && 
                       n->data.function_call_expr.function->type == AST_NODE_IDENTIFIER) {
                // Function variable call: f(args...)
                // Compile arguments first
                for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                    compile_node_to_function(p, func, n->data.function_call_expr.arguments[i]);
                }
                // Load function from environment
                const char* func_name = n->data.function_call_expr.function->data.identifier_value;
                // Load function from global environment (functions are stored globally)
                int name_idx = bc_add_const(p, value_create_string(func_name));
                bc_emit_to_function(func, BC_LOAD_GLOBAL, name_idx, 0, 0);
                // Call the function value
                bc_emit_to_function(func, BC_CALL_FUNCTION_VALUE, (int)n->data.function_call_expr.argument_count, 0, 0);
            } else {
                // Function call expression with complex expression as function - compile the function expression first
                if (n->data.function_call_expr.function) {
                    // Compile the function expression (could be a property access, etc.)
                    compile_node_to_function(p, func, n->data.function_call_expr.function);
                    // Compile arguments
                    for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                        compile_node_to_function(p, func, n->data.function_call_expr.arguments[i]);
                    }
                    // Call the function value on the stack
                    bc_emit_to_function(func, BC_CALL_FUNCTION_VALUE, (int)n->data.function_call_expr.argument_count, 0, 0);
                } else {
                    // No function specified - error
                    if (p->interpreter) {
                        interpreter_set_error(p->interpreter, "Function call expression has no function", 0, 0);
                    }
                }
            }
        } break;
        case AST_NODE_MEMBER_ACCESS: {
            // Property access: obj.name
            const char* member_name = n->data.member_access.member_name;
            
            // Check for common built-in properties and methods
            if (member_name && strcmp(member_name, "toString") == 0) {
                compile_node_to_function(p, func, n->data.member_access.object);
                bc_emit_to_function(func, BC_TO_STRING, 0, 0, 0);
            } else if (member_name && strcmp(member_name, "type") == 0) {
                compile_node_to_function(p, func, n->data.member_access.object);
                bc_emit_to_function(func, BC_GET_TYPE, 0, 0, 0);
            } else if (member_name && strcmp(member_name, "length") == 0) {
                compile_node_to_function(p, func, n->data.member_access.object);
                // Use property access for length
                int length_idx = bc_add_const(p, value_create_string("length"));
                bc_emit_to_function(func, BC_PROPERTY_ACCESS, length_idx, 0, 0);
            } else {
                // Fall back to general property access
                compile_node_to_function(p, func, n->data.member_access.object);
                int property_name_idx = bc_add_const(p, value_create_string(member_name ? member_name : ""));
                bc_emit_to_function(func, BC_PROPERTY_ACCESS, property_name_idx, 0, 0);
            }
        } break;
        case AST_NODE_ARRAY_LITERAL: {
            // Array literal: [elem1, elem2, elem3]
            size_t element_count = n->data.array_literal.element_count;
            
            // Compile all elements
            for (size_t i = 0; i < element_count; i++) {
                if (n->data.array_literal.elements[i]) {
                    compile_node_to_function(p, func, n->data.array_literal.elements[i]);
                }
            }
            
            // Emit array creation instruction
            bc_emit_to_function(func, BC_CREATE_ARRAY, (int)element_count, 0, 0);
        } break;
        case AST_NODE_HASH_MAP_LITERAL: {
            // Hash map literal: {key1: val1, key2: val2}
            size_t pair_count = n->data.hash_map_literal.pair_count;
            
            // Compile all key-value pairs (in reverse order for stack)
            // BC_CREATE_MAP expects (value, key) pairs on stack
            for (size_t i = 0; i < pair_count; i++) {
                // Compile value first, then key
                if (n->data.hash_map_literal.values && n->data.hash_map_literal.values[i]) {
                    compile_node_to_function(p, func, n->data.hash_map_literal.values[i]);
                }
                if (n->data.hash_map_literal.keys && n->data.hash_map_literal.keys[i]) {
                    compile_node_to_function(p, func, n->data.hash_map_literal.keys[i]);
                }
            }
            
            // Emit map creation instruction
            bc_emit_to_function(func, BC_CREATE_MAP, (int)pair_count, 0, 0);
        } break;
        case AST_NODE_ARRAY_ACCESS: {
            // Array access: arr[index]
            compile_node_to_function(p, func, n->data.array_access.array);
            compile_node_to_function(p, func, n->data.array_access.index);
            bc_emit_to_function(func, BC_ARRAY_GET, 0, 0, 0);
        } break;
        case AST_NODE_ASSIGNMENT: {
            // Assignment: var = value
            if (!n->data.assignment.target || !n->data.assignment.value) {
                if (p->interpreter) {
                    interpreter_set_error(p->interpreter, "Invalid assignment structure in compile_node_to_function", 0, 0);
                }
                break;
            }
            
            // Validate target pointer
            uintptr_t target_addr = (uintptr_t)(n->data.assignment.target);
            if (target_addr < 0x1000 || target_addr > 0x7fffffffffffULL) {
                if (p->interpreter) {
                    interpreter_set_error(p->interpreter, "Invalid assignment target pointer in compile_node_to_function", 0, 0);
                }
                break;
            }
            
            if (n->data.assignment.target->type == AST_NODE_IDENTIFIER) {
                // Simple variable assignment
                compile_node_to_function(p, func, n->data.assignment.value);
                if (n->data.assignment.target->data.identifier_value) {
                    int name_idx = bc_add_const(p, value_create_string(n->data.assignment.target->data.identifier_value));
                    bc_emit_to_function(func, BC_STORE_GLOBAL, name_idx, 0, 0);
                }
            } else if (n->data.assignment.target->type == AST_NODE_ARRAY_ACCESS) {
                // Array element assignment: arr[index] = value
                if (n->data.assignment.target->data.array_access.array && n->data.assignment.target->data.array_access.index) {
                    compile_node_to_function(p, func, n->data.assignment.target->data.array_access.array);
                    compile_node_to_function(p, func, n->data.assignment.target->data.array_access.index);
                    compile_node_to_function(p, func, n->data.assignment.value);
                    // Check if array is a simple identifier for environment update
                    int is_simple_var = 0;
                    int var_name_idx = -1;
                    if (n->data.assignment.target->data.array_access.array->type == AST_NODE_IDENTIFIER &&
                        n->data.assignment.target->data.array_access.array->data.identifier_value) {
                        is_simple_var = 1;
                        var_name_idx = bc_add_const(p, value_create_string(n->data.assignment.target->data.array_access.array->data.identifier_value));
                    }
                    bc_emit_to_function(func, BC_ARRAY_SET, is_simple_var, var_name_idx, 0);
                }
            } else {
                // Other assignment types not yet supported in function bodies
                if (p->interpreter) {
                    interpreter_set_error(p->interpreter, "Assignment type not yet supported in function bytecode compilation", 0, 0);
                }
            }
        } break;
        case AST_NODE_VARIABLE_DECLARATION: {
            // Variable declaration: let x = value
            if (n->data.variable_declaration.initial_value) {
                compile_node_to_function(p, func, n->data.variable_declaration.initial_value);
            } else {
                int null_idx = bc_add_const(p, value_create_null());
                bc_emit_to_function(func, BC_LOAD_CONST, null_idx, 0, 0);
            }
            int name_idx = bc_add_const(p, value_create_string(n->data.variable_declaration.variable_name));
            bc_emit_to_function(func, BC_STORE_GLOBAL, name_idx, 0, 0);
        } break;
        case AST_NODE_FOR_LOOP: {
            // For loop: for iterator_name in collection body
            if (!n->data.for_loop.collection || !n->data.for_loop.iterator_name) {
                if (p->interpreter) {
                    interpreter_set_error(p->interpreter, "Invalid for loop structure in compile_node_to_function", 0, 0);
                }
                break;
            }
            
            // Compile the collection expression
            compile_node_to_function(p, func, n->data.for_loop.collection);
            
            // Compile body to bytecode sub-program
            int iterator_name_idx = bc_add_const(p, value_create_string(n->data.for_loop.iterator_name ? n->data.for_loop.iterator_name : "i"));
            int body_func_id = -1;
            if (n->data.for_loop.body) {
                body_func_id = bc_compile_ast_to_subprogram(p, n->data.for_loop.body, "<for_loop_body>");
            }
            
            // Emit BC_FOR_LOOP instruction
            bc_emit_to_function(func, BC_FOR_LOOP, iterator_name_idx, body_func_id, 0);
        } break;
        case AST_NODE_WHILE_LOOP: {
            // While loop
            if (!n->data.while_loop.condition) {
                if (p->interpreter) {
                    interpreter_set_error(p->interpreter, "Invalid while loop structure in compile_node_to_function", 0, 0);
                }
                break;
            }
            
            int loop_start = (int)func->code_count;
            bc_emit_to_function(func, BC_LOOP_START, 0, 0, 0);
            
            // Compile condition
            compile_node_to_function(p, func, n->data.while_loop.condition);
            
            // Jump if false (exit loop)
            int jump_to_end = (int)func->code_count;
            bc_emit_to_function(func, BC_JUMP_IF_FALSE, 0, 0, 0);
            
            // Compile body
            if (n->data.while_loop.body) {
                compile_node_to_function(p, func, n->data.while_loop.body);
            }
            
            // Jump back to condition
            bc_emit_to_function(func, BC_JUMP, loop_start, 0, 0);
            
            // Patch the exit jump
            if (jump_to_end < (int)func->code_count && func->code) {
                func->code[jump_to_end].a = (int)func->code_count;
            }
            
            bc_emit_to_function(func, BC_LOOP_END, 0, 0, 0);
        } break;
        case AST_NODE_BREAK: {
            // Break statement
            bc_emit_to_function(func, BC_BREAK, 0, 0, 0);
        } break;
        case AST_NODE_CONTINUE: {
            // Continue statement
            bc_emit_to_function(func, BC_CONTINUE, 0, 0, 0);
        } break;
        case AST_NODE_TRY_CATCH: {
            // Try-catch: try { ... } catch err { ... } finally { ... }
            int try_start_pos = (int)func->code_count;
            bc_emit_to_function(func, BC_TRY_START, 0, 0, 0);
            
            // Compile try block
            if (n->data.try_catch.try_block) {
                compile_node_to_function(p, func, n->data.try_catch.try_block);
            }
            
            int try_end_pos = (int)func->code_count;
            bc_emit_to_function(func, BC_TRY_END, 0, 0, 0);
            
            // Compile catch block (if exists)
            if (n->data.try_catch.catch_block) {
                int catch_var_idx = bc_add_const(p, value_create_string(
                    (n->data.try_catch.catch_variable && strlen(n->data.try_catch.catch_variable) > 0) 
                        ? n->data.try_catch.catch_variable 
                        : ""));
                int catch_block_func_id = bc_compile_ast_to_subprogram(p, n->data.try_catch.catch_block, "<catch_block>");
                bc_emit_to_function(func, BC_CATCH, catch_var_idx, catch_block_func_id, 0);
            }
            
            // Compile finally block (if exists)
            if (n->data.try_catch.finally_block) {
                compile_node_to_function(p, func, n->data.try_catch.finally_block);
            }
        } break;
        case AST_NODE_ERROR: {
            // Error node - compilation should have failed earlier, but handle gracefully
            // Push null result and continue
            bc_emit_to_function(func, BC_LOAD_CONST, bc_add_const(p, value_create_null()), 0, 0);
        } break;
        
        default:
            // Unknown AST node type in compile_node_to_function - try to compile as main-level node
            // This allows some nodes to fall through to main compilation
            // Report error but don't stop compilation
            if (p->interpreter && n->type != AST_NODE_PATTERN_NOT && 
                n->type != AST_NODE_PATTERN_TYPE && n->type != AST_NODE_PATTERN_WILDCARD &&
                n->type != AST_NODE_PATTERN_DESTRUCTURE && n->type != AST_NODE_PATTERN_GUARD &&
                n->type != AST_NODE_PATTERN_OR && n->type != AST_NODE_PATTERN_AND &&
                n->type != AST_NODE_PATTERN_RANGE && n->type != AST_NODE_PATTERN_REGEX) {
                // Pattern nodes are expected in certain contexts, so we silently skip them
                // For other unknown nodes, report error but continue
                char error_msg[256];
                snprintf(error_msg, sizeof(error_msg), "AST node type %d not supported in function bytecode compilation", n->type);
                interpreter_set_error(p->interpreter, error_msg, 0, 0);
            }
            break;
    }
}

// Compile an AST node to a bytecode sub-program (for loop bodies, catch blocks, etc.)
int bc_compile_ast_to_subprogram(BytecodeProgram* p, ASTNode* node, const char* name) {
    if (!p || !node) return -1;
    
    if (p->function_count + 1 > p->function_capacity) {
        size_t new_cap = p->function_capacity ? p->function_capacity * 2 : 64;
        p->functions = shared_realloc_safe(p->functions, new_cap * sizeof(BytecodeFunction), "bytecode", "bc_compile_ast_to_subprogram", 1);
        p->function_capacity = new_cap;
    }
    
    int func_id = (int)p->function_count;
    BytecodeFunction* bc_func = &p->functions[func_id];
    memset(bc_func, 0, sizeof(BytecodeFunction));
    
    bc_func->name = shared_strdup(name ? name : "<subprogram>");
    bc_func->param_count = 0;
    bc_func->code_capacity = 64;
    bc_func->code = shared_malloc_safe(bc_func->code_capacity * sizeof(BytecodeInstruction), "bytecode", "bc_compile_ast_to_subprogram", 2);
    
    p->function_count++;
    
    compile_node_to_function(p, bc_func, node);
    
    return func_id;
}

static int bc_add_function(BytecodeProgram* p, ASTNode* func) {
    if (p->function_count + 1 > p->function_capacity) {
        size_t new_cap = p->function_capacity ? p->function_capacity * 2 : 64;
        p->functions = shared_realloc_safe(p->functions, new_cap * sizeof(BytecodeFunction), "bytecode", "bc_add_function", 1);
        p->function_capacity = new_cap;
    }
    
    // Get the function index - this will be the function's ID
    int func_id = (int)p->function_count;
    
    BytecodeFunction* bc_func = &p->functions[func_id];
    memset(bc_func, 0, sizeof(BytecodeFunction));
    
    // Handle both function definitions and lambda functions
    if (func->type == AST_NODE_LAMBDA) {
        // Lambda function - use anonymous name
        bc_func->name = shared_strdup("<lambda>");
        
        // Store parameter names from lambda
        bc_func->param_count = func->data.lambda.parameter_count;
        if (bc_func->param_count > 0) {
            bc_func->param_capacity = bc_func->param_count;
            bc_func->param_names = shared_malloc_safe(bc_func->param_count * sizeof(char*), "bytecode", "bc_add_function", 2);
            for (size_t i = 0; i < bc_func->param_count; i++) {
                if (func->data.lambda.parameters[i] && func->data.lambda.parameters[i]->type == AST_NODE_IDENTIFIER) {
                    bc_func->param_names[i] = shared_strdup(func->data.lambda.parameters[i]->data.identifier_value);
                } else {
                    bc_func->param_names[i] = shared_strdup("");
                }
            }
        }
    } else {
        // Regular function definition
        // Store function name FIRST so recursive calls can find it during body compilation
        bc_func->name = shared_strdup(func->data.function_definition.function_name);
        
        // Store parameter names
        bc_func->param_count = func->data.function_definition.parameter_count;
        if (bc_func->param_count > 0) {
            bc_func->param_capacity = bc_func->param_count;
            bc_func->param_names = shared_malloc_safe(bc_func->param_count * sizeof(char*), "bytecode", "bc_add_function", 2);
            for (size_t i = 0; i < bc_func->param_count; i++) {
                bc_func->param_names[i] = shared_strdup(func->data.function_definition.parameters[i]->data.identifier_value);
            }
        }
    }
    
    // Allocate space for function bytecode
    bc_func->code_capacity = 64;
    bc_func->code = shared_malloc_safe(bc_func->code_capacity * sizeof(BytecodeInstruction), "bytecode", "bc_add_function", 3);
    
    // NOW increment count so function is "registered" in the table
    // This allows recursive calls in the body to find this function
    p->function_count++;
    
    // Compile function body to bytecode (function is now in table, so recursive calls work)
    ASTNode* body = NULL;
    if (func->type == AST_NODE_LAMBDA) {
        body = func->data.lambda.body;
    } else {
        body = func->data.function_definition.body;
    }
    
    if (body) {
        compile_node_to_function(p, bc_func, body);
    }
    
    return func_id;
}

BytecodeProgram* bytecode_program_create(void) {
    BytecodeProgram* p = shared_malloc_safe(sizeof(BytecodeProgram), "bytecode", "create", 1);
    if (!p) return NULL;
    memset(p, 0, sizeof(*p));
    
    // Initialize value pool for performance
    p->value_pool_initialized = false;
    p->value_pool_size = 0;
    p->value_pool_next = 0;
    p->value_pool = NULL;
    
    return p;
}

void bytecode_program_free(BytecodeProgram* p) {
    if (!p) return;
    // Free constants
    for (size_t i = 0; i < p->const_count; i++) {
        value_free(&p->constants[i]);
    }
    shared_free_safe(p->constants, "bytecode", "free", 1);
    shared_free_safe(p->code, "bytecode", "free", 2);
    // NOTE: Free ast_nodes array (the array of pointers), but NOT the AST nodes they point to
    // The AST nodes themselves are owned by the parser and will be freed when parser is freed
    // Freeing the array prevents memory leaks, but we must not free the nodes themselves
    if (p->ast_nodes) {
        free(p->ast_nodes);  // Use raw free since we're not tracking this allocation
        p->ast_nodes = NULL;
    }
    if (p->local_names) {
        for (size_t i = 0; i < p->local_count; i++) {
            if (p->local_names[i]) shared_free_safe(p->local_names[i], "bytecode", "free", 4);
        }
        shared_free_safe(p->local_names, "bytecode", "free", 5);
    }
    // Free locals
    if (p->locals) {
        for (size_t i = 0; i < p->local_slot_count; i++) {
            value_free(&p->locals[i]);
        }
        shared_free_safe(p->locals, "bytecode", "free", 6);
    }
    // Free numeric arrays
    shared_free_safe(p->num_constants, "bytecode", "free", 7);
    shared_free_safe(p->num_locals, "bytecode", "free", 8);
    // Free value pool
    if (p->value_pool_initialized && p->value_pool) {
        // Free any string values in the pool
        for (size_t i = 0; i < p->value_pool_next; i++) {
            if (p->value_pool[i].type == VALUE_STRING && p->value_pool[i].data.string_value) {
                shared_free_safe(p->value_pool[i].data.string_value, "bytecode", "free_pool_string", 0);
            }
        }
        shared_free_safe(p->value_pool, "bytecode", "free", 11);
    }
    // Free function definitions
    if (p->functions) {
        for (size_t i = 0; i < p->function_count; i++) {
            if (p->functions[i].name) shared_free_safe(p->functions[i].name, "bytecode", "free", 9);
            if (p->functions[i].code) shared_free_safe(p->functions[i].code, "bytecode", "free", 10);
            if (p->functions[i].param_names) {
                for (size_t j = 0; j < p->functions[i].param_count; j++) {
                    if (p->functions[i].param_names[j]) shared_free_safe(p->functions[i].param_names[j], "bytecode", "free", 11);
                }
                shared_free_safe(p->functions[i].param_names, "bytecode", "free", 12);
            }
        }
        shared_free_safe(p->functions, "bytecode", "free", 13);
    }
    // Free call stack
    shared_free_safe(p->call_stack, "bytecode", "free", 14);
    shared_free_safe(p, "bytecode", "free", 15);
}

static void compile_node(BytecodeProgram* p, ASTNode* n);

static int lookup_local(BytecodeProgram* p, const char* name) {
    for (size_t i = 0; i < p->local_count; i++) {
        if (p->local_names[i] && strcmp(p->local_names[i], name) == 0) return (int)i;
    }
    return -1;
}

static int define_local(BytecodeProgram* p, const char* name) {
    int idx = lookup_local(p, name);
    if (idx >= 0) return idx;
    if (p->local_count + 1 > p->local_capacity) {
        size_t new_cap = p->local_capacity ? p->local_capacity * 2 : 16;
        p->local_names = shared_realloc_safe(p->local_names, new_cap * sizeof(char*), "bytecode", "define_local", 1);
        p->local_capacity = new_cap;
    }
    p->local_names[p->local_count] = shared_strdup(name ? name : "");
    
    // Ensure locals array has enough slots
    if (p->local_count + 1 > p->local_slot_capacity) {
        size_t new_cap = p->local_slot_capacity ? p->local_slot_capacity * 2 : 16;
        p->locals = shared_realloc_safe(p->locals, new_cap * sizeof(Value), "bytecode", "define_local", 2);
        p->local_slot_capacity = new_cap;
    }
    // Initialize new local slot with null
    p->locals[p->local_count] = value_create_null();
    
    // Ensure numeric locals array has enough slots
    if (p->local_count + 1 > p->num_local_capacity) {
        size_t new_cap = p->num_local_capacity ? p->num_local_capacity * 2 : 16;
        p->num_locals = shared_realloc_safe(p->num_locals, new_cap * sizeof(double), "bytecode", "define_local", 3);
        p->num_local_capacity = new_cap;
    }
    // Initialize new numeric local slot with 0
    p->num_locals[p->local_count] = 0.0;
    
    int new_idx = (int)p->local_count;
    p->local_count++;
    p->local_slot_count = p->local_count;
    p->num_local_count = p->local_count;
    return new_idx;
}

// Check if an identifier refers to a numeric variable
static bool is_numeric_identifier(BytecodeProgram* p, const char* name) {
    // Check if it's a local numeric variable
    for (size_t i = 0; i < p->local_count; i++) {
        if (strcmp(p->local_names[i], name) == 0) {
            return true; // Assume locals are numeric for now
        }
    }
    return false;
}

// Check if a function name is a built-in function
static bool is_builtin_function(const char* name) {
    // List of common built-in functions
    const char* builtins[] = {
        "len", "toString", "type", "isInt", "isString", "isArray", "isObject",
        "abs", "sqrt", "pow", "sin", "cos", "tan", "log", "exp",
        "push", "pop", "shift", "unshift", "join", "split", "substring",
        "keys", "values", "has", "get", "set", "delete",
        "add", "remove", "contains", "size", "clear",
        "enqueue", "dequeue", "peek", "isEmpty",
        "push_stack", "pop_stack", "top", "is_empty",
        "insert", "remove_node", "find", "traverse",
        "add_edge", "remove_edge", "has_edge", "neighbors",
        "insert_heap", "extract_min", "peek_min", "is_empty_heap",
        "parse", "stringify", "get", "set", "has", "delete",
        "get", "post", "put", "delete", "head", "options",
        "match", "replace", "split", "test", "exec",
        "now", "sleep", "format", "parse", "add", "subtract",
        "create", "insert", "delete", "search", "inorder", "preorder", "postorder"
    };
    
    for (size_t i = 0; i < sizeof(builtins) / sizeof(builtins[0]); i++) {
        if (strcmp(name, builtins[i]) == 0) {
            return true;
        }
    }
    return false;
}

// Check if a binary operation involves only numbers
static bool is_numeric_binary_op(BytecodeProgram* p, ASTNode* n) {
    if (!n || n->type != AST_NODE_BINARY_OP || !n->data.binary.left || !n->data.binary.right) return false;
    
    // Validate operand pointers before accessing them
    uintptr_t left_addr = (uintptr_t)n->data.binary.left;
    uintptr_t right_addr = (uintptr_t)n->data.binary.right;
    if (left_addr < 0x1000 || left_addr >= 0x800000000000ULL ||
        right_addr < 0x1000 || right_addr >= 0x800000000000ULL ||
        (left_addr >> 48) == 0xffffULL || (right_addr >> 48) == 0xffffULL) {
        return false;
    }
    
    // Only treat explicit number literals as numeric
    // Do NOT assume local variables are numeric - they could be strings, objects, etc.
    bool left_is_num = (n->data.binary.left->type == AST_NODE_NUMBER);
    bool right_is_num = (n->data.binary.right->type == AST_NODE_NUMBER);
    
    return left_is_num && right_is_num;
}

// Check if a node is an array literal or identifier (for array concatenation)
static bool is_array_literal_or_identifier(ASTNode* n) {
    if (!n) return false;
    
    // Check for array literal
    if (n->type == AST_NODE_ARRAY_LITERAL) {
        return true;
    }
    
    // Check for identifier (could be an array variable)
    if (n->type == AST_NODE_IDENTIFIER) {
        return true;
    }
    
    return false;
}

// Compile a node to numeric bytecode (for fast numeric operations)
static void compile_numeric_node(BytecodeProgram* p, ASTNode* n) {
    if (!n) return;
    
    switch (n->type) {
        case AST_NODE_NUMBER: {
            int idx = bc_add_num_const(p, n->data.number_value);
            bc_emit(p, BC_LOAD_NUM, idx, 0);
        } break;
        case AST_NODE_IDENTIFIER: {
            int idx = lookup_local(p, n->data.identifier_value);
            if (idx >= 0) {
                // Load the value from local storage and convert to numeric
                bc_emit(p, BC_LOAD_LOCAL, idx, 0);
                bc_emit(p, BC_VALUE_TO_NUM, 0, 0);
            } else {
                // Fall back to value operations for globals
                compile_node(p, n);
                bc_emit(p, BC_VALUE_TO_NUM, 0, 0);
            }
        } break;
        case AST_NODE_BINARY_OP: {
            if (is_numeric_binary_op(p, n)) {
                compile_numeric_node(p, n->data.binary.left);
                compile_numeric_node(p, n->data.binary.right);
                
                switch (n->data.binary.op) {
                    case OP_ADD: bc_emit(p, BC_ADD_NUM, 0, 0); break;
                    case OP_SUBTRACT: bc_emit(p, BC_SUB_NUM, 0, 0); break;
                    case OP_MULTIPLY: bc_emit(p, BC_MUL_NUM, 0, 0); break;
                    case OP_DIVIDE: bc_emit(p, BC_DIV_NUM, 0, 0); break;
                    case OP_MODULO: bc_emit(p, BC_MOD_NUM, 0, 0); break;
                    default: {
                        // Convert to value and fall back
                        bc_emit(p, BC_NUM_TO_VALUE, 0, 0);
                        compile_node(p, n);
                    } break;
                }
            } else {
                // Fall back to value operations
                compile_node(p, n);
                bc_emit(p, BC_VALUE_TO_NUM, 0, 0);
            }
        } break;
        default: {
            // Fall back to value operations
            compile_node(p, n);
            bc_emit(p, BC_VALUE_TO_NUM, 0, 0);
        } break;
    }
}

static void compile_binary(BytecodeProgram* p, ASTNode* n) {
    // Validate node pointer and type before accessing fields
    if (!n || n->type != AST_NODE_BINARY_OP) {
        // Invalid node - compilation error
        if (p->interpreter) {
            interpreter_set_error(p->interpreter, "Invalid binary operation node in bytecode compilation", 0, 0);
        }
        return;
    }
    
    // Validate node pointer is in reasonable memory range
    uintptr_t node_addr = (uintptr_t)n;
    if (node_addr < 0x1000 || node_addr > 0x7fffffffffffULL || 
        (node_addr >> 48) == 0xffffULL) {
        // Invalid node pointer - compilation error
        if (p->interpreter) {
            interpreter_set_error(p->interpreter, "Invalid node pointer in binary operation compilation", 0, 0);
        }
        return;
    }
    
    // Validate left and right operands before accessing them
    // CRITICAL: We must read the pointers BEFORE validating, but we can't safely read
    // if the memory is corrupted. So we validate the node address first, then try to read.
    // If reading causes a BUS error, AddressSanitizer will catch it.
    ASTNode* left_node = NULL;
    ASTNode* right_node = NULL;
    
    // Try to read operands - if this causes a BUS error, AddressSanitizer will catch it
    // We've already validated node_addr, so this should be safe
    if (node_addr >= 0x1000 && node_addr <= 0x7fffffffffffULL) {
        left_node = n->data.binary.left;
        right_node = n->data.binary.right;
    }
    
    // Validate what we read
    if (left_node) {
        uintptr_t left_addr = (uintptr_t)left_node;
        if (left_addr < 0x1000 || left_addr >= 0x800000000000ULL || 
            left_addr == 0xffffbebebebebebeULL ||
            (left_addr & 0xFFFFFFFFULL) == 0xbebebebeULL ||
            (left_addr & 0xFFFF000000000000ULL) != 0) {
            left_node = NULL;
        }
    }
    
    if (right_node) {
        uintptr_t right_addr = (uintptr_t)right_node;
        if (right_addr < 0x1000 || right_addr >= 0x800000000000ULL ||
            right_addr == 0xffffbebebebebebeULL ||
            (right_addr & 0xFFFFFFFFULL) == 0xbebebebeULL ||
            (right_addr & 0xFFFF000000000000ULL) != 0) {
            right_node = NULL;
        }
    }
    
    // If validation fails, we can't safely compile - but we should still try
    // to avoid AST fallback. The issue is that compile_node(NULL) just returns,
    // which would leave the stack unbalanced. So we need to ensure both operands
    // are compiled, even if one is NULL.
    if (!left_node || !right_node) {
        // Invalid operands - compilation error
        if (p->interpreter) {
            interpreter_set_error(p->interpreter, "Invalid binary operation operands in bytecode compilation", 0, 0);
        }
        return;
    }
    
    // Check for array concatenation FIRST (before numeric check)
    if (n->data.binary.op == OP_ADD && 
        is_array_literal_or_identifier(left_node) && 
        is_array_literal_or_identifier(right_node)) {
        // Array concatenation: arr1 + arr2
        compile_node(p, left_node);
        compile_node(p, right_node);
        bc_emit(p, BC_ARRAY_CONCAT, 0, 0);
    }
    // Check for range operator (..)
    else if (n->data.binary.op == OP_RANGE || n->data.binary.op == OP_RANGE_STEP) {
        // Range: start..end or start..end..step
        compile_node(p, left_node);
        compile_node(p, right_node);
        if (n->data.binary.op == OP_RANGE_STEP && n->data.binary.step) {
            // Range with step: start..end..step
            compile_node(p, n->data.binary.step);
            bc_emit(p, BC_CREATE_RANGE_STEP, 0, 0);
        } else {
            // Simple range: start..end (step = 1.0)
            bc_emit(p, BC_CREATE_RANGE, 0, 0);
        }
    }
    // Check if this is a pure numeric operation
    else if (is_numeric_binary_op(p, n)) {
        // Use fast numeric operations
        compile_numeric_node(p, left_node);
        compile_numeric_node(p, right_node);
        
        switch (n->data.binary.op) {
            case OP_ADD: bc_emit(p, BC_ADD_NUM, 0, 0); break;
            case OP_SUBTRACT: bc_emit(p, BC_SUB_NUM, 0, 0); break;
            case OP_MULTIPLY: bc_emit(p, BC_MUL_NUM, 0, 0); break;
            case OP_DIVIDE: bc_emit(p, BC_DIV_NUM, 0, 0); break;
            case OP_MODULO: bc_emit(p, BC_MOD_NUM, 0, 0); break;
            case OP_EQUAL: bc_emit(p, BC_EQ_NUM, 0, 0); break;
            case OP_NOT_EQUAL: bc_emit(p, BC_NE_NUM, 0, 0); break;
            case OP_LESS_THAN: bc_emit(p, BC_LT_NUM, 0, 0); break;
            case OP_LESS_EQUAL: bc_emit(p, BC_LE_NUM, 0, 0); break;
            case OP_GREATER_THAN: bc_emit(p, BC_GT_NUM, 0, 0); break;
            case OP_GREATER_EQUAL: bc_emit(p, BC_GE_NUM, 0, 0); break;
                default: {
                    // Convert numeric result to value and fall back
                    bc_emit(p, BC_NUM_TO_VALUE, 0, 0);
                    // Binary operation not fully supported - compilation error
                    if (p->interpreter) {
                        interpreter_set_error(p->interpreter, "Binary operation not fully supported in bytecode", 0, 0);
                    }
                } break;
        }
        
        // Convert numeric result back to value for storage (only for arithmetic operations)
        if (n->data.binary.op == OP_ADD || n->data.binary.op == OP_SUBTRACT || 
            n->data.binary.op == OP_MULTIPLY || n->data.binary.op == OP_DIVIDE ||
            n->data.binary.op == OP_MODULO) {
            bc_emit(p, BC_NUM_TO_VALUE, 0, 0);
        }
    } else {
        // Use regular value operations
        compile_node(p, left_node);
        compile_node(p, right_node);
        switch (n->data.binary.op) {
                case OP_ADD: bc_emit(p, BC_ADD, 0, 0); break;
                case OP_SUBTRACT: bc_emit(p, BC_SUB, 0, 0); break;
                case OP_MULTIPLY: bc_emit(p, BC_MUL, 0, 0); break;
                case OP_DIVIDE: bc_emit(p, BC_DIV, 0, 0); break;
                case OP_MODULO: bc_emit(p, BC_MOD, 0, 0); break;
                case OP_EQUAL: bc_emit(p, BC_EQ, 0, 0); break;
                case OP_NOT_EQUAL: bc_emit(p, BC_NE, 0, 0); break;
                case OP_LESS_THAN: bc_emit(p, BC_LT, 0, 0); break;
                case OP_LESS_EQUAL: bc_emit(p, BC_LE, 0, 0); break;
                case OP_GREATER_THAN: bc_emit(p, BC_GT, 0, 0); break;
                case OP_GREATER_EQUAL: bc_emit(p, BC_GE, 0, 0); break;
                case OP_LOGICAL_AND: bc_emit(p, BC_AND, 0, 0); break;
                case OP_LOGICAL_OR: bc_emit(p, BC_OR, 0, 0); break;
                default: {
                    // Unsupported binary operator - compilation error
                    if (p->interpreter) {
                        char error_msg[256];
                        snprintf(error_msg, sizeof(error_msg), "Binary operator %d not supported in bytecode", n->data.binary.op);
                        interpreter_set_error(p->interpreter, error_msg, 0, 0);
                    }
                } break;
            }
    }
}


static void compile_pattern(BytecodeProgram* p, ASTNode* pattern) {
    if (!pattern) return;
    
    switch (pattern->type) {
        case AST_NODE_NUMBER:
        case AST_NODE_STRING:
        case AST_NODE_BOOL:
        case AST_NODE_NULL: {
            // Literal patterns - compile as constants
            compile_node(p, pattern);
            bc_emit(p, BC_PATTERN_LITERAL, 0, 0);
        } break;
        
        case AST_NODE_IDENTIFIER: {
            // Wildcard pattern - matches anything
            bc_emit(p, BC_PATTERN_WILDCARD, 0, 0);
        } break;
        
        case AST_NODE_PATTERN_TYPE: {
            // Type pattern - check if value matches type
            int type_name_idx = bc_add_const(p, value_create_string(pattern->data.pattern_type.type_name));
            bc_emit(p, BC_PATTERN_TYPE, type_name_idx, 0);
        } break;
        
        default: {
            // Complex patterns not yet supported in bytecode
            // TODO: Implement pattern matching bytecode compilation
            if (p->interpreter) {
                interpreter_set_error(p->interpreter, "Complex pattern matching not yet supported in bytecode", 0, 0);
            }
        } break;
    }
}

static void compile_node(BytecodeProgram* p, ASTNode* n) {
    if (!n) return;
    switch (n->type) {
        case AST_NODE_NUMBER: {
            int idx = bc_add_const(p, value_create_number(n->data.number_value));
            bc_emit(p, BC_LOAD_CONST, idx, 0);
        } break;
        case AST_NODE_STRING: {
            int idx = bc_add_const(p, value_create_string(n->data.string_value));
            bc_emit(p, BC_LOAD_CONST, idx, 0);
        } break;
        case AST_NODE_BOOL: {
            int idx = bc_add_const(p, value_create_boolean(n->data.bool_value));
            bc_emit(p, BC_LOAD_CONST, idx, 0);
        } break;
        case AST_NODE_NULL: {
            int idx = bc_add_const(p, value_create_null());
            bc_emit(p, BC_LOAD_CONST, idx, 0);
        } break;
        case AST_NODE_BINARY_OP: compile_binary(p, n); break;
        case AST_NODE_UNARY_OP: {
            // Compile unary operations
            // Validate node and operand before compiling
            if (!n || !n->data.unary.operand) {
                // Invalid operand - compilation error
                if (p->interpreter) {
                    interpreter_set_error(p->interpreter, "Invalid unary operation operand in bytecode compilation", 0, 0);
                }
                break;
            }
            
            // Validate operand pointer before using it
            ASTNode* operand = n->data.unary.operand;
            uintptr_t operand_addr = (uintptr_t)operand;
            if (operand_addr < 0x1000 || operand_addr > 0x7fffffffffffULL || 
                (operand_addr >> 48) == 0xffffULL) {
                // Invalid operand pointer - compilation error
                if (p->interpreter) {
                    interpreter_set_error(p->interpreter, "Invalid operand pointer in unary operation compilation", 0, 0);
                }
                break;
            }
            
            // Handle unary operations
            switch (n->data.unary.op) {
                case OP_NEGATIVE:
                    // Negate: compile operand and multiply by -1
                    compile_node(p, operand);
                    bc_emit(p, BC_LOAD_CONST, bc_add_const(p, value_create_number(-1.0)), 0);
                    bc_emit(p, BC_MUL, 0, 0);
                    break;
                case OP_LOGICAL_NOT:
                    // Logical not: compile operand first, then apply NOT
                    compile_node(p, operand);
                    bc_emit(p, BC_NOT, 0, 0);
                    break;
                default:
                    // Unknown unary operator - compilation error
                    if (p->interpreter) {
                        char error_msg[256];
                        snprintf(error_msg, sizeof(error_msg), "Unary operator %d not supported in bytecode", n->data.unary.op);
                        interpreter_set_error(p->interpreter, error_msg, 0, 0);
                    }
                    break;
            }
        } break;
        case AST_NODE_IDENTIFIER: {
            const char* identifier_name = n->data.identifier_value;
            int idx = lookup_local(p, identifier_name);
            if (idx >= 0) {
                // Load from main locals array (supports all types)
                bc_emit(p, BC_LOAD_LOCAL, idx, 0);
            } else {
                // Not a local - try to load from global environment
                // Even if it doesn't exist yet during compilation, it might exist at runtime
                // (e.g., functions defined later in the same block, or modules imported via use)
                // BC_LOAD_GLOBAL will handle missing globals gracefully at runtime
                int name_idx = bc_add_const(p, value_create_string(identifier_name));
                bc_emit(p, BC_LOAD_GLOBAL, name_idx, 0);
            }
        } break;
        case AST_NODE_VARIABLE_DECLARATION: {
            const char* var_name = n->data.variable_declaration.variable_name;
            
            if (n->data.variable_declaration.initial_value) {
                // Compile the initial value first (e.g., for "let x = mod", compile "mod")
                compile_node(p, n->data.variable_declaration.initial_value);
            } else {
                // Initialize with null
                bc_emit(p, BC_LOAD_CONST, bc_add_const(p, value_create_null()), 0);
            }
            
            // For now, always treat variable declarations as local variables
            // This matches the AST interpreter behavior
            int idx = define_local(p, var_name);
            bc_emit(p, BC_STORE_LOCAL, idx, 0);
            
            // If variable has export/private flags, store metadata
            if (n->data.variable_declaration.is_export || n->data.variable_declaration.is_private) {
                int name_idx = bc_add_const(p, value_create_string(var_name));
                int flags = 0;
                if (n->data.variable_declaration.is_export) {
                    flags |= 1; // bit 0 = export
                }
                if (n->data.variable_declaration.is_private) {
                    flags |= 2; // bit 1 = private
                }
                bc_emit(p, BC_SET_SYMBOL_FLAGS, name_idx, flags);
            }
        } break;
        case AST_NODE_ASSIGNMENT: {
            // Check if this is an array element assignment (target is an array access node)
            if (n->data.assignment.target && 
                n->data.assignment.target->type == AST_NODE_ARRAY_ACCESS) {
                // This is array[index] = value
                ASTNode* array_access = n->data.assignment.target;
                
                // Compile array, index, and value
                compile_node(p, array_access->data.array_access.array);
                compile_node(p, array_access->data.array_access.index);
                compile_node(p, n->data.assignment.value);
                
                // Check if the array is a simple identifier (for updating the variable)
                int is_simple_var = 0;
                int var_name_idx = -1;
                if (array_access->data.array_access.array->type == AST_NODE_IDENTIFIER) {
                    is_simple_var = 1;
                    var_name_idx = bc_add_const(p, value_create_string(
                        array_access->data.array_access.array->data.identifier_value));
                }
                
                // Emit BC_ARRAY_SET instruction
                // instr->a = variable name constant index (-1 if complex expression)
                // instr->b = 0 (reserved for future use)
                bc_emit(p, BC_ARRAY_SET, var_name_idx, is_simple_var ? 1 : 0);
            } else {
                // Regular variable assignment
                const char* var = n->data.assignment.variable_name;
                int dst = lookup_local(p, var);
                ASTNode* v = n->data.assignment.value;
                
                // Check if variable is a local or global
                int is_local = (dst >= 0);
                if (!is_local) {
                    // Variable is not a local - treat as global
                    // Compile the right-hand side (this will correctly handle x + 1)
                    compile_node(p, v);
                    // Store as global
                    int var_name_idx = bc_add_const(p, value_create_string(var));
                    bc_emit(p, BC_STORE_GLOBAL, var_name_idx, 0);
                } else {
                    // Variable is a local - use optimized local operations
                    // dst = dst + id
                    if (v && v->type == AST_NODE_BINARY_OP && v->data.binary.op == OP_ADD &&
                        v->data.binary.left && v->data.binary.left->type == AST_NODE_IDENTIFIER &&
                        strcmp(v->data.binary.left->data.identifier_value, var) == 0 &&
                        v->data.binary.right && v->data.binary.right->type == AST_NODE_IDENTIFIER) {
                        // 
                        int right = define_local(p, v->data.binary.right->data.identifier_value);
                        bc_emit_super(p, BC_ADD_LLL, dst, dst, right); // a = b + c with a=dst, b=dst, c=right
                    }
                    // dst = dst + IMM
                    else if (v && v->type == AST_NODE_BINARY_OP && v->data.binary.op == OP_ADD &&
                             v->data.binary.left && v->data.binary.left->type == AST_NODE_IDENTIFIER &&
                             strcmp(v->data.binary.left->data.identifier_value, var) == 0 &&
                             v->data.binary.right && v->data.binary.right->type == AST_NODE_NUMBER) {
                        int imm_idx = bc_add_num_const(p, v->data.binary.right->data.number_value);
                        // 
                        bc_emit_super(p, BC_ADD_LOCAL_IMM, dst, imm_idx, 0);
                    }
                    // dst = dst + 1
                    else if (v && v->type == AST_NODE_BINARY_OP && v->data.binary.op == OP_ADD &&
                             v->data.binary.left && v->data.binary.left->type == AST_NODE_IDENTIFIER &&
                             strcmp(v->data.binary.left->data.identifier_value, var) == 0 &&
                             v->data.binary.right && v->data.binary.right->type == AST_NODE_NUMBER &&
                             v->data.binary.right->data.number_value == 1.0) {
                        // 
                        bc_emit_super(p, BC_INC_LOCAL, dst, 0, 0);
                    }
                    else {
                        // Fallback - compile the right-hand side and store the result
                        compile_node(p, v);
                        bc_emit(p, BC_STORE_LOCAL, dst, 0);
                    }
                }
            }
        } break;
        case AST_NODE_IF_STATEMENT: {
            // Compile condition
            compile_node(p, n->data.if_statement.condition);
            
            // Jump if false to else/else-if/end
            int jmp_false_pos = (int)p->count;
            bc_emit(p, BC_JUMP_IF_FALSE, 0, 0); // Placeholder, will be patched
            
            // Compile then block
            if (n->data.if_statement.then_block) {
                compile_node(p, n->data.if_statement.then_block);
            }
            
            // Jump to end (skip else/else-if block)
            int jmp_end_pos = (int)p->count;
            bc_emit(p, BC_JUMP, 0, 0); // Placeholder, will be patched
            
            // Patch the false jump to point to else-if chain, else block, or end
            int else_start = (int)p->count;
            
            // Handle else-if chain first (if present)
            if (n->data.if_statement.else_if_chain) {
                // Compile the else-if chain (which is itself an if statement)
                compile_node(p, n->data.if_statement.else_if_chain);
            } else if (n->data.if_statement.else_block) {
                // Compile else block
                compile_node(p, n->data.if_statement.else_block);
            }
            
            int end_pos = (int)p->count;
            
            // Patch jumps
            p->code[jmp_false_pos].a = else_start; // Jump to else-if/else (or end if neither)
            p->code[jmp_end_pos].a = end_pos;      // Jump to after the entire IF statement
        } break;
        case AST_NODE_BLOCK: {
            for (size_t i = 0; i < n->data.block.statement_count; i++) {
                ASTNode* stmt = n->data.block.statements[i];
                compile_node(p, stmt);
                // Pop only for statements that likely push a value
                // IF statements don't push values (they compile their blocks as statements)
                // WHILE loops don't push values (they compile their bodies as statements)
                // BLOCK nodes don't push values (they compile their statements)
                if (stmt->type != AST_NODE_VARIABLE_DECLARATION &&
                    stmt->type != AST_NODE_ASSIGNMENT &&
                    stmt->type != AST_NODE_WHILE_LOOP &&
                    stmt->type != AST_NODE_BLOCK &&
                    stmt->type != AST_NODE_IF_STATEMENT &&
                    stmt->type != AST_NODE_BREAK &&
                    stmt->type != AST_NODE_CONTINUE &&
                    stmt->type != AST_NODE_RETURN &&
                    stmt->type != AST_NODE_USE) {  // Use statements don't push values that need popping
                    bc_emit(p, BC_POP, 0, 0);
                }
            }
        } break;
        
        case AST_NODE_BREAK: {
            // Break statement - set break_depth flag
            bc_emit(p, BC_BREAK, 0, 0);
        } break;
        
        case AST_NODE_CONTINUE: {
            // Continue statement - set continue_depth flag
            bc_emit(p, BC_CONTINUE, 0, 0);
        } break;
        
        case AST_NODE_RETURN: {
            // Return statement - evaluate value and set return flag
            if (n->data.return_statement.value) {
                compile_node(p, n->data.return_statement.value);
            } else {
                bc_emit(p, BC_LOAD_CONST, bc_add_const(p, value_create_null()), 0);
            }
            bc_emit(p, BC_RETURN, 0, 0);
        } break;
        
        case AST_NODE_FUNCTION: {
            // Function definition - add to function table
            int func_id = bc_add_function(p, n);
            // Function body is already compiled in bc_add_function
            
            // Emit instruction to define function in environment
            int name_idx = bc_add_const(p, value_create_string(n->data.function_definition.function_name));
            // Encode export/private flags in c operand: bit 0 = is_export, bit 1 = is_private
            int flags = 0;
            if (n->data.function_definition.is_export) {
                flags |= 1; // bit 0 = export
            }
            if (n->data.function_definition.is_private) {
                flags |= 2; // bit 1 = private
            }
            bc_emit_super(p, BC_DEFINE_FUNCTION, name_idx, func_id, flags);
        } break;
        case AST_NODE_MEMBER_ACCESS: {
            // Property access: obj.name
            const char* member_name = n->data.member_access.member_name;
            
            // Check for common built-in properties and methods
            if (strcmp(member_name, "toString") == 0) {
                compile_node(p, n->data.member_access.object);
                bc_emit(p, BC_TO_STRING, 0, 0);
            } else if (strcmp(member_name, "type") == 0) {
                compile_node(p, n->data.member_access.object);
                bc_emit(p, BC_GET_TYPE, 0, 0);
            } else if (strcmp(member_name, "length") == 0) {
                compile_node(p, n->data.member_access.object);
                bc_emit(p, BC_GET_LENGTH, 0, 0);
            } else if (strcmp(member_name, "isString") == 0) {
                compile_node(p, n->data.member_access.object);
                bc_emit(p, BC_IS_STRING, 0, 0);
            } else if (strcmp(member_name, "isInt") == 0) {
                compile_node(p, n->data.member_access.object);
                bc_emit(p, BC_IS_INT, 0, 0);
            } else if (strcmp(member_name, "isFloat") == 0) {
                compile_node(p, n->data.member_access.object);
                bc_emit(p, BC_IS_FLOAT, 0, 0);
            } else if (strcmp(member_name, "isBool") == 0) {
                compile_node(p, n->data.member_access.object);
                bc_emit(p, BC_IS_BOOL, 0, 0);
            } else if (strcmp(member_name, "isArray") == 0) {
                compile_node(p, n->data.member_access.object);
                bc_emit(p, BC_IS_ARRAY, 0, 0);
            } else if (strcmp(member_name, "isNull") == 0) {
                compile_node(p, n->data.member_access.object);
                bc_emit(p, BC_IS_NULL, 0, 0);
            } else if (strcmp(member_name, "isNumber") == 0) {
                compile_node(p, n->data.member_access.object);
                bc_emit(p, BC_IS_NUMBER, 0, 0);
            } else if (strcmp(member_name, "isObject") == 0) {
                compile_node(p, n->data.member_access.object);
                bc_emit(p, BC_IS_OBJECT, 0, 0);
            } else if (strcmp(member_name, "isFunction") == 0) {
                compile_node(p, n->data.member_access.object);
                bc_emit(p, BC_IS_FUNCTION, 0, 0);
            } else if (strcmp(member_name, "upper") == 0) {
                compile_node(p, n->data.member_access.object);
                bc_emit(p, BC_STRING_UPPER, 0, 0);
            } else if (strcmp(member_name, "lower") == 0) {
                compile_node(p, n->data.member_access.object);
                bc_emit(p, BC_STRING_LOWER, 0, 0);
            } else if (strcmp(member_name, "trim") == 0) {
                compile_node(p, n->data.member_access.object);
                bc_emit(p, BC_STRING_TRIM, 0, 0);
            } else {
                // Fall back to general property access
                compile_node(p, n->data.member_access.object);
                int property_name_idx = bc_add_const(p, value_create_string(member_name));
                bc_emit(p, BC_PROPERTY_ACCESS, property_name_idx, 0);
            }
        } break;
        case AST_NODE_ARRAY_LITERAL: {
            // Array literal: [elem1, elem2, elem3]
            // Compile all elements onto the stack
            for (size_t i = 0; i < n->data.array_literal.element_count; i++) {
                compile_node(p, n->data.array_literal.elements[i]);
            }
            // Emit instruction to create array from stack elements
            bc_emit(p, BC_CREATE_ARRAY, (int)n->data.array_literal.element_count, 0);
        } break;
        case AST_NODE_HASH_MAP_LITERAL: {
            // Map literal: {key1: value1, key2: value2}
            // Compile all key-value pairs and create map using BC_CREATE_MAP
            size_t pair_count = n->data.hash_map_literal.pair_count;
            
            // Compile key-value pairs in reverse order (stack is LIFO)
            // We push pairs as (key, value) so when we pop, we get (value, key)
            // But BC_CREATE_MAP expects (value, key) pairs, so we need to reverse
            // Actually, let's compile in forward order and push (value, key) pairs
            for (size_t i = 0; i < pair_count; i++) {
                // Compile value first (will be popped last)
                if (n->data.hash_map_literal.values && n->data.hash_map_literal.values[i]) {
                    compile_node(p, n->data.hash_map_literal.values[i]);
                } else {
                    bc_emit(p, BC_LOAD_CONST, bc_add_const(p, value_create_null()), 0);
                }
                
                // Compile key second (will be popped first)
                if (n->data.hash_map_literal.keys && n->data.hash_map_literal.keys[i]) {
                    compile_node(p, n->data.hash_map_literal.keys[i]);
                } else {
                    bc_emit(p, BC_LOAD_CONST, bc_add_const(p, value_create_string("")), 0);
                }
            }
            
            // Create map from the pairs on the stack
            bc_emit(p, BC_CREATE_MAP, (int)pair_count, 0);
        } break;
        case AST_NODE_SET_LITERAL: {
            // Set literal: {elem1, elem2, elem3}
            // Compile all elements onto the stack
            for (size_t i = 0; i < n->data.set_literal.element_count; i++) {
                compile_node(p, n->data.set_literal.elements[i]);
            }
            // Emit instruction to create set from stack elements
            bc_emit(p, BC_CREATE_SET, (int)n->data.set_literal.element_count, 0);
        } break;
        case AST_NODE_FUNCTION_CALL_EXPR: {
            // Function call expression: func(args...) or obj.method(args...)
            // Check if the function is a member access
            if (!n->data.function_call_expr.function) {
                // No function specified - error
                if (p->interpreter) {
                    interpreter_set_error(p->interpreter, "Function call expression has no function", 0, 0);
                }
                // Push null result
                bc_emit(p, BC_LOAD_CONST, bc_add_const(p, value_create_null()), 0);
                break;
            }
            if (n->data.function_call_expr.function->type == AST_NODE_MEMBER_ACCESS) {
                ASTNode* member_access = n->data.function_call_expr.function;
                const char* method_name = member_access->data.member_access.member_name;
                
                // Compile the object
                compile_node(p, member_access->data.member_access.object);
                
                // Check for specific built-in methods that have direct bytecode support
                if (strcmp(method_name, "toString") == 0 && n->data.function_call_expr.argument_count == 0) {
                    bc_emit(p, BC_TO_STRING, 0, 0);
                } else if (strcmp(method_name, "type") == 0 && n->data.function_call_expr.argument_count == 0) {
                    bc_emit(p, BC_GET_TYPE, 0, 0);
                } else if (strcmp(method_name, "length") == 0 && n->data.function_call_expr.argument_count == 0) {
                    bc_emit(p, BC_GET_LENGTH, 0, 0);
                } else if (strcmp(method_name, "isString") == 0 && n->data.function_call_expr.argument_count == 0) {
                    bc_emit(p, BC_IS_STRING, 0, 0);
                } else if (strcmp(method_name, "isNumber") == 0 && n->data.function_call_expr.argument_count == 0) {
                    bc_emit(p, BC_IS_NUMBER, 0, 0);
                } else if (strcmp(method_name, "isInt") == 0 && n->data.function_call_expr.argument_count == 0) {
                    bc_emit(p, BC_IS_INT, 0, 0);
                } else if (strcmp(method_name, "isFloat") == 0 && n->data.function_call_expr.argument_count == 0) {
                    bc_emit(p, BC_IS_FLOAT, 0, 0);
                } else if (strcmp(method_name, "isBool") == 0 && n->data.function_call_expr.argument_count == 0) {
                    bc_emit(p, BC_IS_BOOL, 0, 0);
                } else if (strcmp(method_name, "isArray") == 0 && n->data.function_call_expr.argument_count == 0) {
                    bc_emit(p, BC_IS_ARRAY, 0, 0);
                } else if (strcmp(method_name, "isNull") == 0 && n->data.function_call_expr.argument_count == 0) {
                    bc_emit(p, BC_IS_NULL, 0, 0);
                } else if (strcmp(method_name, "isObject") == 0 && n->data.function_call_expr.argument_count == 0) {
                    bc_emit(p, BC_IS_OBJECT, 0, 0);
                } else if (strcmp(method_name, "isFunction") == 0 && n->data.function_call_expr.argument_count == 0) {
                    bc_emit(p, BC_IS_FUNCTION, 0, 0);
                } else {
                    // Check for array methods
                    if (strcmp(method_name, "contains") == 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        bc_emit(p, BC_ARRAY_CONTAINS, 0, 0);
                    } else if (strcmp(method_name, "indexOf") == 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        bc_emit(p, BC_ARRAY_INDEX_OF, 0, 0);
                    } else if (strcmp(method_name, "join") == 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        bc_emit(p, BC_ARRAY_JOIN, 0, 0);
                    } else if (strcmp(method_name, "unique") == 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        bc_emit(p, BC_ARRAY_UNIQUE, 0, 0);
                    } else if (strcmp(method_name, "slice") == 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        bc_emit(p, BC_ARRAY_SLICE, 0, 0);
                    } else if (strcmp(method_name, "concat") == 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        bc_emit(p, BC_ARRAY_CONCAT_METHOD, 0, 0);
                    } else if (strcmp(method_name, "upper") == 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        bc_emit(p, BC_STRING_UPPER, 0, 0);
                    } else if (strcmp(method_name, "lower") == 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        bc_emit(p, BC_STRING_LOWER, 0, 0);
                    } else if (strcmp(method_name, "trim") == 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        bc_emit(p, BC_STRING_TRIM, 0, 0);
                    } else if (strcmp(method_name, "split") == 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        bc_emit(p, BC_STRING_SPLIT, 0, 0);
                    } else if (strcmp(method_name, "replace") == 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        bc_emit(p, BC_STRING_REPLACE, 0, 0);
                    } else if (strcmp(method_name, "abs") == 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        bc_emit(p, BC_MATH_ABS, 0, 0);
                    } else if (strcmp(method_name, "sqrt") == 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        bc_emit(p, BC_MATH_SQRT, 0, 0);
                    } else if (strcmp(method_name, "pow") == 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        bc_emit(p, BC_MATH_POW, 0, 0);
                    } else if (strcmp(method_name, "sin") == 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        bc_emit(p, BC_MATH_SIN, 0, 0);
                    } else if (strcmp(method_name, "cos") == 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        bc_emit(p, BC_MATH_COS, 0, 0);
                    } else if (strcmp(method_name, "tan") == 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        bc_emit(p, BC_MATH_TAN, 0, 0);
                    } else if (strcmp(method_name, "floor") == 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        bc_emit(p, BC_MATH_FLOOR, 0, 0);
                    } else if (strcmp(method_name, "ceil") == 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        bc_emit(p, BC_MATH_CEIL, 0, 0);
                    } else if (strcmp(method_name, "round") == 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        bc_emit(p, BC_MATH_ROUND, 0, 0);
                    } else if (strcmp(method_name, "push") == 0 && n->data.function_call_expr.argument_count == 1) {
                        // Object is already on stack from line 1831
                        // Compile argument
                        compile_node(p, n->data.function_call_expr.arguments[0]);
                        // Stack now: [object, arg1]
                        // Use BC_METHOD_CALL to dispatch to correct push implementation at runtime
                        int method_name_idx = bc_add_const(p, value_create_string(method_name));
                        bc_emit(p, BC_METHOD_CALL, method_name_idx, n->data.function_call_expr.argument_count);
                        // Don't auto-store result - let the caller (assignment or expression context) handle it
                    } else if (strcmp(method_name, "pop") == 0 && n->data.function_call_expr.argument_count == 0) {
                        // Check if this is an array or a stack/queue/heap
                        // For now, we can't determine at compile time, so use method call for non-array objects
                        // Arrays will be handled by BC_ARRAY_POP at runtime
                        // For library objects (Stack, Queue, Heap), use method call
                        // Add method name to constants
                        int method_name_idx = bc_add_const(p, value_create_string(method_name));
                        
                        // Emit method call instruction (runtime will determine if it's array or library object)
                        bc_emit(p, BC_METHOD_CALL, method_name_idx, 0);
                    } else {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                            compile_node(p, n->data.function_call_expr.arguments[i]);
                        }
                        
                        // Add method name to constants
                        int method_name_idx = bc_add_const(p, value_create_string(method_name));
                        
                        // Emit method call instruction
                        bc_emit(p, BC_METHOD_CALL, method_name_idx, (int)n->data.function_call_expr.argument_count);
                    }
                }
            } else if (n->data.function_call_expr.function->type == AST_NODE_IDENTIFIER) {
                // Function variable call: f(args...)
                // Compile arguments first
                for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                    compile_node(p, n->data.function_call_expr.arguments[i]);
                }
                // Load function from environment
                const char* func_name = n->data.function_call_expr.function->data.identifier_value;
                // Check if it's a local variable first
                int local_idx = lookup_local(p, func_name);
                if (local_idx >= 0) {
                    // Load from local storage
                    bc_emit(p, BC_LOAD_LOCAL, local_idx, 0);
                } else {
                    // Load from global environment
                    int name_idx = bc_add_const(p, value_create_string(func_name));
                    bc_emit(p, BC_LOAD_GLOBAL, name_idx, 0);
                }
                // Call the function value
                bc_emit(p, BC_CALL_FUNCTION_VALUE, (int)n->data.function_call_expr.argument_count, 0);
            } else {
                // Function call expression with complex expression as function - compile the function expression first
                if (n->data.function_call_expr.function) {
                    // Compile the function expression (could be a property access, etc.)
                    compile_node(p, n->data.function_call_expr.function);
                    // Compile arguments
                    for (size_t i = 0; i < n->data.function_call_expr.argument_count; i++) {
                        compile_node(p, n->data.function_call_expr.arguments[i]);
                    }
                    // Call the function value on the stack
                    bc_emit(p, BC_CALL_FUNCTION_VALUE, (int)n->data.function_call_expr.argument_count, 0);
                } else {
                    // No function specified - error
                    if (p->interpreter) {
                        interpreter_set_error(p->interpreter, "Function call expression has no function", 0, 0);
                    }
                    // Push null result
                    bc_emit(p, BC_LOAD_CONST, bc_add_const(p, value_create_null()), 0);
                }
            }
        } break;
        case AST_NODE_USE: {
            // Use statements - import library or file module
            const char* library_name = n->data.use_statement.library_name;
            const char* alias = n->data.use_statement.alias;
            char** specific_items = n->data.use_statement.specific_items;
            char** specific_aliases = n->data.use_statement.specific_aliases;
            size_t item_count = n->data.use_statement.item_count;
            
            int lib_name_idx = bc_add_const(p, value_create_string(library_name));
            int alias_idx = 0;
            if (alias) {
                alias_idx = bc_add_const(p, value_create_string(alias));
            }
            
            // Store specific_items array in constant pool if present
            int specific_items_idx = 0;
            if (specific_items && item_count > 0) {
                // Create array of item names
                Value items_array = value_create_array((int)item_count);
                for (size_t i = 0; i < item_count; i++) {
                    if (specific_items[i]) {
                        Value item_name = value_create_string(specific_items[i]);
                        value_array_push(&items_array, item_name);
                        value_free(&item_name);
                    }
                }
                specific_items_idx = bc_add_const(p, items_array);
                // Note: bc_add_const now handles cloning and freeing arrays
                
                // Store specific_aliases array if present (right after items array)
                // VM will check if constant at specific_items_idx + 1 is an aliases array
                if (specific_aliases && item_count > 0) {
                    Value aliases_array = value_create_array((int)item_count);
                    for (size_t i = 0; i < item_count; i++) {
                        if (specific_aliases[i]) {
                            Value alias_name = value_create_string(specific_aliases[i]);
                            value_array_push(&aliases_array, alias_name);
                            value_free(&alias_name);
                        } else {
                            // No alias for this item - push null
                            value_array_push(&aliases_array, value_create_null());
                        }
                    }
                    bc_add_const(p, aliases_array); // Store right after items array
                    // Note: bc_add_const now handles cloning and freeing arrays
                }
            }
            
            // Emit instruction: a = library_name, b = alias, c = specific_items array index
            bc_emit(p, BC_IMPORT_LIB, lib_name_idx, alias_idx);
            // Update the c operand to store specific_items index
            if (p->count > 0) {
                p->code[p->count - 1].c = specific_items_idx;
            }
        } break;
        
        case AST_NODE_FUNCTION_CALL: {
            // For print statements, compile directly
            if (strcmp(n->data.function_call.function_name, "print") == 0) {
                // Compile all arguments first
                for (size_t i = 0; i < n->data.function_call.argument_count; i++) {
                    compile_node(p, n->data.function_call.arguments[i]);
                }
                // Then print all arguments on one line
                bc_emit(p, BC_PRINT_MULTIPLE, (int)n->data.function_call.argument_count, 0);
            } else if (is_builtin_function(n->data.function_call.function_name)) {
                // Built-in function - use BC_CALL_BUILTIN
                for (size_t i = 0; i < n->data.function_call.argument_count; i++) {
                    compile_node(p, n->data.function_call.arguments[i]);
                }
                int name_idx = bc_add_const(p, value_create_string(n->data.function_call.function_name));
                bc_emit(p, BC_CALL_BUILTIN, name_idx, (int)n->data.function_call.argument_count);
            } else if (strchr(n->data.function_call.function_name, '.') != NULL) {
                // This is a method call like math.abs(-5)
                // Parse the object and method name
                char* dot_pos = strchr(n->data.function_call.function_name, '.');
                size_t obj_len = dot_pos - n->data.function_call.function_name;
                size_t method_len = strlen(n->data.function_call.function_name) - obj_len - 1;
                
                char* obj_name = shared_malloc_safe(obj_len + 1, "bytecode_compiler", "AST_NODE_FUNCTION_CALL", 0);
                char* method_name = shared_malloc_safe(method_len + 1, "bytecode_compiler", "AST_NODE_FUNCTION_CALL", 1);
                
                if (obj_name && method_name) {
                    strncpy(obj_name, n->data.function_call.function_name, obj_len);
                    obj_name[obj_len] = '\0';
                    strcpy(method_name, dot_pos + 1);
                    
                    // Compile the object (identifier)
                    ASTNode obj_node = {0};
                    obj_node.type = AST_NODE_IDENTIFIER;
                    obj_node.data.identifier_value = obj_name;
                    compile_node(p, &obj_node);
                    
                    // Check for common array methods that modify in place
                    if (strcmp(method_name, "push") == 0 && n->data.function_call.argument_count == 1) {
                        // For arr.push(val), we need to:
                        // 1. Load arr onto stack
                        // 2. Load val onto stack
                        // 3. Call BC_ARRAY_PUSH (modifies arr and returns it)
                        // 4. Store the result back to arr variable
                        // Compile the argument
                        compile_node(p, n->data.function_call.arguments[0]);
                        // Emit array push instruction (array should already be on stack from compile_node above)
                        bc_emit(p, BC_ARRAY_PUSH, 0, 0);
                        // Now store the result back to the variable
                        // The array is already on the stack, so we need to assign it back
                        // Check if variable is local or global
                        int local_idx = lookup_local(p, obj_name);
                        if (local_idx >= 0) {
                            // Variable is local - use BC_STORE_LOCAL
                            bc_emit(p, BC_STORE_LOCAL, local_idx, 0);
                        } else {
                            // Variable is global - use BC_STORE_GLOBAL
                            int var_name_idx = bc_add_const(p, value_create_string(obj_name));
                            bc_emit(p, BC_STORE_GLOBAL, var_name_idx, 0);
                        }
                    } else if (strcmp(method_name, "pop") == 0 && n->data.function_call.argument_count == 0) {
                        // For arr.pop(), similar handling
                        // Emit array pop instruction
                        bc_emit(p, BC_ARRAY_POP, 0, 0);
                        // Store result back to variable
                        int var_name_idx = bc_add_const(p, value_create_string(obj_name));
                        bc_emit(p, BC_STORE_GLOBAL, var_name_idx, 0);
                    } else {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call.argument_count; i++) {
                            compile_node(p, n->data.function_call.arguments[i]);
                        }
                        
                        // Add method name to constants
                        int method_name_idx = bc_add_const(p, value_create_string(method_name));
                        
                        // Emit method call instruction
                        bc_emit(p, BC_METHOD_CALL, method_name_idx, (int)n->data.function_call.argument_count);
                    }
                }
                
                if (obj_name) shared_free_safe(obj_name, "bytecode_compiler", "AST_NODE_FUNCTION_CALL", 2);
                if (method_name) shared_free_safe(method_name, "bytecode_compiler", "AST_NODE_FUNCTION_CALL", 3);
            } else {
                // Check if this is a class instantiation (at compile time)
                // If class exists in environment, use BC_INSTANTIATE_CLASS
                // Otherwise, compile as a function call and let runtime decide
                int is_class_instantiation = 0;
                if (p->interpreter && environment_exists(p->interpreter->global_environment, n->data.function_call.function_name)) {
                    Value class_val = environment_get(p->interpreter->global_environment, n->data.function_call.function_name);
                    if (class_val.type == VALUE_CLASS) {
                        is_class_instantiation = 1;
                    }
                    value_free(&class_val);
                }
                
                if (is_class_instantiation) {
                    // This is a class instantiation - compile arguments and emit BC_INSTANTIATE_CLASS
                    for (size_t i = 0; i < n->data.function_call.argument_count; i++) {
                        compile_node(p, n->data.function_call.arguments[i]);
                    }
                    int name_idx = bc_add_const(p, value_create_string(n->data.function_call.function_name));
                    bc_emit(p, BC_INSTANTIATE_CLASS, name_idx, (int)n->data.function_call.argument_count);
                } else {
                    // Could be a function call or a class instantiation (forward reference)
                    // Compile as function call - if it's actually a class, runtime will handle it
                    // For now, compile as a function call and let value_function_call handle it
                    // TODO: Add runtime class detection in BC_CALL_FUNCTION_VALUE
                    // Look up existing function and call it
                    int func_id = -1;
                    
                    // First check in bytecode program's function list
                    for (size_t i = 0; i < p->function_count; i++) {
                        if (p->functions[i].name) {
                            if (strcmp(p->functions[i].name, n->data.function_call.function_name) == 0) {
                                func_id = (int)i;
                                break;
                            }
                        }
                    }
                    
                    // If not found in bytecode, try loading from environment
                    // This handles function variables and recursive calls
                    if (func_id < 0) {
                        // Compile arguments first
                        for (size_t i = 0; i < n->data.function_call.argument_count; i++) {
                            compile_node(p, n->data.function_call.arguments[i]);
                        }
                        // Check if it's a local variable first
                        int local_idx = lookup_local(p, n->data.function_call.function_name);
                        if (local_idx >= 0) {
                            // Load from local storage
                            bc_emit(p, BC_LOAD_LOCAL, local_idx, 0);
                        } else {
                            // Load function from global environment
                            int name_idx = bc_add_const(p, value_create_string(n->data.function_call.function_name));
                            bc_emit(p, BC_LOAD_GLOBAL, name_idx, 0);
                        }
                        // Call the function value
                        bc_emit(p, BC_CALL_FUNCTION_VALUE, (int)n->data.function_call.argument_count, 0);
                    } else {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call.argument_count; i++) {
                            compile_node(p, n->data.function_call.arguments[i]);
                        }
                        // Emit user function call
                        bc_emit(p, BC_CALL_USER_FUNCTION, func_id, (int)n->data.function_call.argument_count);
                    }
                }
            }
        } break;
        case AST_NODE_SPORE: {
            // Pattern matching: match expr case pattern1: body1 case pattern2: body2 else: default_body end
            // We don't compile the expression here - each case will compile it when needed
            // This avoids stack management issues
            
            // For each case, we'll:
            // 1. Duplicate the match value
            // 2. Compile the pattern check
            // 3. Jump if pattern doesn't match
            // 4. Execute body if pattern matches
            // 5. Jump to end
            
            size_t end_jump_positions_count = 0;
            int* end_jump_positions = NULL;
            
            // Process each case
            for (size_t i = 0; i < n->data.spore.case_count; i++) {
                ASTNode* case_node = n->data.spore.cases[i];
                if (case_node && case_node->type == AST_NODE_SPORE_CASE) {
                    // Duplicate the match value for pattern checking
                    // We'll keep the original on the stack and use the duplicate for pattern matching
                    Value dup_val = value_create_null(); // Placeholder - we'll use stack manipulation
                    
                    // Compile pattern matching
                    // For simple patterns (string, number, Null, bool), we can use equality
                    ASTNode* pattern = case_node->data.spore_case.pattern;
                    if (pattern) {
                        // Stack: [match_value]
                        // We need to duplicate match_value before comparing, since BC_EQ consumes both operands
                        // For now, we'll reload the expression (simple but not optimal)
                        // TODO: Add BC_DUP instruction for better performance
                        
                        if (pattern->type == AST_NODE_STRING) {
                            // Match string literal: reload match_value, load pattern string, compare
                            // Reload match_value by recompiling expression (simple approach)
                            compile_node(p, n->data.spore.expression);
                            int pattern_str_idx = bc_add_const(p, value_create_string(pattern->data.string_value));
                            bc_emit(p, BC_LOAD_CONST, pattern_str_idx, 0); // Load pattern string
                            bc_emit(p, BC_EQ, 0, 0); // Compare match_value == pattern
                            // Stack: [match_value, bool_result]
                        } else if (pattern->type == AST_NODE_NULL) {
                            // Match Null: check if value is Null
                            // BC_IS_NULL consumes the value and pushes a boolean
                            // But we need to check the match_value, so we need to reload it
                            compile_node(p, n->data.spore.expression);
                            bc_emit(p, BC_IS_NULL, 0, 0);
                            // Stack: [bool_result]
                        } else if (pattern->type == AST_NODE_NUMBER) {
                            // Match number literal
                            compile_node(p, n->data.spore.expression);
                            int pattern_num_idx = bc_add_const(p, value_create_number(pattern->data.number_value));
                            bc_emit(p, BC_LOAD_CONST, pattern_num_idx, 0);
                            bc_emit(p, BC_EQ, 0, 0);
                            // Stack: [match_value, bool_result]
                        } else if (pattern->type == AST_NODE_BOOL) {
                            // Match boolean literal
                            compile_node(p, n->data.spore.expression);
                            int pattern_bool_idx = bc_add_const(p, value_create_boolean(pattern->data.bool_value));
                            bc_emit(p, BC_LOAD_CONST, pattern_bool_idx, 0);
                            bc_emit(p, BC_EQ, 0, 0);
                            // Stack: [match_value, bool_result]
                        } else if (pattern->type == AST_NODE_IDENTIFIER && 
                                   strcmp(pattern->data.identifier_value, "_") == 0) {
                            // Wildcard pattern - always matches
                            bc_emit(p, BC_LOAD_CONST, bc_add_const(p, value_create_boolean(true)), 0);
                            // Stack: [match_value, bool_result]
                        } else {
                            // Complex pattern - not yet supported
                            if (p->interpreter) {
                                interpreter_set_error(p->interpreter, "Complex pattern matching not yet supported in bytecode", 0, 0);
                            }
                            // Pop the match value and return null
                            bc_emit(p, BC_POP, 0, 0);
                            bc_emit(p, BC_LOAD_CONST, bc_add_const(p, value_create_null()), 0);
                            return;
                        }
                        
                        // Check if pattern matched (bool_result is on top)
                        int jump_if_false_pos = (int)p->count;
                        bc_emit(p, BC_JUMP_IF_FALSE, 0, 0); // Will jump to next case if false
                        
                        // Pattern matched - pop the bool result
                        bc_emit(p, BC_POP, 0, 0); // Pop bool result
                        
                        // Note: The match_value was already consumed:
                        // - For BC_EQ: both operands are consumed
                        // - For BC_IS_NULL: the value is consumed
                        // - For wildcard: we never loaded the value
                        // So we don't need to pop match_value - stack is clean
                        
                        // Execute case body
                        if (case_node->data.spore_case.body) {
                            compile_node(p, case_node->data.spore_case.body);
                            // Body might be a BLOCK which doesn't leave a value on stack
                            // If body is a BLOCK, it compiles statements but doesn't push a value
                            // For match statements, we don't need a return value - the body just executes
                            // So we don't need to worry about the stack value here
                        } else {
                            // No body - push null (though this shouldn't happen)
                            bc_emit(p, BC_LOAD_CONST, bc_add_const(p, value_create_null()), 0);
                        }
                        
                        // Jump to end of match statement
                        int end_jump_pos = (int)p->count;
                        bc_emit(p, BC_JUMP, 0, 0); // Placeholder, will be patched
                        
                        // Store jump position for patching
                        if (!end_jump_positions) {
                            end_jump_positions = shared_malloc_safe(n->data.spore.case_count * sizeof(int), "bytecode", "AST_NODE_SPORE", 0);
                        }
                        if (end_jump_positions) {
                            end_jump_positions[end_jump_positions_count++] = end_jump_pos;
                        }
                        
                        // Patch the jump_if_false to point here (next case or else)
                        int next_case_pos = (int)p->count;
                        p->code[jump_if_false_pos].a = next_case_pos;
                    }
                }
            }
            
            // Handle else/default case (root_case)
            // If we reach here, no pattern matched
            // The match_value was consumed by all pattern checks, so stack should be clean
            
            if (n->data.spore.root_case) {
                // Execute else body
                compile_node(p, n->data.spore.root_case);
                // Body might be a BLOCK which doesn't leave a value on stack
            } else {
                // No else case - match statement doesn't return a value, so we don't need to push anything
                // The match statement is used for side effects (assignments), not return values
            }
            
            // Patch all end jumps to point here
            int end_pos = (int)p->count;
            if (end_jump_positions) {
                for (size_t i = 0; i < end_jump_positions_count; i++) {
                    p->code[end_jump_positions[i]].a = end_pos;
                }
                shared_free_safe(end_jump_positions, "bytecode", "AST_NODE_SPORE", 1);
            }
            
            // Match statement doesn't return a value - it's used for side effects
            // The stack should be clean (all case bodies were executed as statements)
        } break;
        
        case AST_NODE_CLASS: {
            // Compile class definition
            // Create class with name and parent
            int name_idx = bc_add_const(p, value_create_string(n->data.class_definition.class_name));
            int parent_idx = n->data.class_definition.parent_class ? 
                bc_add_const(p, value_create_string(n->data.class_definition.parent_class)) : -1;
            
            // Store class body AST for later processing during instantiation
            int body_idx = bc_add_ast(p, n->data.class_definition.body);
            
            bc_emit_super(p, BC_CREATE_CLASS, name_idx, parent_idx, body_idx);
            
            // If class has export/private flags, store metadata
            if (n->data.class_definition.is_export || n->data.class_definition.is_private) {
                int flags = 0;
                if (n->data.class_definition.is_export) {
                    flags |= 1; // bit 0 = export
                }
                if (n->data.class_definition.is_private) {
                    flags |= 2; // bit 1 = private
                }
                bc_emit(p, BC_SET_SYMBOL_FLAGS, name_idx, flags);
            }
            // Don't execute class body here - it will be processed during instantiation
        } break;
        
        case AST_NODE_WHILE_LOOP: {
            // Compile while loop
            int loop_start = p->count;
            bc_emit(p, BC_LOOP_START, 0, 0);
            
            // Compile condition
            compile_node(p, n->data.while_loop.condition);
            
            // Jump if false (exit loop)
            int jump_to_end = p->count;
            bc_emit(p, BC_JUMP_IF_FALSE, 0, 0); // Will be filled later
            
            // Compile body
            compile_node(p, n->data.while_loop.body);
            
            // Jump back to condition
            bc_emit(p, BC_JUMP, loop_start, 0);
            
            // Update jump target
            p->code[jump_to_end].a = p->count;
            
            bc_emit(p, BC_LOOP_END, 0, 0);
        } break;
        
        case AST_NODE_FOR_LOOP: {
            // For loop: for iterator_name in collection body
            // Compile the collection expression (should be array or range)
            compile_node(p, n->data.for_loop.collection);
            
            // Compile body to bytecode instead of storing AST
            int iterator_name_idx = bc_add_const(p, value_create_string(n->data.for_loop.iterator_name));
            int body_func_id = -1;
            if (n->data.for_loop.body) {
                body_func_id = bc_compile_ast_to_subprogram(p, n->data.for_loop.body, "<for_loop_body>");
            }
            
            // Emit BC_FOR_LOOP instruction
            // instr->a = iterator name constant index
            // instr->b = body function ID (bytecode sub-program)
            // Collection is on stack
            bc_emit(p, BC_FOR_LOOP, iterator_name_idx, body_func_id);
        } break;
        
        case AST_NODE_ARRAY_ACCESS: {
            // Array access: arr[index]
            // Compile array and index
            compile_node(p, n->data.array_access.array);
            compile_node(p, n->data.array_access.index);
            // Emit array access instruction
            bc_emit(p, BC_ARRAY_GET, 0, 0);
        } break;
        
        case AST_NODE_SWITCH: {
            // Switch statement: switch expression { cases }
            // Compile the expression
            compile_node(p, n->data.switch_statement.expression);
            
            // Store the expression value temporarily and compare with each case
            // For each case, we'll compare and jump if not equal
            // We'll use a jump table approach: compare expression with each case value
            
            // Compile all cases
            for (size_t i = 0; i < n->data.switch_statement.case_count; i++) {
                ASTNode* case_node = n->data.switch_statement.cases[i];
                if (!case_node) continue;
                
                // For switch cases, we need to compare the expression with the case value
                // We'll duplicate the expression, compile the case value, compare, and jump if not equal
                // Store the case value and body AST indices
                int case_value_idx = -1;
                int case_body_idx = -1;
                
                // Case node structure: typically has a value/pattern and a body
                // Compile case value and body to bytecode
                if (case_node->type == AST_NODE_SPORE_CASE) {
                    // Spore case: pattern and body
                    // Compile pattern to bytecode sub-program
                    if (case_node->data.spore_case.pattern) {
                        case_value_idx = bc_compile_ast_to_subprogram(p, case_node->data.spore_case.pattern, "<switch_case_value>");
                    } else {
                        case_value_idx = -1;
                    }
                    // Compile body to bytecode
                    if (case_node->data.spore_case.body) {
                        case_body_idx = bc_compile_ast_to_subprogram(p, case_node->data.spore_case.body, "<switch_case_body>");
                    } else {
                        case_body_idx = -1;
                    }
                } else {
                    // Regular case: compile value and body to bytecode
                    // Compile case value expression to bytecode sub-program
                    case_value_idx = bc_compile_ast_to_subprogram(p, case_node, "<switch_case_value>");
                    // For case body, compile to bytecode
                    // Note: case_node might be the body itself, so we need to check structure
                    // For now, assume case_node has a body field or is the body
                    case_body_idx = bc_compile_ast_to_subprogram(p, case_node, "<switch_case_body>");
                }
                
                // Emit switch case instruction
                // instr->a = case value function ID (bytecode sub-program)
                // instr->b = case body function ID (bytecode sub-program)
                bc_emit(p, BC_SWITCH_CASE, case_value_idx, case_body_idx);
            }
            
            // Compile default case if exists
            if (n->data.switch_statement.default_case) {
                int default_body_func_id = bc_compile_ast_to_subprogram(p, n->data.switch_statement.default_case, "<switch_default_body>");
                bc_emit(p, BC_SWITCH_DEFAULT, default_body_func_id, 0);
            }
            
            // Emit switch end (marks end of switch statement)
            bc_emit(p, BC_SWITCH, 0, 0);
        } break;
        
        case AST_NODE_MATCH: {
            // Match expression: match expr pattern1 pattern2 ...
            // NOTE: The parser currently creates AST_NODE_SPORE for match statements, not AST_NODE_MATCH
            // This AST node type may not be used by the parser yet
            // For now, treat it similarly to AST_NODE_SPORE but with patterns instead of cases
            // If patterns array is null or empty, this is likely an error or unused node type
            
            if (!n->data.match.expression) {
                // Invalid match expression - push null
                bc_emit(p, BC_LOAD_CONST, bc_add_const(p, value_create_null()), 0);
                break;
            }
            
            if (n->data.match.pattern_count == 0 || !n->data.match.patterns) {
                // No patterns - evaluate expression and return it
                compile_node(p, n->data.match.expression);
                break;
            }
            
            // Evaluates expression, tries each pattern in order, returns first matching pattern's value
            // Compile match expression to bytecode sub-program
            int match_expr_func_id = -1;
            if (n->data.match.expression) {
                match_expr_func_id = bc_compile_ast_to_subprogram(p, n->data.match.expression, "<match_expression>");
            }
            
            // We'll use a sequential approach: try each pattern until one matches
            for (size_t i = 0; i < n->data.match.pattern_count; i++) {
                ASTNode* pattern = n->data.match.patterns[i];
                if (!pattern) continue;
                
                // Compile pattern to bytecode sub-program
                int pattern_func_id = bc_compile_ast_to_subprogram(p, pattern, "<match_pattern>");
                
                // Emit match pattern instruction
                // instr->a = pattern function ID (bytecode sub-program)
                // instr->b = match expression function ID (bytecode sub-program)
                bc_emit(p, BC_MATCH_PATTERN, pattern_func_id, match_expr_func_id);
            }
            
            // Emit match end instruction to clean up and return result
            bc_emit(p, BC_MATCH_END, 0, 0);
        } break;
        
        case AST_NODE_LAMBDA: {
            // Lambda function: (params) => body or (params) -> body
            // Similar to function definition, but creates an anonymous function value
            // Add lambda to function table (like regular functions) for bytecode execution
            int lambda_id = bc_add_function(p, n);
            
            // Store lambda AST node (for parameter extraction) and body AST
            int lambda_node_idx = bc_add_ast(p, n);
            int lambda_body_idx = bc_add_ast(p, n->data.lambda.body);
            
            // Emit BC_CREATE_LAMBDA instruction
            // instr->a = lambda body AST index (for body execution)
            // instr->b = function ID (for bytecode execution)
            // We'll search for the lambda node by body to extract parameters
            bc_emit(p, BC_CREATE_LAMBDA, lambda_body_idx, lambda_id);
        } break;
        
        case AST_NODE_THROW: {
            // Throw statement: throw expression
            if (n->data.throw_statement.value) {
                compile_node(p, n->data.throw_statement.value);
            } else {
                bc_emit(p, BC_LOAD_CONST, bc_add_const(p, value_create_null()), 0);
            }
            bc_emit(p, BC_THROW, 0, 0);
        } break;
        
        case AST_NODE_TRY_CATCH: {
            // Try-catch: try { ... } catch err { ... } finally { ... }
            int try_start_pos = (int)p->count;
            bc_emit(p, BC_TRY_START, 0, 0);
            
            // Compile try block
            if (n->data.try_catch.try_block) {
                compile_node(p, n->data.try_catch.try_block);
            }
            
            int try_end_pos = (int)p->count;
            bc_emit(p, BC_TRY_END, 0, 0);
            
            // Compile catch block (if exists)
            if (n->data.try_catch.catch_block) {
                // Store catch variable name and compile catch block to bytecode
                // Use empty string for catch_var_idx if no catch variable
                int catch_var_idx = bc_add_const(p, value_create_string(
                    (n->data.try_catch.catch_variable && strlen(n->data.try_catch.catch_variable) > 0) 
                        ? n->data.try_catch.catch_variable 
                        : ""));
                int catch_block_func_id = bc_compile_ast_to_subprogram(p, n->data.try_catch.catch_block, "<catch_block>");
                
                // Emit catch instruction
                // instr->a = catch variable name constant index (empty string means no variable)
                // instr->b = catch block function ID (bytecode sub-program)
                bc_emit(p, BC_CATCH, catch_var_idx, catch_block_func_id);
            }
            
            // Compile finally block (if exists) - for now, just compile it after catch
            // TODO: Properly handle finally blocks (they should always execute)
            if (n->data.try_catch.finally_block) {
                compile_node(p, n->data.try_catch.finally_block);
            }
        } break;
        
        case AST_NODE_ERROR: {
            // Error node - compilation should have failed earlier, but handle gracefully
            // Push null result and continue
            bc_emit(p, BC_LOAD_CONST, bc_add_const(p, value_create_null()), 0);
        } break;
        
        default: {
            // Unknown AST node type - compilation error
            // Skip known unsupported nodes that are expected in certain contexts
            if (n->type == AST_NODE_PATTERN_NOT || n->type == AST_NODE_PATTERN_TYPE || 
                n->type == AST_NODE_PATTERN_WILDCARD || n->type == AST_NODE_PATTERN_DESTRUCTURE || 
                n->type == AST_NODE_PATTERN_GUARD || n->type == AST_NODE_PATTERN_OR || 
                n->type == AST_NODE_PATTERN_AND || n->type == AST_NODE_PATTERN_RANGE || 
                n->type == AST_NODE_PATTERN_REGEX) {
                // Pattern nodes are expected in match/spore contexts - silently skip
                break;
            }
            if (p->interpreter) {
                char error_msg[256];
                snprintf(error_msg, sizeof(error_msg), "AST node type %d not supported in bytecode compilation", n->type);
                interpreter_set_error(p->interpreter, error_msg, 0, 0);
            }
            // Don't emit BC_EVAL_AST - compilation fails
        } break;
    }
}

int bytecode_compile_program(BytecodeProgram* program, ASTNode* root, Interpreter* interpreter) {
    if (!program || !root) return 0;
    
    // Initialize compiler optimizations
    init_compiler_optimizations();
    
    // Store interpreter reference for global variable access
    program->interpreter = interpreter;
    
    compile_node(program, root);
    bc_emit(program, BC_HALT, 0, 0);
    
    // Apply optimizations
    apply_compiler_optimizations(program);
    
    // Cleanup optimizations
    cleanup_compiler_optimizations();
    
    return 1;
}


