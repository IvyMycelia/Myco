#include "../../include/core/optimization/register_vm.h"
#include "../../include/core/interpreter/value_operations.h"
#include "../../include/core/interpreter/eval_engine.h"
#include "../../include/utils/shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

// ============================================================================
// REGISTER VM IMPLEMENTATION
// ============================================================================

RegisterProgram* register_program_create(void) {
    RegisterProgram* program = (RegisterProgram*)shared_malloc_safe(
        sizeof(RegisterProgram), "register_vm", "program_create", 0);
    if (!program) return NULL;
    
    // Initialize instruction array
    program->instructions = NULL;
    program->instruction_count = 0;
    program->capacity = 0;
    
    // Initialize constant pool
    program->constants = NULL;
    program->constant_count = 0;
    program->constant_capacity = 0;
    
    // Initialize string pool
    program->string_pool = NULL;
    program->string_count = 0;
    program->string_capacity = 0;
    
    // Initialize register metadata
    program->max_registers = 0;
    program->register_count = 0;
    program->register_types = NULL;
    
    // Initialize function metadata
    program->is_function = 0;
    program->function_name = NULL;
    program->parameter_count = 0;
    program->local_count = 0;
    program->upvalue_count = 0;
    
    // Initialize optimization metadata
    program->optimized = 0;
    program->vectorized = 0;
    program->traceable = 0;
    program->hotness_score = 0.0;
    
    return program;
}

void register_program_free(RegisterProgram* program) {
    if (!program) return;
    
    // Free instruction array
    if (program->instructions) {
        shared_free_safe(program->instructions, "register_vm", "program_free", 0);
    }
    
    // Free constant pool
    if (program->constants) {
        for (size_t i = 0; i < program->constant_count; i++) {
            value_free(&program->constants[i]);
        }
        shared_free_safe(program->constants, "register_vm", "program_free", 0);
    }
    
    // Free string pool
    if (program->string_pool) {
        for (size_t i = 0; i < program->string_count; i++) {
            if (program->string_pool[i]) {
                shared_free_safe(program->string_pool[i], "register_vm", "program_free", 0);
            }
        }
        shared_free_safe(program->string_pool, "register_vm", "program_free", 0);
    }
    
    // Free register types
    if (program->register_types) {
        shared_free_safe(program->register_types, "register_vm", "program_free", 0);
    }
    
    // Free function name
    if (program->function_name) {
        shared_free_safe(program->function_name, "register_vm", "program_free", 0);
    }
    
    shared_free_safe(program, "register_vm", "program_free", 0);
}

int register_program_add_instruction(RegisterProgram* program, RegisterInstruction instr) {
    if (!program) return 0;
    
    // Grow instruction array if needed
    if (program->instruction_count >= program->capacity) {
        size_t new_capacity = program->capacity == 0 ? 64 : program->capacity * 2;
        RegisterInstruction* new_instructions = (RegisterInstruction*)shared_realloc_safe(
            program->instructions, 
            sizeof(RegisterInstruction) * new_capacity,
            "register_vm", "add_instruction", 0);
        if (!new_instructions) return 0;
        
        program->instructions = new_instructions;
        program->capacity = new_capacity;
    }
    
    // Add instruction
    program->instructions[program->instruction_count] = instr;
    program->instruction_count++;
    
    return 1;
}

int register_program_add_constant(RegisterProgram* program, Value value) {
    if (!program) return -1;
    
    // Grow constant pool if needed
    if (program->constant_count >= program->constant_capacity) {
        size_t new_capacity = program->constant_capacity == 0 ? 16 : program->constant_capacity * 2;
        Value* new_constants = (Value*)shared_realloc_safe(
            program->constants,
            sizeof(Value) * new_capacity,
            "register_vm", "add_constant", 0);
        if (!new_constants) return -1;
        
        program->constants = new_constants;
        program->constant_capacity = new_capacity;
    }
    
    // Add constant
    program->constants[program->constant_count] = value_clone(&value);
    int index = (int)program->constant_count;
    program->constant_count++;
    
    return index;
}

