#include "parser.h"
#include "type_checker.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "../../include/utils/shared_utilities.h"

// Forward declarations for helper functions
static void parser_advance(Parser* parser);
static Token* parser_peek(Parser* parser);
static int parser_check(Parser* parser, TokenType type);
static int parser_match(Parser* parser, TokenType type);
static void parser_error(Parser* parser, const char* message);
static const char* get_token_type_name(TokenType type);
static const char* get_error_suggestion(const char* message, Token* token);
static ASTNode* parser_parse_array_literal(Parser* parser);
static ASTNode* parser_parse_hash_map_literal(Parser* parser);
static ASTNode* parser_parse_set_literal(Parser* parser);
static ASTNode* parser_parse_hash_map_key(Parser* parser);
static ASTNode* parser_parse_try_catch_statement(Parser* parser);
static ASTNode* parser_parse_use_statement(Parser* parser);
static ASTNode* parser_collect_block(Parser* parser, int stop_on_else, int* saw_else);
static ASTNode* parser_parse_member_access_chain(Parser* parser, ASTNode* base);

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
 *     parser_shared_free_safe(parser, "parser", "parser", 41);
 * }
 * @endcode
 */
Parser* parser_initialize(Lexer* lexer) {
    if (!lexer) {
        return NULL;  // Invalid lexer
    }
    
    // Allocate memory for the parser structure
    Parser* parser = shared_malloc_safe(sizeof(Parser), "parser", "unknown_function", 51);
    if (!parser) {
        return NULL;  // Memory allocation failed
    }
    
    // Initialize parser state
    parser->lexer = lexer;                    // Store reference to the lexer
    parser->current_token = NULL;             // Current token being processed
    parser->previous_token = NULL;            // Previously processed token
    parser->current_position = 0;             // Current position in token stream
    parser->error_count = 0;                  // Number of parsing errors encountered
    parser->error_message = NULL;             // Description of the last error
    parser->error_line = 0;                   // Line number where error occurred
    parser->error_column = 0;                 // Column number where error occurred
    
    return parser;
}

/**
 * @brief Free all memory associated with a parser
 * 
 * This function cleans up the parser and any error messages it may have stored.
 * Note that this does NOT free the lexer - you need to free that separately.
 * 
 * @param parser The parser to free
 */
void parser_free(Parser* parser) {
    if (!parser) {
        return;  // Nothing to free
    }
    
    // Free any error message that was stored
    if (parser->error_message) {
        shared_free_safe(parser->error_message, "parser", "unknown_function", 84);
        parser->error_message = NULL;
    }
    
    // Free the parser structure itself
    // Note: We don't free the lexer as it's owned by the caller
    shared_free_safe(parser, "parser", "unknown_function", 90);
}

/**
 * @brief Advance to the next token in the stream
 * 
 * This function moves the parser to the next token and updates the internal state.
 * It's called after successfully consuming a token during parsing.
 * 
 * @param parser The parser to advance
 */
static void parser_advance(Parser* parser) {
    if (!parser || !parser->lexer) {
        return;
    }
    
    // Store the current token as the previous token
    parser->previous_token = parser->current_token;
    
    // Get the next token, either from pre-scanned tokens or by scanning
    if (parser->lexer->token_count > 0) {
        // Use pre-scanned tokens
        int next_index = parser->current_position + 1;
        if (next_index < parser->lexer->token_count) {
            parser->current_token = lexer_get_token(parser->lexer, next_index);
        } else {
            // We've reached the end of pre-scanned tokens
            parser->current_token = NULL;
        }
    } else {
        // Fall back to scanning tokens on-demand
        parser->current_token = lexer_scan_next(parser->lexer);
    }
    parser->current_position++;
}

/**
 * @brief Peek at the next token without consuming it
 * 
 * This function returns the next token that would be consumed by parser_advance(),
 * but doesn't actually advance the parser. Useful for lookahead during parsing.
 * 
 * @param parser The parser to peek from
 * @return Pointer to the next token, or NULL if no more tokens
 */
static Token* parser_peek(Parser* parser) {
    if (!parser || !parser->lexer) {
        return NULL;
    }
    
    // Get the current token if we have one, otherwise get the first pre-scanned token
    if (parser->current_token) {
        return parser->current_token;
    }
    
    // Check if we have pre-scanned tokens available
    if (parser->lexer->token_count > 0) {
        // Use pre-scanned tokens
        parser->current_token = lexer_get_token(parser->lexer, 0);
        return parser->current_token;
    } else {
        // Fall back to scanning tokens on-demand
        parser->current_token = lexer_scan_next(parser->lexer);
        return parser->current_token;
    }
}

static Token* parser_peek_next(Parser* parser) {
    if (!parser || !parser->lexer) {
        return NULL;
    }
    
    // Check if we have pre-scanned tokens available
    if (parser->lexer->token_count > 1) {
        // Use pre-scanned tokens
        return lexer_get_token(parser->lexer, 1);
    } else {
        // Fall back to scanning tokens on-demand
        return lexer_scan_next(parser->lexer);
    }
}

/**
 * @brief Check if the current token is of a specific type
 * 
 * This function checks if the current token matches the expected type without
 * consuming it. Useful for conditional parsing logic.
 * 
 * @param parser The parser to check
 * @param type The token type to check for
 * @return 1 if the token matches, 0 otherwise
 */
static int parser_check(Parser* parser, TokenType type) {
    Token* token = parser_peek(parser);
    return token && token->type == type;
}

/**
 * @brief Match and consume a token of the expected type
 * 
 * This function checks if the current token matches the expected type and
 * advances to the next token if it does. If the token doesn't match,
 * an error is reported.
 * 
 * @param parser The parser to use
 * @param type The expected token type
 * @return 1 if the token matched and was consumed, 0 otherwise
 */
static int parser_match(Parser* parser, TokenType type) {
    if (parser_check(parser, type)) {
        parser_advance(parser);
        return 1;
    }
    
    // Report error for unexpected token
    Token* token = parser_peek(parser);
    if (token) {
        const char* expected_type = get_token_type_name(type);
        const char* found_type = get_token_type_name(token->type);
        char error_msg[512];
        snprintf(error_msg, sizeof(error_msg), 
                "Expected %s, but found %s",
                expected_type, found_type);
        parser_error(parser, error_msg);
    } else {
        parser_error(parser, "Unexpected end of input");
    }
    
    return 0;
}



/**
 * @brief Report a parsing error
 * 
 * This function records parsing errors and updates the parser's error state.
 * Multiple errors can be tracked, but only the most recent one is stored
 * in the error message.
 * 
 * @param parser The parser to report the error to
 * @param message Description of the error
 */
static void parser_error(Parser* parser, const char* message) {
    if (!parser) {
        return;
    }
    
        parser->error_count++;
    
    // Free any existing error message
        if (parser->error_message) {
            shared_free_safe(parser->error_message, "parser", "unknown_function", 242);
        }
    
    // Build much richer and more helpful error message
    char buf[1024];
    if (parser->current_token) {
        const char* token_text = parser->current_token->text ? parser->current_token->text : "";
        const char* token_type_name = get_token_type_name(parser->current_token->type);
        
        // Create a concise error message
        snprintf(buf, sizeof(buf), 
                 "Parse Error at Line %d, Column %d: %s (found '%s')",
                 parser->current_token->line,
                 parser->current_token->column,
                 message,
                 token_text);
        
        parser->error_message = strdup(buf);
        
        // Record the location of the error
        parser->error_line = parser->current_token->line;
        parser->error_column = parser->current_token->column;
    } else {
        // No current token, create a basic error message
        snprintf(buf, sizeof(buf), "Parse Error: %s", message);
        parser->error_message = strdup(buf);
    }
    
    // Error message is stored in parser->error_message for the caller to handle
}

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
ASTNode* parser_parse_program(Parser* parser) {
    if (!parser || !parser->lexer) {
        return NULL;  // Invalid parser or lexer
    }
    
    // Initialize the parser by getting the first token
    parser_peek(parser);
    
    // Parse statements until we reach the end of the program
    ASTNode* statements = NULL;
    ASTNode* last_statement = NULL;
    
    int consecutive_failures = 0;
    const int MAX_CONSECUTIVE_FAILURES = 10;  // Prevent infinite loops
    
    while (parser->current_token && parser->current_token->type != TOKEN_EOF) {
        // Treat stray semicolons as empty statements and skip them
        while (parser->current_token && parser->current_token->type == TOKEN_SEMICOLON) {
            parser_advance(parser);
        }
        if (!parser->current_token || parser->current_token->type == TOKEN_EOF) {
            break;
        }
        
        // Store current position before parsing
        int start_position = parser->current_position;
        ASTNode* statement = parser_parse_statement(parser);
        if (statement) {
            // Add the statement to our list
            if (!statements) {
                statements = statement;
                last_statement = statement;
            } else {
                // Link statements together
                last_statement->next = statement;
                last_statement = statement;
            }
            consecutive_failures = 0;  // Reset failure counter on success
        } else {
            // Check if parser position actually advanced
            if (parser->current_position == start_position) {
                consecutive_failures++;
                if (consecutive_failures >= MAX_CONSECUTIVE_FAILURES) {
                    // Force advance to prevent infinite loop
                    parser_advance(parser);
                    consecutive_failures = 0;
                }
            } else {
                consecutive_failures = 0;  // Reset if position advanced
            }
            // Parsing failed, try to synchronize
            parser_synchronize(parser);
        }
    }
    
    // Create a block node containing all statements
    if (statements) {
        // Count the statements
        int statement_count = 0;
        ASTNode* current = statements;
        while (current) {
            statement_count++;
            current = current->next;
        }
        
        // Create an array of statement pointers
        ASTNode** statement_array = shared_malloc_safe((size_t)statement_count * sizeof(ASTNode*), "parser", "unknown_function", 351);
        if (statement_array) {
            current = statements;
            for (int i = 0; i < statement_count; i++) {
                statement_array[i] = current;
                current = current->next;
            }
            
            // Create the block node
            ASTNode* block = ast_create_block(statement_array, statement_count, 0, 0);
            if (block) {
                // Perform type checking on the parsed AST
                TypeCheckerContext* type_context = type_checker_create_context();
                if (type_context) {
                    if (!type_check_ast(type_context, block)) {
                        // Type checking failed, print errors
                        type_checker_print_errors(type_context);
                        // Note: We still return the AST, but type errors are reported
                    }
                    type_checker_free_context(type_context);
                }
                return block;
            }
            shared_free_safe(statement_array, "parser", "unknown_function", 374);
        }
    }
    
    // If no statements were parsed, return an empty block
    return ast_create_block(NULL, 0, 0, 0);
}

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
ASTNode* parser_parse_statement(Parser* parser) {
    if (!parser) {
        return NULL;  // Invalid parser
    }
    
    // Check what type of statement we're dealing with
    if (parser_check(parser, TOKEN_KEYWORD)) {
        Token* token = parser_peek(parser);

        if (token && token->text) {
            if (strcmp(token->text, "let") == 0) {
                parser_advance(parser);  // Consume the 'let' keyword
                return parser_parse_variable_declaration(parser);
            } else if (strcmp(token->text, "if") == 0) {
                parser_advance(parser);  // Consume the 'if' keyword
                return parser_parse_if_statement(parser);
            } else if (strcmp(token->text, "while") == 0) {
                parser_advance(parser);  // Consume the 'while' keyword
                return parser_parse_while_loop(parser);
            } else if (strcmp(token->text, "for") == 0) {
                parser_advance(parser);  // Consume the 'for' keyword
                return parser_parse_for_loop(parser);
            } else if (strcmp(token->text, "func") == 0) {
                parser_advance(parser);  // Consume the 'func' keyword
                return parser_parse_function_declaration(parser);
            } else if (strcmp(token->text, "async") == 0) {
                parser_advance(parser);  // Consume the 'async' keyword
                return parser_parse_async_function_declaration(parser);
            } else if (strcmp(token->text, "class") == 0) {
                parser_advance(parser);  // Consume the 'class' keyword
                return parser_parse_class_declaration(parser);
            } else if (strcmp(token->text, "return") == 0) {
                parser_advance(parser);  // Consume the 'return' keyword
                return parser_parse_return_statement(parser);
            } else if (strcmp(token->text, "throw") == 0) {
                parser_advance(parser);  // Consume the 'throw' keyword
                return parser_parse_throw_statement(parser);
            } else if (strcmp(token->text, "match") == 0) {
                parser_advance(parser);  // Consume the 'match' keyword
                return parser_parse_match_statement(parser);
            } else if (strcmp(token->text, "try") == 0) {
                parser_advance(parser);  // Consume the 'try' keyword
                return parser_parse_try_catch_statement(parser);
            } else if (strcmp(token->text, "use") == 0) {
                parser_advance(parser);  // Consume the 'use' keyword
                return parser_parse_use_statement(parser);
            }
        }
    }
    
    // Skip leading semicolons (empty statements)
    while (parser_check(parser, TOKEN_SEMICOLON)) {
        parser_advance(parser);
    }

    // Check if this looks like an assignment statement (identifier followed by =)
    if (parser_check(parser, TOKEN_IDENTIFIER)) {
        // Peek ahead to see if the next token is an assignment operator
        // We need to temporarily advance to check the next token
        Token* current_id = parser->current_token;
        parser_advance(parser);
        
        if (parser_check(parser, TOKEN_ASSIGN)) {
            // This is definitely an assignment, go back and parse it properly
            parser->current_position--;
            parser->current_token = current_id;
            
            ASTNode* assignment = parser_parse_assignment(parser);
            if (assignment) {
                // Check for semicolon, but be lenient if at end of input
                if (parser_check(parser, TOKEN_SEMICOLON)) {
                    parser_advance(parser);  // Consume the semicolon
                    // collapse multiple semicolons
                    while (parser_check(parser, TOKEN_SEMICOLON)) {
                        parser_advance(parser);
                    }
                    return assignment;
                } else if (!parser->current_token || parser->current_token->type == TOKEN_EOF) {
                    // At end of input, no semicolon required
                    return assignment;
                } else {
                    parser_error(parser, "Missing semicolon (;) at end of statement");
                    parser_synchronize(parser);
                    return assignment;
                }
            }
            return NULL;
        } else {
            // Not an assignment, go back and parse as expression
            parser->current_position--;
            parser->current_token = current_id;
        }
    }
    
    // If it's not an assignment, try parsing as an expression statement
    ASTNode* expression = parser_parse_expression(parser);
    if (expression) {
            // Require a semicolon to terminate statements
    if (parser_check(parser, TOKEN_SEMICOLON)) {
        parser_advance(parser);  // Consume the semicolon
        // collapse multiple semicolons
        while (parser_check(parser, TOKEN_SEMICOLON)) {
        parser_advance(parser);
        }
        return expression;
    } else {
        // Be more lenient with missing semicolons in certain contexts
        // Check if the next token is a keyword that suggests end of statement
        if (parser->current_token && parser->current_token->type == TOKEN_KEYWORD && parser->current_token->text) {
            if (strcmp(parser->current_token->text, "end") == 0 ||
                strcmp(parser->current_token->text, "else") == 0 ||
                strcmp(parser->current_token->text, "catch") == 0 ||
                strcmp(parser->current_token->text, "case") == 0 ||
                strcmp(parser->current_token->text, "root") == 0) {
                // This is likely the end of a statement, continue without error
                return expression;
            }
        }
        
        // Check if we're at the end of a block (next token is 'end')
        if (parser->current_token && parser->current_token->type == TOKEN_KEYWORD && 
            parser->current_token->text && strcmp(parser->current_token->text, "end") == 0) {
            return expression;
        }
        
        // Check if we're at the end of input (EOF) - this is common in REPL
        if (!parser->current_token || parser->current_token->type == TOKEN_EOF) {
            return expression;
        }
        
        // Only require semicolon if there are more statements on the same line
        // or if we're in a context where multiple statements are expected
        parser_error(parser, "Missing semicolon (;) at end of statement");
        parser_synchronize(parser);
        return expression;
    }
    }
    
    // If we can't parse anything, report an error
    parser_error(parser, "Expected a statement");
    return NULL;
}

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
ASTNode* parser_parse_expression(Parser* parser) {
    if (!parser) {
        return NULL;  // Invalid parser
    }
    
    // Start with the lowest precedence level (logical OR)
    return parser_parse_logical_or(parser);
}

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
ASTNode* parser_parse_assignment(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // Parse the left side (variable name)
    if (!parser_check(parser, TOKEN_IDENTIFIER)) {
        parser_error(parser, "Expected identifier for assignment");
        return NULL;
    }
    
    Token* var_token = parser_peek(parser);
    char* var_name = strdup(var_token->text);
    parser_advance(parser);  // Consume the identifier
    
    // Expect an equals sign
    if (!parser_check(parser, TOKEN_ASSIGN)) {
        parser_error(parser, "Expected '=' in assignment");
        shared_free_safe(var_name, "parser", "unknown_function", 588);
        return NULL;
    }
    parser_advance(parser);  // Consume the '='
    
    // Parse the right side (value)
    ASTNode* value = parser_parse_expression(parser);
    if (!value) {
        parser_error(parser, "Expected expression after '='");
        shared_free_safe(var_name, "parser", "unknown_function", 597);
        return NULL;
    }
    
    // Create assignment node
    return ast_create_assignment(var_name, value, 0, 0);
}

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
ASTNode* parser_parse_logical_or(Parser* parser) {
    if (!parser) {
        return NULL;  // Invalid parser
    }
    
    // Parse the left operand (logical AND has higher precedence)
    ASTNode* left = parser_parse_logical_and(parser);
    if (!left) {
        return NULL;
    }
    
    // Check for logical OR operators
    while (parser_check(parser, TOKEN_OR)) {
        parser_advance(parser);  // Consume the 'or' token
        
        // Parse the right operand
        ASTNode* right = parser_parse_logical_and(parser);
        if (!right) {
            parser_error(parser, "Expected expression after 'or'");
            return left;  // Return what we have so far
        }
        
        // Create a logical OR node
        ASTNode* logical_or = ast_create_binary_op(OP_LOGICAL_OR, left, right, 
                                                   parser->current_token->line, 
                                                   parser->current_token->column);
        if (logical_or) {
            left = logical_or;  // Continue building the chain
        }
    }
    
    return left;
}

/**
 * @brief Parse logical AND expressions
 * 
 * Logical AND expressions use the 'and' keyword and have higher precedence
 * than OR but lower than comparison operators.
 * 
 * @param parser The parser to use
 * @return AST node representing the logical AND expression
 */
ASTNode* parser_parse_logical_and(Parser* parser) {
    if (!parser) {
        return NULL;  // Invalid parser
    }
    
    // Parse the left operand (equality has higher precedence)
    ASTNode* left = parser_parse_equality(parser);
    if (!left) {
        return NULL;
    }
    
    // Check for logical AND operators
    while (parser_check(parser, TOKEN_AND)) {
        parser_advance(parser);  // Consume the 'and' token
        
        // Parse the right operand
        ASTNode* right = parser_parse_equality(parser);
        if (!right) {
            parser_error(parser, "Expected expression after 'and'");
            return left;  // Return what we have so far
        }
        
        // Create a logical AND node
        ASTNode* logical_and = ast_create_binary_op(OP_LOGICAL_AND, left, right, 
                                                    parser->current_token->line, 
                                                    parser->current_token->column);
        if (logical_and) {
            left = logical_and;  // Continue building the chain
        }
    }
    
    return left;
}

/**
 * @brief Parse equality comparison expressions
 * 
 * Equality comparisons (==, !=) have higher precedence than logical operators
 * but lower than relational comparisons.
 * 
 * @param parser The parser to use
 * @return AST node representing the equality comparison
 */
ASTNode* parser_parse_equality(Parser* parser) {
    if (!parser) {
        return NULL;  // Invalid parser
    }
    
    // Parse the left operand (comparison has higher precedence)
    ASTNode* left = parser_parse_comparison(parser);
    if (!left) {
        return NULL;
    }
    
    // Check for equality operators
    while (parser_check(parser, TOKEN_EQUAL) || parser_check(parser, TOKEN_NOT_EQUAL)) {
        TokenType operator_type = parser_peek(parser)->type;
        parser_advance(parser);  // Consume the operator
        
        // Parse the right operand
        ASTNode* right = parser_parse_comparison(parser);
        if (!right) {
            parser_error(parser, "Expected expression after equality operator");
            return left;  // Return what we have so far
        }
        
        // Create an equality node
        BinaryOperator op = (operator_type == TOKEN_EQUAL) ? OP_EQUAL : OP_NOT_EQUAL;
        ASTNode* equality = ast_create_binary_op(op, left, right, 
                                                 parser->current_token->line, 
                                                 parser->current_token->column);
        if (equality) {
            left = equality;  // Continue building the chain
        }
    }
    
    return left;
}

