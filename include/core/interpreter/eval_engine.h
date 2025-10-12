#ifndef EVAL_ENGINE_H
#define EVAL_ENGINE_H

#include "interpreter_core.h"
#include <stddef.h>

// ============================================================================
// EXPRESSION EVALUATION FUNCTIONS
// ============================================================================

// Main evaluation function
Value interpreter_execute(Interpreter* interpreter, ASTNode* node);

// Binary operation evaluation
Value eval_binary(Interpreter* interpreter, ASTNode* node);

// Unary operation evaluation
Value eval_unary(Interpreter* interpreter, ASTNode* node);

// ============================================================================
// CONTROL FLOW EVALUATION FUNCTIONS
// ============================================================================

// Control flow statements
Value eval_if_statement(Interpreter* interpreter, ASTNode* node);
Value eval_while_loop(Interpreter* interpreter, ASTNode* node);
Value eval_for_loop(Interpreter* interpreter, ASTNode* node);
Value eval_return_statement(Interpreter* interpreter, ASTNode* node);
Value eval_throw_statement(Interpreter* interpreter, ASTNode* node);
Value eval_try_catch(Interpreter* interpreter, ASTNode* node);
Value eval_block(Interpreter* interpreter, ASTNode* node);

// ============================================================================
// CORE EVALUATION FUNCTIONS
// ============================================================================

// Main AST evaluation dispatcher
Value eval_node(Interpreter* interpreter, ASTNode* node);

#endif // EVAL_ENGINE_H
