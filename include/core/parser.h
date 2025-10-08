#ifndef MYCO_PARSER_H
#define MYCO_PARSER_H

/**
 * @file parser.h
 * @brief Syntax Analysis Module - converts tokens into an Abstract Syntax Tree
 * 
 * The parser is the second step in processing Myco source code. It takes the
 * stream of tokens produced by the lexer and organizes them into a hierarchical
 * structure called an Abstract Syntax Tree (AST). The AST represents the logical
 * structure of the program and makes it easier for the interpreter or compiler
 * to understand and execute the code.
 * 
 * The parser implements a recursive descent parsing strategy, which means it
 * uses separate functions to parse different language constructs. This approach
 * makes the code easy to understand and modify when adding new language features.
 * 
 * Key responsibilities:
 * - Expression parsing with proper operator precedence
 * - Statement parsing (if, while, function calls, etc.)
 * - Error recovery and reporting
 * - Building a well-formed AST structure
 * 
 * When adding new language features, you'll typically need to:
 * 1. Add new parsing functions for the new syntax
 * 2. Update the precedence and associativity rules if needed
 * 3. Integrate the new parsing logic into the main parsing flow
 * 4. Ensure proper error handling and recovery
 */

#include "ast.h"        // For AST node definitions
#include "lexer.h"      // For token definitions

// Parser state and configuration
typedef struct {
    Lexer* lexer;           // The lexer providing tokens
    Token* current_token;   // Current token being processed
    Token* previous_token;  // Previously processed token
    int current_position;   // Current position in token stream
    int error_count;        // Number of parsing errors encountered
    char* error_message;    // Description of the last error
    int error_line;         // Line number where error occurred
    int error_column;       // Column number where error occurred
} Parser;

/**
 * @brief Initialize a new parser with a lexer
 * 
 * This function creates a parser that will process tokens from the given lexer.
 * The parser maintains its own state and error tracking separate from the lexer.
 * 
 * @param lexer The lexer that will provide tokens for parsing
 * @return A pointer to the initialized parser, or NULL if allocation failed
 * 
 * Usage example:
 * @code
 * Lexer* lexer = lexer_initialize(source_code);
 * Parser* parser = parser_initialize(lexer);
 * if (parser) {
 *     ASTNode* ast = parser_parse_program(parser);
 *     // Use the AST...
 *     parser_free(parser);
 * }
 * @endcode
 */
Parser* parser_initialize(Lexer* lexer);

/**
 * @brief Free all memory associated with a parser
 * 
 * This function cleans up the parser and any error messages it may have stored.
 * Note that this does NOT free the lexer - you need to free that separately.
 * 
 * @param parser The parser to free
 */
void parser_free(Parser* parser);

/**
 * @brief Parse the entire program and return the AST
 * 
 * This is the main entry point for parsing. It processes all tokens from the
 * lexer and produces a complete Abstract Syntax Tree representing the program.
 * 
 * @param parser The parser to use
 * @return Root node of the AST, or NULL if parsing failed
 * 
 * The returned AST is owned by the parser and will be freed when parser_free()
 * is called. If parsing fails, check parser->error_message for details.
 */
ASTNode* parser_parse_program(Parser* parser);

/**
 * @brief Parse a single statement from the token stream
 * 
 * This function parses one complete statement (like an if statement, assignment,
 * function call, etc.) and returns the corresponding AST node.
 * 
 * @param parser The parser to use
 * @return AST node representing the statement, or NULL if parsing failed
 * 
 * Statements are the top-level constructs in Myco programs. Each statement
 * represents a complete action or declaration.
 */
ASTNode* parser_parse_statement(Parser* parser);

/**
 * @brief Parse an expression from the token stream
 * 
 * This function parses expressions with proper operator precedence handling.
 * Expressions can be simple literals, variable references, or complex
 * combinations of operators and operands.
 * 
 * @param parser The parser to use
 * @return AST node representing the expression, or NULL if parsing failed
 * 
 * Expressions are the building blocks of statements and can be nested to
 * arbitrary depth. The parser automatically handles operator precedence
 * and associativity.
 */
ASTNode* parser_parse_expression(Parser* parser);

/**
 * @brief Parse an assignment expression
 * 
 * Assignment expressions have the form: variable = value
 * This function handles both simple assignments and compound assignments
 * like +=, -=, *=, etc.
 * 
 * @param parser The parser to use
 * @return AST node representing the assignment, or NULL if parsing failed
 */
