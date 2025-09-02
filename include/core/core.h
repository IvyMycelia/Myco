#ifndef MYCO_CORE_H
#define MYCO_CORE_H

/**
 * @file core.h
 * @brief Core language module - fundamental components that form the foundation of Myco
 * 
 * This module contains the essential building blocks of the Myco programming language:
 * - Lexical analysis (tokenization)
 * - Syntax analysis (parsing)
 * - Abstract Syntax Tree representation
 * - Language interpretation and execution
 * 
 * These components work together to transform source code into executable programs.
 * The core module is the heart of the language and should be the first place you
 * look when adding new language features or modifying existing syntax.
 */

// Core language components
#include "lexer.h"      // Tokenizes source code into meaningful units
#include "parser.h"     // Converts tokens into an Abstract Syntax Tree
#include "ast.h"        // Represents the program structure in memory
#include "interpreter.h" // Executes the AST directly

#endif // MYCO_CORE_H