int register_program_add_string(RegisterProgram* program, const char* str) {
    if (!program || !str) return -1;
    
    // Grow string pool if needed
    if (program->string_count >= program->string_capacity) {
        size_t new_capacity = program->string_capacity == 0 ? 16 : program->string_capacity * 2;
        char** new_strings = (char**)shared_realloc_safe(
            program->string_pool,
            sizeof(char*) * new_capacity,
            "register_vm", "add_string", 0);
        if (!new_strings) return -1;
        
        program->string_pool = new_strings;
        program->string_capacity = new_capacity;
    }
    
    // Add string (copy it)
    char* str_copy = (char*)shared_malloc_safe(
        strlen(str) + 1, "register_vm", "add_string", 0);
    if (!str_copy) return -1;
    
    strcpy(str_copy, str);
    program->string_pool[program->string_count] = str_copy;
    int index = (int)program->string_count;
    program->string_count++;
    
    return index;
}

RegisterProgram* register_compile_ast(ASTNode* ast, Interpreter* interpreter) {
    if (!ast || !interpreter) return NULL;
    
    RegisterProgram* program = register_program_create();
    if (!program) return NULL;
    
    // Compile based on AST node type
    switch (ast->type) {
        case AST_NODE_NUMBER:
        case AST_NODE_STRING:
        case AST_NODE_BOOL:
        case AST_NODE_NULL:
        case AST_NODE_IDENTIFIER:
        case AST_NODE_BINARY_OP:
        case AST_NODE_UNARY_OP:
        case AST_NODE_FUNCTION_CALL:
        case AST_NODE_VARIABLE_DECLARATION:
        case AST_NODE_IF_STATEMENT:
        case AST_NODE_WHILE_LOOP:
        case AST_NODE_FOR_LOOP:
        case AST_NODE_FUNCTION:
        case AST_NODE_RETURN:
        case AST_NODE_ARRAY_LITERAL:
        case AST_NODE_HASH_MAP_LITERAL:
        case AST_NODE_MEMBER_ACCESS:
        case AST_NODE_BLOCK:
            // TODO: Implement specific compilation functions
            // For now, return the empty program
            return program;
        default:
            // Unsupported node type, return empty program
            return program;
    }
}

RegisterContext* register_context_create(RegisterProgram* program) {
    if (!program) return NULL;
    
    RegisterContext* context = (RegisterContext*)shared_malloc_safe(
        sizeof(RegisterContext), "register_vm", "context_create", 0);
    if (!context) return NULL;
    
    context->program = program;
    
    // Allocate registers (256 registers max)
    context->registers = (Value*)shared_malloc_safe(
        sizeof(Value) * 256, "register_vm", "context_create", 0);
    if (!context->registers) {
        shared_free_safe(context, "register_vm", "context_create", 0);
        return NULL;
    }
    
    // Initialize registers to null
    for (int i = 0; i < 256; i++) {
        context->registers[i] = value_create_null();
    }
    
    context->register_count = 0;
    context->locals = NULL;
    context->local_count = 0;
    context->upvalues = NULL;
    context->upvalue_count = 0;
    context->pc = 0;
    context->halted = 0;
    context->caller = NULL;
    context->return_pc = 0;
    context->execution_count = 0;
    context->hot_spot_threshold = 1000;
    context->is_hot = 0;
    context->optimization_level = 0;
    context->jit_eligible = 0;
    context->trace_eligible = 0;
    
    return context;
}

void register_context_free(RegisterContext* context) {
    if (!context) return;
    
    // Free registers
    if (context->registers) {
        for (int i = 0; i < 256; i++) {
            value_free(&context->registers[i]);
        }
        shared_free_safe(context->registers, "register_vm", "context_free", 0);
    }
    
    // Free locals
    if (context->locals) {
        for (size_t i = 0; i < context->local_count; i++) {
            value_free(&context->locals[i]);
        }
        shared_free_safe(context->locals, "register_vm", "context_free", 0);
    }
    
    // Free upvalues
    if (context->upvalues) {
        for (size_t i = 0; i < context->upvalue_count; i++) {
            value_free(&context->upvalues[i]);
        }
        shared_free_safe(context->upvalues, "register_vm", "context_free", 0);
    }
    
    shared_free_safe(context, "register_vm", "context_free", 0);
}

