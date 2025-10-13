#include "../../include/core/optimization/bytecode_engine.h"
#include "../../include/core/interpreter/interpreter_core.h"
#include "../../include/core/ast.h"
#include "../../include/core/interpreter/value_operations.h"
#include "../../include/utils/shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// ============================================================================
// BYTECODE PROGRAM MANAGEMENT
// ============================================================================

BytecodeProgram* bytecode_program_create(void) {
    BytecodeProgram* program = (BytecodeProgram*)shared_malloc_safe(
        sizeof(BytecodeProgram), "bytecode", "program_create", 0);
    if (!program) return NULL;
    
    program->instructions = NULL;
    program->instruction_count = 0;
    program->capacity = 0;
    
    program->constants = NULL;
    program->constant_count = 0;
    program->constant_capacity = 0;
    
    program->string_pool = NULL;
    program->string_count = 0;
    program->string_capacity = 0;
    
    program->stack_size = 0;
    program->local_count = 0;
    program->upvalue_count = 0;
    program->is_function = 0;
    program->function_name = NULL;
    
    return program;
}

void bytecode_program_free(BytecodeProgram* program) {
    if (!program) return;
    
    // Free instructions
    if (program->instructions) {
        shared_free_safe(program->instructions, "bytecode", "program_free", 0);
    }
    
    // Free constants
    if (program->constants) {
        for (size_t i = 0; i < program->constant_count; i++) {
            value_free(&program->constants[i]);
        }
        shared_free_safe(program->constants, "bytecode", "program_free", 0);
    }
    
    // Free string pool
    if (program->string_pool) {
        for (size_t i = 0; i < program->string_count; i++) {
            if (program->string_pool[i]) {
                shared_free_safe(program->string_pool[i], "bytecode", "program_free", 0);
            }
        }
        shared_free_safe(program->string_pool, "bytecode", "program_free", 0);
    }
    
    // Free function name
    if (program->function_name) {
        shared_free_safe(program->function_name, "bytecode", "program_free", 0);
    }
    
    shared_free_safe(program, "bytecode", "program_free", 0);
}

int bytecode_program_add_instruction(BytecodeProgram* program, BytecodeInstruction instr) {
    if (!program) return 0;
    
    // Grow instruction array if needed
    if (program->instruction_count >= program->capacity) {
        size_t new_capacity = program->capacity == 0 ? 16 : program->capacity * 2;
        BytecodeInstruction* new_instructions = (BytecodeInstruction*)shared_malloc_safe(
            new_capacity * sizeof(BytecodeInstruction), "bytecode", "add_instruction", 0);
        if (!new_instructions) return 0;
        
        if (program->instructions) {
            memcpy(new_instructions, program->instructions, 
                   program->instruction_count * sizeof(BytecodeInstruction));
            shared_free_safe(program->instructions, "bytecode", "add_instruction", 0);
        }
        
        program->instructions = new_instructions;
        program->capacity = new_capacity;
    }
    
    program->instructions[program->instruction_count] = instr;
    program->instruction_count++;
    return 1;
}

int bytecode_program_add_constant(BytecodeProgram* program, Value value) {
    if (!program) return 0;
    
    // Grow constant array if needed
    if (program->constant_count >= program->constant_capacity) {
        size_t new_capacity = program->constant_capacity == 0 ? 16 : program->constant_capacity * 2;
        Value* new_constants = (Value*)shared_malloc_safe(
            new_capacity * sizeof(Value), "bytecode", "add_constant", 0);
        if (!new_constants) return 0;
        
        if (program->constants) {
            memcpy(new_constants, program->constants, 
                   program->constant_count * sizeof(Value));
            shared_free_safe(program->constants, "bytecode", "add_constant", 0);
        }
        
        program->constants = new_constants;
        program->constant_capacity = new_capacity;
    }
    
    program->constants[program->constant_count] = value;
    program->constant_count++;
    return 1;
}