ASTNode* parser_parse_assignment(Parser* parser);

/**
 * @brief Parse logical OR expressions (lowest precedence)
 * 
 * Logical OR expressions use the 'or' keyword and have the lowest precedence
 * among logical operators. This allows expressions like:
 * a or b and c  (parsed as: a or (b and c))
 * 
 * @param parser The parser to use
 * @return AST node representing the logical OR expression
 */
ASTNode* parser_parse_logical_or(Parser* parser);

/**
 * @brief Parse logical AND expressions
 * 
 * Logical AND expressions use the 'and' keyword and have higher precedence
 * than OR but lower than comparison operators.
 * 
 * @param parser The parser to use
 * @return AST node representing the logical AND expression
 */
ASTNode* parser_parse_logical_and(Parser* parser);

/**
 * @brief Parse equality comparison expressions
 * 
 * Equality comparisons (==, !=) have higher precedence than logical operators
 * but lower than relational comparisons.
 * 
 * @param parser The parser to use
 * @return AST node representing the equality comparison
 */
ASTNode* parser_parse_equality(Parser* parser);

/**
 * @brief Parse relational comparison expressions
 * 
 * Relational comparisons (<, >, <=, >=) have higher precedence than equality
 * but lower than arithmetic operators.
 * 
 * @param parser The parser to use
 * @return AST node representing the relational comparison
 */
ASTNode* parser_parse_comparison(Parser* parser);

/**
 * @brief Parse addition and subtraction expressions
 * 
 * Addition and subtraction (+ and -) have higher precedence than comparisons
 * but lower than multiplication and division.
 * 
 * @param parser The parser to use
 * @return AST node representing the addition/subtraction expression
 */
ASTNode* parser_parse_term(Parser* parser);

/**
 * @brief Parse multiplication and division expressions
 * 
 * Multiplication and division (* and /) have higher precedence than addition
 * but lower than power operations.
 * 
 * @param parser The parser to use
 * @return AST node representing the multiplication/division expression
 */
ASTNode* parser_parse_factor(Parser* parser);

/**
 * @brief Parse power expressions (highest precedence)
 * 
 * Power operations (**) have the highest precedence among arithmetic operators.
 * This allows expressions like: 2 ** 3 ** 4 (parsed as: 2 ** (3 ** 4))
 * 
 * @param parser The parser to use
 * @return AST node representing the power expression
 */
ASTNode* parser_parse_power(Parser* parser);

/**
 * @brief Parse unary expressions
 * 
 * Unary expressions are operators that apply to a single operand, such as
 * negation (-x), logical NOT (not x), or bitwise NOT (~x).
 * 
 * @param parser The parser to use
 * @return AST node representing the unary expression
 */
ASTNode* parser_parse_unary(Parser* parser);

/**
 * @brief Parse primary expressions
 * 
 * Primary expressions are the most basic building blocks: literals, identifiers,
 * parenthesized expressions, function calls, and member access.
 * 
 * @param parser The parser to use
 * @return AST node representing the primary expression
 */
ASTNode* parser_parse_primary(Parser* parser);

/**
 * @brief Parse function call expressions
 * 
 * Function calls have the form: function_name(arg1, arg2, ...)
 * This function handles the argument parsing and creates the appropriate AST node.
 * 
 * @param parser The parser to use
 * @return AST node representing the function call
 */
ASTNode* parser_parse_call(Parser* parser);

/**
 * @brief Parse member access expressions
 * 
 * Member access allows accessing properties or methods of objects:
 * object.property or object.method()
 * 
 * @param parser The parser to use
 * @return AST node representing the member access
 */
ASTNode* parser_parse_member_access(Parser* parser);

/**
 * @brief Parse array access expressions
 * 
 * Array access allows accessing elements by index: array[index]
 * 
 * @param parser The parser to use
 * @return AST node representing the array access
 */
ASTNode* parser_parse_array_access(Parser* parser);

/**
 * @brief Parse parenthesized expressions
 * 
 * Parentheses can be used to override operator precedence: (a + b) * c
 * 
 * @param parser The parser to use
 * @return AST node representing the parenthesized expression
 */
ASTNode* parser_parse_grouping(Parser* parser);

