#include <math.h>
#include <stdlib.h>
#include "../../include/core/interpreter.h"
#include "../../include/core/ast.h"

// Math library constants
static const double MATH_PI = 3.141592653589793;
static const double MATH_E = 2.718281828459045;
static const double MATH_TAU = 6.283185307179586;
static const double MATH_SQRT2 = 1.4142135623730951;
static const double MATH_SQRT3 = 1.7320508075688772;
static const double MATH_PHI = 1.618033988749895;

// Math library functions
Value builtin_math_abs(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "abs() requires exactly 1 argument", line, column);
        return value_create_null();
    }
    
    Value arg = args[0];
    if (arg.type == VALUE_NUMBER) {
        return value_create_number(fabs(arg.data.number_value));
    } else {
        interpreter_set_error(interpreter, "abs() argument must be a number", line, column);
        return value_create_null();
    }
}

// Math method functions (for number.method() syntax)
Value builtin_number_abs(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 0) {
        interpreter_set_error(interpreter, "abs() requires no arguments when called as a method", line, column);
        return value_create_null();
    }
    
    // The number value is passed as the object in method calls
    // This will be handled by the interpreter when calling number.method()
    return value_create_null(); // This should not be called directly
}

Value builtin_math_min(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "min() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    Value arg1 = args[0];
    Value arg2 = args[1];
    
    if (arg1.type != VALUE_NUMBER || arg2.type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "min() arguments must be numbers", line, column);
        return value_create_null();
    }
    
    return value_create_number(fmin(arg1.data.number_value, arg2.data.number_value));
}

Value builtin_math_max(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "max() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    Value arg1 = args[0];
    Value arg2 = args[1];
    
    if (arg1.type != VALUE_NUMBER || arg2.type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "max() arguments must be numbers", line, column);
        return value_create_null();
    }
    
    return value_create_number(fmax(arg1.data.number_value, arg2.data.number_value));
}

Value builtin_math_sqrt(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "sqrt() requires exactly 1 argument", line, column);
        return value_create_null();
    }
    
    Value arg = args[0];
    if (arg.type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "sqrt() argument must be a number", line, column);
        return value_create_null();
    }
    
    if (arg.data.number_value < 0) {
        interpreter_set_error(interpreter, "sqrt() argument cannot be negative", line, column);
        return value_create_null();
    }
    
    return value_create_number(sqrt(arg.data.number_value));
}

Value builtin_math_pow(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 2) {
        interpreter_set_error(interpreter, "pow() requires exactly 2 arguments", line, column);
        return value_create_null();
    }
    
    Value base = args[0];
    Value exponent = args[1];
    
    if (base.type != VALUE_NUMBER || exponent.type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "pow() arguments must be numbers", line, column);
        return value_create_null();
    }
    
    return value_create_number(pow(base.data.number_value, exponent.data.number_value));
}

Value builtin_math_round(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "round() requires exactly 1 argument", line, column);
        return value_create_null();
    }
    
    Value arg = args[0];
    if (arg.type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "round() argument must be a number", line, column);
        return value_create_null();
    }
    
    return value_create_number(round(arg.data.number_value));
}

Value builtin_math_floor(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "floor() requires exactly 1 argument", line, column);
        return value_create_null();
    }
    
    Value arg = args[0];
    if (arg.type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "floor() argument must be a number", line, column);
        return value_create_null();
    }
    
    return value_create_number(floor(arg.data.number_value));
}

Value builtin_math_ceil(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "ceil() requires exactly 1 argument", line, column);
        return value_create_null();
    }
    
    Value arg = args[0];
    if (arg.type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "ceil() argument must be a number", line, column);
        return value_create_null();
    }
    
    return value_create_number(ceil(arg.data.number_value));
}

Value builtin_math_sin(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "sin() requires exactly 1 argument", line, column);
        return value_create_null();
    }
    
    Value arg = args[0];
    if (arg.type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "sin() argument must be a number", line, column);
        return value_create_null();
    }
    
    return value_create_number(sin(arg.data.number_value));
}

Value builtin_math_cos(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "cos() requires exactly 1 argument", line, column);
        return value_create_null();
    }
    
    Value arg = args[0];
    if (arg.type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "cos() argument must be a number", line, column);
        return value_create_null();
    }
    
    return value_create_number(cos(arg.data.number_value));
}

Value builtin_math_tan(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count != 1) {
        interpreter_set_error(interpreter, "tan() requires exactly 1 argument", line, column);
        return value_create_null();
    }
    
    Value arg = args[0];
    if (arg.type != VALUE_NUMBER) {
        interpreter_set_error(interpreter, "tan() argument must be a number", line, column);
        return value_create_null();
    }
    
    return value_create_number(tan(arg.data.number_value));
}

// Register math library with interpreter
void math_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    // Register constants
    environment_define(interpreter->global_environment, "Pi", value_create_number(MATH_PI));
    environment_define(interpreter->global_environment, "E", value_create_number(MATH_E));
    environment_define(interpreter->global_environment, "Tau", value_create_number(MATH_TAU));
    environment_define(interpreter->global_environment, "Sqrt2", value_create_number(MATH_SQRT2));
    environment_define(interpreter->global_environment, "Sqrt3", value_create_number(MATH_SQRT3));
    environment_define(interpreter->global_environment, "Phi", value_create_number(MATH_PHI));
    
    // Register functions
    environment_define(interpreter->global_environment, "abs", value_create_builtin_function(builtin_math_abs));
    environment_define(interpreter->global_environment, "min", value_create_builtin_function(builtin_math_min));
    environment_define(interpreter->global_environment, "max", value_create_builtin_function(builtin_math_max));
    environment_define(interpreter->global_environment, "sqrt", value_create_builtin_function(builtin_math_sqrt));
    environment_define(interpreter->global_environment, "pow", value_create_builtin_function(builtin_math_pow));
    environment_define(interpreter->global_environment, "round", value_create_builtin_function(builtin_math_round));
    environment_define(interpreter->global_environment, "floor", value_create_builtin_function(builtin_math_floor));
    environment_define(interpreter->global_environment, "ceil", value_create_builtin_function(builtin_math_ceil));
    environment_define(interpreter->global_environment, "sin", value_create_builtin_function(builtin_math_sin));
    environment_define(interpreter->global_environment, "cos", value_create_builtin_function(builtin_math_cos));
    environment_define(interpreter->global_environment, "tan", value_create_builtin_function(builtin_math_tan));
}