int bytecode_program_add_string(BytecodeProgram* program, const char* str) {
    if (!program || !str) return 0;
    
    // Grow string pool if needed
    if (program->string_count >= program->string_capacity) {
        size_t new_capacity = program->string_capacity == 0 ? 16 : program->string_capacity * 2;
        char** new_strings = (char**)shared_malloc_safe(
            new_capacity * sizeof(char*), "bytecode", "add_string", 0);
        if (!new_strings) return 0;
        
        if (program->string_pool) {
            memcpy(new_strings, program->string_pool, 
                   program->string_count * sizeof(char*));
            shared_free_safe(program->string_pool, "bytecode", "add_string", 0);
        }
        
        program->string_pool = new_strings;
        program->string_capacity = new_capacity;
    }
    
    char* str_copy = (char*)shared_malloc_safe(strlen(str) + 1, "bytecode", "add_string", 0);
    if (!str_copy) return 0;
    
    strcpy(str_copy, str);
    program->string_pool[program->string_count] = str_copy;
    program->string_count++;
    return 1;
}

// ============================================================================
// BYTECODE COMPILATION FROM AST
// ============================================================================

BytecodeProgram* bytecode_compile_ast(ASTNode* ast, Interpreter* interpreter) {
    if (!ast) return NULL;
    
    BytecodeProgram* program = bytecode_program_create();
    if (!program) return NULL;
    
    // Compile based on AST node type
    switch (ast->type) {
        case AST_NODE_NUMBER:
            return bytecode_compile_number(ast, program);
        case AST_NODE_STRING:
            return bytecode_compile_string(ast, program);
        case AST_NODE_BOOL:
            return bytecode_compile_bool(ast, program);
        case AST_NODE_NULL:
            return bytecode_compile_null(ast, program);
        case AST_NODE_IDENTIFIER:
            return bytecode_compile_identifier(ast, program, interpreter);
        case AST_NODE_BINARY_OP:
            return bytecode_compile_binary_op(ast, program, interpreter);
        case AST_NODE_UNARY_OP:
            return bytecode_compile_unary_op(ast, program, interpreter);
        case AST_NODE_FUNCTION_CALL:
            return bytecode_compile_function_call(ast, program, interpreter);
        case AST_NODE_VARIABLE_DECLARATION:
            return bytecode_compile_variable_declaration(ast, program, interpreter);
        case AST_NODE_IF_STATEMENT:
            return bytecode_compile_if_statement(ast, program, interpreter);
        case AST_NODE_WHILE_LOOP:
            return bytecode_compile_while_loop(ast, program, interpreter);
        case AST_NODE_FOR_LOOP:
            return bytecode_compile_for_loop(ast, program, interpreter);
        case AST_NODE_BLOCK:
            return bytecode_compile_block(ast, program, interpreter);
        case AST_NODE_RETURN:
            return bytecode_compile_return(ast, program, interpreter);
        case AST_NODE_FUNCTION:
            return bytecode_compile_function(ast, program, interpreter);
        case AST_NODE_ARRAY_LITERAL:
            return bytecode_compile_array_literal(ast, program, interpreter);
        case AST_NODE_MEMBER_ACCESS:
            return bytecode_compile_member_access(ast, program, interpreter);
        default:
            // Unsupported node type, return empty program
            return program;
    }
}

// Compile number literal
BytecodeProgram* bytecode_compile_number(ASTNode* ast, BytecodeProgram* program) {
    if (!ast || !program) return NULL;
    
    // Add constant to pool
    Value const_val = value_create_number(ast->data.number_value);
    if (!bytecode_program_add_constant(program, const_val)) {
        return NULL;
    }
    
    // Emit LOAD_CONST instruction
    BytecodeInstruction instr = {0};
    instr.opcode = BC_LOAD_CONST;
    instr.arg1 = program->constant_count - 1;  // Index of constant
    
    if (!bytecode_program_add_instruction(program, instr)) {
        return NULL;
    }
    
    return program;
}

// Compile string literal
BytecodeProgram* bytecode_compile_string(ASTNode* ast, BytecodeProgram* program) {
    if (!ast || !program) return NULL;
    
    // Add string to pool
    if (!bytecode_program_add_string(program, ast->data.string_value)) {
        return NULL;
    }
    
    // Add constant to pool
    Value const_val = value_create_string(ast->data.string_value);
    if (!bytecode_program_add_constant(program, const_val)) {
        return NULL;
    }
    
    // Emit LOAD_CONST instruction
    BytecodeInstruction instr = {0};
    instr.opcode = BC_LOAD_CONST;
    instr.arg1 = program->constant_count - 1;  // Index of constant
    
    if (!bytecode_program_add_instruction(program, instr)) {
        return NULL;
    }
    
    return program;
}