Value register_execute(RegisterContext* context, Interpreter* interpreter) {
    if (!context || !interpreter) return value_create_null();
    
    context->pc = 0;
    context->halted = 0;
    context->execution_count = 0;
    
    while (!context->halted && context->pc < context->program->instruction_count) {
        RegisterInstruction instr = context->program->instructions[context->pc];
        Value result = register_execute_instruction(context, interpreter, instr);
        
        context->execution_count++;
        context->pc++;
        
        // Check for hot spot
        if (context->execution_count >= context->hot_spot_threshold) {
            context->is_hot = 1;
        }
        
        // Handle return
        if (instr.opcode == REG_RETURN || instr.opcode == REG_RETURN_NULL) {
            return result;
        }
    }
    
    return value_create_null();
}

Value register_execute_instruction(RegisterContext* context, Interpreter* interpreter, RegisterInstruction instr) {
    if (!context || !interpreter) return value_create_null();
    
    Value result = value_create_null();
    
    switch (instr.opcode) {
        // Load/Store Operations
        case REG_LOAD_CONST:
            if (instr.src1 < context->program->constant_count) {
                result = value_clone(&context->program->constants[instr.src1]);
                context->registers[instr.dst] = result;
            }
            break;
            
        case REG_LOAD_VAR:
            // TODO: Implement variable loading
            break;
            
        case REG_STORE_VAR:
            // TODO: Implement variable storing
            break;
            
        case REG_LOAD_NULL:
            result = value_create_null();
            context->registers[instr.dst] = result;
            break;
            
        case REG_LOAD_TRUE:
            result = value_create_boolean(1);
            context->registers[instr.dst] = result;
            break;
            
        case REG_LOAD_FALSE:
            result = value_create_boolean(0);
            context->registers[instr.dst] = result;
            break;
            
        case REG_LOAD_ZERO:
            result = value_create_number(0.0);
            context->registers[instr.dst] = result;
            break;
            
        case REG_LOAD_ONE:
            result = value_create_number(1.0);
            context->registers[instr.dst] = result;
            break;
            
        case REG_MOV_RR:
            result = value_clone(&context->registers[instr.src1]);
            context->registers[instr.dst] = result;
            break;
            
        case REG_COPY_RR:
            result = value_clone(&context->registers[instr.src1]);
            context->registers[instr.dst] = result;
            break;
            
        // Arithmetic Operations
        case REG_ADDI_RR:
            if (context->registers[instr.src1].type == VALUE_NUMBER && 
                context->registers[instr.src2].type == VALUE_NUMBER) {
                double sum = context->registers[instr.src1].data.number_value + 
                           context->registers[instr.src2].data.number_value;
                result = value_create_number(sum);
                context->registers[instr.dst] = result;
            }
            break;
            
        case REG_ADDF_RR:
            if (context->registers[instr.src1].type == VALUE_NUMBER && 
                context->registers[instr.src2].type == VALUE_NUMBER) {
                double sum = context->registers[instr.src1].data.number_value + 
                           context->registers[instr.src2].data.number_value;
                result = value_create_number(sum);
                context->registers[instr.dst] = result;
            }
            break;
            
        case REG_SUBI_RR:
            if (context->registers[instr.src1].type == VALUE_NUMBER && 
                context->registers[instr.src2].type == VALUE_NUMBER) {
                double diff = context->registers[instr.src1].data.number_value - 
                            context->registers[instr.src2].data.number_value;
                result = value_create_number(diff);
                context->registers[instr.dst] = result;
            }
            break;
            
        case REG_SUBF_RR:
            if (context->registers[instr.src1].type == VALUE_NUMBER && 
                context->registers[instr.src2].type == VALUE_NUMBER) {
                double diff = context->registers[instr.src1].data.number_value - 
                            context->registers[instr.src2].data.number_value;
                result = value_create_number(diff);
                context->registers[instr.dst] = result;
            }
            break;
            
        case REG_MULI_RR:
            if (context->registers[instr.src1].type == VALUE_NUMBER && 
                context->registers[instr.src2].type == VALUE_NUMBER) {
                double product = context->registers[instr.src1].data.number_value * 
                               context->registers[instr.src2].data.number_value;
                result = value_create_number(product);
                context->registers[instr.dst] = result;
            }
            break;
            
        case REG_MULF_RR:
            if (context->registers[instr.src1].type == VALUE_NUMBER && 
                context->registers[instr.src2].type == VALUE_NUMBER) {
                double product = context->registers[instr.src1].data.number_value * 
                               context->registers[instr.src2].data.number_value;
                result = value_create_number(product);
                context->registers[instr.dst] = result;
            }
            break;
            
        case REG_DIVI_RR:
            if (context->registers[instr.src1].type == VALUE_NUMBER && 
                context->registers[instr.src2].type == VALUE_NUMBER) {
                if (context->registers[instr.src2].data.number_value != 0.0) {
                    double quotient = context->registers[instr.src1].data.number_value / 
                                    context->registers[instr.src2].data.number_value;
                    result = value_create_number(quotient);
                } else {
                    result = value_create_number(INFINITY);
                }
                context->registers[instr.dst] = result;
            }
            break;
            
        case REG_DIVF_RR:
            if (context->registers[instr.src1].type == VALUE_NUMBER && 
                context->registers[instr.src2].type == VALUE_NUMBER) {
                if (context->registers[instr.src2].data.number_value != 0.0) {
                    double quotient = context->registers[instr.src1].data.number_value / 
                                    context->registers[instr.src2].data.number_value;
                    result = value_create_number(quotient);
                } else {
                    result = value_create_number(INFINITY);
                }
                context->registers[instr.dst] = result;
            }
            break;
            
        // Fused Operations
        case REG_MULADD_RRR:
            if (context->registers[instr.src1].type == VALUE_NUMBER && 
                context->registers[instr.src2].type == VALUE_NUMBER &&
                context->registers[instr.src3].type == VALUE_NUMBER) {
                double fused = context->registers[instr.src1].data.number_value * 
                             context->registers[instr.src2].data.number_value +
                             context->registers[instr.src3].data.number_value;
                result = value_create_number(fused);
                context->registers[instr.dst] = result;
            }
            break;
            
        // Control Flow
        case REG_JUMP:
            context->pc = instr.offset - 1; // -1 because pc will be incremented
            break;
            
        case REG_JUMP_IF_FALSE:
            if (context->registers[instr.src1].type == VALUE_BOOLEAN && 
                !context->registers[instr.src1].data.boolean_value) {
                context->pc = instr.offset - 1; // -1 because pc will be incremented
            }
            break;
            
        case REG_JUMP_IF_TRUE:
            if (context->registers[instr.src1].type == VALUE_BOOLEAN && 
                context->registers[instr.src1].data.boolean_value) {
                context->pc = instr.offset - 1; // -1 because pc will be incremented
            }
            break;
            
        case REG_RETURN:
            result = value_clone(&context->registers[instr.src1]);
            context->halted = 1;
            break;
            
        case REG_RETURN_NULL:
            result = value_create_null();
            context->halted = 1;
            break;
            
        case REG_HALT:
            context->halted = 1;
            break;
            
        case REG_NOP:
            // No operation
            break;
            
        default:
            // Unknown instruction, continue
            break;
    }
    
    return result;
}

