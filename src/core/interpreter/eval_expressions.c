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
    // FAST PATH: Optimize arithmetic operations for performance-critical code
    // Try to evaluate operands quickly and do direct arithmetic when both are numbers
    
    ASTNode* left_node = node->data.binary.left;
    ASTNode* right_node = node->data.binary.right;
    
    // Fast path 1: Both are number literals (fastest - no evaluation needed)
    bool both_literals = (left_node && left_node->type == AST_NODE_NUMBER &&
                          right_node && right_node->type == AST_NODE_NUMBER);
    
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
    Value l = interpreter_execute(interpreter, node->data.binary.left); 
    Value r = interpreter_execute(interpreter, node->data.binary.right);
    
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
    Value operand = interpreter_execute(interpreter, node->data.unary.operand);
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
