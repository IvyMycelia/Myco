#include "interpreter/value_operations.h"
#include "../../include/core/interpreter.h"
#include "../../include/utils/shared_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ============================================================================
// EXPRESSION EVALUATION FUNCTIONS
// ============================================================================

Value eval_binary(Interpreter* interpreter, ASTNode* node) { 
    Value l = interpreter_execute(interpreter, node->data.binary.left); 
    Value r = interpreter_execute(interpreter, node->data.binary.right); 
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
            else res = 1; 
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