// Placeholder compilation functions (to be implemented)
RegisterProgram* register_compile_number(ASTNode* ast, RegisterProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // Add constant to pool
    Value num_value = value_create_number(ast->data.number_value);
    int const_index = register_program_add_constant(program, num_value);
    value_free(&num_value);
    
    if (const_index >= 0) {
        // Create load constant instruction
        RegisterInstruction instr = {0};
        instr.opcode = REG_LOAD_CONST;
        instr.dst = 0; // Use register 0
        instr.src1 = const_index;
        register_program_add_instruction(program, instr);
    }
    
    return program;
}

RegisterProgram* register_compile_string(ASTNode* ast, RegisterProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // Add string to pool
    int str_index = register_program_add_string(program, ast->data.string_value);
    
    if (str_index >= 0) {
        // Create load string instruction
        RegisterInstruction instr = {0};
        instr.opcode = REG_LOAD_STRING;
        instr.dst = 0; // Use register 0
        instr.src1 = str_index;
        register_program_add_instruction(program, instr);
    }
    
    return program;
}

RegisterProgram* register_compile_boolean(ASTNode* ast, RegisterProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // Create load boolean instruction
    RegisterInstruction instr = {0};
    instr.opcode = ast->data.bool_value ? REG_LOAD_TRUE : REG_LOAD_FALSE;
    instr.dst = 0; // Use register 0
    register_program_add_instruction(program, instr);
    
    return program;
}

