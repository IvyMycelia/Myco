#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include "jit_compiler.h"
// #include "compile_time.h" // Temporarily disabled due to type system issues
#include <stddef.h>
#include <stdint.h>

// Include all modular interpreter headers
#include "interpreter/interpreter_core.h"
#include "interpreter/value_operations.h"
#include "interpreter/eval_engine.h"
#include "interpreter/method_handlers.h"

// All type definitions are now in the modular headers above

// All function declarations are now in the modular headers above

#endif // INTERPRETER_H
