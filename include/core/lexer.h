#ifndef MYCO_LEXER_H
#define MYCO_LEXER_H

/**
 * @file lexer.h
 * @brief Lexical Analysis Module - converts source code into tokens
 * 
 * The lexer (also called a tokenizer) is the first step in processing Myco source code.
 * It reads the raw text character by character and groups them into meaningful units
 * called "tokens". These tokens represent the basic building blocks of the language:
 * 
 * Examples of tokens:
 * - Keywords: 'if', 'while', 'function', 'return'
 * - Identifiers: variable names, function names
 * - Literals: numbers (42, 3.14), strings ("hello"), booleans (True, False)
 * - Operators: +, -, *, /, ==, !=, &&, ||
 * - Punctuation: (, ), {, }, ;, :
 * 
 * The lexer handles:
 * - Whitespace and comments
 * - String and number parsing
 * - Identifier recognition
 * - Error reporting for invalid characters
 * - Line and column tracking for error messages
 * 
 * When adding new language features, you'll often need to:
 * 1. Add new token types to the TokenType enum
 * 2. Update the lexer logic to recognize new patterns
 * 3. Handle any special parsing requirements
 */

#include <stddef.h>  // For size_t type

// Token types that the lexer can produce
typedef enum {
    // End of file marker - signals completion of source code
    TOKEN_EOF,
    
    // Literal values
    TOKEN_NUMBER,      // Numeric literals (42, 3.14, 0xFF)
    TOKEN_STRING,      // String literals ("hello", 'world')
    TOKEN_BOOL,        // Boolean literals (True, False)
    
    // Identifiers and keywords
    TOKEN_IDENTIFIER,  // Variable names, function names, etc.
    TOKEN_KEYWORD,     // Language keywords (if, while, function)
    
    // Operators
    TOKEN_PLUS,        // Addition operator (+)
    TOKEN_MINUS,       // Subtraction operator (-)
    TOKEN_MULTIPLY,    // Multiplication operator (*)
    TOKEN_DIVIDE,      // Division operator (/)
    TOKEN_MODULO,      // Modulo operator (%)
    TOKEN_POWER,       // Power operator (**)
    
    // Comparison operators
    TOKEN_EQUAL,       // Equality comparison (==)
    TOKEN_NOT_EQUAL,   // Inequality comparison (!=)
    TOKEN_LESS,        // Less than comparison (<)
    TOKEN_GREATER,     // Greater than comparison (>)
    TOKEN_LESS_EQUAL,  // Less than or equal (<=)
    TOKEN_GREATER_EQUAL, // Greater than or equal (>=)
    
    // Logical operators
    TOKEN_AND,         // Logical AND (and)
    TOKEN_OR,          // Logical OR (or)
    TOKEN_NOT,         // Logical NOT (not)
    
    // Assignment operators
    TOKEN_ASSIGN,      // Simple assignment (=)
    TOKEN_PLUS_ASSIGN, // Add and assign (+=)
    TOKEN_MINUS_ASSIGN, // Subtract and assign (-=)
    TOKEN_MULTIPLY_ASSIGN, // Multiply and assign (*=)
    TOKEN_DIVIDE_ASSIGN,   // Divide and assign (/=)
    
    // Punctuation and structure
    TOKEN_LEFT_PAREN,  // Left parenthesis (
    TOKEN_RIGHT_PAREN, // Right parenthesis )
    TOKEN_LEFT_BRACE,  // Left brace {
    TOKEN_RIGHT_BRACE, // Right brace }
    TOKEN_LEFT_BRACKET, // Left bracket [
    TOKEN_RIGHT_BRACKET, // Right bracket ]
    TOKEN_SEMICOLON,   // Statement separator (;)
    TOKEN_COLON,       // Block separator (:)
    TOKEN_COMMA,       // List separator (,)
    TOKEN_DOT,         // Member access (.)
    TOKEN_DOT_DOT,     // Range operator (..)
    TOKEN_ARROW,       // Arrow operator (=>)
    TOKEN_RETURN_ARROW, // Return type arrow (->)
    TOKEN_PIPE,        // Union type operator (|)
    TOKEN_QUESTION,    // Optional type operator (?)
    TOKEN_AMPERSAND,   // AND operator (&)
    TOKEN_EXCLAMATION, // NOT operator (!)
    TOKEN_UNDERSCORE,  // Wildcard pattern (_)
    TOKEN_SLASH,       // Division or regex delimiter (/)
    
    // Special tokens
    TOKEN_ERROR,       // Indicates a lexical error occurred
    TOKEN_COMMENT,     // Comment tokens (for documentation)
    
    // Macro and metaprogramming tokens
    TOKEN_MACRO,       // macro keyword
    TOKEN_CONST,       // const keyword (compile-time constants)
    TOKEN_TEMPLATE,    // template keyword
    TOKEN_EXPAND,      // expand keyword (manual macro expansion)
    TOKEN_COMPTIME     // comptime keyword (compile-time evaluation)
} TokenType;