/**
 * @brief Parse relational comparison expressions
 * 
 * Relational comparisons (<, >, <=, >=) have higher precedence than equality
 * but lower than arithmetic operators.
 * 
 * @param parser The parser to use
 * @return AST node representing the relational comparison
 */
ASTNode* parser_parse_comparison(Parser* parser) {
    if (!parser) {
        return NULL;  // Invalid parser
    }
    
    // Parse the left operand (range has higher precedence)
    ASTNode* left = parser_parse_range(parser);
    if (!left) {
        return NULL;
    }
    
    // Check for comparison operators
    while (parser_check(parser, TOKEN_LESS) || 
           parser_check(parser, TOKEN_LESS_EQUAL) ||
           parser_check(parser, TOKEN_GREATER) ||
           parser_check(parser, TOKEN_GREATER_EQUAL)) {
        TokenType operator_type = parser_peek(parser)->type;
        parser_advance(parser);  // Consume the operator
        
        // Parse the right operand
        ASTNode* right = parser_parse_range(parser);
        if (!right) {
            parser_error(parser, "Expected expression after comparison operator");
            return left;  // Return what we have so far
        }
        
        // Create a comparison node
        BinaryOperator op;
        switch (operator_type) {
            case TOKEN_LESS: op = OP_LESS_THAN; break;
            case TOKEN_LESS_EQUAL: op = OP_LESS_EQUAL; break;
            case TOKEN_GREATER: op = OP_GREATER_THAN; break;
            case TOKEN_GREATER_EQUAL: op = OP_GREATER_EQUAL; break;
            default: op = OP_LESS_THAN; break; // Should never happen
        }
        ASTNode* comparison = ast_create_binary_op(op, left, right, 
                                                   parser->current_token->line, 
                                                   parser->current_token->column);
        if (comparison) {
            left = comparison;  // Continue building the chain
        }
    }
    
    return left;
}

/**
 * @brief Parse range expressions
 * 
 * Range expressions use the .. operator and have higher precedence than comparisons
 * but lower than addition. They create ranges like 1..10 or start..end.
 * 
 * @param parser The parser to use
 * @return AST node representing the range expression
 */
ASTNode* parser_parse_range(Parser* parser) {
    if (!parser) {
        return NULL;  // Invalid parser
    }
    
    // Parse the left operand (term has higher precedence)
    ASTNode* left = parser_parse_term(parser);
    if (!left) {
        return NULL;
    }
    
    // Check for range operator (..)
    if (parser_check(parser, TOKEN_DOT_DOT)) {
        parser_advance(parser);  // Consume the '..' operator
        
        // Parse the right operand
        ASTNode* right = parser_parse_term(parser);
        if (!right) {
            parser_error(parser, "Expected expression after '..' range operator");
            return left;  // Return what we have so far
        }
        
        // Check for step operator (another ..)
        if (parser_check(parser, TOKEN_DOT_DOT)) {
            parser_advance(parser);  // Consume the second '..' operator
            
            // Parse the step operand
            ASTNode* step = parser_parse_term(parser);
            if (!step) {
                parser_error(parser, "Expected step expression after '..' in range");
                return left;  // Return what we have so far
            }
            
            // Create a range with step node (using OP_RANGE_STEP)
            ASTNode* range = ast_create_range_with_step(left, right, step, 
                                                       parser->current_token->line, 
                                                       parser->current_token->column);
            if (range) {
                return range;
            }
        } else {
            // Create a simple range node (using OP_RANGE for exclusive ranges)
            ASTNode* range = ast_create_binary_op(OP_RANGE, left, right, 
                                                  parser->current_token->line, 
                                                  parser->current_token->column);
            if (range) {
                return range;
            }
        }
    }
    
    return left;
}

/**
 * @brief Parse addition and subtraction expressions
 * 
 * Addition and subtraction (+ and -) have higher precedence than comparisons
 * but lower than multiplication and division.
 * 
 * @param parser The parser to use
 * @return AST node representing the addition/subtraction expression
 */
ASTNode* parser_parse_term(Parser* parser) {
    if (!parser) {
        return NULL;  // Invalid parser
    }
    
    // Parse the left operand (factor has higher precedence)
    ASTNode* left = parser_parse_factor(parser);
    if (!left) {
        return NULL;
    }
    
            // Check for addition and subtraction operators
        while (parser_check(parser, TOKEN_PLUS) || parser_check(parser, TOKEN_MINUS)) {
            TokenType operator_type = parser_peek(parser)->type;
            parser_advance(parser);  // Consume the operator
            
            // Parse the right operand
            ASTNode* right = parser_parse_factor(parser);
            if (!right) {
                parser_error(parser, "Expected expression after addition/subtraction operator");
                return left;  // Return what we have so far
            }
            
            // Create a binary operation node
            BinaryOperator op = (operator_type == TOKEN_PLUS) ? OP_ADD : OP_SUBTRACT;
            ASTNode* binary_op = ast_create_binary_op(op, left, right, 
                                                      parser->current_token->line, 
                                                      parser->current_token->column);
            if (binary_op) {
                left = binary_op;  // Continue building the chain
            }
        }
    
    return left;
}

/**
 * @brief Parse multiplication and division expressions
 * 
 * Multiplication and division (* and /) have higher precedence than addition
 * but lower than power operations.
 * 
 * @param parser The parser to use
 * @return AST node representing the multiplication/division expression
 */
ASTNode* parser_parse_factor(Parser* parser) {
    if (!parser) {
        return NULL;  // Invalid parser
    }
    
    // Parse the left operand (power has higher precedence)
    ASTNode* left = parser_parse_power(parser);
    if (!left) {
        return NULL;
    }
    
    // Check for multiplication and division operators
    while (parser_check(parser, TOKEN_MULTIPLY) || 
           parser_check(parser, TOKEN_DIVIDE) ||
           parser_check(parser, TOKEN_MODULO)) {
        TokenType operator_type = parser_peek(parser)->type;
        parser_advance(parser);  // Consume the operator
        
        // Parse the right operand
        ASTNode* right = parser_parse_power(parser);
        if (!right) {
            parser_error(parser, "Expected expression after multiplication/division operator");
            return left;  // Return what we have so far
        }
        
        // Create a binary operation node
        BinaryOperator op;
        if (operator_type == TOKEN_MULTIPLY) op = OP_MULTIPLY;
        else if (operator_type == TOKEN_DIVIDE) op = OP_DIVIDE;
        else op = OP_MODULO;
        ASTNode* binary_op = ast_create_binary_op(op, left, right, 
                                                   parser->current_token->line, 
                                                   parser->current_token->column);
        if (binary_op) {
            left = binary_op;  // Continue building the chain
        }
    }
    
    return left;
}

/**
 * @brief Parse power expressions (highest precedence)
 * 
 * Power operations (**) have the highest precedence among arithmetic operators.
 * This allows expressions like: 2 ** 3 ** 4 (parsed as: 2 ** (3 ** 4))
 * 
 * @param parser The parser to use
 * @return AST node representing the power expression
 */
ASTNode* parser_parse_power(Parser* parser) {
    if (!parser) {
        return NULL;  // Invalid parser
    }
    
    // Parse the left operand (unary has higher precedence)
    ASTNode* left = parser_parse_unary(parser);
    if (!left) {
        return NULL;
    }
    
    // Check for power operator (right-associative)
    if (parser_check(parser, TOKEN_POWER)) {
        parser_advance(parser);  // Consume the power operator
        
        // Parse the right operand
        ASTNode* right = parser_parse_power(parser);  // Recursive call for right associativity
        if (!right) {
            parser_error(parser, "Expected expression after power operator");
            return left;  // Return what we have so far
        }
        
        // Create a power operation node
        ASTNode* power = ast_create_binary_op(OP_POWER, left, right, 
                                               parser->current_token->line, 
                                               parser->current_token->column);
        if (power) {
            return power;
        }
    }
    
    return left;
}

/**
 * @brief Parse unary expressions
 * 
 * Unary expressions are operators that apply to a single operand, such as
 * negation (-x), logical NOT (not x), or bitwise NOT (~x).
 * 
 * @param parser The parser to use
 * @return AST node representing the unary expression
 */
ASTNode* parser_parse_unary(Parser* parser) {
    if (!parser) {
        return NULL;  // Invalid parser
    }
    
    // Check for unary operators
    if (parser_check(parser, TOKEN_MINUS) || 
        parser_check(parser, TOKEN_NOT)) {
        
        TokenType operator_type = parser_peek(parser)->type;
        parser_advance(parser);  // Consume the unary operator
        
        // Parse the operand
        ASTNode* operand = parser_parse_unary(parser);  // Recursive call for multiple unary operators
        if (!operand) {
            parser_error(parser, "Expected expression after unary operator");
            return NULL;
        }
        
        // Create a unary operation node
        UnaryOperator op;
        if (operator_type == TOKEN_MINUS) op = OP_NEGATIVE;
        else op = OP_LOGICAL_NOT; // TOKEN_NOT
        ASTNode* unary = ast_create_unary_op(op, operand, 0, 0);
        if (unary) {
            return unary;
        }
    }
    
    // If no unary operator, parse the primary expression
    return parser_parse_primary(parser);
}

/**
 * @brief Parse primary expressions
 * 
 * Primary expressions are the most basic building blocks: literals, identifiers,
 * parenthesized expressions, function calls, and member access.
 * 
 * @param parser The parser to use
 * @return AST node representing the primary expression
 */
ASTNode* parser_parse_primary(Parser* parser) {
    if (!parser) {
        return NULL;  // Invalid parser
    }
    
    // Check for different types of primary expressions
    
    // Parse null literal FIRST (before number parsing to avoid precedence issues)
    if (parser_check(parser, TOKEN_KEYWORD) && strcmp(parser_peek(parser)->text, "Null") == 0) {
        // Parse null literal
        Token* token = parser_peek(parser);
        parser_advance(parser);
        
        
        // Create null AST node
        ASTNode* literal = ast_create_null(token->line, token->column);
        if (literal) {
            // Check for member access: Null.method
            if (parser_check(parser, TOKEN_DOT)) {
                return parser_parse_member_access_chain(parser, literal);
            } else {
            return literal;
            }
        }
    }
    
    if (parser_check(parser, TOKEN_NUMBER)) {
        // Parse number literal
        Token* token = parser_peek(parser);
        parser_advance(parser);
        
        
        ASTNode* literal = ast_create_number(token->data.number_value, token->line, token->column);
        if (literal) {
            // Check for member access: 42.method
            if (parser_check(parser, TOKEN_DOT)) {
                return parser_parse_member_access_chain(parser, literal);
            } else {
            return literal;
            }
        }
    }
    
    if (parser_check(parser, TOKEN_STRING)) {
        // Parse string literal
        Token* token = parser_peek(parser);
        parser_advance(parser);
        
        ASTNode* literal = ast_create_string(token->data.string_value, token->line, token->column);
        if (literal) {
            // Check for member access: "string".method
            if (parser_check(parser, TOKEN_DOT)) {
                parser_advance(parser); // consume '.'
                
                // Parse the member name (can be identifier or keyword)
                if (!parser_check(parser, TOKEN_IDENTIFIER) && !parser_check(parser, TOKEN_KEYWORD)) {
                    parser_error(parser, "Expected member name after '.'");
                    ast_free(literal);
                    return NULL;
                }
                
                Token* member_token = parser_peek(parser);
                parser_advance(parser); // consume member name
                
                // Create member access node
                ASTNode* member_access = ast_create_member_access(literal, member_token->text, token->line, token->column);
                if (!member_access) {
                    ast_free(literal);
                    return NULL;
                }
                
                // Check for function calls on the member access: "string".method(args)
                if (parser_check(parser, TOKEN_LEFT_PAREN)) {
                    parser_advance(parser); // consume '('
                    
                    ASTNode** args = NULL;
                    size_t arg_count = 0;
                    size_t arg_capacity = 0;
                    
                    // Parse optional arguments
                    if (!parser_check(parser, TOKEN_RIGHT_PAREN)) {
                        while (1) {
                            ASTNode* arg = parser_parse_expression(parser);
                            if (!arg) {
                                parser_error(parser, "Function call arguments must be valid expressions");
                                break;
                            }
                            if (arg_count == arg_capacity) {
                                size_t new_cap = arg_capacity == 0 ? 4 : arg_capacity * 2;
                                ASTNode** new_args = (ASTNode**)shared_realloc_safe(args, new_cap * sizeof(ASTNode*), "parser", "unknown_function", 1133);
                                if (!new_args) {
                                    parser_error(parser, "Out of memory while parsing arguments");
                                    break;
                                }
                                args = new_args;
                                arg_capacity = new_cap;
                            }
                            args[arg_count++] = arg;
                            if (parser_check(parser, TOKEN_COMMA)) {
                                parser_advance(parser);
                                continue;
                            }
                            break;
                        }
                    }
                    
                    if (!parser_match(parser, TOKEN_RIGHT_PAREN)) {
                        parser_error(parser, "Expected ')' after arguments");
                    }
                    
                    // Create function call expression node
                    ASTNode* call = ast_create_function_call_expr(member_access, args, arg_count, token->line, token->column);
                    if (call) {
                        // Check for additional method chaining
                        return parser_parse_member_access_chain(parser, call);
                    } else {
                        ast_free(member_access);
                        return NULL;
                    }
                } else {
                    // Just member access, no function call
                    return member_access;
                }
            } else {
                // No member access, just return the literal
            return literal;
            }
        }
    }
    
    if (parser_check(parser, TOKEN_BOOL)) {
        // Parse boolean literal
        Token* token = parser_peek(parser);
        parser_advance(parser);
        
        // Create proper boolean AST node
        int bool_value = (strcmp(token->text, "True") == 0) ? 1 : 0;
        ASTNode* literal = ast_create_bool(bool_value, token->line, token->column);
        if (literal) {
            // Check for member access: True.method
            if (parser_check(parser, TOKEN_DOT)) {
                return parser_parse_member_access_chain(parser, literal);
            } else {
            return literal;
            }
        }
    }
    
    if (parser_check(parser, TOKEN_KEYWORD) && strcmp(parser_peek(parser)->text, "await") == 0) {
        // Parse await expression
        Token* token = parser_peek(parser);
        parser_advance(parser);
        
        // Parse the expression to await
        ASTNode* expression = parser_parse_expression(parser);
        if (!expression) {
            parser_error(parser, "Expected expression after 'await'");
            return NULL;
        }
        
        // Create await AST node
        ASTNode* await_node = ast_create_await(expression, token->line, token->column);
        if (!await_node) {
            ast_free(expression);
            return NULL;
        }
        
        return await_node;
    }
    
    if (parser_check(parser, TOKEN_IDENTIFIER) && strcmp(parser_peek(parser)->text, "Promise") == 0) {
        // Parse Promise creation
        Token* token = parser_peek(parser);
        parser_advance(parser);
        
        // Expect '(' and expression
        if (!parser_match(parser, TOKEN_LEFT_PAREN)) {
            parser_error(parser, "Expected '(' after 'Promise'");
            return NULL;
        }
        
        // Parse the expression inside Promise
        ASTNode* expression = parser_parse_expression(parser);
        if (!expression) {
            parser_error(parser, "Expected expression inside 'Promise'");
            return NULL;
        }
        
        if (!parser_match(parser, TOKEN_RIGHT_PAREN)) {
            parser_error(parser, "Expected ')' after Promise expression");
            ast_free(expression);
            return NULL;
        }
        
        // Create Promise AST node
        ASTNode* promise_node = ast_create_promise(expression, token->line, token->column);
        if (!promise_node) {
            ast_free(expression);
            return NULL;
        }
        
        return promise_node;
    }
    
    if (parser_check(parser, TOKEN_IDENTIFIER) || 
        (parser_check(parser, TOKEN_KEYWORD) && parser->current_token->text && 
         (strcmp(parser->current_token->text, "self") == 0 || strcmp(parser->current_token->text, "super") == 0))) {
        // Parse identifier or self keyword
        Token* ident_token = parser_peek(parser);
        parser_advance(parser);
        
        // Check if this is a simple function call: identifier '(' args? ')'
        if (parser_check(parser, TOKEN_LEFT_PAREN)) {
            parser_advance(parser); // consume '('
            
            ASTNode** args = NULL;
            size_t arg_count = 0;
            size_t arg_capacity = 0;
            
            // Parse optional arguments
            if (!parser_check(parser, TOKEN_RIGHT_PAREN)) {
                while (1) {
                    ASTNode* arg = parser_parse_expression(parser);
                    if (!arg) {
                        parser_error(parser, "Function call arguments must be valid expressions");
                        break;
                    }
                    if (arg_count == arg_capacity) {
                        size_t new_cap = arg_capacity == 0 ? 4 : arg_capacity * 2;
                        ASTNode** new_args = (ASTNode**)shared_realloc_safe(args, new_cap * sizeof(ASTNode*), "parser", "unknown_function", 1273);
                        if (!new_args) {
                            parser_error(parser, "Out of memory while parsing arguments");
                            break;
                        }
                        args = new_args;
                        arg_capacity = new_cap;
                    }
                    args[arg_count++] = arg;
                    if (parser_check(parser, TOKEN_COMMA)) {
                        parser_advance(parser);
                        continue;
                    }
                    break;
                }
            }
            
            if (!parser_match(parser, TOKEN_RIGHT_PAREN)) {
                parser_error(parser, "Expected ')' after arguments");
            }
            
            // Create simple function call node
            ASTNode* call = ast_create_function_call(ident_token->text, args, arg_count, ident_token->line, ident_token->column);
            if (!call && args) {
                shared_free_safe(args, "parser", "unknown_function", 1297);
            }
            return call;
        }
        
        // Create base identifier
        ASTNode* base = ast_create_identifier(ident_token->text, ident_token->line, ident_token->column);
        if (!base) {
            return NULL;
        }
        
        // Check if this is array indexing
        if (parser_check(parser, TOKEN_LEFT_BRACKET)) {
            parser_advance(parser); // consume '['
            
            // Parse the index expression
            ASTNode* index = parser_parse_expression(parser);
            if (!index) {
                parser_error(parser, "Expected index expression in array access");
                ast_free(base);
                return NULL;
            }
            
            // Expect ']'
            if (!parser_match(parser, TOKEN_RIGHT_BRACKET)) {
                parser_error(parser, "Expected ']' to close array index");
                ast_free(base);
                ast_free(index);
                return NULL;
            }
            
            // Create array access node
            ASTNode* access = ast_create_array_access(base, index, ident_token->line, ident_token->column);
            if (!access) {
                ast_free(base);
                ast_free(index);
                return NULL;
            }
            
            // Check for member access after array access: arr[index].method
            if (parser_check(parser, TOKEN_DOT)) {
                return parser_parse_member_access_chain(parser, access);
            }
            
            return access;
        }
        
        // Check for member access: identifier '.' member
        if (parser_check(parser, TOKEN_DOT)) {
            parser_advance(parser); // consume '.'
            
            // Parse the member name (can be identifier or keyword)
            if (!parser_check(parser, TOKEN_IDENTIFIER) && !parser_check(parser, TOKEN_KEYWORD)) {
                parser_error(parser, "Expected member name after '.'");
                ast_free(base);
                return NULL;
            }
            
            Token* member_token = parser_peek(parser);
            parser_advance(parser); // consume member name
            
            // Create member access node
            ASTNode* member_access = ast_create_member_access(base, member_token->text, ident_token->line, ident_token->column);
            if (!member_access) {
                ast_free(base);
                return NULL;
            }
            
            // Check for function calls on the member access: member_access(args)
            if (parser_check(parser, TOKEN_LEFT_PAREN)) {
                parser_advance(parser); // consume '('
                
                ASTNode** args = NULL;
                size_t arg_count = 0;
                size_t arg_capacity = 0;
                
                // Parse optional arguments
                if (!parser_check(parser, TOKEN_RIGHT_PAREN)) {
                    while (1) {
                        ASTNode* arg = parser_parse_expression(parser);
                        if (!arg) {
                            parser_error(parser, "Function call arguments must be valid expressions");
                            break;
                        }
                        if (arg_count == arg_capacity) {
                            size_t new_cap = arg_capacity == 0 ? 4 : arg_capacity * 2;
                            ASTNode** new_args = (ASTNode**)shared_realloc_safe(args, new_cap * sizeof(ASTNode*), "parser", "unknown_function", 1383);
                            if (!new_args) {
                                parser_error(parser, "Out of memory while parsing arguments");
                                break;
                            }
                            args = new_args;
                            arg_capacity = new_cap;
                        }
                        args[arg_count++] = arg;
                        if (parser_check(parser, TOKEN_COMMA)) {
                            parser_advance(parser);
                            continue;
                        }
                        break;
                    }
                }
                
                if (!parser_match(parser, TOKEN_RIGHT_PAREN)) {
                    parser_error(parser, "Expected ')' after arguments");
                }
                
                // Create function call expression node with the member access as the function
                ASTNode* call = ast_create_function_call_expr(member_access, args, arg_count, ident_token->line, ident_token->column);
                if (!call) {
                    if (args) {
                        shared_free_safe(args, "parser", "unknown_function", 1408);
                    }
                    return member_access;
                }
                
                // Check for additional method chaining
                return parser_parse_member_access_chain(parser, call);
            }
            
            return member_access;
        }
        
        return base;
    }
    

    
    if (parser_check(parser, TOKEN_LEFT_PAREN)) {
        // Parse parenthesized expression
        return parser_parse_grouping(parser);
    }
    
    if (parser_check(parser, TOKEN_LEFT_BRACKET)) {
        // Parse array literal [1, 2, 3]
        return parser_parse_array_literal(parser);
    }
    
    if (parser_check(parser, TOKEN_LEFT_BRACE)) {
        // Parse hash map or set literal {key: value} or {item1, item2}
        // We need to peek ahead to determine if it's a hash map (has colon) or set (no colon)
        int current_pos = parser->current_position;
        parser_advance(parser); // consume opening brace
        
        // Handle empty braces {} - default to set
        if (parser_check(parser, TOKEN_RIGHT_BRACE)) {
            parser_advance(parser); // consume closing brace
            return ast_create_set_literal(NULL, 0, parser->previous_token->line, parser->previous_token->column);
        }
        
        // Parse first expression to determine type
        ASTNode* first_expr = parser_parse_expression(parser);
        if (!first_expr) {
            parser_error(parser, "Expected expression in hash map or set literal");
            return NULL;
        }
        
        // Check if next token is colon (hash map) or comma/brace (set)
        if (parser_check(parser, TOKEN_COLON)) {
            // It's a hash map - rewind and parse as hash map
            ast_free(first_expr);
            parser->current_position = current_pos;
            return parser_parse_hash_map_literal(parser);
        } else {
            // It's a set - rewind and parse as set
            ast_free(first_expr);
            parser->current_position = current_pos;
            return parser_parse_set_literal(parser);
        }
    }
    
    // Check for lambda expressions: func (params) -> returnType: body end
    if (parser_check(parser, TOKEN_KEYWORD) && strcmp(parser_peek(parser)->text, "func") == 0) {
        return parser_parse_lambda_expression(parser);
    }
    
    // If we can't parse anything, report an error
    parser_error(parser, "Expected a primary expression");
    return NULL;
}

