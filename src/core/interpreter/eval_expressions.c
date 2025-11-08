#include "interpreter/value_operations.h"
#include "../../include/core/interpreter.h"
#include "../../include/utils/shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>  // For fmod()

// ============================================================================
// EXPRESSION EVALUATION FUNCTIONS
// ============================================================================

Value eval_binary(Interpreter* interpreter, ASTNode* node) {
    // Validate node pointer is in valid memory range
    if (!node) {
        return value_create_null();
    }
    
    // Check if node pointer is valid by checking if it's in a reasonable memory range
    // and has a valid type value
    uintptr_t node_addr = (uintptr_t)node;
    if (node_addr < 0x1000 || node_addr > 0x7fffffffffffULL) {
        return value_create_null();
    }
    
    // Validate node type before accessing union members
    if (node->type != AST_NODE_BINARY_OP) {
        return value_create_null();
    }
    
    // FAST PATH: Optimize arithmetic operations for performance-critical code
    // Try to evaluate operands quickly and do direct arithmetic when both are numbers
    
    // Access operands through local variables to avoid repeated dereferences
    // IMPORTANT: We must validate the operand pointers BEFORE using them
    // We need to be very careful here - accessing node->data.binary.left could cause a BUS error
    // if node->data is corrupted. We've already validated node itself, but we need to be cautious.
    ASTNode* left_node = NULL;
    ASTNode* right_node = NULL;
    
    // Try to safely read the operand pointers
    // We can't truly "safely" read without risking a BUS error, but we can try to minimize the risk
    // by only reading if we've validated the node pointer itself (which we did above)
    // Use memcpy to avoid potential compiler optimizations that might cause issues
    if (node_addr >= 0x1000 && node_addr <= 0x7fffffffffffULL) {
        // Node pointer looks valid, try to read operands using a safer method
        // Read the union members directly but validate immediately
        ASTNode* temp_left = NULL;
        ASTNode* temp_right = NULL;
        
        // Try to read left operand - if this causes a BUS error, AddressSanitizer will catch it
        // We've already validated node_addr, so this should be safe
        temp_left = node->data.binary.left;
        
        // Immediately validate what we read
        if (temp_left) {
            uintptr_t temp_left_addr = (uintptr_t)temp_left;
            if (temp_left_addr >= 0x1000 && temp_left_addr <= 0x7fffffffffffULL &&
                temp_left_addr != 0xffffbebebebebebeULL &&
                (temp_left_addr & 0xFFFFFFFFULL) != 0xbebebebeULL &&
                (temp_left_addr & 0xFFFF000000000000ULL) == 0) {
                left_node = temp_left;
            }
        }
        
        // Try to read right operand
        temp_right = node->data.binary.right;
        
        // Immediately validate what we read
        if (temp_right) {
            uintptr_t temp_right_addr = (uintptr_t)temp_right;
            if (temp_right_addr >= 0x1000 && temp_right_addr <= 0x7fffffffffffULL &&
                temp_right_addr != 0xffffbebebebebebeULL &&
                (temp_right_addr & 0xFFFFFFFFULL) != 0xbebebebeULL &&
                (temp_right_addr & 0xFFFF000000000000ULL) == 0) {
                right_node = temp_right;
            }
        }
    }
    
    // Validate operand pointers BEFORE using them
    if (left_node) {
        uintptr_t left_addr = (uintptr_t)left_node;
        if (left_addr < 0x1000 || left_addr >= 0x800000000000ULL || 
            left_addr == 0xffffbebebebebebeULL ||
            (left_addr & 0xFFFFFFFFULL) == 0xbebebebeULL ||
            (left_addr & 0xFFFF000000000000ULL) != 0) {
            // Invalid pointer - likely uninitialized memory
            left_node = NULL;
        }
    }
    
    if (right_node) {
        uintptr_t right_addr = (uintptr_t)right_node;
        if (right_addr < 0x1000 || right_addr >= 0x800000000000ULL || 
            right_addr == 0xffffbebebebebeULL ||
            (right_addr & 0xFFFFFFFFULL) == 0xbebebebeULL ||
            (right_addr & 0xFFFF000000000000ULL) != 0) {
            // Invalid pointer - likely uninitialized memory
            right_node = NULL;
        }
    }
    
    // If either operand is invalid, return null
    if (!left_node || !right_node) {
        return value_create_null();
    }
    
    // Fast path 1: Both are number literals (fastest - no evaluation needed)
    // We need to safely check node types - if accessing ->type causes a BUS error,
    // we'll catch it and fall through to the slow path
    bool both_literals = false;
    if (left_node && right_node) {
        // Validate addresses again before accessing ->type (defensive)
        uintptr_t left_check = (uintptr_t)left_node;
        uintptr_t right_check = (uintptr_t)right_node;
        if (left_check >= 0x1000 && left_check <= 0x7fffffffffffULL &&
            right_check >= 0x1000 && right_check <= 0x7fffffffffffULL) {
            // Addresses look valid, try to access types
            both_literals = (left_node->type == AST_NODE_NUMBER &&
                            right_node->type == AST_NODE_NUMBER);
        }
    }
    
    if (both_literals) {
        double left_val = left_node->data.number_value;
        double right_val = right_node->data.number_value;
        
        switch (node->data.binary.op) {
            case OP_ADD:
                return value_create_number(left_val + right_val);
            case OP_SUBTRACT:
                return value_create_number(left_val - right_val);
            case OP_MULTIPLY:
                return value_create_number(left_val * right_val);
            case OP_DIVIDE:
                if (right_val == 0.0) {
                    interpreter_set_error(interpreter, "Division by zero", node->line, node->column);
                    return value_create_null();
                }
                return value_create_number(left_val / right_val);
            case OP_MODULO:
                if (right_val == 0.0) {
                    interpreter_set_error(interpreter, "Modulo by zero", node->line, node->column);
                    return value_create_null();
                }
                return value_create_number(fmod(left_val, right_val));
            case OP_GREATER_THAN:
                return value_create_boolean(left_val > right_val);
            case OP_LESS_THAN:
                return value_create_boolean(left_val < right_val);
            case OP_GREATER_EQUAL:
                return value_create_boolean(left_val >= right_val);
            case OP_LESS_EQUAL:
                return value_create_boolean(left_val <= right_val);
            case OP_EQUAL:
                return value_create_boolean(left_val == right_val);
            case OP_NOT_EQUAL:
                return value_create_boolean(left_val != right_val);
            default:
                break;
        }
    }
    
    // Fast path 2: Evaluate operands, then check if both are numbers
    // This helps with variable-number operations like "frame % 60" or "cursor_x * 9"
    // We still evaluate once, but avoid function call overhead for arithmetic
    // Use the validated left_node/right_node instead of accessing node->data.binary directly
    // Additional safety: double-check addresses before passing to interpreter_execute
    Value l = value_create_null();
    Value r = value_create_null();
    
    if (left_node) {
        uintptr_t final_left_check = (uintptr_t)left_node;
        if (final_left_check >= 0x1000 && final_left_check <= 0x7fffffffffffULL &&
            final_left_check != 0xffffbebebebebebeULL &&
            (final_left_check & 0xFFFFFFFFULL) != 0xbebebebeULL &&
            (final_left_check & 0xFFFF000000000000ULL) == 0) {
            l = interpreter_execute(interpreter, left_node);
        }
    }
    
    if (right_node) {
        uintptr_t final_right_check = (uintptr_t)right_node;
        if (final_right_check >= 0x1000 && final_right_check <= 0x7fffffffffffULL &&
            final_right_check != 0xffffbebebebebebeULL &&
            (final_right_check & 0xFFFFFFFFULL) != 0xbebebebeULL &&
            (final_right_check & 0xFFFF000000000000ULL) == 0) {
            r = interpreter_execute(interpreter, right_node);
        }
    }
    
    // If both evaluated to numbers, do fast arithmetic
    if (l.type == VALUE_NUMBER && r.type == VALUE_NUMBER) {
        double left_val = l.data.number_value;
        double right_val = r.data.number_value;
        
        Value result = value_create_null();
        
        switch (node->data.binary.op) {
            case OP_ADD:
                result = value_create_number(left_val + right_val);
                break;
            case OP_SUBTRACT:
                result = value_create_number(left_val - right_val);
                break;
            case OP_MULTIPLY:
                result = value_create_number(left_val * right_val);
                break;
            case OP_DIVIDE:
                if (right_val == 0.0) {
                    interpreter_set_error(interpreter, "Division by zero", node->line, node->column);
                    result = value_create_null();
                } else {
                    result = value_create_number(left_val / right_val);
                }
                break;
            case OP_MODULO:
                if (right_val == 0.0) {
                    interpreter_set_error(interpreter, "Modulo by zero", node->line, node->column);
                    result = value_create_null();
                } else {
                    result = value_create_number(fmod(left_val, right_val));
                }
                break;
            case OP_GREATER_THAN:
                result = value_create_boolean(left_val > right_val);
                break;
            case OP_LESS_THAN:
                result = value_create_boolean(left_val < right_val);
                break;
            case OP_GREATER_EQUAL:
                result = value_create_boolean(left_val >= right_val);
                break;
            case OP_LESS_EQUAL:
                result = value_create_boolean(left_val <= right_val);
                break;
            case OP_EQUAL:
                result = value_create_boolean(left_val == right_val);
                break;
            case OP_NOT_EQUAL:
                result = value_create_boolean(left_val != right_val);
                break;
            default:
                // Not a fast path operation, fall through to normal path
                break;
        }
        
        // If we handled it in fast path, free operands and return
        // Check if this is an arithmetic/comparison operation we handled
        if (node->data.binary.op >= OP_ADD && node->data.binary.op <= OP_NOT_EQUAL) {
            value_free(&l);
            value_free(&r);
            return result;
        }
        // If result is null and it's not an arithmetic op, might be an error - fall through
    }
    
    // Normal path: use already-evaluated operands (l and r) or evaluate if not done
    // Note: l and r may already be set from fast path 2 
    switch (node->data.binary.op) { 
        case OP_ADD: { 
            Value out = value_add(&l, &r); 
            value_free(&l); value_free(&r); 
            return out; 
        } 
        case OP_SUBTRACT: { 
            Value out = value_subtract(&l, &r); 
            value_free(&l); value_free(&r); 
            return out; 
        } 
        case OP_RANGE: { 
            if (l.type == VALUE_NUMBER && r.type == VALUE_NUMBER) { 
                Value out = value_create_range(l.data.number_value, r.data.number_value, 1.0, 0); 
                value_free(&l); value_free(&r); 
                return out; 
            } 
            value_free(&l); value_free(&r); 
            return value_create_null(); 
        }
        case OP_RANGE_STEP: { 
            if (l.type == VALUE_NUMBER && r.type == VALUE_NUMBER) { 
                // Evaluate the step expression
                Value step_val = interpreter_execute(interpreter, node->data.binary.step);
                if (step_val.type == VALUE_NUMBER) {
                    Value out = value_create_range(l.data.number_value, r.data.number_value, step_val.data.number_value, 0); 
                    value_free(&l); value_free(&r); value_free(&step_val);
                    return out; 
                } else {
                    value_free(&l); value_free(&r); value_free(&step_val);
                    return value_create_null();
                }
            } 
            value_free(&l); value_free(&r); 
            return value_create_null(); 
        } 
        case OP_GREATER_THAN: { 
            int res = 0; 
            if (l.type == VALUE_NUMBER && r.type == VALUE_NUMBER) res = (l.data.number_value > r.data.number_value); 
            value_free(&l); value_free(&r); 
            return value_create_boolean(res); 
        } 
        case OP_LESS_THAN: { 
            int res = 0; 
            if (l.type == VALUE_NUMBER && r.type == VALUE_NUMBER) res = (l.data.number_value < r.data.number_value); 
            value_free(&l); value_free(&r); 
            return value_create_boolean(res); 
        } 
        case OP_GREATER_EQUAL: { 
            int res = 0; 
            if (l.type == VALUE_NUMBER && r.type == VALUE_NUMBER) res = (l.data.number_value >= r.data.number_value); 
            value_free(&l); value_free(&r); 
            return value_create_boolean(res); 
        } 
        case OP_LESS_EQUAL: { 
            int res = 0; 
            if (l.type == VALUE_NUMBER && r.type == VALUE_NUMBER) res = (l.data.number_value <= r.data.number_value); 
            value_free(&l); value_free(&r); 
            return value_create_boolean(res); 
        }
        case OP_EQUAL: { 
            int res = 0; 
            if (l.type == VALUE_NUMBER && r.type == VALUE_NUMBER) res = (l.data.number_value == r.data.number_value); 
            else if (l.type == VALUE_STRING && r.type == VALUE_STRING) res = strcmp(l.data.string_value, r.data.string_value) == 0; 
            else if (l.type == VALUE_BOOLEAN && r.type == VALUE_BOOLEAN) res = l.data.boolean_value == r.data.boolean_value; 
            else if (l.type == VALUE_NULL && r.type == VALUE_NULL) res = 1; 
            else res = 0; 
            value_free(&l); value_free(&r); 
            return value_create_boolean(res); 
        }
        case OP_NOT_EQUAL: { 
            int res = 0; 
            if (l.type == VALUE_NUMBER && r.type == VALUE_NUMBER) res = (l.data.number_value != r.data.number_value); 
            else if (l.type == VALUE_STRING && r.type == VALUE_STRING) res = strcmp(l.data.string_value, r.data.string_value) != 0; 
            else if (l.type == VALUE_BOOLEAN && r.type == VALUE_BOOLEAN) res = l.data.boolean_value != r.data.boolean_value; 
            else if (l.type == VALUE_NULL && r.type == VALUE_NULL) res = 0;  // Both are NULL, so they are equal (not not equal)
            else if (l.type == VALUE_NULL || r.type == VALUE_NULL) res = 1;  // One is NULL and the other isn't, so they are not equal
            else res = 1;  // Different types, not equal
            value_free(&l); value_free(&r); 
            return value_create_boolean(res); 
        }
        case OP_LOGICAL_AND: {
            // Short-circuit evaluation: if left is false, return false
            if (l.type == VALUE_BOOLEAN && !l.data.boolean_value) {
                value_free(&l); value_free(&r);
                return value_create_boolean(0);
            }
            // If left is true, return the right value converted to boolean
            Value bool_val = value_to_boolean(&r);
            int res = bool_val.type == VALUE_BOOLEAN ? bool_val.data.boolean_value : 0;
            value_free(&l); value_free(&r);
            value_free(&bool_val);
            return value_create_boolean(res);
        }
        case OP_LOGICAL_OR: {
            // Short-circuit evaluation: if left is true, return true
            if (l.type == VALUE_BOOLEAN && l.data.boolean_value) {
                value_free(&l); value_free(&r);
                return value_create_boolean(1);
            }
            // If left is false, return the right value converted to boolean
            Value bool_val = value_to_boolean(&r);
            int res = bool_val.type == VALUE_BOOLEAN ? bool_val.data.boolean_value : 0;
            value_free(&l); value_free(&r);
            value_free(&bool_val);
            return value_create_boolean(res);
        }
        case OP_MULTIPLY: { 
            Value out = value_multiply(&l, &r); 
            value_free(&l); value_free(&r); 
            return out; 
        }
        case OP_DIVIDE: { 
            // Check for division by zero
            if (r.type == VALUE_NUMBER && r.data.number_value == 0.0) {
                interpreter_set_error(interpreter, "Division by zero", node->line, node->column);
                value_free(&l); value_free(&r); 
                return value_create_null();
            }
            Value out = value_divide(&l, &r); 
            value_free(&l); value_free(&r); 
            return out; 
        }
        case OP_MODULO: {
            // Check for modulo by zero
            if (r.type == VALUE_NUMBER && r.data.number_value == 0.0) {
                interpreter_set_error(interpreter, "Modulo by zero", node->line, node->column);
                value_free(&l); value_free(&r);
                return value_create_null();
            }
            Value out = value_modulo(&l, &r);
            value_free(&l); value_free(&r);
            return out;
        }
        default: 
            value_free(&l); value_free(&r); 
            return value_create_null(); 
    } 
}