// Compile boolean literal
BytecodeProgram* bytecode_compile_bool(ASTNode* ast, BytecodeProgram* program) {
    if (!ast || !program) return NULL;
    
    // Emit appropriate load instruction
    BytecodeInstruction instr = {0};
    if (ast->data.bool_value) {
        instr.opcode = BC_LOAD_TRUE;
    } else {
        instr.opcode = BC_LOAD_FALSE;
    }
    
    if (!bytecode_program_add_instruction(program, instr)) {
        return NULL;
    }
    
    return program;
}

// Compile null literal
BytecodeProgram* bytecode_compile_null(ASTNode* ast, BytecodeProgram* program) {
    if (!ast || !program) return NULL;
    
    // Emit LOAD_NULL instruction
    BytecodeInstruction instr = {0};
    instr.opcode = BC_LOAD_NULL;
    
    if (!bytecode_program_add_instruction(program, instr)) {
        return NULL;
    }
    
    return program;
}

// Compile identifier
BytecodeProgram* bytecode_compile_identifier(ASTNode* ast, BytecodeProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // For now, emit LOAD_VAR instruction
    // TODO: Optimize for local vs global variables
    BytecodeInstruction instr = {0};
    instr.opcode = BC_LOAD_VAR;
    // arg1 will be filled with variable index during execution
    
    if (!bytecode_program_add_instruction(program, instr)) {
        return NULL;
    }
    
    return program;
}

// Compile binary operation
BytecodeProgram* bytecode_compile_binary_op(ASTNode* ast, BytecodeProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // Compile left operand
    BytecodeProgram* left_program = bytecode_compile_ast(ast->data.binary_op.left, interpreter);
    if (!left_program) return NULL;
    
    // Compile right operand
    BytecodeProgram* right_program = bytecode_compile_ast(ast->data.binary_op.right, interpreter);
    if (!right_program) return NULL;
    
    // Merge programs
    // TODO: Implement program merging
    
    // Emit appropriate operation instruction
    BytecodeInstruction instr = {0};
    switch (ast->data.binary_op.op) {
        case BINARY_OP_ADD:
            instr.opcode = BC_ADD_INT;  // TODO: Type specialization
            break;
        case BINARY_OP_SUBTRACT:
            instr.opcode = BC_SUB_INT;
            break;
        case BINARY_OP_MULTIPLY:
            instr.opcode = BC_MUL_INT;
            break;
        case BINARY_OP_DIVIDE:
            instr.opcode = BC_DIV_INT;
            break;
        case BINARY_OP_EQUAL:
            instr.opcode = BC_EQ_INT;
            break;
        case BINARY_OP_NOT_EQUAL:
            instr.opcode = BC_NE_INT;
            break;
        case BINARY_OP_LESS_THAN:
            instr.opcode = BC_LT_INT;
            break;
        case BINARY_OP_GREATER_THAN:
            instr.opcode = BC_GT_INT;
            break;
        case BINARY_OP_LESS_EQUAL:
            instr.opcode = BC_LE_INT;
            break;
        case BINARY_OP_GREATER_EQUAL:
            instr.opcode = BC_GE_INT;
            break;
        case BINARY_OP_AND:
            instr.opcode = BC_AND;
            break;
        case BINARY_OP_OR:
            instr.opcode = BC_OR;
            break;
        default:
            // Unsupported operation, return empty program
            return program;
    }
    
    if (!bytecode_program_add_instruction(program, instr)) {
        return NULL;
    }
    
    return program;
}

// Compile unary operation
BytecodeProgram* bytecode_compile_unary_op(ASTNode* ast, BytecodeProgram* program, Interpreter* interpreter) {
    if (!ast || !program) return NULL;
    
    // Compile operand
    BytecodeProgram* operand_program = bytecode_compile_ast(ast->data.unary_op.operand, interpreter);
    if (!operand_program) return NULL;
    
    // Emit appropriate operation instruction
    BytecodeInstruction instr = {0};
    switch (ast->data.unary_op.op) {
        case UNARY_OP_NEGATE:
            instr.opcode = BC_NEG_INT;  // TODO: Type specialization
            break;
        case UNARY_OP_NOT:
            instr.opcode = BC_NOT;
            break;
        default:
            // Unsupported operation, return empty program
            return program;
    }
    
    if (!bytecode_program_add_instruction(program, instr)) {
        return NULL;
    }
    
    return program;
}