/**
 * @brief Parse function call expressions
 * 
 * Function calls have the form: function_name(arg1, arg2, ...)
 * This function handles the argument parsing and creates the appropriate AST node.
 * 
 * @param parser The parser to use
 * @return AST node representing the function call
 */
ASTNode* parser_parse_call(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // TODO: Implement function call parsing logic
    return NULL;  // Not implemented yet
}

/**
 * @brief Parse member access expressions
 * 
 * Member access allows accessing properties or methods of objects:
 * object.property or object.method()
 * 
 * @param parser The parser to use
 * @return AST node representing the member access
 */
ASTNode* parser_parse_member_access(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // TODO: Implement member access parsing logic
    return NULL;  // Not implemented yet
}

/**
 * @brief Parse array access expressions
 * 
 * Array access allows accessing elements by index: array[index]
 * 
 * @param parser The parser to use
 * @return AST node representing the array access
 */
ASTNode* parser_parse_array_access(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // TODO: Implement array access parsing logic
    return NULL;  // Not implemented yet
}

/**
 * @brief Parse parenthesized expressions
 * 
 * Parentheses can be used to override operator precedence: (a + b) * c
 * 
 * @param parser The parser to use
 * @return AST node representing the parenthesized expression
 */
ASTNode* parser_parse_grouping(Parser* parser) {
    if (!parser) {
        return NULL;  // Invalid parser
    }
    
    // Expect opening parenthesis
    if (!parser_match(parser, TOKEN_LEFT_PAREN)) {
        return NULL;
    }
    
    // Parse the expression inside the parentheses
    ASTNode* expression = parser_parse_expression(parser);
    if (!expression) {
        parser_error(parser, "Expected expression inside parentheses");
        return NULL;
    }
    
    // Expect closing parenthesis
    if (!parser_match(parser, TOKEN_RIGHT_PAREN)) {
        parser_error(parser, "Expected closing parenthesis");
        return NULL;
    }
    
            // For grouping, we just return the expression (parentheses don't change the AST structure)
        return expression;
    
    return NULL;
}

/**
 * @brief Parse literal values
 * 
 * Literals are the basic values in the language: numbers, strings, booleans,
 * null, and arrays/objects.
 * 
 * @param parser The parser to use
 * @return AST node representing the literal
 */
ASTNode* parser_parse_literal(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // TODO: Implement literal parsing logic
    return NULL;  // Not implemented yet
}

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
ASTNode* parser_parse_expression_statement(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // TODO: Implement expression statement parsing logic
    return NULL;  // Not implemented yet
}

/**
 * @brief Parse a declaration statement
 * 
 * Declaration statements introduce new variables, constants, or types:
 * let x = 42, const PI = 3.14, type Point = {x: Int, y: Int}
 * 
 * @param parser The parser to use
 * @return AST node representing the declaration
 */
ASTNode* parser_parse_declaration_statement(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // TODO: Implement declaration statement parsing logic
    return NULL;  // Not implemented yet
}

/**
 * @brief Parse a type annotation (supports union types like "String | Int")
 * 
 * @param parser The parser to use
 * @return Allocated string containing the type annotation
 */
char* parser_parse_type_annotation(Parser* parser) {
    if (!parser) return NULL;
    
    // Start with the first type (identifier or keyword like Null)
    if (!parser_check(parser, TOKEN_IDENTIFIER) && !parser_check(parser, TOKEN_KEYWORD)) {
        return NULL;
    }
    
    // Advance past the type token
    parser_advance(parser);
    
    char* result = strdup(parser->previous_token->text);
    
    // Check for union type (pipe operator)
    while (parser_check(parser, TOKEN_PIPE)) {
        parser_advance(parser);  // Consume the pipe
        
        // Expect another type after the pipe (identifier or keyword like Null)
        if (!parser_check(parser, TOKEN_IDENTIFIER) && !parser_check(parser, TOKEN_KEYWORD)) {
            shared_free_safe(result, "parser", "unknown_function", 1649);
            return NULL;
        }
        
        // Advance past the type token
        parser_advance(parser);
        
        // Append to the result: "String | Int"
        char* new_result = shared_malloc_safe(strlen(result) + strlen(parser->previous_token->text) + 4, "parser", "unknown_function", 1657);
        strcpy(new_result, result);
        strcat(new_result, " | ");
        strcat(new_result, parser->previous_token->text);
        
        shared_free_safe(result, "parser", "unknown_function", 1662);
        result = new_result;
    }
    
    // Check for optional type (question mark)
    if (parser_check(parser, TOKEN_QUESTION)) {
        parser_advance(parser);  // Consume the question mark
        
        // Append to the result: "String?" or "String | Int?"
        char* new_result = shared_malloc_safe(strlen(result) + 2, "parser", "unknown_function", 1671);
        strcpy(new_result, result);
        strcat(new_result, "?");
        
        shared_free_safe(result, "parser", "unknown_function", 1675);
        result = new_result;
    }
    
    return result;
}

/**
 * @brief Parse a variable declaration
 * 
 * Variable declarations use the 'let' or 'var' keyword:
 * let x = 42, var mutable_var = "hello"
 * 
 * @param parser The parser to use
 * @return AST node representing the variable declaration
 */
ASTNode* parser_parse_variable_declaration(Parser* parser) {
    if (!parser) {
        return NULL;  // Invalid parser
    }
    
    // The 'let' keyword has already been consumed by the statement parser
    // We can verify it was 'let' by checking the previous token
    if (!parser->previous_token || !parser->previous_token->text || 
        strcmp(parser->previous_token->text, "let") != 0) {
        parser_error(parser, "Internal parser error: expected 'let' keyword");
        return NULL;
    }
    
    // The 'let' keyword has already been consumed by the statement parser
    
    // Parse the variable name
    if (!parser_match(parser, TOKEN_IDENTIFIER)) {
        parser_error(parser, "Variable name must be a valid identifier (letters, numbers, underscores)");
        return NULL;
    }
    
    char* variable_name = strdup(parser->previous_token->text);
    // Capture line/column info for the variable name
    int var_line = parser->previous_token->line;
    int var_column = parser->previous_token->column;
    
    // Check for optional type annotation
    char* type_name = NULL;
    if (parser_check(parser, TOKEN_COLON)) {
        parser_advance(parser);  // Consume the colon
        
        // Check for array type [Type] or [Any] for heterogeneous arrays
        if (parser_check(parser, TOKEN_LEFT_BRACKET)) {
            parser_advance(parser);  // Consume the [
            
            if (!parser_check(parser, TOKEN_IDENTIFIER) && !parser_check(parser, TOKEN_KEYWORD)) {
                parser_error(parser, "Array type must specify element type (e.g., [Int], [String], [Any] for mixed types)");
                shared_free_safe(variable_name, "parser", "unknown_function", 1728);
                return NULL;
            }
            
            // Advance past the type token
            parser_advance(parser);
            
            char* element_type = strdup(parser->previous_token->text);
            
            if (!parser_match(parser, TOKEN_RIGHT_BRACKET)) {
                parser_error(parser, "Array type must end with ']'");
                shared_free_safe(variable_name, "parser", "unknown_function", 1739);
                shared_free_safe(element_type, "parser", "unknown_function", 1740);
                return NULL;
            }
            
            // Create array type string: [Type] or [Any]
            int len = strlen(element_type) + 3;
            type_name = shared_malloc_safe(len, "parser", "unknown_function", 1746);
            snprintf(type_name, len, "[%s]", element_type);
            shared_free_safe(element_type, "parser", "unknown_function", 1748);
        } else {
            // Regular type annotation or union type
            type_name = parser_parse_type_annotation(parser);
            if (!type_name) {
                parser_error(parser, "Type annotation must be a valid type name (Int, String, Bool, Float, or union type like String | Int)");
                shared_free_safe(variable_name, "parser", "unknown_function", 1754);
                return NULL;
            }
        }
    }
    // If no type annotation provided, type will be inferred from the initial value
    
    // Expect assignment operator
    if (!parser_match(parser, TOKEN_ASSIGN)) {
        parser_error(parser, "Variable declarations require an equals sign (=) followed by an initial value");
        // Create an error node instead of failing completely
        ASTNode* error_node = ast_create_error_node("Invalid variable declaration: missing '=' operator", 
                                                   parser->previous_token ? parser->previous_token->line : 0,
                                                   parser->previous_token ? parser->previous_token->column : 0);
        shared_free_safe(variable_name, "parser", "unknown_function", 1768);
        if (type_name) shared_free_safe(type_name, "parser", "unknown_function", 1769);
        return error_node;
    }
    
    // Parse the initial value
    ASTNode* initial_value = parser_parse_expression(parser);
    if (!initial_value) {
        parser_error(parser, "Variable declarations must have an initial value after the equals sign (=)");
        shared_free_safe(variable_name, "parser", "unknown_function", 1777);
        if (type_name) shared_free_safe(type_name, "parser", "unknown_function", 1778);
        return NULL;
    }
    
    // Expect semicolon
    if (parser_check(parser, TOKEN_SEMICOLON)) {
        parser_advance(parser);
    }
    
    // Create the variable declaration node with proper line/column info
    // Use the captured line/column info from the variable name token
    ASTNode* var_decl = ast_create_variable_declaration(variable_name, type_name, initial_value, 1, var_line, var_column);
    if (var_decl) {
        return var_decl;
    }
    
    // Cleanup on failure
    shared_free_safe(variable_name, "parser", "unknown_function", 1795);
    if (type_name) shared_free_safe(type_name, "parser", "unknown_function", 1796);
    return NULL;
}

/**
 * @brief Parse a constant declaration
 * 
 * Constant declarations use the 'const' keyword and must have an initial value:
 * const MAX_SIZE = 100, const GREETING = "Hello, World!"
 * 
 * @param parser The parser to use
 * @return AST node representing the constant declaration
 */
ASTNode* parser_parse_constant_declaration(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // TODO: Implement constant declaration parsing logic
    return NULL;  // Not implemented yet
}

/**
 * @brief Parse a type declaration
 * 
 * Type declarations define new types or type aliases:
 * type Point = {x: Int, y: Int}, type StringArray = [String]
 * 
 * @param parser The parser to use
 * @return AST node representing the type declaration
 */
ASTNode* parser_parse_type_declaration(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // TODO: Implement type declaration parsing logic
    return NULL;  // Not implemented yet
}

/**
 * @brief Parse an if statement
 * 
 * If statements have the form: if condition: block [else: block]
 * The else clause is optional.
 * 
 * @param parser The parser to use
 * @return AST node representing the if statement
 */
/**
 * @brief Parse an else-if statement (used internally for chaining)
 * 
 * This function parses an else-if statement that's part of a chain.
 * It expects the 'if' token to already be consumed.
 * 
 * @param parser The parser to use
 * @return AST node representing the else-if statement
 */
static ASTNode* parser_parse_else_if_statement(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    // The 'if' keyword has already been consumed

    // Parse condition expression
    ASTNode* condition = parser_parse_expression(parser);
    if (!condition) {
        parser_error(parser, "Expected condition after 'else if'");
        parser_synchronize(parser);
        return NULL;
    }

    // Expect ':'
    if (!parser_match(parser, TOKEN_COLON)) {
        parser_error(parser, "Expected ':' after else if condition");
        parser_synchronize(parser);
    }

    // Parse the then block - stop on 'else' to check for more else-if or else
    int saw_else = 0;
    ASTNode* then_block = parser_collect_block(parser, /*stop_on_else*/1, &saw_else);
    ASTNode* else_block = NULL;
    ASTNode* else_if_chain = NULL;

    if (saw_else) {
        // We are at 'else'; check if it's 'else if' or just 'else'
        if (parser_check(parser, TOKEN_KEYWORD) && parser->current_token && parser->current_token->text && 
            strcmp(parser->current_token->text, "else") == 0) {
            
            // Consume 'else' and check if next token is 'if'
            parser_advance(parser); // consume 'else'
            
            if (parser_check(parser, TOKEN_KEYWORD) && parser->current_token && parser->current_token->text && 
                strcmp(parser->current_token->text, "if") == 0) {
                
                // This is another 'else if' - parse it recursively
                parser_advance(parser); // consume 'if'
                
                // Recursively parse the next else if statement
                else_if_chain = parser_parse_else_if_statement(parser);
                if (!else_if_chain) {
                    parser_error(parser, "Failed to parse else if statement");
                    return NULL;
                }
                
            } else {
                // This is just 'else' - expect ':' and parse else block
                if (!parser_match(parser, TOKEN_COLON)) {
                    parser_error(parser, "Expected ':' after else");
                    parser_synchronize(parser);
                }
                // Collect else block until 'end'
                int dummy = 0;
                else_block = parser_collect_block(parser, /*stop_on_else*/0, &dummy);
            }
        }
    }

    return ast_create_if_statement(condition, then_block, else_block, else_if_chain, 0, 0);
}

ASTNode* parser_parse_if_statement(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    // The 'if' keyword has already been consumed by the statement parser

    // Parse condition expression
    ASTNode* condition = parser_parse_expression(parser);
    if (!condition) {
        parser_error(parser, "Expected condition after 'if'");
        parser_synchronize(parser);
        return NULL;
    }

    // Expect ':'
    if (!parser_match(parser, TOKEN_COLON)) {
        parser_error(parser, "Expected ':' after if condition");
        parser_synchronize(parser);
    }

    // Parse the then block - stop on 'else' to check for else-if or else
    int saw_else = 0;
    ASTNode* then_block = parser_collect_block(parser, /*stop_on_else*/1, &saw_else);
    ASTNode* else_block = NULL;
    ASTNode* else_if_chain = NULL;

    if (saw_else) {
        // We are at 'else'; check if it's 'else if' or just 'else'
        if (parser_check(parser, TOKEN_KEYWORD) && parser->current_token && parser->current_token->text && 
            strcmp(parser->current_token->text, "else") == 0) {
            
            // Consume 'else' and check if next token is 'if'
            parser_advance(parser); // consume 'else'
            
            if (parser_check(parser, TOKEN_KEYWORD) && parser->current_token && parser->current_token->text && 
                strcmp(parser->current_token->text, "if") == 0) {
                
                // This is an 'else if' - parse it using the else-if parser
                parser_advance(parser); // consume 'if'
                
                // Parse the else if statement (which will handle its own else-if chains)
                else_if_chain = parser_parse_else_if_statement(parser);
                if (!else_if_chain) {
                    parser_error(parser, "Failed to parse else if statement");
                    return NULL;
                }
                
            } else {
                // This is just 'else' - expect ':' and parse else block
                if (!parser_match(parser, TOKEN_COLON)) {
                    parser_error(parser, "Expected ':' after else");
                    parser_synchronize(parser);
                }
                // Collect else block until 'end'
                int dummy = 0;
                else_block = parser_collect_block(parser, /*stop_on_else*/0, &dummy);
            }
        }
    }

    // Always expect 'end' to close the if statement
    if (!(parser_check(parser, TOKEN_KEYWORD) && parser->current_token && parser->current_token->text && strcmp(parser->current_token->text, "end") == 0)) {
        parser_error(parser, "Expected 'end' to close if block");
        parser_synchronize(parser);
    } else {
        parser_advance(parser); // consume 'end'
    }

    return ast_create_if_statement(condition, then_block, else_block, else_if_chain, 0, 0);
}

/**
 * @brief Parse a switch statement
 * 
 * Switch statements provide multi-way branching:
 * switch value: case pattern1: block case pattern2: block end
 * 
 * @param parser The parser to use
 * @return AST node representing the switch statement
 */
ASTNode* parser_parse_switch_statement(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // TODO: Implement switch statement parsing logic
    return NULL;  // Not implemented yet
}

/**
 * @brief Parse a while loop
 * 
 * While loops repeat a block while a condition is true:
 * while condition: block
 * 
 * @param parser The parser to use
 * @return AST node representing the while loop
 */
ASTNode* parser_parse_while_loop(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    // 'while' token has already been consumed by the statement parser

    // Parse condition
    ASTNode* condition = parser_parse_expression(parser);
    if (!condition) {
        parser_error(parser, "Expected condition after 'while'");
        parser_synchronize(parser);
        return NULL;
    }

    // Expect ':'
    if (!parser_match(parser, TOKEN_COLON)) {
        parser_error(parser, "Expected ':' after while condition");
        parser_synchronize(parser);
    }

    int dummy = 0;
    ASTNode* body = parser_collect_block(parser, /*stop_on_else*/0, &dummy);

    // Expect 'end'
    if (!(parser_check(parser, TOKEN_KEYWORD) && parser->current_token && parser->current_token->text && strcmp(parser->current_token->text, "end") == 0)) {
        parser_error(parser, "Expected 'end' to close while block");
        parser_synchronize(parser);
    } else {
        parser_advance(parser); // consume 'end'
    }

    return ast_create_while_loop(condition, body, 0, 0);
}

