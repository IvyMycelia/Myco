#include "../../include/core/bytecode.h"
#include "../../include/utils/shared_utilities.h"
#include "../../include/core/interpreter/value_operations.h"
#include "../../include/core/environment.h"
#include "../../include/libs/array.h"
#include "../../include/libs/math.h"
#include "../../include/libs/maps.h"
#include "../../include/libs/sets.h"
#include <ctype.h>
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
                    // Always clone to avoid memory issues
                    value_stack_push(value_clone(&local_val));
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
                    
                    // Also update numeric locals if this is a number
                    if (val.type == VALUE_NUMBER && instr->a < program->num_local_count) {
                        program->num_locals[instr->a] = val.data.number_value;
                    }
                }
                pc++;
                break;
            }
            
            case BC_LOAD_GLOBAL: {
                // Load global variable by name
                if (instr->a < program->const_count && program->constants[instr->a].type == VALUE_STRING) {
                    const char* var_name = program->constants[instr->a].data.string_value;
                    Value global_val = environment_get(interpreter->global_environment, var_name);
                    if (global_val.type != VALUE_NULL) {
                        value_stack_push(value_clone(&global_val));
                    } else {
                        value_stack_push(value_create_null());
                    }
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_STORE_GLOBAL: {
                // Store global variable by name
                if (instr->a < program->const_count && program->constants[instr->a].type == VALUE_STRING) {
                    const char* var_name = program->constants[instr->a].data.string_value;
                    Value val = value_stack_pop();
                    environment_assign(interpreter->global_environment, var_name, val);
                    value_stack_push(value_clone(&val));
                } else {
                    value_stack_pop(); // Remove value from stack
                    value_stack_push(value_create_null());
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
            
            case BC_LOOP_START: {
                // Mark the start of a loop for potential optimization
                // For now, just continue execution
                pc++;
                break;
            }
            
            case BC_LOOP_END: {
                // Mark the end of a loop iteration
                // For now, just continue execution
                pc++;
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
                        // Check if it's a library object first
                        Value library_type = value_object_get(&object, "__type__");
                        if (library_type.type == VALUE_STRING && strcmp(library_type.data.string_value, "Library") == 0) {
                            // It's a library object - get method directly
                            Value method = value_object_get(&object, method_name);
                            if (method.type == VALUE_FUNCTION) {
                                // Get arguments from stack
                                size_t arg_count = instr->b;
                                Value* args = NULL;
                                if (arg_count > 0) {
                                    args = shared_malloc_safe(arg_count * sizeof(Value), "bytecode_vm", "BC_METHOD_CALL", 5);
                                    for (size_t i = 0; i < arg_count; i++) {
                                        args[arg_count - 1 - i] = value_stack_pop();
                                    }
                                }
                                
                                // Call library method
                                Value result = value_function_call(&method, args, arg_count, interpreter, 0, 0);
                                
                                // Clean up arguments
                                if (args) {
                                    for (size_t i = 0; i < arg_count; i++) {
                                        value_free(&args[i]);
                                    }
                                    shared_free_safe(args, "bytecode_vm", "BC_METHOD_CALL", 6);
                                }
                                
                                value_stack_push(result);
                            } else {
                                value_stack_push(value_create_null());
                            }
                            value_free(&method);
                        }
                        value_free(&library_type);
                        
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
                    
                    // Special handling for .type property (matches AST interpreter logic)
                    if (strcmp(prop_name, "type") == 0) {
                        // For objects, check for __type__ override (e.g., Library)
                        if (object.type == VALUE_OBJECT) {
                            Value type_override = value_object_get(&object, "__type__");
                            if (type_override.type == VALUE_STRING) {
                                value_stack_push(type_override);
                                value_free(&object);
                                pc++;
                                break;
                            }
                            value_free(&type_override);
                            
                            // Check for __class_name__ (class instances)
                            Value class_name = value_object_get(&object, "__class_name__");
                            if (class_name.type == VALUE_STRING) {
                                value_stack_push(class_name);
                                value_free(&object);
                                pc++;
                                break;
                            }
                            value_free(&class_name);
                        }
                        
                        // For numbers, distinguish Int vs Float
                        if (object.type == VALUE_NUMBER) {
                            const char* num_type = (object.data.number_value == (long long)object.data.number_value) ? "Int" : "Float";
                            value_stack_push(value_create_string(num_type));
                            value_free(&object);
                            pc++;
                            break;
                        }
                        
                        // Default: return type string
                        value_stack_push(value_create_string(value_type_to_string(object.type)));
                        value_free(&object);
                        pc++;
                        break;
                    }
                    
                    // Default property access
                    Value prop = value_object_get(&object, prop_name);
                    value_stack_push(prop);
                    value_free(&object);
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_CALL_BUILTIN: {
                // Call built-in function by name - for now, fall back to AST evaluation
                // TODO: Implement direct built-in function calling
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
            
            case BC_TO_STRING: {
                // Convert value to string
                Value val = value_stack_pop();
                Value result = value_to_string(&val);
                value_free(&val);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_GET_TYPE: {
                // Get value type as string (matches AST interpreter logic)
                Value val = value_stack_pop();
                Value result;
                
                // For objects, check for __type__ override (e.g., Library)
                if (val.type == VALUE_OBJECT) {
                    Value type_override = value_object_get(&val, "__type__");
                    if (type_override.type == VALUE_STRING) {
                        result = type_override;
                        value_free(&val);
                        value_stack_push(result);
                        pc++;
                        break;
                    }
                    value_free(&type_override);
                    
                    // Check for __class_name__ (class instances)
                    Value class_name = value_object_get(&val, "__class_name__");
                    if (class_name.type == VALUE_STRING) {
                        result = class_name;
                        value_free(&val);
                        value_stack_push(result);
                        pc++;
                        break;
                    }
                    value_free(&class_name);
                }
                
                // For numbers, distinguish Int vs Float
                if (val.type == VALUE_NUMBER) {
                    const char* num_type = (val.data.number_value == (long long)val.data.number_value) ? "Int" : "Float";
                    result = value_create_string(num_type);
                    value_free(&val);
                    value_stack_push(result);
                    pc++;
                    break;
                }
                
                // Default: return type string
                result = value_create_string(value_type_to_string(val.type));
                value_free(&val);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_GET_LENGTH: {
                // Get value length
                Value val = value_stack_pop();
                Value result;
                if (val.type == VALUE_STRING) {
                    result = value_create_number((double)strlen(val.data.string_value));
                } else if (val.type == VALUE_ARRAY) {
                    result = value_create_number((double)val.data.array_value.count);
                } else {
                    result = value_create_number(0.0);
                }
                value_free(&val);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_IS_STRING: {
                // Check if value is string
                Value val = value_stack_pop();
                Value result = value_create_boolean(val.type == VALUE_STRING);
                value_free(&val);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_IS_INT: {
                // Check if value is int
                Value val = value_stack_pop();
                Value result = value_create_boolean(val.type == VALUE_NUMBER && val.data.number_value == (int)val.data.number_value);
                value_free(&val);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_IS_FLOAT: {
                // Check if value is float
                Value val = value_stack_pop();
                Value result = value_create_boolean(val.type == VALUE_NUMBER);
                value_free(&val);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_IS_BOOL: {
                // Check if value is bool
                Value val = value_stack_pop();
                Value result = value_create_boolean(val.type == VALUE_BOOLEAN);
                value_free(&val);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_IS_ARRAY: {
                // Check if value is array
                Value val = value_stack_pop();
                Value result = value_create_boolean(val.type == VALUE_ARRAY);
                value_free(&val);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_IS_NULL: {
                // Check if value is null
                Value val = value_stack_pop();
                Value result = value_create_boolean(val.type == VALUE_NULL);
                value_free(&val);
                value_stack_push(result);
                pc++;
                break;
            }
            
            case BC_ARRAY_PUSH: {
                // Push value to array (requires array and value on stack)
                Value val = value_stack_pop();
                Value arr = value_stack_pop();
                if (arr.type == VALUE_ARRAY) {
                    // Use built-in array push function
                    Value args[2] = {arr, val};
                    Value result = builtin_array_push(interpreter, args, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                value_free(&arr);
                pc++;
                break;
            }
            
            case BC_ARRAY_POP: {
                // Pop value from array
                Value arr = value_stack_pop();
                if (arr.type == VALUE_ARRAY) {
                    // Use built-in array pop function
                    Value args[1] = {arr};
                    Value result = builtin_array_pop(interpreter, args, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                value_free(&arr);
                pc++;
                break;
            }
            
            case BC_ARRAY_CONTAINS: {
                // Check if array contains value
                Value search_val = value_stack_pop();
                Value arr = value_stack_pop();
                if (arr.type == VALUE_ARRAY) {
                    // Use built-in array contains function
                    Value args[2] = {arr, search_val};
                    Value result = builtin_array_contains(interpreter, args, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                value_free(&arr);
                value_free(&search_val);
                pc++;
                break;
            }
            
            case BC_ARRAY_INDEX_OF: {
                // Get index of value in array
                Value search_val = value_stack_pop();
                Value arr = value_stack_pop();
                if (arr.type == VALUE_ARRAY) {
                    // Use built-in array indexOf function
                    Value args[2] = {arr, search_val};
                    Value result = builtin_array_index_of(interpreter, args, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                value_free(&arr);
                value_free(&search_val);
                pc++;
                break;
            }
            
            case BC_ARRAY_JOIN: {
                // Array join: arr.join(separator)
                Value separator = value_stack_pop();
                Value array = value_stack_pop();
                
                if (array.type == VALUE_ARRAY) {
                    Value result = builtin_array_join(NULL, (Value[]){array, separator}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&array);
                value_free(&separator);
                pc++;
                break;
            }
            
            case BC_ARRAY_UNIQUE: {
                // Array unique: arr.unique()
                Value array = value_stack_pop();
                
                if (array.type == VALUE_ARRAY) {
                    Value result = builtin_array_unique(NULL, (Value[]){array}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&array);
                pc++;
                break;
            }
            
            case BC_ARRAY_SLICE: {
                // Array slice: arr.slice(start, end)
                Value end = value_stack_pop();
                Value start = value_stack_pop();
                Value array = value_stack_pop();
                
                if (array.type == VALUE_ARRAY) {
                    Value result = builtin_array_slice(NULL, (Value[]){array, start, end}, 3, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&array);
                value_free(&start);
                value_free(&end);
                pc++;
                break;
            }
            
            case BC_ARRAY_CONCAT_METHOD: {
                // Array concat method: arr.concat(other)
                Value other = value_stack_pop();
                Value array = value_stack_pop();
                
                if (array.type == VALUE_ARRAY) {
                    Value result = builtin_array_concat(NULL, (Value[]){array, other}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&array);
                value_free(&other);
                pc++;
                break;
            }
            
            case BC_CREATE_ARRAY: {
                // Create array from stack elements
                size_t element_count = instr->a;
                
                // Create array with initial capacity
                Value array_val = value_create_array(element_count > 0 ? element_count : 1);
                
                // Handle empty arrays
                if (element_count == 0) {
                    value_stack_push(array_val);
                    pc++;
                    break;
                }
                
                // Allocate temporary storage for elements
                Value* elements = shared_malloc_safe(element_count * sizeof(Value), "bytecode_vm", "BC_CREATE_ARRAY", 1);
                
                // Pop elements from stack (in reverse order)
                for (size_t i = 0; i < element_count; i++) {
                    elements[element_count - 1 - i] = value_stack_pop();
                }
                
                // Push elements into the array
                for (size_t i = 0; i < element_count; i++) {
                    value_array_push(&array_val, elements[i]);
                }
                
                value_stack_push(array_val);
                
                // Free the elements array (values are now owned by the array)
                shared_free_safe(elements, "bytecode_vm", "BC_CREATE_ARRAY", 2);
                pc++;
                break;
            }
            
            case BC_ARRAY_CONCAT: {
                // Array concatenation: arr1 + arr2
                Value arr2 = value_stack_pop();
                Value arr1 = value_stack_pop();
                
                if (arr1.type == VALUE_ARRAY && arr2.type == VALUE_ARRAY) {
                    // Create new array with combined capacity
                    size_t total_capacity = arr1.data.array_value.capacity + arr2.data.array_value.capacity;
                    Value result = value_create_array(total_capacity);
                    
                    // Copy elements from first array
                    for (size_t i = 0; i < arr1.data.array_value.count; i++) {
                        Value* element = (Value*)arr1.data.array_value.elements[i];
                        Value cloned_element = value_clone(element);
                        value_array_push(&result, cloned_element);
                    }
                    
                    // Copy elements from second array
                    for (size_t i = 0; i < arr2.data.array_value.count; i++) {
                        Value* element = (Value*)arr2.data.array_value.elements[i];
                        Value cloned_element = value_clone(element);
                        value_array_push(&result, cloned_element);
                    }
                    
                    value_stack_push(result);
                } else {
                    // Fallback to AST for non-array types
                    value_stack_push(value_create_null());
                }
                
                value_free(&arr1);
                value_free(&arr2);
                pc++;
                break;
            }
            
            case BC_CREATE_OBJECT: {
                // Create object from key-value pairs on stack
                size_t pair_count = instr->a;
                Value object_val = value_create_object(pair_count > 0 ? pair_count : 4);
                
                // Pop key-value pairs from stack (in reverse order)
                for (size_t i = 0; i < pair_count; i++) {
                    Value value = value_stack_pop();
                    Value key = value_stack_pop();
                    
                    if (key.type == VALUE_STRING) {
                        value_object_set(&object_val, key.data.string_value, value);
                    }
                    
                    value_free(&key);
                    value_free(&value);
                }
                
                value_stack_push(object_val);
                pc++;
                break;
            }
            
            case BC_IMPORT_LIB: {
                // Import library: use library_name
                if (instr->a < program->const_count && program->constants[instr->a].type == VALUE_STRING) {
                    const char* library_name = program->constants[instr->a].data.string_value;
                    
                    // Get the library from global environment (it should already be registered)
                    Value lib = environment_get(interpreter->global_environment, library_name);
                    
                    // Define it in the current environment
                    environment_define(interpreter->current_environment, library_name, lib);
                    
                    // Push null result (use statements don't return a value)
                    value_stack_push(value_create_null());
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            
            case BC_STRING_UPPER: {
                // Convert string to uppercase
                Value val = value_stack_pop();
                if (val.type == VALUE_STRING) {
                    char* upper = strdup(val.data.string_value);
                    for (char* p = upper; *p; p++) {
                        *p = toupper(*p);
                    }
                    Value result = value_create_string(upper);
                    free(upper);
                    value_free(&val);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_STRING_LOWER: {
                // Convert string to lowercase
                Value val = value_stack_pop();
                if (val.type == VALUE_STRING) {
                    char* lower = strdup(val.data.string_value);
                    for (char* p = lower; *p; p++) {
                        *p = tolower(*p);
                    }
                    Value result = value_create_string(lower);
                    free(lower);
                    value_free(&val);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_STRING_TRIM: {
                // Trim string whitespace
                Value val = value_stack_pop();
                if (val.type == VALUE_STRING) {
                    char* str = val.data.string_value;
                    char* start = str;
                    char* end = str + strlen(str) - 1;
                    
                    // Trim leading whitespace
                    while (start <= end && isspace(*start)) start++;
                    
                    // Trim trailing whitespace
                    while (end >= start && isspace(*end)) end--;
                    
                    // Create trimmed string
                    size_t len = end - start + 1;
                    char* trimmed = malloc(len + 1);
                    strncpy(trimmed, start, len);
                    trimmed[len] = '\0';
                    
                    Value result = value_create_string(trimmed);
                    free(trimmed);
                    value_free(&val);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                pc++;
                break;
            }
            
            case BC_MATH_ABS: {
                // Math abs: math.abs(value)
                Value value = value_stack_pop();
                
                if (value.type == VALUE_NUMBER) {
                    Value result = builtin_math_abs(NULL, (Value[]){value}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&value);
                pc++;
                break;
            }
            
            case BC_MATH_SQRT: {
                // Math sqrt: math.sqrt(value)
                Value value = value_stack_pop();
                
                if (value.type == VALUE_NUMBER) {
                    Value result = builtin_math_sqrt(NULL, (Value[]){value}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&value);
                pc++;
                break;
            }
            
            case BC_MATH_POW: {
                // Math pow: math.pow(base, exponent)
                Value exponent = value_stack_pop();
                Value base = value_stack_pop();
                
                if (base.type == VALUE_NUMBER && exponent.type == VALUE_NUMBER) {
                    Value result = builtin_math_pow(NULL, (Value[]){base, exponent}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&base);
                value_free(&exponent);
                pc++;
                break;
            }
            
            case BC_MATH_SIN: {
                // Math sin: math.sin(value)
                Value value = value_stack_pop();
                
                if (value.type == VALUE_NUMBER) {
                    Value result = builtin_math_sin(NULL, (Value[]){value}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&value);
                pc++;
                break;
            }
            
            case BC_MATH_COS: {
                // Math cos: math.cos(value)
                Value value = value_stack_pop();
                
                if (value.type == VALUE_NUMBER) {
                    Value result = builtin_math_cos(NULL, (Value[]){value}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&value);
                pc++;
                break;
            }
            
            case BC_MATH_TAN: {
                // Math tan: math.tan(value)
                Value value = value_stack_pop();
                
                if (value.type == VALUE_NUMBER) {
                    Value result = builtin_math_tan(NULL, (Value[]){value}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&value);
                pc++;
                break;
            }
            
            case BC_MATH_FLOOR: {
                // Math floor: math.floor(value)
                Value value = value_stack_pop();
                
                if (value.type == VALUE_NUMBER) {
                    Value result = builtin_math_floor(NULL, (Value[]){value}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&value);
                pc++;
                break;
            }
            
            case BC_MATH_CEIL: {
                // Math ceil: math.ceil(value)
                Value value = value_stack_pop();
                
                if (value.type == VALUE_NUMBER) {
                    Value result = builtin_math_ceil(NULL, (Value[]){value}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&value);
                pc++;
                break;
            }
            
            case BC_MATH_ROUND: {
                // Math round: math.round(value)
                Value value = value_stack_pop();
                
                if (value.type == VALUE_NUMBER) {
                    Value result = builtin_math_round(NULL, (Value[]){value}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&value);
                pc++;
                break;
            }
            
            case BC_MAP_HAS: {
                // Map has key check: map.has(key)
                Value key = value_stack_pop();
                Value map = value_stack_pop();
                
                if (map.type == VALUE_HASH_MAP) {
                    Value result = builtin_map_has(NULL, (Value[]){map, key}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_boolean(false));
                }
                
                value_free(&map);
                value_free(&key);
                pc++;
                break;
            }
            
            case BC_MAP_SIZE: {
                // Map size property: map.size
                Value map = value_stack_pop();
                
                if (map.type == VALUE_HASH_MAP) {
                    Value result = builtin_map_size(NULL, (Value[]){map}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_number(0));
                }
                
                value_free(&map);
                pc++;
                break;
            }
            
            case BC_MAP_KEYS: {
                // Map keys method: map.keys()
                Value map = value_stack_pop();
                
                if (map.type == VALUE_HASH_MAP) {
                    Value result = builtin_map_keys(NULL, (Value[]){map}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_array(0));
                }
                
                value_free(&map);
                pc++;
                break;
            }
            
            case BC_MAP_DELETE: {
                // Map delete method: map.delete(key)
                Value key = value_stack_pop();
                Value map = value_stack_pop();
                
                if (map.type == VALUE_HASH_MAP) {
                    Value result = builtin_map_delete(NULL, (Value[]){map, key}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&map);
                value_free(&key);
                pc++;
                break;
            }
            
            case BC_MAP_CLEAR: {
                // Map clear method: map.clear()
                Value map = value_stack_pop();
                
                if (map.type == VALUE_HASH_MAP) {
                    Value result = builtin_map_clear(NULL, (Value[]){map}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&map);
                pc++;
                break;
            }
            
            case BC_MAP_UPDATE: {
                // Map update method: map.update(other_map)
                Value other_map = value_stack_pop();
                Value map = value_stack_pop();
                
                if (map.type == VALUE_HASH_MAP) {
                    Value result = builtin_map_update(NULL, (Value[]){map, other_map}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&map);
                value_free(&other_map);
                pc++;
                break;
            }
            
            case BC_SET_ADD: {
                // Set add method: set.add(element)
                Value element = value_stack_pop();
                Value set = value_stack_pop();
                
                if (set.type == VALUE_SET) {
                    Value result = builtin_set_add(NULL, (Value[]){set, element}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&set);
                value_free(&element);
                pc++;
                break;
            }
            
            case BC_SET_HAS: {
                // Set has element check: set.has(element)
                Value element = value_stack_pop();
                Value set = value_stack_pop();
                
                if (set.type == VALUE_SET) {
                    Value result = builtin_set_has(NULL, (Value[]){set, element}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_boolean(false));
                }
                
                value_free(&set);
                value_free(&element);
                pc++;
                break;
            }
            
            case BC_SET_REMOVE: {
                // Set remove method: set.remove(element)
                Value element = value_stack_pop();
                Value set = value_stack_pop();
                
                if (set.type == VALUE_SET) {
                    Value result = builtin_set_remove(NULL, (Value[]){set, element}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&set);
                value_free(&element);
                pc++;
                break;
            }
            
            case BC_SET_SIZE: {
                // Set size property: set.size
                Value set = value_stack_pop();
                
                if (set.type == VALUE_SET) {
                    Value result = builtin_set_size(NULL, (Value[]){set}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_number(0));
                }
                
                value_free(&set);
                pc++;
                break;
            }
            
            case BC_SET_CLEAR: {
                // Set clear method: set.clear()
                Value set = value_stack_pop();
                
                if (set.type == VALUE_SET) {
                    Value result = builtin_set_clear(NULL, (Value[]){set}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&set);
                pc++;
                break;
            }
            
            case BC_SET_TO_ARRAY: {
                // Set toArray method: set.toArray()
                Value set = value_stack_pop();
                
                if (set.type == VALUE_SET) {
                    Value result = builtin_set_to_array(NULL, (Value[]){set}, 1, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&set);
                pc++;
                break;
            }
            
            case BC_SET_UNION: {
                // Set union method: set.union(other_set)
                Value other_set = value_stack_pop();
                Value set = value_stack_pop();
                
                if (set.type == VALUE_SET) {
                    Value result = builtin_set_union(NULL, (Value[]){set, other_set}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&set);
                value_free(&other_set);
                pc++;
                break;
            }
            
            case BC_SET_INTERSECTION: {
                // Set intersection method: set.intersection(other_set)
                Value other_set = value_stack_pop();
                Value set = value_stack_pop();
                
                if (set.type == VALUE_SET) {
                    Value result = builtin_set_intersection(NULL, (Value[]){set, other_set}, 2, 0, 0);
                    value_stack_push(result);
                } else {
                    value_stack_push(value_create_null());
                }
                
                value_free(&set);
                value_free(&other_set);
                pc++;
                break;
            }
            
            case BC_EVAL_AST: {
                // Fallback to AST evaluation
                if (instr->a < program->ast_count) {
                    ASTNode* node = program->ast_nodes[instr->a];
                    if (node) {
                        // Synchronize bytecode variables with AST interpreter environment
                        // This allows AST fallbacks to access bytecode-defined variables
                        for (size_t i = 0; i < program->local_count && i < program->local_slot_count; i++) {
                            if (program->local_names[i]) {
                                Value local_val = program->locals[i];
                                // Check if variable exists in environment
                                if (environment_exists(interpreter->current_environment, program->local_names[i])) {
                                    // Update existing variable
                                    environment_assign(interpreter->current_environment, program->local_names[i], value_clone(&local_val));
                                } else {
                                    // Define new variable
                                    environment_define(interpreter->current_environment, program->local_names[i], value_clone(&local_val));
                                }
                            }
                        }
                        
                        Value result = interpreter_execute(interpreter, node);
                        
                        // Synchronize back from AST environment to bytecode locals
                        // This allows modifications in AST fallbacks to be visible in bytecode
                        for (size_t i = 0; i < program->local_count && i < program->local_slot_count; i++) {
                            if (program->local_names[i]) {
                                Value env_val = environment_get(interpreter->current_environment, program->local_names[i]);
                                if (env_val.type != VALUE_NULL) {
                                    value_free(&program->locals[i]);
                                    program->locals[i] = value_clone(&env_val);
                                }
                            }
                        }
                        
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
            
            case BC_NUM_TO_VALUE: {
                double num = num_stack_pop();
                value_stack_push(value_create_number(num));
                pc++;
                break;
            }
            
            
            case BC_INC_LOCAL: {
                if (instr->a < program->num_local_count) {
                    program->num_locals[instr->a] += 1.0;
                }
                pc++;
                break;
            }
            
            case BC_ADD_LOCAL_IMM: {
                if (instr->a < program->num_local_count && instr->b < program->num_const_count) {
                    program->num_locals[instr->a] += program->num_constants[instr->b];
                    
                    // Also update the value locals array for consistency
                    if (instr->a < program->local_slot_count) {
                        value_free(&program->locals[instr->a]);
                        program->locals[instr->a] = value_create_number(program->num_locals[instr->a]);
                    }
                }
                pc++;
                break;
            }
            
            case BC_ADD_LLL: {
                double c = num_stack_pop();
                double b = num_stack_pop();
                double a = num_stack_pop();
                num_stack_push(a + b + c);
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