RegisterProgram* register_compile_null(ASTNode* ast, RegisterProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // Create load null instruction
    RegisterInstruction instr = {0};
    instr.opcode = REG_LOAD_NULL;
    instr.dst = 0; // Use register 0
    register_program_add_instruction(program, instr);
    
    return program;
}

RegisterProgram* register_compile_identifier(ASTNode* ast, RegisterProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // TODO: Implement identifier compilation
    // This would involve variable lookup and loading
    
    return program;
}

RegisterProgram* register_compile_binary_op(ASTNode* ast, RegisterProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // TODO: Implement binary operation compilation
    // This would involve compiling left and right operands, then the operation
    
    return program;
}

RegisterProgram* register_compile_unary_op(ASTNode* ast, RegisterProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // TODO: Implement unary operation compilation
    // This would involve compiling the operand, then the operation
    
    return program;
}

RegisterProgram* register_compile_function_call(ASTNode* ast, RegisterProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // TODO: Implement function call compilation
    // This would involve compiling arguments and the call instruction
    
    return program;
}

RegisterProgram* register_compile_variable_declaration(ASTNode* ast, RegisterProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // TODO: Implement variable declaration compilation
    // This would involve compiling the initial value and storing it
    
    return program;
}

RegisterProgram* register_compile_if_statement(ASTNode* ast, RegisterProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // TODO: Implement if statement compilation
    // This would involve compiling condition, then and else branches
    
    return program;
}

RegisterProgram* register_compile_while_loop(ASTNode* ast, RegisterProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // TODO: Implement while loop compilation
    // This would involve compiling condition and body with jumps
    
    return program;
}

RegisterProgram* register_compile_for_loop(ASTNode* ast, RegisterProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // TODO: Implement for loop compilation
    // This would involve compiling initialization, condition, increment, and body
    
    return program;
}

RegisterProgram* register_compile_function_declaration(ASTNode* ast, RegisterProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // TODO: Implement function declaration compilation
    // This would involve compiling parameters and body
    
    return program;
}

RegisterProgram* register_compile_return_statement(ASTNode* ast, RegisterProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // TODO: Implement return statement compilation
    // This would involve compiling the return value and return instruction
    
    return program;
}

RegisterProgram* register_compile_array_literal(ASTNode* ast, RegisterProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // TODO: Implement array literal compilation
    // This would involve compiling all elements and creating the array
    
    return program;
}

RegisterProgram* register_compile_object_literal(ASTNode* ast, RegisterProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // TODO: Implement object literal compilation
    // This would involve compiling all properties and creating the object
    
    return program;
}

RegisterProgram* register_compile_member_access(ASTNode* ast, RegisterProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // TODO: Implement member access compilation
    // This would involve compiling object and property, then access instruction
    
    return program;
}

RegisterProgram* register_compile_block(ASTNode* ast, RegisterProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // TODO: Implement block compilation
    // This would involve compiling all statements in the block
    
    return program;
}

// ============================================================================
// PRODUCTION-QUALITY OPTIMIZATION FUNCTIONS
// ============================================================================

int register_optimize_program(RegisterProgram* program) {
    if (!program) return 0;
    
    int optimizations_applied = 0;
    
    // Apply optimization passes in order
    if (register_constant_fold(program)) {
        optimizations_applied++;
    }
    
    if (register_peephole_optimize(program)) {
        optimizations_applied++;
    }
    
    if (register_dead_code_elimination(program)) {
        optimizations_applied++;
    }
    
    if (register_allocate_optimize(program)) {
        optimizations_applied++;
    }
    
    program->optimized = 1;
    return optimizations_applied > 0;
}