/**
 * @brief Parse a for loop
 * 
 * For loops iterate over collections or ranges:
 * for item in collection: block, for i in 0..10: block
 * 
 * @param parser The parser to use
 * @return AST node representing the for loop
 */
ASTNode* parser_parse_for_loop(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    // The 'for' keyword has already been consumed by the statement parser

    // Iterator name
    if (!parser_match(parser, TOKEN_IDENTIFIER)) {
        parser_error(parser, "Expected iterator name after 'for'");
        parser_synchronize(parser);
        return NULL;
    }
    char* iterator_name = strdup(parser->previous_token->text);

    // Expect 'in'
    if (!(parser_check(parser, TOKEN_KEYWORD) && parser->current_token && parser->current_token->text && strcmp(parser->current_token->text, "in") == 0)) {
        parser_error(parser, "Expected 'in' in for-loop");
        shared_free_safe(iterator_name, "parser", "unknown_function", 2075);
        parser_synchronize(parser);
        return NULL;
    }
    parser_advance(parser); // consume 'in'

    // Collection/expression
    ASTNode* collection = parser_parse_expression(parser);
    if (!collection) {
        parser_error(parser, "Expected collection expression after 'in'");
        shared_free_safe(iterator_name, "parser", "unknown_function", 2085);
        parser_synchronize(parser);
        return NULL;
    }

    // Expect ':'
    if (!parser_match(parser, TOKEN_COLON)) {
        parser_error(parser, "Expected ':' after for header");
        shared_free_safe(iterator_name, "parser", "unknown_function", 2093);
        parser_synchronize(parser);
    }

    int dummy = 0;
    ASTNode* body = parser_collect_block(parser, /*stop_on_else*/0, &dummy);

    // Expect 'end'
    if (!(parser_check(parser, TOKEN_KEYWORD) && parser->current_token && parser->current_token->text && strcmp(parser->current_token->text, "end") == 0)) {
        parser_error(parser, "Expected 'end' to close for block");
        parser_synchronize(parser);
    } else {
        parser_advance(parser);
    }

    // Build a simple block where we define iterator each iteration (execution to be handled in interpreter later)
    // For now, return a for_loop AST node if available, else reuse block holder
    return ast_create_for_loop(iterator_name, collection, body, 0, 0);
}

/**
 * @brief Parse a try-catch statement
 * 
 * Try-catch statements handle exceptions:
 * try: block catch error: block
 * 
 * @param parser The parser to use
 * @return AST node representing the try-catch statement
 */
ASTNode* parser_parse_try_statement(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // TODO: Implement try-catch statement parsing logic
    return NULL;  // Not implemented yet
}

/**
 * @brief Parse a match statement
 * 
 * Match statements provide pattern matching:
 * match value: case pattern1 => result case pattern2 => result end
 * 
 * @param parser The parser to use
 * @return AST node representing the match statement
 */
ASTNode* parser_parse_match_statement(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // The 'match' keyword has already been consumed by the statement parser
    
    // Parse the expression to match against
    ASTNode* expression = parser_parse_expression(parser);
    if (!expression) {
        parser_error(parser, "Expected expression after \"match\"");
        return NULL;
    }

    // Expect colon
    if (!parser_match(parser, TOKEN_COLON)) {
        parser_error(parser, "Expected \":\" after match expression");
        ast_free(expression);
        return NULL;
    }
    
    // Parse cases until "end"
    ASTNode** cases = NULL;
    size_t case_count = 0;
    size_t case_capacity = 0;
    
    while (parser->current_token && parser->current_token->type != TOKEN_EOF) {
        if (parser->current_token->type == TOKEN_KEYWORD && parser->current_token->text) {
            if (strcmp(parser->current_token->text, "end") == 0) {
                break;
            }
        }
        
        // Parse case or else
        if (parser->current_token->type == TOKEN_KEYWORD && parser->current_token->text) {
            if (strcmp(parser->current_token->text, "case") == 0) {
                // Parse case pattern
                parser_advance(parser); // consume "case"
                
                ASTNode* pattern = parser_parse_expression(parser);
                if (!pattern) {
                    parser_error(parser, "Expected pattern after \"case\"");
                    parser_synchronize(parser);
                    continue;
                }
                
                // Check for lambda style (=>) or block style (:)
                if (parser_check(parser, TOKEN_ARROW)) {
                    // Lambda style: case pattern => expression
                    parser_advance(parser); // consume "=>"
                    
                    ASTNode* result = parser_parse_expression(parser);
                    if (!result) {
                        parser_error(parser, "Expected expression after '=>' in lambda case");
                        ast_free(pattern);
                        parser_synchronize(parser);
                        continue;
                    }
                    
                    // Create lambda case node
                    ASTNode* case_node = ast_create_spore_case(pattern, result, 1, 0, 0);
                    if (case_node) {
                        if (case_count >= case_capacity) {
                            size_t new_capacity = case_capacity == 0 ? 4 : case_capacity * 2;
                            ASTNode** new_cases = shared_realloc_safe(cases, new_capacity * sizeof(ASTNode*), "parser", "unknown_function", 2204);
                            if (new_cases) {
                                cases = new_cases;
                                case_capacity = new_capacity;
                            }
                        }
                        cases[case_count++] = case_node;
                    }
                } else if (parser_match(parser, TOKEN_COLON)) {
                    // Block style: case pattern: statements until next case/else/end
                    ASTNode** case_statements = NULL;
                    size_t case_stmt_count = 0;
                    size_t case_stmt_capacity = 0;
                    
                    while (parser->current_token && parser->current_token->type != TOKEN_EOF) {
                        // Check for keywords that end this case
                        if (parser->current_token->type == TOKEN_KEYWORD && parser->current_token->text) {
                            if (strcmp(parser->current_token->text, "case") == 0 ||
                                strcmp(parser->current_token->text, "else") == 0 ||
                                strcmp(parser->current_token->text, "end") == 0) {
                                break;
                            }
                        }
                        
                        ASTNode* stmt = parser_parse_statement(parser);
                        if (stmt) {
                            if (case_stmt_count >= case_stmt_capacity) {
                                size_t new_capacity = case_stmt_capacity == 0 ? 4 : case_stmt_capacity * 2;
                                ASTNode** new_statements = shared_realloc_safe(case_statements, new_capacity * sizeof(ASTNode*), "parser", "unknown_function", 2232);
                                if (new_statements) {
                                    case_statements = new_statements;
                                    case_stmt_capacity = new_capacity;
                                }
                            }
                            case_statements[case_stmt_count++] = stmt;
                        }
                    }
                    
                    ASTNode* body = ast_create_block(case_statements, case_stmt_count, 0, 0);
                    if (!body && case_statements) {
                        shared_free_safe(case_statements, "parser", "unknown_function", 2244);
                    }
                    
                    if (!body) {
                        parser_error(parser, "Expected case body");
                        ast_free(pattern);
                        parser_synchronize(parser);
                        continue;
                    }
                    
                    // Create block case node
                    ASTNode* case_node = ast_create_spore_case(pattern, body, 0, 0, 0);
                    if (case_node) {
                        if (case_count >= case_capacity) {
                            size_t new_capacity = case_capacity == 0 ? 4 : case_capacity * 2;
                            ASTNode** new_cases = shared_realloc_safe(cases, new_capacity * sizeof(ASTNode*), "parser", "unknown_function", 2259);
                            if (new_cases) {
                                cases = new_cases;
                                case_capacity = new_capacity;
                            }
                        }
                        cases[case_count++] = case_node;
                    }
                } else {
                    parser_error(parser, "Expected \"=>\" or \":\" after case pattern");
                    ast_free(pattern);
                    parser_synchronize(parser);
                    continue;
                }
            } else if (strcmp(parser->current_token->text, "else") == 0) {
                // Parse else (default) case
                parser_advance(parser); // consume "else"
                
                // Check for lambda style (=>) or block style (:)
                if (parser_check(parser, TOKEN_ARROW)) {
                    // Lambda style: else => expression
                    parser_advance(parser); // consume "=>"
                    
                    ASTNode* result = parser_parse_expression(parser);
                    if (!result) {
                        parser_error(parser, "Expected expression after '=>' in else case");
                        parser_synchronize(parser);
                        continue;
                    }
                    
                    ASTNode* else_node = ast_create_spore_case(NULL, result, 1, 0, 0);
                    if (else_node) {
                        if (case_count >= case_capacity) {
                            size_t new_capacity = case_capacity == 0 ? 4 : case_capacity * 2;
                            ASTNode** new_cases = shared_realloc_safe(cases, new_capacity * sizeof(ASTNode*), "parser", "unknown_function", 2293);
                            if (new_cases) {
                                cases = new_cases;
                                case_capacity = new_capacity;
                            }
                        }
                        cases[case_count++] = else_node;
                    }
                } else if (parser_match(parser, TOKEN_COLON)) {
                    // Block style: else: statements until end
                    ASTNode** else_statements = NULL;
                    size_t else_stmt_count = 0;
                    size_t else_stmt_capacity = 0;
                    
                    while (parser->current_token && parser->current_token->type != TOKEN_EOF) {
                        // Check for 'end' keyword to end else case
                        if (parser->current_token->type == TOKEN_KEYWORD && parser->current_token->text) {
                            if (strcmp(parser->current_token->text, "end") == 0) {
                                break;
                            }
                        }
                        
                        ASTNode* stmt = parser_parse_statement(parser);
                        if (stmt) {
                            if (else_stmt_count >= else_stmt_capacity) {
                                size_t new_capacity = else_stmt_capacity == 0 ? 4 : else_stmt_capacity * 2;
                                ASTNode** new_statements = shared_realloc_safe(else_statements, new_capacity * sizeof(ASTNode*), "parser", "unknown_function", 2319);
                                if (new_statements) {
                                    else_statements = new_statements;
                                    else_stmt_capacity = new_capacity;
                                }
                            }
                            else_statements[else_stmt_count++] = stmt;
                        }
                    }
                    
                    ASTNode* body = ast_create_block(else_statements, else_stmt_count, 0, 0);
                    if (!body && else_statements) {
                        shared_free_safe(else_statements, "parser", "unknown_function", 2331);
                    }
                    
                    if (!body) {
                        parser_error(parser, "Expected else body");
                        parser_synchronize(parser);
                        continue;
                    }
                    
                    ASTNode* else_node = ast_create_spore_case(NULL, body, 0, 0, 0);
                    if (else_node) {
                        if (case_count >= case_capacity) {
                            size_t new_capacity = case_capacity == 0 ? 4 : case_capacity * 2;
                            ASTNode** new_cases = shared_realloc_safe(cases, new_capacity * sizeof(ASTNode*), "parser", "unknown_function", 2344);
                            if (new_cases) {
                                cases = new_cases;
                                case_capacity = new_capacity;
                            }
                        }
                        cases[case_count++] = else_node;
                    }
                } else {
                    parser_error(parser, "Expected \"=>\" or \":\" after else");
                    parser_synchronize(parser);
                    continue;
                }
            } else {
                // Unknown keyword, skip
                parser_advance(parser);
            }
        } else {
            // Not a keyword, skip
            parser_advance(parser);
        }
    }
    
    // Expect "end"
    if (!parser_match(parser, TOKEN_KEYWORD) || 
        !parser->previous_token || 
        !parser->previous_token->text || 
        strcmp(parser->previous_token->text, "end") != 0) {
        parser_error(parser, "Expected \"end\" to close match statement");
        // Clean up cases
        for (size_t i = 0; i < case_count; i++) {
            ast_free(cases[i]);
        }
        shared_free_safe(cases, "parser", "unknown_function", 2377);
        ast_free(expression);
        return NULL;
    }
    
    ASTNode* match_node = ast_create_spore(expression, cases, case_count, NULL, 0, 0);
    if (!match_node && cases) {
        shared_free_safe(cases, "parser", "unknown_function", 2384);
    }
    
    return match_node;
}

/**
 * @brief Parse a return statement
 * 
 * Return statements exit functions with optional values:
 * return, return 42, return "success"
 * 
 * @param parser The parser to use
 * @return AST node representing the return statement
 */
ASTNode* parser_parse_return_statement(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    // The 'return' keyword has already been consumed by the statement parser

    // Optional expression until ';' or end-of-block keyword
    ASTNode* value = NULL;
    if (parser->current_token && parser->current_token->type != TOKEN_SEMICOLON &&
        !(parser->current_token->type == TOKEN_KEYWORD && parser->current_token->text && strcmp(parser->current_token->text, "end") == 0)) {
        value = parser_parse_expression(parser);
        if (!value) {
            parser_error(parser, "Expected expression after 'return'");
        }
    }

    // Optional semicolon termination
    if (parser_check(parser, TOKEN_SEMICOLON)) {
        parser_advance(parser);
    }

    // Build return node
    ASTNode* node = ast_create_return(value, 0, 0);
    return node;
}

/**
 * @brief Parse a break statement
 * 
 * Break statements exit loops early:
 * break, break loop_name
 * 
 * @param parser The parser to use
 * @return AST node representing the break statement
 */
ASTNode* parser_parse_break_statement(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // TODO: Implement break statement parsing logic
    return NULL;  // Not implemented yet
}

/**
 * @brief Parse a continue statement
 * 
 * Continue statements skip to the next iteration of a loop:
 * continue, continue loop_name
 * 
 * @param parser The parser to use
 * @return AST node representing the continue statement
 */
ASTNode* parser_parse_continue_statement(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // TODO: Implement continue statement parsing logic
    return NULL;  // Not implemented yet
}

/**
 * @brief Parse a throw statement
 * 
 * Throw statements raise exceptions:
 * throw "error message", throw CustomError("details")
 * 
 * @param parser The parser to use
 * @return AST node representing the throw statement
 */
ASTNode* parser_parse_throw_statement(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // The 'throw' keyword has already been consumed by the statement parser
    
    // Parse the expression to throw (required)
    ASTNode* value = parser_parse_expression(parser);
    if (!value) {
        parser_error(parser, "Expected expression after 'throw'");
        return NULL;
    }
    
    // Expect semicolon or end of statement
    if (parser_check(parser, TOKEN_SEMICOLON)) {
        parser_advance(parser);  // Consume semicolon
    }
    
    return ast_create_throw(value, parser->previous_token ? parser->previous_token->line : 0, 
                           parser->previous_token ? parser->previous_token->column : 0);
}

/**
 * @brief Parse a block of statements
 * 
 * Blocks group multiple statements together:
 * { statement1; statement2; } or statement1; statement2; end
 * 
 * @param parser The parser to use
 * @return AST node representing the block
 */
ASTNode* parser_parse_block(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    // Expect ':' then collect statements until 'end'
    if (!parser_match(parser, TOKEN_COLON)) {
        parser_error(parser, "Expected ':' to start block");
        parser_synchronize(parser);
    }
    int dummy = 0;
    ASTNode* body = parser_collect_block(parser, /*stop_on_else*/0, &dummy);
    if (!(parser_check(parser, TOKEN_KEYWORD) && parser->current_token && parser->current_token->text && strcmp(parser->current_token->text, "end") == 0)) {
        parser_error(parser, "Expected 'end' to close block");
        parser_synchronize(parser);
    } else {
        parser_advance(parser);
    }
    return body;
}

/**
 * @brief Parse a function declaration
 * 
 * Function declarations define reusable code blocks:
 * function name(params): block
 * 
 * @param parser The parser to use
 * @return AST node representing the function declaration
 */
ASTNode* parser_parse_function_declaration(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    // The 'func' keyword has already been consumed by the statement parser

    // Function name
    if (!parser_match(parser, TOKEN_IDENTIFIER)) {
        parser_error(parser, "Expected function name");
        parser_synchronize(parser);
        return NULL;
    }
    char* func_name = strdup(parser->previous_token->text);

    // Parameters
    if (!parser_match(parser, TOKEN_LEFT_PAREN)) {
        parser_error(parser, "Expected '(' after function name");
        parser_synchronize(parser);
    }
    // Parse simple comma-separated identifiers with optional : Type (ignored for now)
    ASTNode** params = NULL; size_t param_count = 0; size_t param_cap = 0;
    if (!parser_check(parser, TOKEN_RIGHT_PAREN)) {
        while (1) {
            if (!parser_match(parser, TOKEN_IDENTIFIER)) {
                parser_error(parser, "Expected parameter name");
                parser_synchronize(parser);
                break;
            }
            // Create parameter node
            if (parser->previous_token && parser->previous_token->text) {
                if (param_count == param_cap) {
                    size_t new_cap = param_cap == 0 ? 4 : param_cap * 2;
                    ASTNode** new_params = (ASTNode**)shared_realloc_safe(params, new_cap * sizeof(ASTNode*), "parser", "unknown_function", 2563);
                    if (!new_params) { parser_error(parser, "Out of memory while parsing parameters"); break; }
                    params = new_params; param_cap = new_cap;
                }
                
                char* param_name = strdup(parser->previous_token->text); // Store the parameter name
                
                // Check for type annotation
                if (parser_check(parser, TOKEN_COLON)) {
                    parser_advance(parser);
                    char* type_name = parser_parse_type_annotation(parser);
                    if (!type_name) {
                        parser_error(parser, "Expected type name after ':' (supports union types like String | Int)");
                        parser_synchronize(parser);
                        shared_free_safe(param_name, "parser", "unknown_function", 2577);
                        break;
                    }
                    // Create typed parameter node
                    ASTNode* tparam = ast_create_typed_parameter(param_name, type_name, 0, 0);
                    shared_free_safe(type_name, "parser", "unknown_function", 2582);
                    params[param_count++] = tparam;
                } else {
                    // Create regular identifier node for untyped parameter
                    ASTNode* pid = ast_create_identifier(param_name, 0, 0);
                    params[param_count++] = pid;
                }
                shared_free_safe(param_name, "parser", "unknown_function", 2589);
            }
            if (parser_check(parser, TOKEN_COMMA)) { parser_advance(parser); continue; }
            break;
        }
    }
    if (!parser_match(parser, TOKEN_RIGHT_PAREN)) {
        parser_error(parser, "Expected ')' after parameters");
        parser_synchronize(parser);
    }

    // Optional return type: -> Type
    char* return_type = NULL;
    if (parser_check(parser, TOKEN_RETURN_ARROW)) {
        parser_advance(parser);
        // return type identifier or union type
        return_type = parser_parse_type_annotation(parser);
        if (!return_type) {
            parser_error(parser, "Expected return type after '->' (supports union types like String | Int)");
            parser_synchronize(parser);
        }
    }

    // Expect ':' and body until 'end'
    if (!parser_match(parser, TOKEN_COLON)) {
        parser_error(parser, "Expected ':' to start function body");
        parser_synchronize(parser);
    }
    int dummy = 0;
    ASTNode* body = parser_collect_block(parser, /*stop_on_else*/0, &dummy);
    if (!(parser_check(parser, TOKEN_KEYWORD) && parser->current_token && parser->current_token->text && strcmp(parser->current_token->text, "end") == 0)) {
        parser_error(parser, "Expected 'end' to close function");
        parser_synchronize(parser);
    } else {
        parser_advance(parser);
    }
    // Build function node (store name and body)
    ASTNode* func = ast_create_function(func_name, params, param_count, return_type, body, 0, 0);
    
    // Clean up return type if we allocated it
    if (return_type) {
        shared_free_safe(return_type, "parser", "unknown_function", 2630);
    }
    
    return func;
}

/**
 * @brief Parse an async function declaration
 * 
 * Async function declarations have the syntax: async func name(params) -> returnType: body end
 * or just: async func name(params): body end
 * 
 * @param parser The parser to use
 * @return AST node representing the async function declaration
 */
