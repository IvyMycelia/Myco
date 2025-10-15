#include "../../include/core/bytecode.h"
#include "../../include/utils/shared_utilities.h"
#include "../../include/core/interpreter/value_operations.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Bytecode VM implementation
// This implements a stack-based virtual machine for executing Myco bytecode

// Stack operations
static Value* value_stack = NULL;
static size_t value_stack_size = 0;
static size_t value_stack_capacity = 0;

static double* num_stack = NULL;
static size_t num_stack_size = 0;
static size_t num_stack_capacity = 0;

// Stack management functions
static void value_stack_push(Value v) {
    if (value_stack_size + 1 > value_stack_capacity) {
        size_t new_cap = value_stack_capacity ? value_stack_capacity * 2 : 128;
        value_stack = shared_realloc_safe(value_stack, new_cap * sizeof(Value), "bytecode_vm", "value_stack_push", 1);
        value_stack_capacity = new_cap;
    }
    value_stack[value_stack_size] = v;
    value_stack_size++;
}

static Value value_stack_pop(void) {
    if (value_stack_size == 0) {
        return value_create_null();
    }
    value_stack_size--;
    return value_stack[value_stack_size];
}

static Value value_stack_peek(void) {
    if (value_stack_size == 0) {
        return value_create_null();
    }
    return value_stack[value_stack_size - 1];
}

static void num_stack_push(double v) {
    if (num_stack_size + 1 > num_stack_capacity) {
        size_t new_cap = num_stack_capacity ? num_stack_capacity * 2 : 128;
        num_stack = shared_realloc_safe(num_stack, new_cap * sizeof(double), "bytecode_vm", "num_stack_push", 1);
        num_stack_capacity = new_cap;
    }
    num_stack[num_stack_size] = v;
    num_stack_size++;
}

static double num_stack_pop(void) {
    if (num_stack_size == 0) {
        return 0.0;
    }
    num_stack_size--;
    return num_stack[num_stack_size];
}

static double num_stack_peek(void) {
    if (num_stack_size == 0) {
        return 0.0;
    }
    return num_stack[num_stack_size - 1];
}

// These functions are implemented in bytecode_compiler.c
// We only implement the execution part here