/**
 * @brief Parse literal values
 * 
 * Literals are the basic values in the language: numbers, strings, booleans,
 * null, and arrays/objects.
 * 
 * @param parser The parser to use
 * @return AST node representing the literal
 */
ASTNode* parser_parse_literal(Parser* parser);

// Statement parsing functions

/**
 * @brief Parse an expression statement
 * 
 * Expression statements are expressions followed by a semicolon or newline.
 * They're the most common type of statement in Myco programs.
 * 
 * @param parser The parser to use
 * @return AST node representing the expression statement
 */
ASTNode* parser_parse_expression_statement(Parser* parser);

/**
 * @brief Parse a declaration statement
 * 
 * Declaration statements introduce new variables, constants, or types:
 * let x = 42, const PI = 3.14, type Point = {x: Int, y: Int}
 * 
 * @param parser The parser to use
 * @return AST node representing the declaration
 */
ASTNode* parser_parse_declaration_statement(Parser* parser);

/**
 * @brief Parse a variable declaration
 * 
 * Variable declarations use the 'let' or 'var' keyword:
 * let x = 42, var mutable_var = "hello"
 * 
 * @param parser The parser to use
 * @return AST node representing the variable declaration
 */
char* parser_parse_type_annotation(Parser* parser);
ASTNode* parser_parse_variable_declaration(Parser* parser);

/**
 * @brief Parse a constant declaration
 * 
 * Constant declarations use the 'const' keyword and must have an initial value:
 * const MAX_SIZE = 100, const GREETING = "Hello, World!"
 * 
 * @param parser The parser to use
 * @return AST node representing the constant declaration
 */
ASTNode* parser_parse_constant_declaration(Parser* parser);

/**
 * @brief Parse a type declaration
 * 
 * Type declarations define new types or type aliases:
 * type Point = {x: Int, y: Int}, type StringArray = [String]
 * 
 * @param parser The parser to use
 * @return AST node representing the type declaration
 */
ASTNode* parser_parse_type_declaration(Parser* parser);

/**
 * @brief Parse an if statement
 * 
 * If statements have the form: if condition: block [else: block]
 * The else clause is optional.
 * 
 * @param parser The parser to use
 * @return AST node representing the if statement
 */
ASTNode* parser_parse_if_statement(Parser* parser);

/**
 * @brief Parse a switch statement
 * 
 * Switch statements provide multi-way branching:
 * switch value: case pattern1: block case pattern2: block end
 * 
 * @param parser The parser to use
 * @return AST node representing the switch statement
 */
ASTNode* parser_parse_switch_statement(Parser* parser);

/**
 * @brief Parse a while loop
 * 
 * While loops repeat a block while a condition is true:
 * while condition: block
 * 
 * @param parser The parser to use
 * @return AST node representing the while loop
 */
ASTNode* parser_parse_while_loop(Parser* parser);

/**
 * @brief Parse a for loop
 * 
 * For loops iterate over collections or ranges:
 * for item in collection: block, for i in 0..10: block
 * 
 * @param parser The parser to use
 * @return AST node representing the for loop
 */
ASTNode* parser_parse_for_loop(Parser* parser);

/**
 * @brief Parse a try-catch statement
 * 
 * Try-catch statements handle exceptions:
 * try: block catch error: block
 * 
 * @param parser The parser to use
 * @return AST node representing the try-catch statement
 */
ASTNode* parser_parse_try_statement(Parser* parser);

/**
 * @brief Parse a match statement
 * 
 * Match statements provide pattern matching:
 * match value: case pattern1 => result case pattern2 => result end
 * 
 * @param parser The parser to use
 * @return AST node representing the match statement
 */
ASTNode* parser_parse_match_statement(Parser* parser);

/**
 * @brief Parse a return statement
 * 
 * Return statements exit functions with optional values:
 * return, return 42, return "success"
 * 
 * @param parser The parser to use
 * @return AST node representing the return statement
 */
ASTNode* parser_parse_return_statement(Parser* parser);

/**
 * @brief Parse a spore statement (pattern matching)
 * 
 * Spore statements provide pattern matching with both lambda and block syntax:
 * spore value: case pattern1 => result case pattern2: block root => result end
 * 
 * @param parser The parser to use
 * @return AST node representing the spore statement
 */
ASTNode* parser_parse_spore_statement(Parser* parser);