ASTNode* parser_parse_async_function_declaration(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // The 'async' keyword has already been consumed by the statement parser
    // Now we expect 'func'
    if (!parser_match(parser, TOKEN_KEYWORD) || 
        !parser->previous_token || 
        !parser->previous_token->text || 
        strcmp(parser->previous_token->text, "func") != 0) {
        parser_error(parser, "Expected 'func' after 'async'");
        parser_synchronize(parser);
        return NULL;
    }
    
    // Function name
    if (!parser_match(parser, TOKEN_IDENTIFIER)) {
        parser_error(parser, "Expected function name");
        parser_synchronize(parser);
        return NULL;
    }
    char* func_name = strdup(parser->previous_token->text);

    // Parameters
    if (!parser_match(parser, TOKEN_LEFT_PAREN)) {
        parser_error(parser, "Expected '(' after function name");
        parser_synchronize(parser);
    }
    // Parse simple comma-separated identifiers with optional : Type (ignored for now)
    ASTNode** params = NULL; size_t param_count = 0; size_t param_cap = 0;
    if (!parser_check(parser, TOKEN_RIGHT_PAREN)) {
        while (1) {
            if (!parser_match(parser, TOKEN_IDENTIFIER)) {
                parser_error(parser, "Expected parameter name");
                parser_synchronize(parser);
                break;
            }
            // Create parameter node
            if (parser->previous_token && parser->previous_token->text) {
                if (param_count == param_cap) {
                    size_t new_cap = param_cap == 0 ? 4 : param_cap * 2;
                    ASTNode** new_params = (ASTNode**)shared_realloc_safe(params, new_cap * sizeof(ASTNode*), "parser", "unknown_function", 2687);
                    if (!new_params) { parser_error(parser, "Out of memory while parsing parameters"); break; }
                    params = new_params; param_cap = new_cap;
                }
                
                char* param_name = strdup(parser->previous_token->text); // Store the parameter name
                
                // Check for type annotation
                if (parser_check(parser, TOKEN_COLON)) {
                    parser_advance(parser);
                    char* type_name = parser_parse_type_annotation(parser);
                    if (!type_name) {
                        parser_error(parser, "Expected type name after ':' (supports union types like String | Int)");
                        parser_synchronize(parser);
                        shared_free_safe(param_name, "parser", "unknown_function", 2701);
                        break;
                    }
                    // Create typed parameter node
                    ASTNode* tparam = ast_create_typed_parameter(param_name, type_name, 0, 0);
                    shared_free_safe(type_name, "parser", "unknown_function", 2706);
                    params[param_count++] = tparam;
                } else {
                    // Create regular identifier node for untyped parameter
                    ASTNode* pid = ast_create_identifier(param_name, 0, 0);
                    params[param_count++] = pid;
                }
                shared_free_safe(param_name, "parser", "unknown_function", 2713);
            }
            if (parser_check(parser, TOKEN_COMMA)) { parser_advance(parser); continue; }
            break;
        }
    }
    if (!parser_match(parser, TOKEN_RIGHT_PAREN)) {
        parser_error(parser, "Expected ')' after parameters");
        parser_synchronize(parser);
    }

    // Optional return type: -> Type
    char* return_type = NULL;
    if (parser_check(parser, TOKEN_RETURN_ARROW)) {
        parser_advance(parser);
        // return type identifier or union type
        return_type = parser_parse_type_annotation(parser);
        if (!return_type) {
            parser_error(parser, "Expected return type after '->' (supports union types like String | Int)");
            parser_synchronize(parser);
        }
    }

    // Expect ':' and body until 'end'
    if (!parser_match(parser, TOKEN_COLON)) {
        parser_error(parser, "Expected ':' to start function body");
        parser_synchronize(parser);
    }
    int dummy = 0;
    ASTNode* body = parser_collect_block(parser, /*stop_on_else*/0, &dummy);
    if (!(parser_check(parser, TOKEN_KEYWORD) && parser->current_token && parser->current_token->text && strcmp(parser->current_token->text, "end") == 0)) {
        parser_error(parser, "Expected 'end' to close function");
        parser_synchronize(parser);
    } else {
        parser_advance(parser);
    }
    // Build async function node (store name and body)
    ASTNode* func = ast_create_async_function(func_name, params, param_count, return_type, body, 0, 0);
    
    // Clean up return type if we allocated it
    if (return_type) {
        shared_free_safe(return_type, "parser", "unknown_function", 2754);
    }
    
    return func;
}

/**
 * @brief Parse a lambda expression
 * 
 * Lambda expressions have the syntax: func (params) -> returnType: body end
 * or just: func (params): body end
 * 
 * @param parser The parser to use
 * @return AST node representing the lambda expression
 */
ASTNode* parser_parse_lambda_expression(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // The 'func' keyword has already been consumed by the primary parser
    parser_advance(parser); // consume 'func'
    
    // Parameters
    if (!parser_match(parser, TOKEN_LEFT_PAREN)) {
        parser_error(parser, "Expected '(' after 'func' in lambda expression");
        parser_synchronize(parser);
        return NULL;
    }
    
    // Parse simple comma-separated identifiers with optional : Type (ignored for now)
    ASTNode** params = NULL; 
    size_t param_count = 0; 
    size_t param_cap = 0;
    
    if (!parser_check(parser, TOKEN_RIGHT_PAREN)) {
        while (1) {
            if (!parser_match(parser, TOKEN_IDENTIFIER)) {
                parser_error(parser, "Expected parameter name");
                parser_synchronize(parser);
                break;
            }
            // Create identifier node for parameter
            if (parser->previous_token && parser->previous_token->text) {
                if (param_count == param_cap) {
                    size_t new_cap = param_cap == 0 ? 4 : param_cap * 2;
                    ASTNode** new_params = (ASTNode**)shared_realloc_safe(params, new_cap * sizeof(ASTNode*), "parser", "unknown_function", 2800);
                    if (!new_params) { 
                        parser_error(parser, "Out of memory while parsing parameters"); 
                        break; 
                    }
                    params = new_params; 
                    param_cap = new_cap;
                }
                ASTNode* pid = ast_create_identifier(parser->previous_token->text, 0, 0);
                params[param_count++] = pid;
            }
            // Optional : Type (skip)
            if (parser_check(parser, TOKEN_COLON)) {
                parser_advance(parser);
                if (!parser_match(parser, TOKEN_IDENTIFIER)) {
                    parser_error(parser, "Expected type name after ':'");
                    parser_synchronize(parser);
                }
            }
            if (parser_check(parser, TOKEN_COMMA)) { 
                parser_advance(parser); 
                continue; 
            }
            break;
        }
    }
    
    if (!parser_match(parser, TOKEN_RIGHT_PAREN)) {
        parser_error(parser, "Expected ')' after parameters");
        parser_synchronize(parser);
    }

    // Optional return type: -> Type
    char* return_type = NULL;
    if (parser_check(parser, TOKEN_RETURN_ARROW)) {
        parser_advance(parser);
        // return type identifier or union type
        return_type = parser_parse_type_annotation(parser);
        if (!return_type) {
            parser_error(parser, "Expected return type after '->' (supports union types like String | Int)");
            parser_synchronize(parser);
        }
    }

    // Expect ':' and body until 'end'
    if (!parser_match(parser, TOKEN_COLON)) {
        parser_error(parser, "Expected ':' to start lambda body");
        parser_synchronize(parser);
        return NULL;
    }
    
    int dummy = 0;
    ASTNode* body = parser_collect_block(parser, /*stop_on_else*/0, &dummy);
    if (!(parser_check(parser, TOKEN_KEYWORD) && parser->current_token && parser->current_token->text && strcmp(parser->current_token->text, "end") == 0)) {
        parser_error(parser, "Expected 'end' to close lambda expression");
        parser_synchronize(parser);
    } else {
        parser_advance(parser);
    }
    
    // Build lambda node
    ASTNode* lambda = ast_create_lambda(params, param_count, return_type, body, 0, 0);
    
    // Clean up return type if we allocated it
    if (return_type) {
        shared_free_safe(return_type, "parser", "unknown_function", 2865);
    }
    
    return lambda;
}

/**
 * @brief Parse a class declaration
 * 
 * Class declarations define object types:
 * class Name: field1: Type, field2: Type; method1(): block
 * 
 * @param parser The parser to use
 * @return AST node representing the class declaration
 */
ASTNode* parser_parse_class_declaration(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // Parse 'class' keyword (already consumed by caller)
    // Parse class name
    if (!parser_match(parser, TOKEN_IDENTIFIER)) {
        parser_error(parser, "Expected class name after 'class'");
        parser_synchronize(parser);
        return NULL;
    }
    
    char* class_name = strdup(parser->previous_token->text);
    char* parent_class = NULL;
    
    // Check for inheritance (extends keyword)
    if (parser_check(parser, TOKEN_KEYWORD) && 
        parser->current_token->text && 
        strcmp(parser->current_token->text, "extends") == 0) {
        parser_advance(parser);  // Consume 'extends'
        
        // Parse parent class name
        if (!parser_match(parser, TOKEN_IDENTIFIER)) {
            parser_error(parser, "Expected parent class name after 'extends'");
            parser_synchronize(parser);
            shared_free_safe(class_name, "parser", "unknown_function", 2906);
            return NULL;
        }
        parent_class = strdup(parser->previous_token->text);
    }
    
    // Parse ':'
    if (!parser_match(parser, TOKEN_COLON)) {
        parser_error(parser, "Expected ':' after class name");
        parser_synchronize(parser);
        shared_free_safe(class_name, "parser", "unknown_function", 2916);
        if (parent_class) shared_free_safe(parent_class, "parser", "unknown_function", 2917);
        return NULL;
    }
    
    // Parse class body (fields and methods)
    ASTNode* body = parser_parse_class_body(parser);
    if (!body) {
        parser_error(parser, "Expected class body");
        parser_synchronize(parser);
        shared_free_safe(class_name, "parser", "unknown_function", 2926);
        return NULL;
    }
    
    // Create class node
    ASTNode* class_node = ast_create_class(class_name, parent_class, body, 0, 0);
    shared_free_safe(class_name, "parser", "unknown_function", 2932);
    if (parent_class) shared_free_safe(parent_class, "parser", "unknown_function", 2933);
    
    return class_node;
}

/**
 * @brief Parse a class body containing fields and methods
 * 
 * Class bodies can contain:
 * - Field declarations: let name: String or let name = "default"
 * - Method declarations: func method_name(): ... end
 * 
 * @param parser The parser to use
 * @return AST node representing the class body
 */
ASTNode* parser_parse_class_body(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    ASTNode* head = NULL;
    ASTNode* tail = NULL;
    
    // Parse until we hit 'end'
    while (parser->current_token && parser->current_token->type != TOKEN_EOF) {
        // Skip semicolons
        if (parser->current_token->type == TOKEN_SEMICOLON) {
            parser_advance(parser);
            continue;
        }
        
        // Check for 'end' keyword
        if (parser_check(parser, TOKEN_KEYWORD) && 
            parser->current_token->text && 
            strcmp(parser->current_token->text, "end") == 0) {
            parser_advance(parser);  // Consume 'end'
            break;
        }
        
        ASTNode* stmt = NULL;
        
        // Check what type of statement we're dealing with
        if (parser_check(parser, TOKEN_KEYWORD)) {
            Token* token = parser_peek(parser);
            
            if (token && token->text) {
                if (strcmp(token->text, "let") == 0) {
                    // Parse field declaration
                    parser_advance(parser);  // Consume 'let'
                    stmt = parser_parse_class_field(parser);
                } else if (strcmp(token->text, "func") == 0) {
                    // Parse method declaration
                    parser_advance(parser);  // Consume 'func'
                    stmt = parser_parse_function_declaration(parser);
                }
            }
        }
        
        if (stmt) {
            if (!head) { 
                head = tail = stmt; 
            } else { 
                tail->next = stmt; 
                tail = stmt; 
            }
        } else {
            parser_synchronize(parser);
        }
    }
    
    // Convert linked list to block node
    if (!head) return ast_create_block(NULL, 0, 0, 0);
    int count = 0; 
    for (ASTNode* n = head; n; n = n->next) count++;
    
    ASTNode** statements = shared_malloc_safe(count * sizeof(ASTNode*), "parser", "unknown_function", 3008);
    if (!statements) return NULL;
    
    int i = 0;
    for (ASTNode* n = head; n; n = n->next) {
        statements[i++] = n;
    }
    
    return ast_create_block(statements, count, 0, 0);
}

/**
 * @brief Parse a class field declaration
 * 
 * Class fields can be:
 * - let name: String (type only)
 * - let name = "default" (with default value)
 * 
 * @param parser The parser to use
 * @return AST node representing the field declaration
 */
ASTNode* parser_parse_class_field(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // Parse field name
    if (!parser_match(parser, TOKEN_IDENTIFIER)) {
        parser_error(parser, "Expected field name after 'let'");
        parser_synchronize(parser);
        return NULL;
    }
    
    char* field_name = strdup(parser->previous_token->text);
    
    // Check if this is a type declaration or assignment
    if (parser_check(parser, TOKEN_COLON)) {
        // Type declaration: let name: String
        parser_advance(parser);  // Consume ':'
        
        if (!parser_match(parser, TOKEN_IDENTIFIER)) {
            parser_error(parser, "Expected type name after ':'");
            parser_synchronize(parser);
            shared_free_safe(field_name, "parser", "unknown_function", 3051);
            return NULL;
        }
        
        char* type_name = strdup(parser->previous_token->text);
        
        // Create a variable declaration with type but no initial value
        int line = parser->previous_token ? parser->previous_token->line : 0;
        int column = parser->previous_token ? parser->previous_token->column : 0;
        ASTNode* field = ast_create_variable_declaration(field_name, type_name, NULL, 0, line, column);
        shared_free_safe(field_name, "parser", "unknown_function", 3061);
        shared_free_safe(type_name, "parser", "unknown_function", 3062);
        return field;
        
    } else if (parser_check(parser, TOKEN_ASSIGN)) {
        // Assignment: let name = value
        parser_advance(parser);  // Consume '='
        
        ASTNode* initial_value = parser_parse_expression(parser);
        if (!initial_value) {
            parser_error(parser, "Expected initial value after '='");
            parser_synchronize(parser);
            shared_free_safe(field_name, "parser", "unknown_function", 3073);
            return NULL;
        }
        
        // Create a variable declaration with initial value
        int line = parser->previous_token ? parser->previous_token->line : 0;
        int column = parser->previous_token ? parser->previous_token->column : 0;
        ASTNode* field = ast_create_variable_declaration(field_name, NULL, initial_value, 0, line, column);
        shared_free_safe(field_name, "parser", "unknown_function", 3081);
        return field;
        
    } else {
        parser_error(parser, "Expected ':' for type or '=' for assignment");
        parser_synchronize(parser);
        shared_free_safe(field_name, "parser", "unknown_function", 3087);
        return NULL;
    }
}

/**
 * @brief Parse an interface declaration
 * 
 * Interface declarations define contracts for classes:
 * interface Name: method1(): ReturnType, method2(): ReturnType
 * 
 * @param parser The parser to use
 * @return AST node representing the interface declaration
 */
ASTNode* parser_parse_interface_declaration(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // TODO: Implement interface declaration parsing logic
    return NULL;  // Not implemented yet
}

/**
 * @brief Parse a module declaration
 * 
 * Module declarations group related functionality:
 * module Math: export function sqrt(x: Float): Float
 * 
 * @param parser The parser to use
 * @return AST node representing the module declaration
 */
ASTNode* parser_parse_module_declaration(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // TODO: Implement module declaration parsing logic
    return NULL;  // Not implemented yet
}

/**
 * @brief Parse a package declaration
 * 
 * Package declarations define project metadata and dependencies:
 * package myproject: dependencies: "stdlib": "1.0.0"
 * 
 * @param parser The parser to use
 * @return AST node representing the package declaration
 */
ASTNode* parser_parse_package_declaration(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // TODO: Implement package declaration parsing logic
    return NULL;  // Not implemented yet
}

/**
 * @brief Parse an import statement
 * 
 * Import statements bring external functionality into scope:
 * import stdlib, import {map, filter} from stdlib
 * 
 * @param parser The parser to use
 * @return AST node representing the import statement
 */
ASTNode* parser_parse_import_statement(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // TODO: Implement import statement parsing logic
    return NULL;  // Not implemented yet
}

/**
 * @brief Parse a use statement
 * 
 * Use statements import functionality from libraries or files:
 * use math, use math as m, use Pi, E from math, use Pi, E from math as pi, e, use "./utils.myco" as utils
 * 
 * @param parser The parser to use
 * @return AST node representing the use statement
 */
ASTNode* parser_parse_use_statement(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // The 'use' keyword has already been consumed by parser_parse_statement
    // so we start parsing the content that comes after 'use'
    
    // Check if this is a specific import: use item1, item2 from library
    char** specific_items = NULL;
    char** specific_aliases = NULL;
    size_t item_count = 0;
    
    // Check if we have specific items (identifiers separated by commas)
    // We need to peek ahead to see if this is "item1, item2 from library" or just "library"
    if (parser_check(parser, TOKEN_IDENTIFIER)) {
        // Store the current position to backtrack if needed
        int current_pos = parser->current_position;
        Token* current_token = parser->current_token;
        
        // Try to parse as specific items
        int has_specific_items = 0;
        while (parser_check(parser, TOKEN_IDENTIFIER)) {
            parser_advance(parser); // consume identifier
            
            if (parser_check(parser, TOKEN_COMMA)) {
                parser_advance(parser); // consume comma
                has_specific_items = 1;
                continue;
            }
            break;
        }
        
        // Check if the next token is 'from'
        if (has_specific_items && parser_check(parser, TOKEN_KEYWORD) && 
            parser->current_token->text && strcmp(parser->current_token->text, "from") == 0) {
            // This is indeed a specific import, go back and parse it properly
            parser->current_position = current_pos;
            parser->current_token = current_token;
            
            // Parse comma-separated list of items
            while (1) {
                if (!parser_check(parser, TOKEN_IDENTIFIER)) {
                    parser_error(parser, "Expected identifier in use statement");
                    return NULL;
                }
                
                // Add item to list
                char** new_items = shared_realloc_safe(specific_items, (item_count + 1) * sizeof(char*), "parser", "unknown_function", 3221);
                if (!new_items) {
                    parser_error(parser, "Out of memory while parsing use statement");
                    return NULL;
                }
                specific_items = new_items;
                specific_items[item_count++] = strdup(parser->current_token->text);
                
                parser_advance(parser); // consume identifier
                
                if (parser_check(parser, TOKEN_COMMA)) {
                    parser_advance(parser); // consume comma
                    continue;
                }
                break;
            }
            
            // Expect 'from' keyword
            if (!parser_match(parser, TOKEN_KEYWORD) || 
                !parser->previous_token->text || 
                strcmp(parser->previous_token->text, "from") != 0) {
                parser_error(parser, "Expected 'from' keyword in use statement");
                return NULL;
            }
        } else {
            // This is not a specific import, go back to the beginning
            parser->current_position = current_pos;
            parser->current_token = current_token;
        }
    }
    
    // Parse library name (identifier or string)
    char* library_name = NULL;
    if (parser_check(parser, TOKEN_IDENTIFIER)) {
        library_name = strdup(parser->current_token->text);
        parser_advance(parser);
    } else if (parser_check(parser, TOKEN_STRING)) {
        library_name = strdup(parser->current_token->data.string_value);
        parser_advance(parser);
    } else {
        parser_error(parser, "Expected library name or file path in use statement");
        return NULL;
    }
    
    // Check for 'as' alias
    char* alias = NULL;
    if (parser_check(parser, TOKEN_KEYWORD) && parser->current_token->text && 
        strcmp(parser->current_token->text, "as") == 0) {
        parser_advance(parser); // consume 'as'
        
        if (specific_items && item_count > 0) {
            // This is a specific import with aliases: use Pi, E from math as pi, e
            specific_aliases = shared_malloc_safe(item_count * sizeof(char*), "parser", "unknown_function", 3273);
            if (!specific_aliases) {
                parser_error(parser, "Out of memory while parsing use statement aliases");
                shared_free_safe(library_name, "parser", "unknown_function", 3276);
                return NULL;
            }
            
            // Parse comma-separated list of aliases
            for (size_t i = 0; i < item_count; i++) {
                if (!parser_match(parser, TOKEN_IDENTIFIER)) {
                    parser_error(parser, "Expected alias name after 'as'");
                    // Clean up
                    for (size_t j = 0; j < i; j++) {
                        shared_free_safe(specific_aliases[j], "parser", "unknown_function", 3286);
                    }
                    shared_free_safe(specific_aliases, "parser", "unknown_function", 3288);
                    shared_free_safe(library_name, "parser", "unknown_function", 3289);
                    return NULL;
                }
                specific_aliases[i] = strdup(parser->previous_token->text);
                
                if (i < item_count - 1) {
                    if (!parser_check(parser, TOKEN_COMMA)) {
                        parser_error(parser, "Expected comma between aliases");
                        // Clean up
                        for (size_t j = 0; j <= i; j++) {
                            shared_free_safe(specific_aliases[j], "parser", "unknown_function", 3299);
                        }
                        shared_free_safe(specific_aliases, "parser", "unknown_function", 3301);
                        shared_free_safe(library_name, "parser", "unknown_function", 3302);
                        return NULL;
                    }
                    parser_advance(parser); // consume comma
                }
            }
        } else {
            // This is a general import with alias: use math as m
            if (!parser_match(parser, TOKEN_IDENTIFIER)) {
                parser_error(parser, "Expected alias name after 'as'");
                shared_free_safe(library_name, "parser", "unknown_function", 3312);
                return NULL;
            }
            alias = strdup(parser->previous_token->text);
        }
    }
    
    // Create the use statement node
    ASTNode* node = ast_create_use(library_name, alias, specific_items, specific_aliases, item_count, 
                                  parser->previous_token->line, parser->previous_token->column);
    
    // Clean up temporary storage
    shared_free_safe(library_name, "parser", "unknown_function", 3324);
    if (specific_items) {
        for (size_t i = 0; i < item_count; i++) {
            shared_free_safe(specific_items[i], "parser", "unknown_function", 3327);
        }
        shared_free_safe(specific_items, "parser", "unknown_function", 3329);
    }
    if (specific_aliases) {
        for (size_t i = 0; i < item_count; i++) {
            if (specific_aliases[i]) {
                shared_free_safe(specific_aliases[i], "parser", "unknown_function", 3334);
            }
        }
        shared_free_safe(specific_aliases, "parser", "unknown_function", 3337);
    }
    
    return node;
}