// Main execution function
Value bytecode_execute(BytecodeProgram* program, Interpreter* interpreter, int debug) {
    if (!program || !interpreter) {
        return value_create_null();
    }
    
    // Initialize stacks
    value_stack_size = 0;
    num_stack_size = 0;
    
    // Set interpreter reference
    program->interpreter = interpreter;
    
    // Execute main program
    size_t pc = 0;
    Value result = value_create_null();
    
    if (debug) {
        printf("DEBUG: Starting bytecode execution, %zu instructions\n", program->count);
    }
    
    while (pc < program->count) {
        BytecodeInstruction* instr = &program->code[pc];
        
        if (debug) {
            printf("PC: %zu, Op: %d, A: %d, B: %d\n", pc, instr->op, instr->a, instr->b);
        }
        
        // Handle superinstructions
        if (instr->op >= BC_SUPER_START) {
            switch ((BytecodeSuperOp)instr->op) {
                case BC_NUM_TO_VALUE: {
                    double num = num_stack_pop();
                    value_stack_push(value_create_number(num));
                    pc++;
                    break;
                }
                
                case BC_CALL_FUNCTION: {
                    // Get function from AST nodes
                    if (instr->a < program->ast_count) {
                        ASTNode* func_node = program->ast_nodes[instr->a];
                        if (func_node && func_node->type == AST_NODE_FUNCTION_CALL) {
                            // Fallback to AST evaluation for function calls
                            Value result = interpreter_execute(interpreter, func_node);
                            value_stack_push(result);
                        } else {
                            value_stack_push(value_create_null());
                        }
                    } else {
                        value_stack_push(value_create_null());
                    }
                    pc++;
                    break;
                }
                
                case BC_RETURN: {
                    if (instr->a == 0) {
                        // Void return
                        result = value_create_null();
                    } else {
                        // Value return
                        result = value_stack_pop();
                    }
                    goto cleanup;
                }
                
                default: {
                    // Unknown superinstruction - fallback to AST
                    if (instr->a < program->ast_count) {
                        ASTNode* node = program->ast_nodes[instr->a];
                        if (node) {
                            Value result = interpreter_execute(interpreter, node);
                            value_stack_push(result);
                        } else {
                            value_stack_push(value_create_null());
                        }
                    } else {
                        value_stack_push(value_create_null());
                    }
                    pc++;
                    break;
                }
            }
        } else {
            // Handle regular bytecode operations
            switch (instr->op) {
            case BC_LOAD_CONST: {
                if (instr->a < program->const_count) {
                    Value const_val = program->constants[instr->a];
                    // For numbers, avoid cloning by using direct push
                    if (const_val.type == VALUE_NUMBER) {
                        value_stack_push(value_create_number(const_val.data.number_value));
                    } else {
                        value_stack_push(value_clone(&const_val));
                    }
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_LOAD_LOCAL: {
                if (instr->a < program->local_slot_count) {
                    Value local_val = program->locals[instr->a];
                    // For numbers, avoid cloning by using direct push
                    if (local_val.type == VALUE_NUMBER) {
                        value_stack_push(value_create_number(local_val.data.number_value));
                    } else {
                        value_stack_push(value_clone(&local_val));
                    }
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_STORE_LOCAL: {
                if (instr->a < program->local_slot_count) {
                    Value val = value_stack_pop();
                    value_free(&program->locals[instr->a]);
                    program->locals[instr->a] = val;
                }
                pc++;
                break;
            }
            
            case BC_ADD: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                Value result = value_add(&a, &b);
                value_free(&a);
                value_free(&b);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_SUB: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                Value result = value_subtract(&a, &b);
                value_free(&a);
                value_free(&b);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_MUL: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                Value result = value_multiply(&a, &b);
                value_free(&a);
                value_free(&b);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_DIV: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                Value result = value_divide(&a, &b);
                value_free(&a);
                value_free(&b);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_EQ: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                Value result = value_equal(&a, &b);
                value_free(&a);
                value_free(&b);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_NE: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                Value eq = value_equal(&a, &b);
                Value result = value_logical_not(&eq);
                value_free(&a);
                value_free(&b);
                value_free(&eq);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_LT: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                Value result = value_less_than(&a, &b);
                value_free(&a);
                value_free(&b);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_LE: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                Value lt = value_less_than(&a, &b);
                Value eq = value_equal(&a, &b);
                Value result = value_logical_or(&lt, &eq);
                value_free(&a);
                value_free(&b);
                value_free(&lt);
                value_free(&eq);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_GT: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                Value result = value_greater_than(&a, &b);
                value_free(&a);
                value_free(&b);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_GE: {
                Value b = value_stack_pop();
                Value a = value_stack_pop();
                Value gt = value_greater_than(&a, &b);
                Value eq = value_equal(&a, &b);
                Value result = value_logical_or(&gt, &eq);
                value_free(&a);
                value_free(&b);
                value_free(&gt);
                value_free(&eq);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_JUMP: {
                pc = instr->a;
                break;
            }
            
            case BC_JUMP_IF_FALSE: {
                Value condition = value_stack_pop();
                if (condition.type == VALUE_BOOLEAN && !condition.data.boolean_value) {
                    pc = instr->a;
                } else {
                    pc++;
                }
                value_free(&condition);
                break;
            }
            
            case BC_PRINT: {
                Value val = value_stack_pop();
                value_print(&val);
                value_free(&val);
                pc++;
                break;
            }
            
            case BC_PRINT_MULTIPLE: {
                // Print multiple values on one line
                int count = instr->a;
                for (int i = 0; i < count; i++) {
                    Value val = value_stack_pop();
                    value_print(&val);
                    if (i < count - 1) {
                        printf(" ");
                    }
                    value_free(&val);
                }
                printf("\n");
                pc++;
                break;
            }
            
            case BC_METHOD_CALL: {
                // Get method name from constant pool
                if (instr->a < program->const_count && program->constants[instr->a].type == VALUE_STRING) {
                    const char* method_name = program->constants[instr->a].data.string_value;
                    
                    // Get object from stack
                    Value object = value_stack_pop();
                    
                    // Handle different object types
                    if (object.type == VALUE_OBJECT) {
                        // Check if it's a class instance
                        Value class_name = value_object_get(&object, "__class_name__");
                        if (class_name.type == VALUE_STRING) {
                            // It's a class instance - find method in inheritance chain
                            Value class_def = environment_get(interpreter->global_environment, class_name.data.string_value);
                            if (class_def.type == VALUE_CLASS) {
                                Value method = find_method_in_inheritance_chain(interpreter, &class_def, method_name);
                                if (method.type == VALUE_FUNCTION) {
                                    // Set self context and call method
                                    interpreter_set_self_context(interpreter, &object);
                                    
                                    // Get arguments from stack
                                    size_t arg_count = instr->b;
                                    Value* args = NULL;
                                    if (arg_count > 0) {
                                        args = shared_malloc_safe(arg_count * sizeof(Value), "bytecode_vm", "BC_METHOD_CALL", 1);
                                        for (size_t i = 0; i < arg_count; i++) {
                                            args[arg_count - 1 - i] = value_stack_pop();
                                        }
                                    }
                                    
                                    Value result = value_function_call_with_self(&method, args, arg_count, interpreter, &object, 0, 0);
                                    
                                    // Clean up arguments
                                    if (args) {
                                        for (size_t i = 0; i < arg_count; i++) {
                                            value_free(&args[i]);
                                        }
                                        shared_free_safe(args, "bytecode_vm", "BC_METHOD_CALL", 2);
                                    }
                                    
                                    // Clear self context
                                    interpreter_set_self_context(interpreter, NULL);
                                    
                                    value_stack_push(result);
                                    value_free(&method);
                                } else {
                                    value_stack_push(value_create_null());
                                }
                            } else {
                                value_stack_push(value_create_null());
                            }
                            value_free(&class_def);
                        } else {
                            // It's a regular object - get method directly
                            Value method = value_object_get(&object, method_name);
                            if (method.type == VALUE_FUNCTION) {
                                // Set self context for method calls
                                interpreter_set_self_context(interpreter, &object);
                                
                                // Get arguments from stack
                                size_t arg_count = instr->b;
                                Value* args = NULL;
                                if (arg_count > 0) {
                                    args = shared_malloc_safe(arg_count * sizeof(Value), "bytecode_vm", "BC_METHOD_CALL", 3);
                                    for (size_t i = 0; i < arg_count; i++) {
                                        args[arg_count - 1 - i] = value_stack_pop();
                                    }
                                }
                                
                                Value result = value_function_call_with_self(&method, args, arg_count, interpreter, &object, 0, 0);
                                
                                // Clean up arguments
                                if (args) {
                                    for (size_t i = 0; i < arg_count; i++) {
                                        value_free(&args[i]);
                                    }
                                    shared_free_safe(args, "bytecode_vm", "BC_METHOD_CALL", 4);
                                }
                                
                                // Clear self context
                                interpreter_set_self_context(interpreter, NULL);
                                
                                value_stack_push(result);
                            } else {
                                value_stack_push(value_create_null());
                            }
                            value_free(&method);
                        }
                        value_free(&class_name);
                    } else {
                        value_stack_push(value_create_null());
                    }
                    value_free(&object);
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_PROPERTY_ACCESS: {
                // Get property name from constant pool
                if (instr->a < program->const_count && program->constants[instr->a].type == VALUE_STRING) {
                    const char* prop_name = program->constants[instr->a].data.string_value;
                    Value object = value_stack_pop();
                    Value prop = value_object_get(&object, prop_name);
                    value_stack_push(prop);
                    value_free(&object);
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_EVAL_AST: {
                // Fallback to AST evaluation
                if (instr->a < program->ast_count) {
                    ASTNode* node = program->ast_nodes[instr->a];
                    if (node) {
                        Value result = interpreter_execute(interpreter, node);
                        value_stack_push(result);
                    } else {
                        value_stack_push(value_create_null());
                    }
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_POP: {
                Value val = value_stack_pop();
                value_free(&val);
                pc++;
                break;
            }
            
            case BC_HALT: {
                result = value_stack_pop();
                goto cleanup;
            }
            
            // Numeric operations
            case BC_LOAD_NUM: {
                if (instr->a < program->num_const_count) {
                    num_stack_push(program->num_constants[instr->a]);
                } else {
                    num_stack_push(0.0);
                }
                pc++;
                break;
            }
            
            case BC_LOAD_NUM_LOCAL: {
                if (instr->a < program->num_local_count) {
                    num_stack_push(program->num_locals[instr->a]);
                } else {
                    num_stack_push(0.0);
                }
                pc++;
                break;
            }
            
            case BC_STORE_NUM_LOCAL: {
                if (instr->a < program->num_local_count) {
                    program->num_locals[instr->a] = num_stack_pop();
                }
                pc++;
                break;
            }
            
            case BC_ADD_NUM: {
                double b = num_stack_pop();
                double a = num_stack_pop();
                num_stack_push(a + b);
                pc++;
                break;
            }
            
            case BC_SUB_NUM: {
                double b = num_stack_pop();
                double a = num_stack_pop();
                num_stack_push(a - b);
                pc++;
                break;
            }
            
            case BC_MUL_NUM: {
                double b = num_stack_pop();
                double a = num_stack_pop();
                num_stack_push(a * b);
                pc++;
                break;
            }
            
            case BC_DIV_NUM: {
                double b = num_stack_pop();
                double a = num_stack_pop();
                if (b != 0.0) {
                    num_stack_push(a / b);
                } else {
                    num_stack_push(0.0);
                }
                pc++;
                break;
            }
            
            case BC_LT_NUM: {
                double b = num_stack_pop();
                double a = num_stack_pop();
                value_stack_push(value_create_boolean(a < b));
                pc++;
                break;
            }
            
            case BC_LE_NUM: {
                double b = num_stack_pop();
                double a = num_stack_pop();
                value_stack_push(value_create_boolean(a <= b));
                pc++;
                break;
            }
            
            case BC_GT_NUM: {
                double b = num_stack_pop();
                double a = num_stack_pop();
                value_stack_push(value_create_boolean(a > b));
                pc++;
                break;
            }
            
            case BC_GE_NUM: {
                double b = num_stack_pop();
                double a = num_stack_pop();
                value_stack_push(value_create_boolean(a >= b));
                pc++;
                break;
            }
            
            case BC_EQ_NUM: {
                double b = num_stack_pop();
                double a = num_stack_pop();
                value_stack_push(value_create_boolean(a == b));
                pc++;
                break;
            }
            
            case BC_NE_NUM: {
                double b = num_stack_pop();
                double a = num_stack_pop();
                value_stack_push(value_create_boolean(a != b));
                pc++;
                break;
            }
            
            case BC_VALUE_TO_NUM: {
                Value val = value_stack_pop();
                if (val.type == VALUE_NUMBER) {
                    num_stack_push(val.data.number_value);
                } else {
                    num_stack_push(0.0);
                }
                value_free(&val);
                pc++;
                break;
            }
            
            
            default: {
                // Unknown opcode - fallback to AST
                if (instr->a < program->ast_count) {
                    ASTNode* node = program->ast_nodes[instr->a];
                    if (node) {
                        Value result = interpreter_execute(interpreter, node);
                        value_stack_push(result);
                    } else {
                        value_stack_push(value_create_null());
                    }
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            }
        }
    }
    
cleanup:
    // Clean up any remaining stack values
    while (value_stack_size > 0) {
        Value val = value_stack_pop();
        value_free(&val);
    }
    
    return result;
}