/**
 * @brief Parse a pattern (enhanced pattern matching)
 * 
 * Patterns support type matching, destructuring, guards, and combinators:
 * Int, String, {name: n, age: a}, [x, y, z], pattern when condition, pattern1 | pattern2
 * 
 * @param parser The parser to use
 * @return AST node representing the pattern
 */
ASTNode* parser_parse_pattern(Parser* parser);
static ASTNode* parser_parse_pattern_or(Parser* parser);
static ASTNode* parser_parse_pattern_and(Parser* parser);
static ASTNode* parser_parse_pattern_not(Parser* parser);
static ASTNode* parser_parse_pattern_primary(Parser* parser);
static ASTNode* parser_parse_array_destructure(Parser* parser);
static ASTNode* parser_parse_object_destructure(Parser* parser);

/**
 * @brief Parse a spore statement (pattern matching)
 * 
 * Spore statements provide pattern matching:
 * spore value: case pattern1: block case pattern2: block root: block end
 * 
 * @param parser The parser to use
 * @return AST node representing the spore statement
 */
ASTNode* parser_parse_spore_statement(Parser* parser);

/**
 * @brief Parse a break statement
 * 
 * Break statements exit loops early:
 * break, break loop_name
 * 
 * @param parser The parser to use
 * @return AST node representing the break statement
 */
ASTNode* parser_parse_break_statement(Parser* parser);

/**
 * @brief Parse a continue statement
 * 
 * Continue statements skip to the next iteration of a loop:
 * continue, continue loop_name
 * 
 * @param parser The parser to use
 * @return AST node representing the continue statement
 */
ASTNode* parser_parse_continue_statement(Parser* parser);

/**
 * @brief Parse a throw statement
 * 
 * Throw statements raise exceptions:
 * throw "error message", throw CustomError("details")
 * 
 * @param parser The parser to use
 * @return AST node representing the throw statement
 */
ASTNode* parser_parse_throw_statement(Parser* parser);

/**
 * @brief Parse a block of statements
 * 
 * Blocks group multiple statements together:
 * { statement1; statement2; } or statement1; statement2; end
 * 
 * @param parser The parser to use
 * @return AST node representing the block
 */
ASTNode* parser_parse_block(Parser* parser);

/**
 * @brief Parse a function declaration
 * 
 * Function declarations define reusable code blocks:
 * function name(params): block
 * 
 * @param parser The parser to use
 * @return AST node representing the function declaration
 */
ASTNode* parser_parse_function_declaration(Parser* parser);
ASTNode* parser_parse_async_function_declaration(Parser* parser);

/**
 * @brief Parse a lambda expression
 * 
 * Lambda expressions are anonymous functions that can be assigned to variables:
 * func (params) -> returnType: body end
 * 
 * @param parser The parser to use
 * @return AST node representing the lambda expression
 */
ASTNode* parser_parse_lambda_expression(Parser* parser);

/**
 * @brief Parse a class declaration
 * 
 * Class declarations define object types:
 * class Name: field1: Type, field2: Type; method1(): block
 * 
 * @param parser The parser to use
 * @return AST node representing the class declaration
 */
ASTNode* parser_parse_class_declaration(Parser* parser);
ASTNode* parser_parse_class_body(Parser* parser);
ASTNode* parser_parse_class_field(Parser* parser);

/**
 * @brief Parse an interface declaration
 * 
 * Interface declarations define contracts for classes:
 * interface Name: method1(): ReturnType, method2(): ReturnType
 * 
 * @param parser The parser to use
 * @return AST node representing the interface declaration
 */
ASTNode* parser_parse_interface_declaration(Parser* parser);

/**
 * @brief Parse a module declaration
 * 
 * Module declarations group related functionality:
 * module Math: export function sqrt(x: Float): Float
 * 
 * @param parser The parser to use
 * @return AST node representing the module declaration
 */
ASTNode* parser_parse_module_declaration(Parser* parser);

/**
 * @brief Parse a package declaration
 * 
 * Package declarations define project metadata and dependencies:
 * package myproject: dependencies: "stdlib": "1.0.0"
 * 
 * @param parser The parser to use
 * @return AST node representing the package declaration
 */
ASTNode* parser_parse_package_declaration(Parser* parser);