// Represents a single token in the source code
typedef struct {
    TokenType type;        // What kind of token this is
    char* text;            // The actual text that was tokenized
    int line;              // Line number in source file (1-based)
    int column;            // Column number in source file (1-based)
    union {
        double number_value;    // Numeric value for TOKEN_NUMBER
        char* string_value;     // String value for TOKEN_STRING
        int bool_value;         // Boolean value for TOKEN_BOOL
    } data;
} Token;

// Lexer state and configuration
typedef struct {
    const char* source;    // Pointer to the source code text
    int start;             // Start position of current token
    int current;           // Current position being examined
    int line;              // Current line number
    int column;            // Current column number
    Token* tokens;         // Array of tokens found so far
    int token_count;       // Number of tokens in the array
    int token_capacity;    // Maximum number of tokens that can be stored
} Lexer;

/**
 * @brief Initialize a new lexer with source code
 * 
 * This function sets up a lexer to process the given source code. It initializes
 * all internal state variables and prepares the lexer to start tokenization.
 * 
 * @param source The source code string to tokenize
 * @return A pointer to the initialized lexer, or NULL if allocation failed
 * 
 * Usage example:
 * @code
 * Lexer* lexer = lexer_initialize("let x = 42;");
 * if (lexer) {
 *     // Use the lexer...
 *     lexer_free(lexer);
 * }
 * @endcode
 */
Lexer* lexer_initialize(const char* source);

/**
 * @brief Free all memory associated with a lexer
 * 
 * This function cleans up the lexer and all tokens it has produced. Call this
 * when you're done using the lexer to prevent memory leaks.
 * 
 * @param lexer The lexer to free
 */
void lexer_free(Lexer* lexer);

/**
 * @brief Scan all tokens from the source code
 * 
 * This is the main function that processes the entire source code and produces
 * a complete list of tokens. It handles all the complexity of recognizing
 * different token types and managing the token array.
 * 
 * @param lexer The lexer to use for scanning
 * @return Number of tokens found, or -1 if an error occurred
 * 
 * After calling this function, you can access the tokens through lexer->tokens
 * and lexer->token_count.
 */
int lexer_scan_all(Lexer* lexer);

/**
 * @brief Scan the next token from the source code
 * 
 * This function processes the next token from the current position in the source.
 * It's useful for incremental parsing or when you want to process tokens one
 * at a time rather than all at once.
 * 
 * @param lexer The lexer to use for scanning
 * @return A pointer to the next token, or NULL if no more tokens
 * 
 * The returned token is owned by the lexer and will be freed when lexer_free()
 * is called.
 */
Token* lexer_scan_next(Lexer* lexer);

/**
 * @brief Get a specific token by index
 * 
 * This function provides safe access to tokens in the lexer's token array.
 * It checks bounds and returns NULL if the index is invalid.
 * 
 * @param lexer The lexer containing the tokens
 * @param index The index of the token to retrieve (0-based)
 * @return Pointer to the token, or NULL if index is out of bounds
 */
Token* lexer_get_token(Lexer* lexer, int index);

/**
 * @brief Print all tokens for debugging purposes
 * 
 * This function outputs all tokens in a human-readable format. It's very useful
 * for debugging lexer issues or understanding how source code is being tokenized.
 * 
 * @param lexer The lexer containing the tokens to print
 */
void lexer_print_tokens(Lexer* lexer);

/**
 * @brief Check if the lexer encountered any errors during scanning
 * 
 * This function returns whether any TOKEN_ERROR tokens were produced during
 * tokenization. It's useful for determining if the source code is valid.
 * 
 * @param lexer The lexer to check
 * @return 1 if errors occurred, 0 if no errors
 */
int lexer_has_errors(Lexer* lexer);

/**
 * @brief Get the total number of tokens found
 * 
 * This function returns the count of tokens that have been successfully
 * scanned from the source code.
 * 
 * @param lexer The lexer to check
 * @return Number of tokens found
 */
int lexer_get_token_count(Lexer* lexer);

#endif // MYCO_LEXER_H