/**
 * @brief Parse an export statement
 * 
 * Export statements make functionality available to other modules:
 * export function public_function(): Int
 * 
 * @param parser The parser to use
 * @return AST node representing the export statement
 */
ASTNode* parser_parse_export_statement(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // TODO: Implement export statement parsing logic
    return NULL;  // Not implemented yet
}

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
int parser_get_precedence(TokenType type) {
    // TODO: Implement precedence logic for different token types
    (void)type;  // Suppress unused parameter warning
    return 0;    // Default precedence
}

/**
 * @brief Check if a token type is right-associative
 * 
 * Right-associative operators group from right to left.
 * For example, a ** b ** c is parsed as a ** (b ** c).
 * 
 * @param type The token type to check
 * @return 1 if right-associative, 0 if left-associative
 */
int parser_is_right_associative(TokenType type) {
    // TODO: Implement associativity logic for different token types
    (void)type;  // Suppress unused parameter warning
    return 0;    // Default to left-associative
}

/**
 * @brief Check if a token type is a binary operator
 * 
 * Binary operators take two operands (left and right).
 * Examples: +, -, *, /, ==, !=, <, >, and, or
 * 
 * @param type The token type to check
 * @return 1 if binary operator, 0 otherwise
 */
int parser_is_binary_operator(TokenType type) {
    // TODO: Implement binary operator detection logic
    (void)type;  // Suppress unused parameter warning
    return 0;    // Default to not a binary operator
}

/**
 * @brief Check if a token type is a unary operator
 * 
 * Unary operators take one operand.
 * Examples: -, not, ~ (bitwise NOT)
 * 
 * @param type The token type to check
 * @return 1 if unary operator, 0 otherwise
 */
int parser_is_unary_operator(TokenType type) {
    // TODO: Implement unary operator detection logic
    (void)type;  // Suppress unused parameter warning
    return 0;    // Default to not a unary operator
}

/**
 * @brief Check if a token type is an assignment operator
 * 
 * Assignment operators assign values to variables.
 * Examples: =, +=, -=, *=, /=
 * 
 * @param type The token type to check
 * @return 1 if assignment operator, 0 otherwise
 */
int parser_is_assignment_operator(TokenType type) {
    // TODO: Implement assignment operator detection logic
    (void)type;  // Suppress unused parameter warning
    return 0;    // Default to not an assignment operator
}

/**
 * @brief Check if a token type is a comparison operator
 * 
 * Comparison operators compare two values.
 * Examples: ==, !=, <, >, <=, >=
 * 
 * @param type The token type to check
 * @return 1 if comparison operator, 0 otherwise
 */
int parser_is_comparison_operator(TokenType type) {
    // TODO: Implement comparison operator detection logic
    (void)type;  // Suppress unused parameter warning
    return 0;    // Default to not a comparison operator
}

/**
 * @brief Check if a token type is a logical operator
 * 
 * Logical operators perform boolean logic.
 * Examples: and, or, not
 * 
 * @param type The token type to check
 * @return 1 if logical operator, 0 otherwise
 */
int parser_is_logical_operator(TokenType type) {
    // TODO: Implement logical operator detection logic
    (void)type;  // Suppress unused parameter warning
    return 0;    // Default to not a logical operator
}

/**
 * @brief Check if a token type is an arithmetic operator
 * 
 * Arithmetic operators perform mathematical operations.
 * Examples: +, -, *, /, %, **
 * 
 * @param type The token type to check
 * @return 1 if arithmetic operator, 0 otherwise
 */
int parser_is_arithmetic_operator(TokenType type) {
    // TODO: Implement arithmetic operator detection logic
    (void)type;  // Suppress unused parameter warning
    return 0;    // Default to not an arithmetic operator
}

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
void parser_synchronize(Parser* parser) {
    if (!parser) {
        return;
    }

    // Advance at least once to avoid infinite loops when stuck on a bad token
    parser_advance(parser);

    // Skip tokens until we reach a likely statement boundary or a plausible start
    while (parser->current_token && parser->current_token->type != TOKEN_EOF) {
        Token* token = parser->current_token;

        // Stop after consuming a statement terminator; move past it
        if (token->type == TOKEN_SEMICOLON) {
            parser_advance(parser);
            break;
        }

        // Stop at closing delimiters to let caller handle resync at higher level
        if (token->type == TOKEN_RIGHT_BRACE || token->type == TOKEN_RIGHT_PAREN || token->type == TOKEN_RIGHT_BRACKET) {
            parser_advance(parser);
            break;
        }

        // If we encounter a keyword that can begin a statement, stop here
        if (token->type == TOKEN_KEYWORD && token->text) {
            if (strcmp(token->text, "let") == 0 ||
                strcmp(token->text, "if") == 0 ||
                strcmp(token->text, "while") == 0 ||
                strcmp(token->text, "for") == 0 ||
                strcmp(token->text, "function") == 0 ||
                strcmp(token->text, "return") == 0 ||
                strcmp(token->text, "else") == 0 ||
                strcmp(token->text, "end") == 0) {
                break;
            }
        }

        // Otherwise keep skipping
        parser_advance(parser);
    }
}

/**
 * @brief Synchronize to a statement boundary
 * 
 * This function skips tokens until it finds the end of the current
 * statement, allowing parsing to continue with the next statement.
 * 
 * @param parser The parser to synchronize
 * @return 1 if synchronization was successful, 0 otherwise
 */
int parser_sync_to_statement_boundary(Parser* parser) {
    if (!parser) {
        return 0;
    }
    
    // TODO: Implement statement boundary synchronization logic
    // Skip until semicolon, 'end' or EOF
    while (parser->current_token && parser->current_token->type != TOKEN_EOF) {
        if (parser->current_token->type == TOKEN_SEMICOLON) { parser_advance(parser); break; }
        if (parser->current_token->type == TOKEN_KEYWORD && parser->current_token->text && strcmp(parser->current_token->text, "end") == 0) {
            break;
        }
        parser_advance(parser);
    }
        return 1;
    }

/**
 * @brief Synchronize to an expression boundary
 * 
 * This function skips tokens until it finds the end of the current
 * expression, allowing parsing to continue with the next expression.
 * 
 * @param parser The parser to synchronize
 * @return 1 if synchronization was successful, 0 otherwise
 */
int parser_sync_to_expression_boundary(Parser* parser) {
    if (!parser) {
    return 0;
}

    // Skip until semicolon or right delimiter or EOF
    while (parser->current_token && parser->current_token->type != TOKEN_EOF) {
        TokenType t = parser->current_token->type;
        if (t == TOKEN_SEMICOLON || t == TOKEN_RIGHT_PAREN || t == TOKEN_RIGHT_BRACE || t == TOKEN_RIGHT_BRACKET) {
            break;
        }
        parser_advance(parser);
    }
    return 1;
}

// Collect statements until 'end' or optional 'else'
static ASTNode* parser_collect_block(Parser* parser, int stop_on_else, int* saw_else) {
    if (saw_else) *saw_else = 0;
    ASTNode* head = NULL; ASTNode* tail = NULL;
    int consecutive_failures = 0;
    const int MAX_CONSECUTIVE_FAILURES = 10;  // Prevent infinite loops
    
    while (parser->current_token && parser->current_token->type != TOKEN_EOF) {
        if (parser->current_token->type == TOKEN_KEYWORD && parser->current_token->text) {
            if (strcmp(parser->current_token->text, "end") == 0) {
                break;
            }
            if (stop_on_else && strcmp(parser->current_token->text, "else") == 0) {
                if (saw_else) *saw_else = 1;
                break;
            }
        }
        // Skip empty statements
        if (parser->current_token->type == TOKEN_SEMICOLON) {
            parser_advance(parser);
            continue;
        }
        
        // Store current position before parsing
        int start_position = parser->current_position;
        ASTNode* stmt = parser_parse_statement(parser);
        if (stmt) {
            if (!head) { head = tail = stmt; }
            else { tail->next = stmt; tail = stmt; }
            consecutive_failures = 0;  // Reset failure counter on success
        } else {
            // Check if parser position actually advanced
            if (parser->current_position == start_position) {
                consecutive_failures++;
                if (consecutive_failures >= MAX_CONSECUTIVE_FAILURES) {
                    // Force advance to prevent infinite loop
                    parser_advance(parser);
                    consecutive_failures = 0;
                }
            } else {
                consecutive_failures = 0;  // Reset if position advanced
            }
            parser_synchronize(parser);
        }
    }
    // Convert linked list to block node
    if (!head) return ast_create_block(NULL, 0, 0, 0);
    int count = 0; for (ASTNode* n = head; n; n = n->next) count++;
    ASTNode** arr = (ASTNode**)shared_malloc_safe(sizeof(ASTNode*) * (size_t)count, "parser", "unknown_function", 3633);
    ASTNode* cur = head; for (int i = 0; i < count; i++) { arr[i] = cur; cur = cur->next; }
    ASTNode* block = ast_create_block(arr, (size_t)count, 0, 0);
    if (!block) shared_free_safe(arr, "parser", "unknown_function", 3636);
    return block;
}

/**
 * @brief Parse a spore statement (pattern matching)
 * 
 * Spore statements support both lambda and block syntax:
 * - Lambda: case pattern => expression
 * - Block: case pattern: statements end
 * 
 * @param parser The parser to use
 * @return AST node representing the spore statement
 */
ASTNode* parser_parse_spore_statement(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // The 'spore' keyword has already been consumed by the statement parser
    
    // Parse the expression to match against
    ASTNode* expression = parser_parse_expression(parser);
    if (!expression) {
        parser_error(parser, "Expected expression after \"spore\"");
    return NULL;
}

    // Expect colon
    if (!parser_match(parser, TOKEN_COLON)) {
        parser_error(parser, "Expected \":\" after spore expression");
        ast_free(expression);
        return NULL;
    }
    
    // Parse cases until "end"
    ASTNode** cases = NULL;
    size_t case_count = 0;
    size_t case_capacity = 0;
    
    while (parser->current_token && parser->current_token->type != TOKEN_EOF) {
        if (parser->current_token->type == TOKEN_KEYWORD && parser->current_token->text) {
            if (strcmp(parser->current_token->text, "end") == 0) {
                break;
            }
        }
        

        
        // Parse case or root
        if (parser->current_token->type == TOKEN_KEYWORD && parser->current_token->text) {
            if (strcmp(parser->current_token->text, "case") == 0) {
                // Parse case pattern
                parser_advance(parser); // consume "case"
                
                ASTNode* pattern = parser_parse_expression(parser);
                if (!pattern) {
                    parser_error(parser, "Expected pattern after \"case\"");
                    parser_synchronize(parser);
                    continue;
                }
                
                // Check for lambda style (=>) or block style (:)
                if (parser_check(parser, TOKEN_ARROW)) {
                    // Lambda style: case pattern => expression
                    parser_advance(parser); // consume "=>"
                    
                    ASTNode* result = parser_parse_expression(parser);
                    if (!result) {
                        parser_error(parser, "Expected expression after '=>' in lambda case");
                        ast_free(pattern);
                        parser_synchronize(parser);
                        continue;
                    }
                    
                    // Create lambda case node
                    ASTNode* case_node = ast_create_spore_case(pattern, result, 1, 0, 0);
                    if (case_node) {
                        // Expand array if needed
                        if (case_count >= case_capacity) {
                            size_t new_capacity = case_capacity == 0 ? 4 : case_capacity * 2;
                            ASTNode** new_cases = shared_realloc_safe(cases, new_capacity * sizeof(ASTNode*), "parser", "unknown_function", 3767);
                            if (new_cases) {
                                cases = new_cases;
                                case_capacity = new_capacity;
                            }
                        }
                        cases[case_count++] = case_node;
                    }
                    
                } else if (parser_match(parser, TOKEN_COLON)) {
                    // Block style: case pattern: statements until next case/root/end
                    
                    // Manually collect case body statements until next case/root/end
                    ASTNode** case_statements = NULL;
                    size_t case_stmt_count = 0;
                    size_t case_stmt_capacity = 0;
                    
                    while (parser->current_token && parser->current_token->type != TOKEN_EOF) {
                        // Check for keywords that end this case
                        if (parser->current_token->type == TOKEN_KEYWORD && parser->current_token->text) {
                            if (strcmp(parser->current_token->text, "case") == 0 ||
                                strcmp(parser->current_token->text, "root") == 0 ||
                                strcmp(parser->current_token->text, "end") == 0) {
                                break;
                            }
                        }
                        
                        // Skip empty statements
                        if (parser->current_token->type == TOKEN_SEMICOLON) {
                            parser_advance(parser);
                            continue;
                        }
                        
                        ASTNode* stmt = parser_parse_statement(parser);
                        if (stmt) {
                            // Expand array if needed
                            if (case_stmt_count >= case_stmt_capacity) {
                                size_t new_capacity = case_stmt_capacity == 0 ? 4 : case_stmt_capacity * 2;
                                ASTNode** new_statements = shared_realloc_safe(case_statements, new_capacity * sizeof(ASTNode*), "parser", "unknown_function", 3805);
                                if (new_statements) {
                                    case_statements = new_statements;
                                    case_stmt_capacity = new_capacity;
                                }
                            }
                            case_statements[case_stmt_count++] = stmt;
                        } else {
                            parser_synchronize(parser);
                        }
                    }
                    
                    ASTNode* body = ast_create_block(case_statements, case_stmt_count, 0, 0);
                    if (!body && case_statements) {
                        shared_free_safe(case_statements, "parser", "unknown_function", 3819);
                    }
                    if (!body) {
                        parser_error(parser, "Expected case body");
                        ast_free(pattern);
                        parser_synchronize(parser);
                        continue;
                    }
                    
                    // Create block case node
                    ASTNode* case_node = ast_create_spore_case(pattern, body, 0, 0, 0);
                    if (case_node) {
                        // Expand array if needed
                        if (case_count >= case_capacity) {
                            size_t new_capacity = case_capacity == 0 ? 4 : case_capacity * 2;
                            ASTNode** new_cases = shared_realloc_safe(cases, new_capacity * sizeof(ASTNode*), "parser", "unknown_function", 3834);
                            if (new_cases) {
                                cases = new_cases;
                                case_capacity = new_capacity;
                            }
                        }
                        cases[case_count++] = case_node;
                    }
                    
                } else {
                    parser_error(parser, "Expected \"=>\" or \":\" after case pattern");
                    ast_free(pattern);
                    parser_synchronize(parser);
                    continue;
                }
                
            } else if (strcmp(parser->current_token->text, "root") == 0) {
                // Parse root (default) case
                parser_advance(parser); // consume "root"
                
                // Check for lambda style (=>) or block style (:)
                if (parser_check(parser, TOKEN_ARROW)) {
                    // Lambda style: root => expression
                    parser_advance(parser); // consume "=>"
                    
                    ASTNode* result = parser_parse_expression(parser);
                    if (!result) {
                        parser_error(parser, "Expected expression after \"=>\"");
                        parser_synchronize(parser);
                        continue;
                    }
                    
                    // Create lambda root node
                    ASTNode* root_node = ast_create_spore_case(NULL, result, 1, 0, 0);
                    if (root_node) {
                        // Expand array if needed
                        if (case_count >= case_capacity) {
                            size_t new_capacity = case_capacity == 0 ? 4 : case_capacity * 2;
                            ASTNode** new_cases = shared_realloc_safe(cases, new_capacity * sizeof(ASTNode*), "parser", "unknown_function", 3872);
                            if (new_cases) {
                                cases = new_cases;
                                case_capacity = new_capacity;
                            }
                        }
                        cases[case_count++] = root_node;
                    }
                    
                } else if (parser_match(parser, TOKEN_COLON)) {
                    // Block style: root: statements until end
                    
                    // Manually collect root body statements until end
                    ASTNode** root_statements = NULL;
                    size_t root_stmt_count = 0;
                    size_t root_stmt_capacity = 0;
                    
                    while (parser->current_token && parser->current_token->type != TOKEN_EOF) {
                        // Check for 'end' keyword to end root case
                        if (parser->current_token->type == TOKEN_KEYWORD && parser->current_token->text &&
                            strcmp(parser->current_token->text, "end") == 0) {
                            break;
                        }
                        
                        // Skip empty statements
                        if (parser->current_token->type == TOKEN_SEMICOLON) {
                            parser_advance(parser);
                            continue;
                        }
                        
                        ASTNode* stmt = parser_parse_statement(parser);
                        if (stmt) {
                            // Expand array if needed
                            if (root_stmt_count >= root_stmt_capacity) {
                                size_t new_capacity = root_stmt_capacity == 0 ? 4 : root_stmt_capacity * 2;
                                ASTNode** new_statements = shared_realloc_safe(root_statements, new_capacity * sizeof(ASTNode*), "parser", "unknown_function", 3907);
                                if (new_statements) {
                                    root_statements = new_statements;
                                    root_stmt_capacity = new_capacity;
                                }
                            }
                            root_statements[root_stmt_count++] = stmt;
                        } else {
                            parser_synchronize(parser);
                        }
                    }
                    
                    ASTNode* body = ast_create_block(root_statements, root_stmt_count, 0, 0);
                    if (!body && root_statements) {
                        shared_free_safe(root_statements, "parser", "unknown_function", 3921);
                    }
                    if (!body) {
                        parser_error(parser, "Expected root body");
                        parser_synchronize(parser);
                        continue;
                    }
                    // Create block root node
                    ASTNode* root_node = ast_create_spore_case(NULL, body, 0, 0, 0);
                    if (root_node) {
                        // Expand array if needed
                        if (case_count >= case_capacity) {
                            size_t new_capacity = case_capacity == 0 ? 4 : case_capacity * 2;
                            ASTNode** new_cases = shared_realloc_safe(cases, new_capacity * sizeof(ASTNode*), "parser", "unknown_function", 3934);
                            if (new_cases) {
                                cases = new_cases;
                                case_capacity = new_capacity;
                            }
                        }
                        cases[case_count++] = root_node;
                    }
                } else {
                    parser_error(parser, "Expected \"=>\" or \":\" after \"root\"");
                    parser_synchronize(parser);
                    continue;
                }
                
            } else {
                // Unknown keyword, skip
                parser_advance(parser);
            }
        } else {
            // Skip non-keyword tokens
            parser_advance(parser);
        }
    }
    
    // Expect "end"
    if (!parser_match(parser, TOKEN_KEYWORD) || 
        !parser->previous_token || !parser->previous_token->text || 
        strcmp(parser->previous_token->text, "end") != 0) {
        parser_error(parser, "Expected \"end\" to close spore statement");
        ast_free(expression);
        // Clean up cases
        for (size_t i = 0; i < case_count; i++) {
            ast_free(cases[i]);
        }
        shared_free_safe(cases, "parser", "unknown_function", 3968);
        return NULL;
    }
    
    // Create proper spore node
    ASTNode* spore_node = ast_create_spore(expression, cases, case_count, NULL, 0, 0);
    if (!spore_node && cases) {
        shared_free_safe(cases, "parser", "unknown_function", 3975);
    }
    
    return spore_node;
}