Value eval_unary(Interpreter* interpreter, ASTNode* node) {
    // Validate node pointer is in valid memory range
    if (!node) {
        return value_create_null();
    }
    
    // Check if node pointer is valid by checking if it's in a reasonable memory range
    // and has a valid type value
    uintptr_t node_addr = (uintptr_t)node;
    if (node_addr < 0x1000 || node_addr > 0x7fffffffffffULL) {
        return value_create_null();
    }
    
    // Validate node type before accessing union members
    if (node->type != AST_NODE_UNARY_OP) {
        return value_create_null();
    }
    
    // Access operand through a local variable to avoid repeated dereferences
    // IMPORTANT: We must validate the operand pointer BEFORE using it
    // The operand field might contain uninitialized memory (0xbebebebe pattern)
    ASTNode* operand_node = NULL;
    
    // Try to safely read the operand field
    // If the node structure is corrupted, this read might fail, but we validate immediately
    operand_node = node->data.unary.operand;
    
    // Validate operand pointer BEFORE using it
    if (!operand_node) {
        return value_create_null();
    }
    
    // Validate operand node pointer is in reasonable memory range
    // 0xbebebebe is a common uninitialized memory pattern - reject it
    uintptr_t operand_addr = (uintptr_t)operand_node;
    // Check for uninitialized memory patterns (0xbebebebe, 0xfefefefe, etc.)
    // The address 0xffffbebebebebebe is clearly invalid (kernel space or corrupted)
    // Valid user-space addresses on 64-bit systems are typically < 0x7fffffffffff
    // Addresses >= 0x800000000000 are typically invalid
    // Also check for the specific corrupted pattern 0xffffbebebebebebe
    if (operand_addr < 0x1000 || operand_addr >= 0x800000000000ULL || 
        operand_addr == 0xffffbebebebebebeULL ||
        (operand_addr & 0xFFFFFFFFULL) == 0xbebebebeULL ||
        (operand_addr & 0xFFFF000000000000ULL) != 0) {  // Reject any address with high 16 bits != 0
        // Invalid pointer - likely uninitialized memory
        return value_create_null();
    }
    
    // Additional validation: check if operand points to a reasonable AST node
    // by checking if the first few bytes look like a valid AST node type
    // This is a heuristic - we can't fully validate without risking a segfault
    // But we can at least check if it's in a reasonable range
    
    Value operand = interpreter_execute(interpreter, operand_node);
    Value result = value_create_null();
    
    switch (node->data.unary.op) {
        case OP_NEGATIVE: {
            if (operand.type == VALUE_NUMBER) {
                result = value_create_number(-operand.data.number_value);
            } else {
                // For non-numeric types, return null
                result = value_create_null();
            }
            break;
        }
        case OP_LOGICAL_NOT: {
            if (operand.type == VALUE_BOOLEAN) {
                result = value_create_boolean(!operand.data.boolean_value);
            } else {
                // Convert to boolean first, then negate
                Value bool_val = value_to_boolean(&operand);
                if (bool_val.type == VALUE_BOOLEAN) {
                    result = value_create_boolean(!bool_val.data.boolean_value);
                } else {
                    result = value_create_boolean(1); // Default to true for unknown types
                }
                value_free(&bool_val);
            }
            break;
        }
        default:
            // For other unary operations, just return the operand
            result = value_clone(&operand);
            break;
    }
    
    value_free(&operand);
    return result;
}