// Placeholder implementations for other compilation functions
BytecodeProgram* bytecode_compile_function_call(ASTNode* ast, BytecodeProgram* program, Interpreter* interpreter) {
    // TODO: Implement function call compilation
    return program;
}

BytecodeProgram* bytecode_compile_variable_declaration(ASTNode* ast, BytecodeProgram* program, Interpreter* interpreter) {
    // TODO: Implement variable declaration compilation
    return program;
}

BytecodeProgram* bytecode_compile_if_statement(ASTNode* ast, BytecodeProgram* program, Interpreter* interpreter) {
    // TODO: Implement if statement compilation
    return program;
}

BytecodeProgram* bytecode_compile_while_loop(ASTNode* ast, BytecodeProgram* program, Interpreter* interpreter) {
    // TODO: Implement while loop compilation
    return program;
}

BytecodeProgram* bytecode_compile_for_loop(ASTNode* ast, BytecodeProgram* program, Interpreter* interpreter) {
    // TODO: Implement for loop compilation
    return program;
}

BytecodeProgram* bytecode_compile_block(ASTNode* ast, BytecodeProgram* program, Interpreter* interpreter) {
    // TODO: Implement block compilation
    return program;
}

BytecodeProgram* bytecode_compile_return(ASTNode* ast, BytecodeProgram* program, Interpreter* interpreter) {
    // TODO: Implement return compilation
    return program;
}

BytecodeProgram* bytecode_compile_function(ASTNode* ast, BytecodeProgram* program, Interpreter* interpreter) {
    // TODO: Implement function compilation
    return program;
}

BytecodeProgram* bytecode_compile_array_literal(ASTNode* ast, BytecodeProgram* program, Interpreter* interpreter) {
    // TODO: Implement array literal compilation
    return program;
}

BytecodeProgram* bytecode_compile_member_access(ASTNode* ast, BytecodeProgram* program, Interpreter* interpreter) {
    // TODO: Implement member access compilation
    return program;
}

// ============================================================================
// BYTECODE EXECUTION
// ============================================================================

BytecodeContext* bytecode_context_create(BytecodeProgram* program) {
    if (!program) return NULL;
    
    BytecodeContext* context = (BytecodeContext*)shared_malloc_safe(
        sizeof(BytecodeContext), "bytecode", "context_create", 0);
    if (!context) return NULL;
    
    context->program = program;
    context->stack = NULL;
    context->stack_top = 0;
    context->stack_capacity = 0;
    context->locals = NULL;
    context->local_count = 0;
    context->upvalues = NULL;
    context->upvalue_count = 0;
    context->pc = 0;
    context->halted = 0;
    context->caller = NULL;
    context->return_pc = 0;
    
    // Initialize stack
    context->stack_capacity = program->stack_size > 0 ? program->stack_size : 64;
    context->stack = (Value*)shared_malloc_safe(
        context->stack_capacity * sizeof(Value), "bytecode", "context_create", 0);
    if (!context->stack) {
        bytecode_context_free(context);
        return NULL;
    }
    
    return context;
}

void bytecode_context_free(BytecodeContext* context) {
    if (!context) return;
    
    if (context->stack) {
        // Free stack values
        for (size_t i = 0; i < context->stack_top; i++) {
            value_free(&context->stack[i]);
        }
        shared_free_safe(context->stack, "bytecode", "context_free", 0);
    }
    
    if (context->locals) {
        // Free local values
        for (size_t i = 0; i < context->local_count; i++) {
            value_free(&context->locals[i]);
        }
        shared_free_safe(context->locals, "bytecode", "context_free", 0);
    }
    
    if (context->upvalues) {
        // Free upvalue values
        for (size_t i = 0; i < context->upvalue_count; i++) {
            value_free(&context->upvalues[i]);
        }
        shared_free_safe(context->upvalues, "bytecode", "context_free", 0);
    }
    
    shared_free_safe(context, "bytecode", "context_free", 0);
}

Value bytecode_execute(BytecodeContext* context, Interpreter* interpreter) {
    if (!context || !context->program) return value_create_null();
    
    // Execute instructions until halt
    while (!context->halted && context->pc < context->program->instruction_count) {
        Value result = bytecode_execute_instruction(context, interpreter);
        if (interpreter && interpreter->has_error) {
            return value_create_null();
        }
        context->pc++;
    }
    
    // Return top of stack if any
    if (context->stack_top > 0) {
        return context->stack[context->stack_top - 1];
    }
    
    return value_create_null();
}