/**
 * @brief Get a human-readable name for a token type
 * 
 * This function converts token type enums to readable strings for error messages.
 * 
 * @param type The token type to convert
 * @return Human-readable string representation of the token type
 */
static const char* get_token_type_name(TokenType type) {
    switch (type) {
        case TOKEN_EOF: return "End of File";
        case TOKEN_ERROR: return "Error";
        case TOKEN_NUMBER: return "Number";
        case TOKEN_STRING: return "String";
        case TOKEN_BOOL: return "Boolean";
        case TOKEN_IDENTIFIER: return "Identifier";
        case TOKEN_KEYWORD: return "Keyword";
        case TOKEN_PLUS: return "Plus (+)";
        case TOKEN_MINUS: return "Minus (-)";
        case TOKEN_MULTIPLY: return "Multiply (*)";
        case TOKEN_DIVIDE: return "Divide (/)";
        case TOKEN_MODULO: return "Modulo (%)";
        case TOKEN_POWER: return "Power (^)";
        case TOKEN_ASSIGN: return "Assignment (=)";
        case TOKEN_EQUAL: return "Equality (==)";
        case TOKEN_NOT_EQUAL: return "Inequality (!=)";
        case TOKEN_LESS: return "Less Than (<)";
        case TOKEN_GREATER: return "Greater Than (>)";
        case TOKEN_LESS_EQUAL: return "Less Than or Equal (<=)";
        case TOKEN_GREATER_EQUAL: return "Greater Than or Equal (>=)";
        case TOKEN_LEFT_PAREN: return "Left Parenthesis ((";
        case TOKEN_RIGHT_PAREN: return "Right Parenthesis )";
        case TOKEN_LEFT_BRACE: return "Left Brace {";
        case TOKEN_RIGHT_BRACE: return "Right Brace }";
        case TOKEN_LEFT_BRACKET: return "Left Bracket [";
        case TOKEN_RIGHT_BRACKET: return "Right Bracket ]";
        case TOKEN_SEMICOLON: return "Semicolon (;)";
        case TOKEN_COLON: return "Colon (:)";
        case TOKEN_COMMA: return "Comma (,)";
        case TOKEN_DOT: return "Dot (.)";
        case TOKEN_DOT_DOT: return "Range (..)";
        case TOKEN_ARROW: return "Arrow (=>)";
        case TOKEN_AND: return "Logical AND (and)";
        case TOKEN_OR: return "Logical OR (or)";
        case TOKEN_NOT: return "Logical NOT (not)";
        case TOKEN_PLUS_ASSIGN: return "Add and Assign (+=)";
        case TOKEN_MINUS_ASSIGN: return "Subtract and Assign (-=)";
        case TOKEN_MULTIPLY_ASSIGN: return "Multiply and Assign (*=)";
        case TOKEN_DIVIDE_ASSIGN: return "Divide and Assign (/=)";
        case TOKEN_COMMENT: return "Comment";
        default: return "Unknown Token";
    }
}

/**
 * @brief Get helpful suggestions for common parsing errors
 * 
 * This function provides specific, actionable advice for common syntax errors
 * to help developers fix their code quickly.
 * 
 * @param message The error message
 * @param token The token that caused the error
 * @return Helpful suggestion string
 */
/**
 * @brief Parse an array literal
 * 
 * Array literals have the form: [element1, element2, ...]
 * 
 * @param parser The parser to use
 * @return AST node representing the array literal
 */
static ASTNode* parser_parse_array_literal(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // Consume the opening bracket
    parser_advance(parser);
    
    // Parse array elements
    ASTNode** elements = NULL;
    size_t element_count = 0;
    size_t element_capacity = 0;
    
    // Check if array is empty []
    if (parser_check(parser, TOKEN_RIGHT_BRACKET)) {
        parser_advance(parser);  // Consume the closing bracket
        ASTNode* array_literal = ast_create_array_literal(NULL, 0, parser->previous_token->line, parser->previous_token->column);
        if (array_literal) {
            // Check for member access: [].method
            if (parser_check(parser, TOKEN_DOT)) {
                parser_advance(parser); // consume '.'
                
                // Parse the member name (can be identifier or keyword)
                if (!parser_check(parser, TOKEN_IDENTIFIER) && !parser_check(parser, TOKEN_KEYWORD)) {
                    parser_error(parser, "Expected member name after '.'");
                    ast_free(array_literal);
                    return NULL;
                }
                
                Token* member_token = parser_peek(parser);
                parser_advance(parser); // consume member name
                
                // Create member access node
                ASTNode* member_access = ast_create_member_access(array_literal, member_token->text, parser->previous_token->line, parser->previous_token->column);
                if (!member_access) {
                    ast_free(array_literal);
                    return NULL;
                }
                
                // Check for function calls on the member access: [].method(args)
                if (parser_check(parser, TOKEN_LEFT_PAREN)) {
                    parser_advance(parser); // consume '('
                    
                    ASTNode** args = NULL;
                    size_t arg_count = 0;
                    size_t arg_capacity = 0;
                    
                    // Parse optional arguments
                    if (!parser_check(parser, TOKEN_RIGHT_PAREN)) {
                        while (1) {
                            ASTNode* arg = parser_parse_expression(parser);
                            if (!arg) {
                                parser_error(parser, "Function call arguments must be valid expressions");
                                break;
                            }
                            if (arg_count == arg_capacity) {
                                size_t new_cap = arg_capacity == 0 ? 4 : arg_capacity * 2;
                                ASTNode** new_args = (ASTNode**)shared_realloc_safe(args, new_cap * sizeof(ASTNode*), "parser", "unknown_function", 4110);
                                if (!new_args) {
                                    parser_error(parser, "Out of memory while parsing arguments");
                                    break;
                                }
                                args = new_args;
                                arg_capacity = new_cap;
                            }
                            args[arg_count++] = arg;
                            if (parser_check(parser, TOKEN_COMMA)) {
                                parser_advance(parser);
                                continue;
                            }
                            break;
                        }
                    }
                    
                    if (!parser_match(parser, TOKEN_RIGHT_PAREN)) {
                        parser_error(parser, "Expected ')' after arguments");
                    }
                    
                    // Create function call expression node
                    ASTNode* call = ast_create_function_call_expr(member_access, args, arg_count, parser->previous_token->line, parser->previous_token->column);
                    if (call) {
                        // Check for additional method chaining
                        return parser_parse_member_access_chain(parser, call);
                    } else {
                        ast_free(member_access);
                        return NULL;
                    }
                } else {
                    // Just member access, no function call
                    return member_access;
                }
            } else {
                // No member access, just return the array literal
                return array_literal;
            }
        }
        return array_literal;
    }
    
    // Parse first element
    ASTNode* first_element = parser_parse_expression(parser);
    if (!first_element) {
        parser_error(parser, "Expected expression in array literal");
        return NULL;
    }
    
    // Add first element
    elements = shared_malloc_safe(sizeof(ASTNode*), "parser", "unknown_function", 4160);
    if (!elements) {
        parser_error(parser, "Out of memory while parsing array literal");
        ast_free(first_element);
        return NULL;
    }
    elements[0] = first_element;
    element_count = 1;
    element_capacity = 1;
    
    // Parse remaining elements
    while (parser_check(parser, TOKEN_COMMA)) {
        parser_advance(parser);  // Consume the comma
        
        ASTNode* element = parser_parse_expression(parser);
        if (!element) {
            parser_error(parser, "Expected expression after comma in array literal");
            // Clean up and return what we have
            for (size_t i = 0; i < element_count; i++) {
                ast_free(elements[i]);
            }
            shared_free_safe(elements, "parser", "unknown_function", 4181);
            return NULL;
        }
        
        // Expand array if needed
        if (element_count >= element_capacity) {
            element_capacity *= 2;
            ASTNode** new_elements = shared_realloc_safe(elements, element_capacity * sizeof(ASTNode*), "parser", "unknown_function", 4188);
            if (!new_elements) {
                parser_error(parser, "Out of memory while parsing array literal");
                ast_free(element);
                for (size_t i = 0; i < element_count; i++) {
                    ast_free(elements[i]);
                }
                shared_free_safe(elements, "parser", "unknown_function", 4195);
                return NULL;
            }
            elements = new_elements;
        }
        
        elements[element_count++] = element;
    }
    
    // Expect closing bracket
    if (!parser_match(parser, TOKEN_RIGHT_BRACKET)) {
        parser_error(parser, "Expected ']' to close array literal");
        // Clean up and return what we have
        for (size_t i = 0; i < element_count; i++) {
                ast_free(elements[i]);
        }
        shared_free_safe(elements, "parser", "unknown_function", 4211);
        return NULL;
    }
    
    ASTNode* array_literal = ast_create_array_literal(elements, element_count, parser->previous_token->line, parser->previous_token->column);
    if (array_literal) {
        // Check for member access: [1, 2, 3].method
        if (parser_check(parser, TOKEN_DOT)) {
            parser_advance(parser); // consume '.'
            
            // Parse the member name (can be identifier or keyword)
            if (!parser_check(parser, TOKEN_IDENTIFIER) && !parser_check(parser, TOKEN_KEYWORD)) {
                parser_error(parser, "Expected member name after '.'");
                ast_free(array_literal);
                return NULL;
            }
            
            Token* member_token = parser_peek(parser);
            parser_advance(parser); // consume member name
            
            // Create member access node
            ASTNode* member_access = ast_create_member_access(array_literal, member_token->text, parser->previous_token->line, parser->previous_token->column);
            if (!member_access) {
                ast_free(array_literal);
                return NULL;
            }
            
            // Check for function calls on the member access: [1, 2, 3].method(args)
            if (parser_check(parser, TOKEN_LEFT_PAREN)) {
                parser_advance(parser); // consume '('
                
                ASTNode** args = NULL;
                size_t arg_count = 0;
                size_t arg_capacity = 0;
                
                // Parse optional arguments
                if (!parser_check(parser, TOKEN_RIGHT_PAREN)) {
                    while (1) {
                        ASTNode* arg = parser_parse_expression(parser);
                        if (!arg) {
                            parser_error(parser, "Function call arguments must be valid expressions");
                            break;
                        }
                        if (arg_count == arg_capacity) {
                            size_t new_cap = arg_capacity == 0 ? 4 : arg_capacity * 2;
                            ASTNode** new_args = (ASTNode**)shared_realloc_safe(args, new_cap * sizeof(ASTNode*), "parser", "unknown_function", 4256);
                            if (!new_args) {
                                parser_error(parser, "Out of memory while parsing arguments");
                                break;
                            }
                            args = new_args;
                            arg_capacity = new_cap;
                        }
                        args[arg_count++] = arg;
                        if (parser_check(parser, TOKEN_COMMA)) {
                            parser_advance(parser);
                            continue;
                        }
                        break;
                    }
                }
                
                if (!parser_match(parser, TOKEN_RIGHT_PAREN)) {
                    parser_error(parser, "Expected ')' after arguments");
                }
                
                // Create function call expression node
                ASTNode* call = ast_create_function_call_expr(member_access, args, arg_count, parser->previous_token->line, parser->previous_token->column);
                if (call) {
                    // Check for additional method chaining
                    return parser_parse_member_access_chain(parser, call);
                } else {
                    ast_free(member_access);
                    return NULL;
                }
            } else {
                // Just member access, no function call
                return member_access;
            }
        } else {
            // No member access, just return the array literal
            return array_literal;
        }
    }
    return array_literal;
}

/**
 * Parse hash map literal: {key1: value1, key2: value2, ...}
 * 
 * @param parser The parser to use
 * @return AST node representing the hash map literal
 */
/**
 * Parse a hash map key, converting unquoted identifiers to string literals
 * 
 * @param parser The parser to use
 * @return AST node representing the key (string literal for identifiers, expression for others)
 */
static ASTNode* parser_parse_hash_map_key(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    Token* current = parser_peek(parser);
    if (!current) {
        parser_error(parser, "Expected key in hash map literal");
        return NULL;
    }
    
    // If it's an identifier, convert it to a string literal
    if (current->type == TOKEN_IDENTIFIER) {
        parser_advance(parser); // consume the identifier
        return ast_create_string(strdup(current->text), current->line, current->column);
    }
    
    // For all other cases, parse as a regular expression
    return parser_parse_expression(parser);
}

static ASTNode* parser_parse_hash_map_literal(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // Consume the opening brace
    parser_advance(parser);
    
    // Handle empty hash map
    Token* current = parser_peek(parser);
    if (current && current->type == TOKEN_RIGHT_BRACE) {
        parser_advance(parser);
        return ast_create_hash_map_literal(NULL, NULL, 0, parser->previous_token->line, parser->previous_token->column);
    }
    
    // Parse first key-value pair
    ASTNode* first_key = parser_parse_hash_map_key(parser);
    if (!first_key) {
        parser_error(parser, "Expected key expression in hash map literal");
        return NULL;
    }
    
    // Expect colon
    if (!parser_match(parser, TOKEN_COLON)) {
        parser_error(parser, "Expected ':' after key in hash map literal");
        ast_free(first_key);
        return NULL;
    }
    
    ASTNode* first_value = parser_parse_expression(parser);
    if (!first_value) {
        parser_error(parser, "Expected value expression after ':' in hash map literal");
        ast_free(first_key);
        return NULL;
    }
    
    // Allocate arrays for keys and values
    ASTNode** keys = shared_malloc_safe(sizeof(ASTNode*), "parser", "unknown_function", 4368);
    ASTNode** values = shared_malloc_safe(sizeof(ASTNode*), "parser", "unknown_function", 4369);
    if (!keys || !values) {
        parser_error(parser, "Memory allocation failed for hash map literal");
        ast_free(first_key);
        ast_free(first_value);
        shared_free_safe(keys, "parser", "unknown_function", 4374);
        shared_free_safe(values, "parser", "unknown_function", 4375);
        return NULL;
    }
    keys[0] = first_key;
    values[0] = first_value;
    size_t pair_count = 1;
    size_t pair_capacity = 1;
    
    // Parse remaining key-value pairs
    while (parser_check(parser, TOKEN_COMMA)) {
        parser_advance(parser);  // Consume the comma
        
        ASTNode* key = parser_parse_hash_map_key(parser);
        if (!key) {
            parser_error(parser, "Expected key expression after comma in hash map literal");
            // Clean up and return what we have
            for (size_t i = 0; i < pair_count; i++) {
                ast_free(keys[i]);
                ast_free(values[i]);
            }
            shared_free_safe(keys, "parser", "unknown_function", 4395);
            shared_free_safe(values, "parser", "unknown_function", 4396);
            return NULL;
        }
        
        // Expect colon
        if (!parser_match(parser, TOKEN_COLON)) {
            parser_error(parser, "Expected ':' after key in hash map literal");
            ast_free(key);
            // Clean up and return what we have
            for (size_t i = 0; i < pair_count; i++) {
                ast_free(keys[i]);
                ast_free(values[i]);
            }
            shared_free_safe(keys, "parser", "unknown_function", 4409);
            shared_free_safe(values, "parser", "unknown_function", 4410);
            return NULL;
        }
        
        ASTNode* value = parser_parse_expression(parser);
        if (!value) {
            parser_error(parser, "Expected value expression after ':' in hash map literal");
            ast_free(key);
            // Clean up and return what we have
            for (size_t i = 0; i < pair_count; i++) {
                ast_free(keys[i]);
                ast_free(values[i]);
            }
            shared_free_safe(keys, "parser", "unknown_function", 4423);
            shared_free_safe(values, "parser", "unknown_function", 4424);
            return NULL;
        }
        
        // Expand arrays if needed
        if (pair_count >= pair_capacity) {
            pair_capacity *= 2;
            keys = shared_realloc_safe(keys, pair_capacity * sizeof(ASTNode*), "parser", "unknown_function", 4431);
            values = shared_realloc_safe(values, pair_capacity * sizeof(ASTNode*), "parser", "unknown_function", 4432);
            if (!keys || !values) {
                parser_error(parser, "Memory allocation failed for hash map literal");
                ast_free(key);
                ast_free(value);
                // Clean up and return what we have
                for (size_t i = 0; i < pair_count; i++) {
                    ast_free(keys[i]);
                    ast_free(values[i]);
                }
                shared_free_safe(keys, "parser", "unknown_function", 4442);
                shared_free_safe(values, "parser", "unknown_function", 4443);
                return NULL;
            }
        }
        
        keys[pair_count] = key;
        values[pair_count] = value;
        pair_count++;
    }
    
    // Expect closing brace
    if (!parser_match(parser, TOKEN_RIGHT_BRACE)) {
        parser_error(parser, "Expected '}' to close hash map literal");
        // Clean up and return what we have
        for (size_t i = 0; i < pair_count; i++) {
            ast_free(keys[i]);
            ast_free(values[i]);
        }
        shared_free_safe(keys, "parser", "unknown_function", 4461);
        shared_free_safe(values, "parser", "unknown_function", 4462);
        return NULL;
    }
    
    return ast_create_hash_map_literal(keys, values, pair_count, parser->previous_token->line, parser->previous_token->column);
}

/**
 * Parse set literal: {item1, item2, item3, ...}
 * 
 * @param parser The parser to use
 * @return AST node representing the set literal
 */
static ASTNode* parser_parse_set_literal(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // Consume the opening brace
    parser_advance(parser);
    
    // Handle empty set
    if (parser_check(parser, TOKEN_RIGHT_BRACE)) {
        parser_advance(parser);
        return ast_create_set_literal(NULL, 0, parser->previous_token->line, parser->previous_token->column);
    }
    
    // Parse first element
    ASTNode* first_element = parser_parse_expression(parser);
    if (!first_element) {
        parser_error(parser, "Expected expression in set literal");
        return NULL;
    }
    
    // Allocate array for elements
    ASTNode** elements = shared_malloc_safe(sizeof(ASTNode*), "parser", "unknown_function", 4497);
    if (!elements) {
        parser_error(parser, "Memory allocation failed for set literal");
        ast_free(first_element);
        return NULL;
    }
    elements[0] = first_element;
    size_t element_count = 1;
    size_t element_capacity = 1;
    
    // Parse remaining elements
    while (parser_check(parser, TOKEN_COMMA)) {
        parser_advance(parser);  // Consume the comma
        
        ASTNode* element = parser_parse_expression(parser);
        if (!element) {
            parser_error(parser, "Expected expression after comma in set literal");
            // Clean up and return what we have
            for (size_t i = 0; i < element_count; i++) {
                ast_free(elements[i]);
            }
            shared_free_safe(elements, "parser", "unknown_function", 4518);
            return NULL;
        }
        
        // Expand array if needed
        if (element_count >= element_capacity) {
            element_capacity *= 2;
            elements = shared_realloc_safe(elements, element_capacity * sizeof(ASTNode*), "parser", "unknown_function", 4525);
            if (!elements) {
                parser_error(parser, "Memory allocation failed for set literal");
                ast_free(element);
                // Clean up and return what we have
                for (size_t i = 0; i < element_count; i++) {
                    ast_free(elements[i]);
                }
                shared_free_safe(elements, "parser", "unknown_function", 4533);
                return NULL;
            }
        }
        
        elements[element_count] = element;
        element_count++;
    }
    
    // Expect closing brace
    if (!parser_match(parser, TOKEN_RIGHT_BRACE)) {
        parser_error(parser, "Expected '}' to close set literal");
        // Clean up and return what we have
        for (size_t i = 0; i < element_count; i++) {
            ast_free(elements[i]);
        }
        shared_free_safe(elements, "parser", "unknown_function", 4549);
        return NULL;
    }
    
    return ast_create_set_literal(elements, element_count, parser->previous_token->line, parser->previous_token->column);
}

/**
 * @brief Parse a try-catch statement
 * 
 * Try-catch statements have the form: try: block catch error: block end
 * 
 * @param parser The parser to use
 * @return AST node representing the try-catch statement
 */