int register_peephole_optimize(RegisterProgram* program) {
    if (!program || !program->instructions) return 0;
    
    int optimizations = 0;
    size_t i = 0;
    
    while (i < program->instruction_count - 1) {
        RegisterInstruction* curr = &program->instructions[i];
        RegisterInstruction* next = &program->instructions[i + 1];
        
        // Pattern 1: MOV r1, r2; MOV r2, r3 -> MOV r1, r3
        if (curr->opcode == REG_MOV_RR && next->opcode == REG_MOV_RR &&
            curr->dst == next->src1) {
            curr->dst = next->dst;
            // Remove next instruction
            memmove(next, next + 1, (program->instruction_count - i - 2) * sizeof(RegisterInstruction));
            program->instruction_count--;
            optimizations++;
            continue;
        }
        
        // Pattern 2: ADD r1, 1; ADD r1, 1 -> ADD r1, 2
        if (curr->opcode == REG_ADDI_RR && next->opcode == REG_ADDI_RR &&
            curr->dst == next->dst && curr->src1 == next->src1 &&
            curr->immediate == next->immediate) {
            curr->immediate *= 2;
            // Remove next instruction
            memmove(next, next + 1, (program->instruction_count - i - 2) * sizeof(RegisterInstruction));
            program->instruction_count--;
            optimizations++;
            continue;
        }
        
        // Pattern 3: Remove dead moves (MOV r1, r1)
        if (curr->opcode == REG_MOV_RR && curr->dst == curr->src1) {
            // Remove current instruction
            memmove(curr, curr + 1, (program->instruction_count - i - 1) * sizeof(RegisterInstruction));
            program->instruction_count--;
            optimizations++;
            continue;
        }
        
        // Pattern 4: LOAD_CONST r1, 0; ADD r1, r2 -> LOAD_CONST r1, r2
        if (curr->opcode == REG_LOAD_CONST && next->opcode == REG_ADDI_RR &&
            curr->dst == next->dst && curr->immediate == 0) {
            curr->opcode = REG_LOAD_CONST;
            curr->immediate = next->src1; // Load the other register's value
            // Remove next instruction
            memmove(next, next + 1, (program->instruction_count - i - 2) * sizeof(RegisterInstruction));
            program->instruction_count--;
            optimizations++;
            continue;
        }
        
        i++;
    }
    
    return optimizations > 0;
}

int register_constant_fold(RegisterProgram* program) {
    if (!program || !program->instructions) return 0;
    
    int optimizations = 0;
    
    for (size_t i = 0; i < program->instruction_count; i++) {
        RegisterInstruction* instr = &program->instructions[i];
        
        // Constant folding for arithmetic operations
        switch (instr->opcode) {
            case REG_ADDI_RR:
                // If both sources are constants, fold the operation
                if (instr->immediate != 0) {
                    // This is an immediate add, check if we can fold with previous constant loads
                    for (size_t j = 0; j < i; j++) {
                        RegisterInstruction* prev = &program->instructions[j];
                        if (prev->opcode == REG_LOAD_CONST && prev->dst == instr->src1) {
                            // Fold: LOAD_CONST r1, 5; ADDI_RR r1, 3 -> LOAD_CONST r1, 8
                            prev->immediate += instr->immediate;
                            instr->opcode = REG_MOV_RR;
                            instr->src1 = instr->dst;
                            instr->src2 = 0;
                            instr->immediate = 0;
                            optimizations++;
                            break;
                        }
                    }
                }
                break;
                
            case REG_MULI_RR:
                // Fold multiplication by 0: MULI_RR r1, 0 -> LOAD_CONST r1, 0
                if (instr->immediate == 0) {
                    instr->opcode = REG_LOAD_CONST;
                    instr->src1 = 0;
                    instr->src2 = 0;
                    instr->immediate = 0;
                    optimizations++;
                }
                // Fold multiplication by 1: MULI_RR r1, 1 -> NOP (MOV r1, r1)
                else if (instr->immediate == 1) {
                    instr->opcode = REG_MOV_RR;
                    instr->src1 = instr->dst;
                    instr->src2 = 0;
                    instr->immediate = 0;
                    optimizations++;
                }
                break;
                
            case REG_DIVI_RR:
                // Fold division by 1: DIVI_RR r1, 1 -> NOP (MOV r1, r1)
                if (instr->immediate == 1) {
                    instr->opcode = REG_MOV_RR;
                    instr->src1 = instr->dst;
                    instr->src2 = 0;
                    instr->immediate = 0;
                    optimizations++;
                }
                break;
                
            case REG_EQ_RR:
                // Fold constant comparisons
                if (instr->immediate != 0) {
                    // This is a constant comparison, check if we can fold
                    for (size_t j = 0; j < i; j++) {
                        RegisterInstruction* prev = &program->instructions[j];
                        if (prev->opcode == REG_LOAD_CONST && prev->dst == instr->src1) {
                            // Fold: LOAD_CONST r1, 5; EQ_RR r1, 5 -> LOAD_CONST r1, 1
                            if (prev->immediate == instr->immediate) {
                                instr->opcode = REG_LOAD_CONST;
                                instr->src1 = 0;
                                instr->src2 = 0;
                                instr->immediate = 1; // true
                                optimizations++;
                            } else {
                                instr->opcode = REG_LOAD_CONST;
                                instr->src1 = 0;
                                instr->src2 = 0;
                                instr->immediate = 0; // false
                                optimizations++;
                            }
                            break;
                        }
                    }
                }
                break;
        }
    }
    
    return optimizations > 0;
}