Value bytecode_execute_instruction(BytecodeContext* context, Interpreter* interpreter) {
    if (!context || !context->program || context->pc >= context->program->instruction_count) {
        return value_create_null();
    }
    
    BytecodeInstruction instr = context->program->instructions[context->pc];
    
    switch (instr.opcode) {
        case BC_LOAD_CONST: {
            if (instr.arg1 < context->program->constant_count) {
                Value const_val = context->program->constants[instr.arg1];
                // Push constant onto stack
                if (context->stack_top < context->stack_capacity) {
                    context->stack[context->stack_top] = value_clone(&const_val);
                    context->stack_top++;
                }
            }
            break;
        }
        
        case BC_LOAD_NULL: {
            if (context->stack_top < context->stack_capacity) {
                context->stack[context->stack_top] = value_create_null();
                context->stack_top++;
            }
            break;
        }
        
        case BC_LOAD_TRUE: {
            if (context->stack_top < context->stack_capacity) {
                context->stack[context->stack_top] = value_create_boolean(1);
                context->stack_top++;
            }
            break;
        }
        
        case BC_LOAD_FALSE: {
            if (context->stack_top < context->stack_capacity) {
                context->stack[context->stack_top] = value_create_boolean(0);
                context->stack_top++;
            }
            break;
        }
        
        case BC_ADD_INT: {
            if (context->stack_top >= 2) {
                Value b = context->stack[--context->stack_top];
                Value a = context->stack[--context->stack_top];
                Value result = value_add(&a, &b);
                context->stack[context->stack_top++] = result;
                value_free(&a);
                value_free(&b);
            }
            break;
        }
        
        case BC_HALT: {
            context->halted = 1;
            break;
        }
        
        default:
            // Unsupported instruction
            break;
    }
    
    return value_create_null();
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

const char* bytecode_opcode_name(BytecodeOpcode opcode) {
    switch (opcode) {
        case BC_LOAD_CONST: return "LOAD_CONST";
        case BC_LOAD_VAR: return "LOAD_VAR";
        case BC_STORE_VAR: return "STORE_VAR";
        case BC_LOAD_GLOBAL: return "LOAD_GLOBAL";
        case BC_STORE_GLOBAL: return "STORE_GLOBAL";
        case BC_LOAD_LOCAL: return "LOAD_LOCAL";
        case BC_STORE_LOCAL: return "STORE_LOCAL";
        case BC_LOAD_UPVALUE: return "LOAD_UPVALUE";
        case BC_STORE_UPVALUE: return "STORE_UPVALUE";
        case BC_LOAD_NULL: return "LOAD_NULL";
        case BC_LOAD_TRUE: return "LOAD_TRUE";
        case BC_LOAD_FALSE: return "LOAD_FALSE";
        case BC_LOAD_ZERO: return "LOAD_ZERO";
        case BC_LOAD_ONE: return "LOAD_ONE";
        case BC_DUP: return "DUP";
        case BC_POP: return "POP";
        case BC_ADD_INT: return "ADD_INT";
        case BC_ADD_FLOAT: return "ADD_FLOAT";
        case BC_SUB_INT: return "SUB_INT";
        case BC_SUB_FLOAT: return "SUB_FLOAT";
        case BC_MUL_INT: return "MUL_INT";
        case BC_MUL_FLOAT: return "MUL_FLOAT";
        case BC_DIV_INT: return "DIV_INT";
        case BC_DIV_FLOAT: return "DIV_FLOAT";
        case BC_MOD_INT: return "MOD_INT";
        case BC_MOD_FLOAT: return "MOD_FLOAT";
        case BC_POW_INT: return "POW_INT";
        case BC_POW_FLOAT: return "POW_FLOAT";
        case BC_NEG_INT: return "NEG_INT";
        case BC_NEG_FLOAT: return "NEG_FLOAT";
        case BC_INC: return "INC";
        case BC_DEC: return "DEC";
        case BC_EQ_INT: return "EQ_INT";
        case BC_EQ_FLOAT: return "EQ_FLOAT";
        case BC_EQ_STRING: return "EQ_STRING";
        case BC_NE_INT: return "NE_INT";
        case BC_NE_FLOAT: return "NE_FLOAT";
        case BC_LT_INT: return "LT_INT";
        case BC_LT_FLOAT: return "LT_FLOAT";
        case BC_LE_INT: return "LE_INT";
        case BC_LE_FLOAT: return "LE_FLOAT";
        case BC_GT_INT: return "GT_INT";
        case BC_GT_FLOAT: return "GT_FLOAT";
        case BC_GE_INT: return "GE_INT";
        case BC_GE_FLOAT: return "GE_FLOAT";
        case BC_IS_NULL: return "IS_NULL";
        case BC_IS_TRUE: return "IS_TRUE";
        case BC_AND: return "AND";
        case BC_OR: return "OR";
        case BC_NOT: return "NOT";
        case BC_XOR: return "XOR";
        case BC_JUMP: return "JUMP";
        case BC_JUMP_IF_FALSE: return "JUMP_IF_FALSE";
        case BC_JUMP_IF_TRUE: return "JUMP_IF_TRUE";
        case BC_JUMP_IF_NULL: return "JUMP_IF_NULL";
        case BC_JUMP_IF_NOT_NULL: return "JUMP_IF_NOT_NULL";
        case BC_CALL: return "CALL";
        case BC_CALL_METHOD: return "CALL_METHOD";
        case BC_RETURN: return "RETURN";
        case BC_RETURN_NULL: return "RETURN_NULL";
        case BC_TAIL_CALL: return "TAIL_CALL";
        case BC_HALT: return "HALT";
        case BC_NOP: return "NOP";
        case BC_DEBUG: return "DEBUG";
        default: return "UNKNOWN";
    }
}

void bytecode_disassemble(BytecodeProgram* program) {
    if (!program) return;
    
    printf("Bytecode Program Disassembly:\n");
    printf("Instructions: %zu\n", program->instruction_count);
    printf("Constants: %zu\n", program->constant_count);
    printf("Strings: %zu\n", program->string_count);
    printf("\n");
    
    for (size_t i = 0; i < program->instruction_count; i++) {
        printf("%4zu: ", i);
        bytecode_print_instruction(program->instructions[i], i);
    }
}

void bytecode_print_instruction(BytecodeInstruction instr, size_t index) {
    printf("%s", bytecode_opcode_name(instr.opcode));
    
    if (instr.arg1 != 0 || instr.arg2 != 0 || instr.arg3 != 0) {
        printf(" %d", instr.arg1);
        if (instr.arg2 != 0 || instr.arg3 != 0) {
            printf(" %d", instr.arg2);
        }
        if (instr.arg3 != 0) {
            printf(" %d", instr.arg3);
        }
    }
    
    printf("\n");
}

const char* bytecode_error_string(BytecodeError error) {
    switch (error) {
        case BYTECODE_ERROR_NONE: return "No error";
        case BYTECODE_ERROR_STACK_OVERFLOW: return "Stack overflow";
        case BYTECODE_ERROR_STACK_UNDERFLOW: return "Stack underflow";
        case BYTECODE_ERROR_INVALID_OPCODE: return "Invalid opcode";
        case BYTECODE_ERROR_INVALID_JUMP: return "Invalid jump";
        case BYTECODE_ERROR_MEMORY_ALLOCATION: return "Memory allocation failed";
        case BYTECODE_ERROR_COMPILATION_FAILED: return "Compilation failed";
        default: return "Unknown error";
    }
}

// ============================================================================
// AST INTEGRATION
// ============================================================================

void ast_node_set_bytecode(ASTNode* node, BytecodeProgram* bytecode) {
    if (node) {
        node->cached_bytecode = bytecode;
    }
}

BytecodeProgram* ast_node_get_bytecode(ASTNode* node) {
    return node ? (BytecodeProgram*)node->cached_bytecode : NULL;
}

void ast_node_clear_bytecode(ASTNode* node) {
    if (node && node->cached_bytecode) {
        bytecode_program_free((BytecodeProgram*)node->cached_bytecode);
        node->cached_bytecode = NULL;
    }
}

// ============================================================================
// INTERPRETER INTEGRATION
// ============================================================================

Value interpreter_execute_bytecode(Interpreter* interpreter, BytecodeProgram* program) {
    if (!interpreter || !program) return value_create_null();
    
    BytecodeContext* context = bytecode_context_create(program);
    if (!context) return value_create_null();
    
    Value result = bytecode_execute(context, interpreter);
    
    bytecode_context_free(context);
    return result;
}

int interpreter_has_bytecode_cached(ASTNode* node) {
    return node && node->cached_bytecode != NULL;
}