static ASTNode* parser_parse_try_catch_statement(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // Parse try block
    if (!parser_match(parser, TOKEN_COLON)) {
        parser_error(parser, "Try block must start with colon (:)");
        return NULL;
    }
    
    // Manually collect try block statements until 'catch'
    ASTNode** try_statements = NULL;
    size_t try_count = 0;
    size_t try_capacity = 0;
    
    while (parser->current_token && parser->current_token->type != TOKEN_EOF) {
        // Check for 'catch' keyword to end try block
        if (parser->current_token->type == TOKEN_KEYWORD && parser->current_token->text &&
            strcmp(parser->current_token->text, "catch") == 0) {
            break;
        }
        
        // Skip empty statements
        if (parser->current_token->type == TOKEN_SEMICOLON) {
            parser_advance(parser);
            continue;
        }
        
        ASTNode* stmt = parser_parse_statement(parser);
        if (stmt) {
            // Expand array if needed
            if (try_count >= try_capacity) {
                size_t new_capacity = try_capacity == 0 ? 4 : try_capacity * 2;
                ASTNode** new_statements = shared_realloc_safe(try_statements, new_capacity * sizeof(ASTNode*), "parser", "unknown_function", 4598);
                if (new_statements) {
                    try_statements = new_statements;
                    try_capacity = new_capacity;
                }
            }
            try_statements[try_count++] = stmt;
        } else {
            parser_synchronize(parser);
        }
    }
    
    ASTNode* try_block = ast_create_block(try_statements, try_count, 0, 0);
    if (!try_block && try_statements) {
        shared_free_safe(try_statements, "parser", "unknown_function", 4612);
    }
    if (!try_block) {
        parser_error(parser, "Expected try block");
        return NULL;
    }
    
    // Expect 'catch' keyword
    if (!parser_check(parser, TOKEN_KEYWORD) || 
        !parser->current_token || !parser->current_token->text || 
        strcmp(parser->current_token->text, "catch") != 0) {
        parser_error(parser, "Expected 'catch' after try block");
        ast_free(try_block);
        return NULL;
    }
    parser_advance(parser);  // Consume 'catch'
    
    // Parse catch variable name
    char* catch_variable = NULL;
    if (parser_match(parser, TOKEN_IDENTIFIER)) {
        catch_variable = strdup(parser->previous_token->text);
    } else {
        parser_error(parser, "Expected catch variable name");
        ast_free(try_block);
        return NULL;
    }
    
    // Expect colon after catch variable
    if (!parser_match(parser, TOKEN_COLON)) {
        parser_error(parser, "Catch block must start with colon (:)");
        shared_free_safe(catch_variable, "parser", "unknown_function", 4642);
        ast_free(try_block);
        return NULL;
    }
    
    // Parse catch block
    ASTNode* catch_block = parser_collect_block(parser, 0, NULL);
    if (!catch_block) {
        parser_error(parser, "Expected catch block");
        shared_free_safe(catch_variable, "parser", "unknown_function", 4651);
        ast_free(try_block);
        return NULL;
    }
    
    // Expect 'end' to close try-catch
    if (!parser_check(parser, TOKEN_KEYWORD) || 
        !parser->current_token || !parser->current_token->text || 
        strcmp(parser->current_token->text, "end") != 0) {
        parser_error(parser, "Expected 'end' to close try-catch statement");
        shared_free_safe(catch_variable, "parser", "unknown_function", 4661);
        ast_free(try_block);
        ast_free(catch_block);
        return NULL;
    }
    parser_advance(parser);  // Consume 'end'
    
    return ast_create_try_catch(try_block, catch_variable, catch_block, NULL, 0, 0);
}

/**
 * @brief Parse member access chain after a base expression
 * 
 * This function handles method chaining like obj.method1().method2().method3()
 * It continues parsing member access and function calls until there are no more dots.
 * 
 * @param parser The parser to use
 * @param base The base expression to start the chain from
 * @return AST node representing the complete member access chain
 */
static ASTNode* parser_parse_member_access_chain(Parser* parser, ASTNode* base) {
    if (!parser || !base) {
        return base;
    }
    
    ASTNode* current = base;
    
    // Continue parsing member access chains while we see dots
    while (parser_check(parser, TOKEN_DOT)) {
        parser_advance(parser); // consume '.'
        
        // Parse the member name (can be identifier or keyword)
        if (!parser_check(parser, TOKEN_IDENTIFIER) && !parser_check(parser, TOKEN_KEYWORD)) {
            parser_error(parser, "Expected member name after '.'");
            ast_free(current);
            return NULL;
        }
        
        Token* member_token = parser_peek(parser);
        parser_advance(parser); // consume member name
        
        // Create member access node
        ASTNode* member_access = ast_create_member_access(current, member_token->text, member_token->line, member_token->column);
        if (!member_access) {
            ast_free(current);
            return NULL;
        }
        
        // Check for function calls on the member access: obj.method(args)
        if (parser_check(parser, TOKEN_LEFT_PAREN)) {
            parser_advance(parser); // consume '('
            
            ASTNode** args = NULL;
            size_t arg_count = 0;
            size_t arg_capacity = 0;
            
            // Parse optional arguments
            if (!parser_check(parser, TOKEN_RIGHT_PAREN)) {
                while (1) {
                    ASTNode* arg = parser_parse_expression(parser);
                    if (!arg) {
                        parser_error(parser, "Function call arguments must be valid expressions");
                        break;
                    }
                    if (arg_count == arg_capacity) {
                        size_t new_cap = arg_capacity == 0 ? 4 : arg_capacity * 2;
                        ASTNode** new_args = (ASTNode**)shared_realloc_safe(args, new_cap * sizeof(ASTNode*), "parser", "unknown_function", 4727);
                        if (!new_args) {
                            parser_error(parser, "Out of memory while parsing arguments");
                            break;
                        }
                        args = new_args;
                        arg_capacity = new_cap;
                    }
                    args[arg_count++] = arg;
                    if (parser_check(parser, TOKEN_COMMA)) {
                        parser_advance(parser);
                        continue;
                    }
                    break;
                }
            }
            
            if (!parser_match(parser, TOKEN_RIGHT_PAREN)) {
                parser_error(parser, "Expected ')' after arguments");
            }
            
            // Create function call expression node
            ASTNode* call = ast_create_function_call_expr(member_access, args, arg_count, member_token->line, member_token->column);
            if (call) {
                current = call;
            } else {
                ast_free(member_access);
                return NULL;
            }
        } else {
            // Just member access, no function call
            current = member_access;
        }
    }
    
    return current;
}

static const char* get_error_suggestion(const char* message, Token* token) {
    if (!token || !message) return "Check your syntax and try again.";
    
    // Check for common error patterns and provide specific suggestions
    if (strstr(message, "Missing ';'")) {
        return "Add a semicolon (;) at the end of this statement.";
    }
    
    if (strstr(message, "Expected 'let'")) {
        return "Variable declarations must start with 'let' followed by a name, type, and value.";
    }
    
    if (strstr(message, "Expected variable name")) {
        return "Variable names must be valid identifiers (letters, numbers, underscores).";
    }
    
    if (strstr(message, "Expected type name")) {
        return "Valid types are: Int, String, Bool, Float, or custom types. Use camelCase for custom types.";
    }
    
    if (strstr(message, "Expected '='")) {
        return "Variable declarations require an equals sign (=) followed by an initial value.";
    }
    
    if (strstr(message, "Expected ':'")) {
        return "Blocks must start with a colon (:) and end with 'end'.";
    }
    
    if (strstr(message, "Expected 'end'")) {
        return "Blocks must be closed with the 'end' keyword.";
    }
    
    if (strstr(message, "Expected '('")) {
        return "Function calls require parentheses around arguments.";
    }
    
    if (strstr(message, "Expected ')'")) {
        return "Function calls must have matching parentheses.";
    }
    
    if (strstr(message, "Expected 'func'")) {
        return "Function declarations must start with 'func' followed by a name and parameters.";
    }
    
    if (strstr(message, "Expected 'if'")) {
        return "If statements must start with 'if' followed by a condition and colon (:).";
    }
    
    if (strstr(message, "Expected 'while'")) {
        return "While loops must start with 'while' followed by a condition and colon (:).";
    }
    
    if (strstr(message, "Expected 'for'")) {
        return "For loops must use the format: for variable in range: ... end";
    }
    
    if (strstr(message, "Expected 'spore'")) {
        return "Pattern matching statements must start with 'spore' followed by an expression and colon (:).";
    }
    
    if (strstr(message, "Expected 'return'")) {
        return "Return statements must start with 'return' followed by an expression.";
    }
    
    if (strstr(message, "Expected '['")) {
        return "Array literals must start with '[' and end with ']'. Elements are separated by commas.";
    }
    
    if (strstr(message, "Expected ']'")) {
        return "Array literals must be closed with ']'. Check for missing closing bracket.";
    }
    
    if (strstr(message, "Expected expression")) {
        return "This location expects a valid expression (number, string, variable, function call, etc.).";
    }
    
    if (strstr(message, "Expected statement")) {
        return "This location expects a complete statement (assignment, function call, control flow, etc.).";
    }
    
    if (strstr(message, "Expected primary expression")) {
        return "This location expects a basic expression (number, string, variable, or parenthesized expression).";
    }
    
    // Default suggestion based on token type
    if (token->type == TOKEN_SEMICOLON) {
        return "Semicolons are used to separate statements. Check if you need one here.";
    }
    
    if (token->type == TOKEN_COLON) {
        return "Colons are used to start blocks. Make sure you have a matching 'end' statement.";
    }
    
    if (token->type == TOKEN_IDENTIFIER) {
        return "Identifiers must be declared before use. Check if this variable or function is defined.";
    }
    
    if (token->type == TOKEN_KEYWORD) {
        return "Keywords have specific syntax. Check the Myco language reference for proper usage.";
    }
    
    if (token->type == TOKEN_LEFT_BRACKET) {
        return "Array literals start with '[' and can contain mixed types like [1, \"hello\", [nested, array]].";
    }
    
    if (token->type == TOKEN_RIGHT_BRACKET) {
        return "Array literals must be closed with ']'. Check for matching opening bracket.";
    }
    
    if (token->type == TOKEN_LEFT_BRACE) {
        return "Braces {} are not used in Myco. Use colons (:) to start blocks and 'end' to close them.";
    }
    
    if (token->type == TOKEN_RIGHT_BRACE) {
        return "Braces } are not used in Myco. Use 'end' to close blocks.";
    }
    
    return "Review the syntax around this location. Myco uses semicolons to separate statements and colons to start blocks.";
}

/**
 * @brief Parse a pattern (enhanced pattern matching)
 * 
 * Patterns support type matching, destructuring, guards, and combinators:
 * Int, String, {name: n, age: a}, [x, y, z], pattern when condition, pattern1 | pattern2
 * 
 * @param parser The parser to use
 * @return AST node representing the pattern
 */
ASTNode* parser_parse_pattern(Parser* parser) {
    if (!parser) {
        return NULL;
    }
    
    // Parse pattern with OR precedence (lowest precedence)
    return parser_parse_pattern_or(parser);
}

/**
 * @brief Parse pattern OR (alternative patterns)
 * 
 * @param parser The parser to use
 * @return AST node representing the pattern
 */
static ASTNode* parser_parse_pattern_or(Parser* parser) {
    ASTNode* left = parser_parse_pattern_and(parser);
    
    while (parser_check(parser, TOKEN_PIPE)) {
        parser_advance(parser); // consume '|'
        ASTNode* right = parser_parse_pattern_and(parser);
        if (!right) {
            ast_free(left);
            return NULL;
        }
        left = ast_create_pattern_or(left, right, 0, 0);
    }
    
    return left;
}

/**
 * @brief Parse pattern AND (conjunctive patterns)
 * 
 * @param parser The parser to use
 * @return AST node representing the pattern
 */
static ASTNode* parser_parse_pattern_and(Parser* parser) {
    ASTNode* left = parser_parse_pattern_not(parser);
    
    while (parser_check(parser, TOKEN_AMPERSAND)) {
        parser_advance(parser); // consume '&'
        ASTNode* right = parser_parse_pattern_not(parser);
        if (!right) {
            ast_free(left);
            return NULL;
        }
        left = ast_create_pattern_and(left, right, 0, 0);
    }
    
    return left;
}

/**
 * @brief Parse pattern NOT (negation)
 * 
 * @param parser The parser to use
 * @return AST node representing the pattern
 */
static ASTNode* parser_parse_pattern_not(Parser* parser) {
    if (parser_check(parser, TOKEN_EXCLAMATION)) {
        parser_advance(parser); // consume '!'
        ASTNode* pattern = parser_parse_pattern_primary(parser);
        if (!pattern) {
            return NULL;
        }
        return ast_create_pattern_not(pattern, 0, 0);
    }
    
    return parser_parse_pattern_primary(parser);
}

/**
 * @brief Parse primary pattern (base patterns)
 * 
 * @param parser The parser to use
 * @return AST node representing the pattern
 */
static ASTNode* parser_parse_pattern_primary(Parser* parser) {
    if (!parser || !parser->current_token) {
        return NULL;
    }
    
    Token* token = parser->current_token;
    
    // Type pattern: Int, String, Array, etc.
    if (token->type == TOKEN_IDENTIFIER && token->text) {
        // Check if it's a type name (capitalized)
        if (isupper(token->text[0])) {
            parser_advance(parser);
            return ast_create_pattern_type(token->text, 0, 0);
        }
    }
    
    // Wildcard pattern: _
    if (token->type == TOKEN_UNDERSCORE) {
        parser_advance(parser);
        return ast_create_pattern_wildcard(0, 0);
    }
    
    // Range pattern: start..end or start...end
    if (token->type == TOKEN_NUMBER) {
        ASTNode* start = parser_parse_expression(parser);
        if (parser_check(parser, TOKEN_DOT_DOT)) {
            parser_advance(parser); // consume '..'
            int inclusive = 1;
            if (parser_check(parser, TOKEN_DOT)) {
                parser_advance(parser); // consume '.'
                inclusive = 0; // exclusive range
            }
            ASTNode* end = parser_parse_expression(parser);
            if (!end) {
                ast_free(start);
                return NULL;
            }
            return ast_create_pattern_range(start, end, inclusive, 0, 0);
        }
        return start; // Not a range, just a number
    }
    
    // Regex pattern: /pattern/
    if (token->type == TOKEN_SLASH) {
        parser_advance(parser); // consume '/'
        if (parser->current_token && parser->current_token->type == TOKEN_STRING) {
            char* regex_pattern = strdup(parser->current_token->text);
            parser_advance(parser);
            if (!parser_check(parser, TOKEN_SLASH)) {
                shared_free_safe(regex_pattern, "parser", "unknown_function", 0);
                parser_error(parser, "Expected closing '/' for regex pattern");
                return NULL;
            }
            parser_advance(parser); // consume closing '/'
            return ast_create_pattern_regex(regex_pattern, 0, 0, 0);
        }
    }
    
    // Destructuring patterns: [a, b, c] or {name: n, age: a}
    if (token->type == TOKEN_LEFT_BRACKET) {
        return parser_parse_array_destructure(parser);
    }
    
    if (token->type == TOKEN_LEFT_BRACE) {
        return parser_parse_object_destructure(parser);
    }
    
    // Guard pattern: pattern when condition
    ASTNode* base_pattern = parser_parse_expression(parser);
    if (base_pattern && parser_check(parser, TOKEN_KEYWORD) && 
        parser->current_token->text && strcmp(parser->current_token->text, "when") == 0) {
        parser_advance(parser); // consume 'when'
        ASTNode* condition = parser_parse_expression(parser);
        if (!condition) {
            ast_free(base_pattern);
            return NULL;
        }
        return ast_create_pattern_guard(base_pattern, condition, 0, 0);
    }
    
    return base_pattern;
}

/**
 * @brief Parse array destructuring pattern [a, b, c] or [head, ...tail]
 * 
 * @param parser The parser to use
 * @return AST node representing the pattern
 */
static ASTNode* parser_parse_array_destructure(Parser* parser) {
    parser_advance(parser); // consume '['
    
    ASTNode** patterns = NULL;
    size_t pattern_count = 0;
    size_t pattern_capacity = 0;
    
    while (parser->current_token && parser->current_token->type != TOKEN_RIGHT_BRACKET) {
        // Parse pattern element
        ASTNode* pattern = parser_parse_pattern(parser);
        if (!pattern) {
            // Cleanup on error
            for (size_t i = 0; i < pattern_count; i++) {
                ast_free(patterns[i]);
            }
            shared_free_safe(patterns, "parser", "unknown_function", 0);
            return NULL;
        }
        
        // Resize array if needed
        if (pattern_count >= pattern_capacity) {
            pattern_capacity = pattern_capacity ? pattern_capacity * 2 : 4;
            patterns = shared_realloc_safe(patterns, pattern_capacity * sizeof(ASTNode*), "parser", "unknown_function", 0);
        }
        
        patterns[pattern_count++] = pattern;
        
        // Check for comma or end
        if (parser_check(parser, TOKEN_COMMA)) {
            parser_advance(parser); // consume ','
        } else if (parser->current_token->type != TOKEN_RIGHT_BRACKET) {
            parser_error(parser, "Expected ',' or ']' in array destructuring");
            break;
        }
    }
    
    if (!parser_check(parser, TOKEN_RIGHT_BRACKET)) {
        parser_error(parser, "Expected ']' to close array destructuring");
        // Cleanup
        for (size_t i = 0; i < pattern_count; i++) {
            ast_free(patterns[i]);
        }
        shared_free_safe(patterns, "parser", "unknown_function", 0);
        return NULL;
    }
    
    parser_advance(parser); // consume ']'
    return ast_create_pattern_destructure(patterns, pattern_count, 1, 0, 0);
}

/**
 * @brief Parse object destructuring pattern {name: n, age: a}
 * 
 * @param parser The parser to use
 * @return AST node representing the pattern
 */
static ASTNode* parser_parse_object_destructure(Parser* parser) {
    parser_advance(parser); // consume '{'
    
    ASTNode** patterns = NULL;
    size_t pattern_count = 0;
    size_t pattern_capacity = 0;
    
    while (parser->current_token && parser->current_token->type != TOKEN_RIGHT_BRACE) {
        // Parse field pattern: name: pattern or name
        if (parser->current_token->type != TOKEN_IDENTIFIER) {
            parser_error(parser, "Expected field name in object destructuring");
            break;
        }
        
        char* field_name = strdup(parser->current_token->text);
        parser_advance(parser);
        
        ASTNode* pattern;
        if (parser_check(parser, TOKEN_COLON)) {
            parser_advance(parser); // consume ':'
            pattern = parser_parse_pattern(parser);
        } else {
            // Shorthand: {name} is equivalent to {name: name}
            pattern = ast_create_identifier(field_name, 0, 0);
        }
        
        if (!pattern) {
            shared_free_safe(field_name, "parser", "unknown_function", 0);
            // Cleanup on error
            for (size_t i = 0; i < pattern_count; i++) {
                ast_free(patterns[i]);
            }
            shared_free_safe(patterns, "parser", "unknown_function", 0);
            return NULL;
        }
        
        // Resize array if needed
        if (pattern_count >= pattern_capacity) {
            pattern_capacity = pattern_capacity ? pattern_capacity * 2 : 4;
            patterns = shared_realloc_safe(patterns, pattern_capacity * sizeof(ASTNode*), "parser", "unknown_function", 0);
        }
        
        patterns[pattern_count++] = pattern;
        shared_free_safe(field_name, "parser", "unknown_function", 0);
        
        // Check for comma or end
        if (parser_check(parser, TOKEN_COMMA)) {
            parser_advance(parser); // consume ','
        } else if (parser->current_token->type != TOKEN_RIGHT_BRACE) {
            parser_error(parser, "Expected ',' or '}' in object destructuring");
            break;
        }
    }
    
    if (!parser_check(parser, TOKEN_RIGHT_BRACE)) {
        parser_error(parser, "Expected '}' to close object destructuring");
        // Cleanup
        for (size_t i = 0; i < pattern_count; i++) {
            ast_free(patterns[i]);
        }
        shared_free_safe(patterns, "parser", "unknown_function", 0);
        return NULL;
    }
    
    parser_advance(parser); // consume '}'
    return ast_create_pattern_destructure(patterns, pattern_count, 0, 0, 0);
}