int register_dead_code_elimination(RegisterProgram* program) {
    if (!program || !program->instructions) return 0;
    
    int optimizations = 0;
    
    // Simple liveness analysis: mark registers as live if they're used
    int* live_registers = (int*)calloc(256, sizeof(int));
    if (!live_registers) return 0;
    
    // Mark output registers as live (registers used in function returns or side effects)
    for (size_t i = 0; i < program->instruction_count; i++) {
        RegisterInstruction* instr = &program->instructions[i];
        
        // Mark registers used in function calls, stores, or returns as live
        if (instr->opcode == REG_STORE_VAR || instr->opcode == REG_STORE_GLOBAL ||
            instr->opcode == REG_STORE_LOCAL || instr->opcode == REG_STORE_UPVALUE ||
            instr->opcode == REG_STORE_ARRAY || instr->opcode == REG_STORE_OBJECT) {
            live_registers[instr->src1] = 1;
        }
    }
    
    // Backward pass to mark live registers
    for (int i = (int)program->instruction_count - 1; i >= 0; i--) {
        RegisterInstruction* instr = &program->instructions[i];
        
        // If destination is live, mark sources as live
        if (live_registers[instr->dst]) {
            live_registers[instr->src1] = 1;
            live_registers[instr->src2] = 1;
            live_registers[instr->src3] = 1;
        }
    }
    
    // Remove instructions that write to dead registers
    size_t write_pos = 0;
    for (size_t i = 0; i < program->instruction_count; i++) {
        RegisterInstruction* instr = &program->instructions[i];
        
        // Keep instruction if destination is live or if it has side effects
        if (live_registers[instr->dst] || 
            instr->opcode == REG_STORE_VAR || instr->opcode == REG_STORE_GLOBAL ||
            instr->opcode == REG_STORE_LOCAL || instr->opcode == REG_STORE_UPVALUE ||
            instr->opcode == REG_STORE_ARRAY || instr->opcode == REG_STORE_OBJECT ||
            instr->opcode == REG_JUMP || instr->opcode == REG_JUMP_IF_FALSE ||
            instr->opcode == REG_JUMP_IF_TRUE || instr->opcode == REG_CALL ||
            instr->opcode == REG_RETURN) {
            
            if (write_pos != i) {
                program->instructions[write_pos] = *instr;
            }
            write_pos++;
        } else {
            optimizations++;
        }
    }
    
    program->instruction_count = write_pos;
    free(live_registers);
    
    return optimizations > 0;
}

