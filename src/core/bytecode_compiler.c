#include "../../include/core/bytecode.h"
#include "../../include/utils/shared_utilities.h"
#include <string.h>
#include <stdio.h>

static void bc_emit(BytecodeProgram* p, BytecodeOp op, int a, int b) {
    if (p->count + 1 > p->capacity) {
        size_t new_cap = p->capacity ? p->capacity * 2 : 128;
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
        p->constants = shared_realloc_safe(p->constants, new_cap * sizeof(Value), "bytecode", "bc_add_const", 1);
        p->const_capacity = new_cap;
    }
    p->constants[p->const_count] = v; // Stored by value; execution will clone as needed
    return (int)p->const_count++;
}

static int bc_add_num_const(BytecodeProgram* p, double val) {
    if (p->num_const_count + 1 > p->num_const_capacity) {
        size_t new_cap = p->num_const_capacity ? p->num_const_capacity * 2 : 64;
        p->num_constants = shared_realloc_safe(p->num_constants, new_cap * sizeof(double), "bytecode", "bc_add_num_const", 1);
        p->num_const_capacity = new_cap;
    }
    p->num_constants[p->num_const_count] = val;
    return (int)p->num_const_count++;
}

static int bc_add_ast(BytecodeProgram* p, ASTNode* n) {
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

static void compile_node_to_function(BytecodeProgram* p, BytecodeFunction* func, ASTNode* n) {
    if (!n) return;
    
    switch (n->type) {
        case AST_NODE_NUMBER: {
            int const_idx = bc_add_num_const(p, n->data.number_value);
            bc_emit_to_function(func, BC_LOAD_NUM, const_idx, 0, 0);
        } break;
        case AST_NODE_STRING: {
            // String literals should be loaded as constants onto the value stack
            int const_idx = bc_add_const(p, value_create_string(n->data.string_value));
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
                // This is a regular variable - load as constant for now
                int const_idx = bc_add_const(p, value_create_string(n->data.identifier_value));
                bc_emit_to_function(func, BC_LOAD_CONST, const_idx, 0, 0);
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
                default:
                    // Fallback to AST evaluation
                    break;
            }
        } break;
        case AST_NODE_RETURN: {
            if (n->data.return_statement.value) {
                // Check if the return value is a number or string
                if (n->data.return_statement.value->type == AST_NODE_NUMBER) {
                    // Numeric return - compile to numeric stack and convert
                    compile_node_to_function(p, func, n->data.return_statement.value);
                    bc_emit_to_function(func, (BytecodeOp)BC_NUM_TO_VALUE, 0, 0, 0);
                } else {
                    // Non-numeric return - compile directly to value stack
                    compile_node_to_function(p, func, n->data.return_statement.value);
                }
            }
            bc_emit_to_function(func, (BytecodeOp)BC_RETURN, 1, 0, 0); // Return with value
        } break;
        case AST_NODE_BLOCK: {
            // Compile all statements in the block
            ASTNode* stmt = *n->data.block.statements;
            while (stmt) {
                compile_node_to_function(p, func, stmt);
                stmt = stmt->next;
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
                
                for (size_t i = 0; i < p->function_count; i++) {
                    if (p->functions[i].name) {
                        if (strcmp(p->functions[i].name, n->data.function_call.function_name) == 0) {
                            func_id = (int)i;
                            break;
                        }
                    }
                }
                
                if (func_id >= 0) {
                    // Compile arguments
                    for (size_t i = 0; i < n->data.function_call.argument_count; i++) {
                        compile_node_to_function(p, func, n->data.function_call.arguments[i]);
                    }
                    // Emit user function call
                    bc_emit_to_function(func, BC_CALL_USER_FUNCTION, func_id, (int)n->data.function_call.argument_count, 0);
                } else {
                    // Function not found, fall back to AST
                    int id = bc_add_ast(p, n);
                    bc_emit_to_function(func, BC_EVAL_AST, id, 0, 0);
                }
            }
        } break;
        case AST_NODE_FUNCTION_CALL_EXPR: {
            // Method call expression: obj.method(args)
            if (n->data.function_call_expr.function->type == AST_NODE_MEMBER_ACCESS) {
                ASTNode* member_access = n->data.function_call_expr.function;
                const char* method_name = member_access->data.member_access.member_name;
                
                // Compile the object
                compile_node_to_function(p, func, member_access->data.member_access.object);
                
                // Handle toString specially
                if (strcmp(method_name, "toString") == 0 && n->data.function_call_expr.argument_count == 0) {
                    // Convert number to value if needed, then call toString
                    bc_emit_to_function(func, BC_NUM_TO_VALUE, 0, 0, 0);
                    bc_emit_to_function(func, BC_TO_STRING, 0, 0, 0);
                } else {
                    // Fall back to AST for complex method calls
                    int id = bc_add_ast(p, n);
                    bc_emit_to_function(func, BC_EVAL_AST, id, 0, 0);
                }
            } else {
                // Fall back to AST
                int id = bc_add_ast(p, n);
                bc_emit_to_function(func, BC_EVAL_AST, id, 0, 0);
            }
        } break;
        default:
            // For truly unsupported nodes, fall back to AST
            break;
    }
}

