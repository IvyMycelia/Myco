#ifndef MYCO_MATH_H
#define MYCO_MATH_H

#include "../core/interpreter.h"

// Math constants
extern const double MYCO_PI;
extern const double MYCO_E;
extern const double MYCO_TAU;
extern const double MYCO_SQRT2;
extern const double MYCO_SQRT3;
extern const double MYCO_PHI;

// Math functions
Value builtin_math_abs(Interpreter* interpreter, Value* args, size_t arg_count);
Value builtin_math_min(Interpreter* interpreter, Value* args, size_t arg_count);
Value builtin_math_max(Interpreter* interpreter, Value* args, size_t arg_count);
Value builtin_math_sqrt(Interpreter* interpreter, Value* args, size_t arg_count);
Value builtin_math_pow(Interpreter* interpreter, Value* args, size_t arg_count);
Value builtin_math_round(Interpreter* interpreter, Value* args, size_t arg_count);
Value builtin_math_floor(Interpreter* interpreter, Value* args, size_t arg_count);
Value builtin_math_ceil(Interpreter* interpreter, Value* args, size_t arg_count);
Value builtin_math_sin(Interpreter* interpreter, Value* args, size_t arg_count);
Value builtin_math_cos(Interpreter* interpreter, Value* args, size_t arg_count);
Value builtin_math_tan(Interpreter* interpreter, Value* args, size_t arg_count);

// Math library registration
void math_library_register(Interpreter* interpreter);

#endif // MYCO_MATH_H