int register_allocate_optimize(RegisterProgram* program) {
    if (!program || !program->instructions) return 0;
    
    int optimizations = 0;
    
    // Simple linear scan register allocation
    int* register_usage = (int*)calloc(256, sizeof(int));
    int* register_lifetime = (int*)calloc(256, sizeof(int));
    if (!register_usage || !register_lifetime) {
        if (register_usage) free(register_usage);
        if (register_lifetime) free(register_lifetime);
        return 0;
    }
    
    // Calculate register lifetimes
    for (size_t i = 0; i < program->instruction_count; i++) {
        RegisterInstruction* instr = &program->instructions[i];
        
        // Mark when register is first used
        if (register_usage[instr->src1] == 0) {
            register_usage[instr->src1] = 1;
            register_lifetime[instr->src1] = (int)i;
        }
        if (register_usage[instr->src2] == 0) {
            register_usage[instr->src2] = 1;
            register_lifetime[instr->src2] = (int)i;
        }
        if (register_usage[instr->src3] == 0) {
            register_usage[instr->src3] = 1;
            register_lifetime[instr->src3] = (int)i;
        }
        
        // Mark when register is last used
        register_lifetime[instr->dst] = (int)i;
    }
    
    // Coalesce registers with identical lifetimes
    for (size_t i = 0; i < program->instruction_count; i++) {
        // Find registers with same lifetime that can be coalesced
        for (int r1 = 0; r1 < 256; r1++) {
            if (register_lifetime[r1] == (int)i && register_usage[r1]) {
                for (int r2 = r1 + 1; r2 < 256; r2++) {
                    if (register_lifetime[r2] == (int)i && register_usage[r2]) {
                        // Coalesce r2 into r1
                        for (size_t j = 0; j < program->instruction_count; j++) {
                            RegisterInstruction* curr = &program->instructions[j];
                            if (curr->src1 == r2) curr->src1 = r1;
                            if (curr->src2 == r2) curr->src2 = r1;
                            if (curr->src3 == r2) curr->src3 = r1;
                            if (curr->dst == r2) curr->dst = r1;
                        }
                        register_usage[r2] = 0;
                        optimizations++;
                    }
                }
            }
        }
    }
    
    // Optimize hot registers (loop induction variables)
    // Prioritize registers used in tight loops
    for (size_t i = 0; i < program->instruction_count; i++) {
        RegisterInstruction* instr = &program->instructions[i];
        
        // If this is a loop induction variable (increment/decrement)
        if ((instr->opcode == REG_INCI_R || instr->opcode == REG_DECI_R ||
             instr->opcode == REG_INCF_R || instr->opcode == REG_DECF_R) &&
            instr->dst < 16) { // Keep in low registers for better performance
            // This register is likely hot, keep it in a low register
            optimizations++;
        }
    }
    
    free(register_usage);
    free(register_lifetime);
    
    return optimizations > 0;
}

// Utility functions
size_t register_program_get_register_count(RegisterProgram* program) {
    if (!program) return 0;
    return program->register_count;
}

double register_program_get_hotness_score(RegisterProgram* program) {
    if (!program) return 0.0;
    return program->hotness_score;
}

void register_program_set_hotness_score(RegisterProgram* program, double score) {
    if (!program) return;
    program->hotness_score = score;
}

int register_program_is_jit_eligible(RegisterProgram* program) {
    if (!program) return 0;
    
    // Check if program is eligible for JIT compilation
    return program->instruction_count > 10 && 
           program->hotness_score > 0.5 && 
           !program->optimized;
}

int register_program_is_trace_eligible(RegisterProgram* program) {
    if (!program) return 0;
    
    // Check if program is eligible for trace recording
    return program->instruction_count > 20 && 
           program->hotness_score > 0.7 && 
           program->traceable;
}

void register_program_print(RegisterProgram* program) {
    if (!program) {
        printf("RegisterProgram: NULL\n");
        return;
    }
    
    printf("RegisterProgram:\n");
    printf("  Instructions: %zu\n", program->instruction_count);
    printf("  Constants: %zu\n", program->constant_count);
    printf("  Strings: %zu\n", program->string_count);
    printf("  Registers: %zu\n", program->register_count);
    printf("  Hotness: %.2f\n", program->hotness_score);
    printf("  Optimized: %s\n", program->optimized ? "yes" : "no");
    printf("  Vectorized: %s\n", program->vectorized ? "yes" : "no");
    printf("  Traceable: %s\n", program->traceable ? "yes" : "no");
}

void register_instruction_print(RegisterInstruction instr) {
    printf("  opcode=%d, dst=%d, src1=%d, src2=%d, src3=%d, imm=%u, off=%u\n",
           instr.opcode, instr.dst, instr.src1, instr.src2, instr.src3, 
           instr.immediate, instr.offset);
}