static int bc_add_function(BytecodeProgram* p, ASTNode* func) {
    if (p->function_count + 1 > p->function_capacity) {
        size_t new_cap = p->function_capacity ? p->function_capacity * 2 : 64;
        p->functions = shared_realloc_safe(p->functions, new_cap * sizeof(BytecodeFunction), "bytecode", "bc_add_function", 1);
        p->function_capacity = new_cap;
    }
    
    BytecodeFunction* bc_func = &p->functions[p->function_count];
    memset(bc_func, 0, sizeof(BytecodeFunction));
    
    // Store function name
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
    
    // Allocate space for function bytecode
    bc_func->code_capacity = 64;
    bc_func->code = shared_malloc_safe(bc_func->code_capacity * sizeof(BytecodeInstruction), "bytecode", "bc_add_function", 3);
    
    // Compile function body to bytecode
    if (func->data.function_definition.body) {
        compile_node_to_function(p, bc_func, func->data.function_definition.body);
    }
    
    return (int)p->function_count++;
}

BytecodeProgram* bytecode_program_create(void) {
    BytecodeProgram* p = shared_malloc_safe(sizeof(BytecodeProgram), "bytecode", "create", 1);
    if (!p) return NULL;
    memset(p, 0, sizeof(*p));
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
    shared_free_safe(p->ast_nodes, "bytecode", "free", 3);
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
    if (n->type != AST_NODE_BINARY_OP) return false;
    
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
                bc_emit_super(p, BC_VALUE_TO_NUM, 0, 0, 0);
            } else {
                // Fall back to value operations for globals
                compile_node(p, n);
                bc_emit_super(p, BC_VALUE_TO_NUM, 0, 0, 0);
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
                    default: {
                        // Convert to value and fall back
                        bc_emit_super(p, BC_NUM_TO_VALUE, 0, 0, 0);
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
    // Check for array concatenation FIRST (before numeric check)
    if (n->data.binary.op == OP_ADD && 
        is_array_literal_or_identifier(n->data.binary.left) && 
        is_array_literal_or_identifier(n->data.binary.right)) {
        // Array concatenation: arr1 + arr2
        compile_node(p, n->data.binary.left);
        compile_node(p, n->data.binary.right);
        bc_emit(p, BC_ARRAY_CONCAT, 0, 0);
    }
    // Check if this is a pure numeric operation
    else if (is_numeric_binary_op(p, n)) {
        // Use fast numeric operations
        compile_numeric_node(p, n->data.binary.left);
        compile_numeric_node(p, n->data.binary.right);
        
        switch (n->data.binary.op) {
            case OP_ADD: bc_emit(p, BC_ADD_NUM, 0, 0); break;
            case OP_SUBTRACT: bc_emit(p, BC_SUB_NUM, 0, 0); break;
            case OP_MULTIPLY: bc_emit(p, BC_MUL_NUM, 0, 0); break;
            case OP_DIVIDE: bc_emit(p, BC_DIV_NUM, 0, 0); break;
            case OP_EQUAL: bc_emit(p, BC_EQ_NUM, 0, 0); break;
            case OP_NOT_EQUAL: bc_emit(p, BC_NE_NUM, 0, 0); break;
            case OP_LESS_THAN: bc_emit(p, BC_LT_NUM, 0, 0); break;
            case OP_LESS_EQUAL: bc_emit(p, BC_LE_NUM, 0, 0); break;
            case OP_GREATER_THAN: bc_emit(p, BC_GT_NUM, 0, 0); break;
            case OP_GREATER_EQUAL: bc_emit(p, BC_GE_NUM, 0, 0); break;
            default: {
                // Convert numeric result to value and fall back
                bc_emit_super(p, BC_NUM_TO_VALUE, 0, 0, 0);
                int id = bc_add_ast(p, n);
                bc_emit(p, BC_EVAL_AST, id, 0);
            } break;
        }
        
        // Convert numeric result back to value for storage (only for arithmetic operations)
        if (n->data.binary.op == OP_ADD || n->data.binary.op == OP_SUBTRACT || 
            n->data.binary.op == OP_MULTIPLY || n->data.binary.op == OP_DIVIDE) {
            bc_emit_super(p, BC_NUM_TO_VALUE, 0, 0, 0);
        }
    } else {
        // Use regular value operations
        compile_node(p, n->data.binary.left);
        compile_node(p, n->data.binary.right);
        switch (n->data.binary.op) {
                case OP_ADD: bc_emit(p, BC_ADD, 0, 0); break;
                case OP_SUBTRACT: bc_emit(p, BC_SUB, 0, 0); break;
                case OP_MULTIPLY: bc_emit(p, BC_MUL, 0, 0); break;
                case OP_DIVIDE: bc_emit(p, BC_DIV, 0, 0); break;
                case OP_EQUAL: bc_emit(p, BC_EQ, 0, 0); break;
                case OP_NOT_EQUAL: bc_emit(p, BC_NE, 0, 0); break;
                case OP_LESS_THAN: bc_emit(p, BC_LT, 0, 0); break;
                case OP_LESS_EQUAL: bc_emit(p, BC_LE, 0, 0); break;
                case OP_GREATER_THAN: bc_emit(p, BC_GT, 0, 0); break;
                case OP_GREATER_EQUAL: bc_emit(p, BC_GE, 0, 0); break;
                default: {
                    int id = bc_add_ast(p, n);
                    bc_emit(p, BC_EVAL_AST, id, 0);
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
            // For complex patterns, fall back to AST evaluation
            int id = bc_add_ast(p, pattern);
            bc_emit(p, BC_EVAL_AST, id, 0);
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
        case AST_NODE_IDENTIFIER: {
            int idx = lookup_local(p, n->data.identifier_value);
            if (idx >= 0) {
                // Load from main locals array (supports all types)
                bc_emit(p, BC_LOAD_LOCAL, idx, 0);
            } else if (p->interpreter && environment_exists(p->interpreter->global_environment, n->data.identifier_value)) {
                // Global variable found - use BC_LOAD_GLOBAL
                int name_idx = bc_add_const(p, value_create_string(n->data.identifier_value));
                bc_emit(p, BC_LOAD_GLOBAL, name_idx, 0);
            } else {
                // Variable not found - fall back to AST evaluation
                int id = bc_add_ast(p, n);
                bc_emit(p, BC_EVAL_AST, id, 0);
            }
        } break;
        case AST_NODE_VARIABLE_DECLARATION: {
            const char* var_name = n->data.variable_declaration.variable_name;
            
            if (n->data.variable_declaration.initial_value) {
                compile_node(p, n->data.variable_declaration.initial_value);
            } else {
                // Initialize with null
                bc_emit(p, BC_LOAD_CONST, bc_add_const(p, value_create_null()), 0);
            }
            
            // For now, always treat variable declarations as local variables
            // This matches the AST interpreter behavior
            int idx = define_local(p, var_name);
            bc_emit(p, BC_STORE_LOCAL, idx, 0);
        } break;
        case AST_NODE_ASSIGNMENT: {
            const char* var = n->data.assignment.variable_name;
            int dst = define_local(p, var);
            ASTNode* v = n->data.assignment.value;
            
            
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
        } break;
        case AST_NODE_IF_STATEMENT: {
            // Compile condition
            compile_node(p, n->data.if_statement.condition);
            
            // Jump if false to else/end
            int jmp_false_pos = (int)p->count;
            bc_emit(p, BC_JUMP_IF_FALSE, 0, 0); // Placeholder, will be patched
            
            // Compile then block
            if (n->data.if_statement.then_block) {
                compile_node(p, n->data.if_statement.then_block);
            }
            
            // Jump to end (skip else block)
            int jmp_end_pos = (int)p->count;
            bc_emit(p, BC_JUMP, 0, 0); // Placeholder, will be patched
            
            // Patch the false jump to point to else block or end
            int else_start = (int)p->count;
            if (n->data.if_statement.else_block) {
                compile_node(p, n->data.if_statement.else_block);
            }
            int end_pos = (int)p->count;
            
            // Patch jumps
            p->code[jmp_false_pos].a = else_start;
            p->code[jmp_end_pos].a = end_pos;
        } break;
        case AST_NODE_BLOCK: {
            for (size_t i = 0; i < n->data.block.statement_count; i++) {
                ASTNode* stmt = n->data.block.statements[i];
                compile_node(p, stmt);
                // Pop only for statements that likely push a value
                if (stmt->type != AST_NODE_VARIABLE_DECLARATION &&
                    stmt->type != AST_NODE_ASSIGNMENT &&
                    stmt->type != AST_NODE_WHILE_LOOP &&
                    stmt->type != AST_NODE_BLOCK) {
                    bc_emit(p, BC_POP, 0, 0);
                }
            }
        } break;
        case AST_NODE_FUNCTION: {
            // Function definition - add to function table
            int func_id = bc_add_function(p, n);
            // Function body is already compiled in bc_add_function
            
            // Emit instruction to define function in environment
            int name_idx = bc_add_const(p, value_create_string(n->data.function_definition.function_name));
            bc_emit_super(p, BC_DEFINE_FUNCTION, name_idx, func_id, 0);
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
            // Fall back to AST evaluation for now due to crashes
            int id = bc_add_ast(p, n);
            bc_emit(p, BC_EVAL_AST, id, 0);
        } break;
        case AST_NODE_FUNCTION_CALL_EXPR: {
            // Method call: obj.method(args...)
            // Check if the function is a member access
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
            } else {
                // Regular function call - fall back to AST
                int id = bc_add_ast(p, n);
                bc_emit(p, BC_EVAL_AST, id, 0);
            }
        } break;
        case AST_NODE_USE: {
            // Use statements - import library
            const char* library_name = n->data.use_statement.library_name;
            int lib_name_idx = bc_add_const(p, value_create_string(library_name));
            bc_emit(p, BC_IMPORT_LIB, lib_name_idx, 0);
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
                    
                    // Check for common array methods
                    if (strcmp(method_name, "push") == 0 && n->data.function_call.argument_count == 1) {
                        // Compile the argument
                        compile_node(p, n->data.function_call.arguments[0]);
                        // Emit array push instruction
                        bc_emit(p, BC_ARRAY_PUSH, 0, 0);
                    } else if (strcmp(method_name, "pop") == 0 && n->data.function_call.argument_count == 0) {
                        // Emit array pop instruction
                        bc_emit(p, BC_ARRAY_POP, 0, 0);
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
                // Check if this is a class instantiation
                if (p->interpreter && environment_exists(p->interpreter->global_environment, n->data.function_call.function_name)) {
                    Value class_val = environment_get(p->interpreter->global_environment, n->data.function_call.function_name);
                    if (class_val.type == VALUE_CLASS) {
                        // This is a class instantiation
                        for (size_t i = 0; i < n->data.function_call.argument_count; i++) {
                            compile_node(p, n->data.function_call.arguments[i]);
                        }
                        int name_idx = bc_add_const(p, value_create_string(n->data.function_call.function_name));
                        bc_emit(p, BC_INSTANTIATE_CLASS, name_idx, (int)n->data.function_call.argument_count);
                    } else {
                        // Not a class, fall back to AST
                        int id = bc_add_ast(p, n);
                        bc_emit(p, BC_EVAL_AST, id, 0);
                    }
                } else {
                    // Look up existing function and call it
                    int func_id = -1;
                    
                    for (size_t i = 0; i < p->function_count; i++) {
                        if (p->functions[i].name) {
                            
                            if (strcmp(p->functions[i].name, n->data.function_call.function_name) == 0) {
                                func_id = (int)i;
                                
                                break;
                            }
                        }
                    }
                    if (func_id >= 0) {
                        // Compile arguments
                        for (size_t i = 0; i < n->data.function_call.argument_count; i++) {
                            compile_node(p, n->data.function_call.arguments[i]);
                        }
                        // Emit user function call
                        bc_emit(p, BC_CALL_USER_FUNCTION, func_id, (int)n->data.function_call.argument_count);
                    } else {
                        // Function not found, fall back to AST
                        int id = bc_add_ast(p, n);
                        bc_emit(p, BC_EVAL_AST, id, 0);
                    }
                }
            }
        } break;
        case AST_NODE_SPORE: {
            // Compile pattern matching (spore/match statement)
            // For now, fall back to AST evaluation
            // Pattern matching is complex and needs more work
            int id = bc_add_ast(p, n);
            bc_emit(p, BC_EVAL_AST, id, 0);
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
            // For loops are complex and need proper environment scoping
            // Fall back to AST evaluation to ensure variables are accessible
            int id = bc_add_ast(p, n);
            bc_emit(p, BC_EVAL_AST, id, 0);
        } break;
        default: {
            int id = bc_add_ast(p, n);
            bc_emit(p, BC_EVAL_AST, id, 0);
        } break;
    }
}

int bytecode_compile_program(BytecodeProgram* program, ASTNode* root, Interpreter* interpreter) {
    if (!program || !root) return 0;
    
    // Store interpreter reference for global variable access
    program->interpreter = interpreter;
    
    compile_node(program, root);
    bc_emit(program, BC_HALT, 0, 0);
    
    return 1;
}