/**
 * @brief Parse an import statement
 * 
 * Import statements bring external functionality into scope:
 * import stdlib, import {map, filter} from stdlib
 * 
 * @param parser The parser to use
 * @return AST node representing the import statement
 */
ASTNode* parser_parse_import_statement(Parser* parser);

/**
 * @brief Parse an export statement
 * 
 * Export statements make functionality available to other modules:
 * export function public_function(): Int
 * 
 * @param parser The parser to use
 * @return AST node representing the export statement
 */
ASTNode* parser_parse_export_statement(Parser* parser);

/**
 * @brief Parse range expressions
 * 
 * Range expressions use the .. operator and create ranges like 1..10 or start..end.
 * They have higher precedence than comparisons but lower than addition.
 * 
 * @param parser The parser to use
 * @return AST node representing the range expression
 */
ASTNode* parser_parse_range(Parser* parser);

// Utility functions for parsing

/**
 * @brief Get the precedence level of a token type
 * 
 * Precedence determines the order of operations in expressions.
 * Higher precedence operations are performed first.
 * 
 * @param type The token type to check
 * @return Precedence level (higher numbers = higher precedence)
 */
int parser_get_precedence(TokenType type);

/**
 * @brief Check if a token type is right-associative
 * 
 * Right-associative operators group from right to left.
 * For example, a ** b ** c is parsed as a ** (b ** c).
 * 
 * @param type The token type to check
 * @return 1 if right-associative, 0 if left-associative
 */
int parser_is_right_associative(TokenType type);

/**
 * @brief Check if a token type is a binary operator
 * 
 * Binary operators take two operands (left and right).
 * Examples: +, -, *, /, ==, !=, <, >, and, or
 * 
 * @param type The token type to check
 * @return 1 if binary operator, 0 otherwise
 */
int parser_is_binary_operator(TokenType type);

/**
 * @brief Check if a token type is a unary operator
 * 
 * Unary operators take one operand.
 * Examples: -, not, ~ (bitwise NOT)
 * 
 * @param type The token type to check
 * @return 1 if unary operator, 0 otherwise
 */
int parser_is_unary_operator(TokenType type);

/**
 * @brief Check if a token type is an assignment operator
 * 
 * Assignment operators assign values to variables.
 * Examples: =, +=, -=, *=, /=
 * 
 * @param type The token type to check
 * @return 1 if assignment operator, 0 otherwise
 */
int parser_is_assignment_operator(TokenType type);

/**
 * @brief Check if a token type is a comparison operator
 * 
 * Comparison operators compare two values.
 * Examples: ==, !=, <, >, <=, >=
 * 
 * @param type The token type to check
 * @return 1 if comparison operator, 0 otherwise
 */
int parser_is_comparison_operator(TokenType type);

/**
 * @brief Check if a token type is a logical operator
 * 
 * Logical operators perform boolean logic.
 * Examples: and, or, not
 * 
 * @param type The token type to check
 * @return 1 if logical operator, 0 otherwise
 */
int parser_is_logical_operator(TokenType type);

/**
 * @brief Check if a token type is an arithmetic operator
 * 
 * Arithmetic operators perform mathematical operations.
 * Examples: +, -, *, /, %, **
 * 
 * @param type The token type to check
 * @return 1 if arithmetic operator, 0 otherwise
 */
int parser_is_arithmetic_operator(TokenType type);

/**
 * @brief Synchronize the parser after an error
 * 
 * When a parsing error occurs, this function attempts to recover by
 * skipping tokens until it finds a safe point to continue parsing.
 * This helps provide multiple error messages rather than stopping
 * at the first error.
 * 
 * @param parser The parser to synchronize
 */
void parser_synchronize(Parser* parser);

/**
 * @brief Synchronize to a statement boundary
 * 
 * This function skips tokens until it finds the end of the current
 * statement, allowing parsing to continue with the next statement.
 * 
 * @param parser The parser to synchronize
 * @return 1 if synchronization was successful, 0 otherwise
 */
int parser_sync_to_statement_boundary(Parser* parser);

/**
 * @brief Synchronize to an expression boundary
 * 
 * This function skips tokens until it finds the end of the current
 * expression, allowing parsing to continue with the next expression.
 * 
 * @param parser The parser to synchronize
 * @return 1 if synchronization was successful, 0 otherwise
 */
int parser_sync_to_expression_boundary(Parser* parser);

#endif // MYCO_PARSER_H
