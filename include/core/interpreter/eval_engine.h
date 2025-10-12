#ifndef EVAL_ENGINE_H
#define EVAL_ENGINE_H

#include "../interpreter.h"
#include <stddef.h>

// ============================================================================
// EXPRESSION EVALUATION FUNCTIONS
// ============================================================================

// Binary operation evaluation
Value eval_binary(Interpreter* interpreter, ASTNode* node);

// Unary operation evaluation
Value eval_unary(Interpreter* interpreter, ASTNode* node);

#endif // EVAL_ENGINE_H
