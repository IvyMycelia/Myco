#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "../../include/utils/shared_utilities.h"

/**
 * @brief Initialize a new lexer with source code
 * 
 * This function creates and initializes a new lexer instance that will
 * process the given source code string. The lexer maintains its own
 * copy of the source code and initializes all internal state variables.
 * 
 * @param source The source code string to tokenize (must not be NULL)
 * @return A pointer to the initialized lexer, or NULL if allocation failed
 * 
 * The lexer will make a copy of the source string, so the original
 * can be freed after calling this function.
 */
Lexer* lexer_initialize(const char* source) {
    // Validate input parameters
    if (!source) {
        return NULL;
    }
    
    // Allocate memory for the lexer structure
    Lexer* lexer = shared_malloc_safe(sizeof(Lexer), "core", "unknown_function", 28);
    if (!lexer) {
        return NULL;  // Memory allocation failed
    }
    
    // Initialize lexer state
    lexer->source = (source ? strdup(source) : NULL);        // Make a copy of the source
    lexer->start = 0;                      // Start position of current token
    lexer->current = 0;                    // Current position being examined
    lexer->line = 1;                       // Current line number (1-based)
    lexer->column = 1;                     // Current column number (1-based)
    lexer->tokens = NULL;                  // Token array (allocated on demand)
    lexer->token_count = 0;                // Number of tokens found so far
    lexer->token_capacity = 0;             // Current capacity of token array
    
    return lexer;
}

/**
 * @brief Free all memory associated with a lexer
 * 
 * This function cleans up the lexer and all tokens it has produced.
 * It frees the source code copy, the token array, and the lexer structure itself.
 * 
 * @param lexer The lexer to free (can be NULL)
 * 
 * After calling this function, the lexer pointer is no longer valid
 * and should not be used.
 */
void lexer_free(Lexer* lexer) {
    if (!lexer) {
        return;  // Nothing to free
    }
    
    // Free the source code copy
        if (lexer->source) {
        // Note: source is const char*, so we don't free it
        // The caller is responsible for managing the source memory
        lexer->source = NULL;
    }
    
    // Free all tokens in the array
    if (lexer->tokens) {
        for (int i = 0; i < lexer->token_count; i++) {
            if (lexer->tokens[i].text) {
                shared_free_safe(lexer->tokens[i].text, "core", "unknown_function", 73);
            }
        }
        shared_free_safe(lexer->tokens, "core", "unknown_function", 76);
        lexer->tokens = NULL;
    }
    
    // Free the lexer structure itself
        shared_free_safe(lexer, "core", "unknown_function", 81);
}

/**
 * @brief Add a token to the lexer's token array
 * 
 * This function adds a new token to the lexer's internal token array,
 * automatically expanding the array if necessary.
 * 
 * @param lexer The lexer to add the token to
 * @param type The type of token to add
 * @param text The text content of the token
 * @param line The line number where the token was found
 * @param column The column number where the token was found
 * @return 1 if successful, 0 if failed
 */
static int lexer_add_token(Lexer* lexer, TokenType type, const char* text, int line, int column) {
    // Expand token array if needed
    if (lexer->token_count >= lexer->token_capacity) {
        int new_capacity = lexer->token_capacity == 0 ? 100 : lexer->token_capacity * 2;
        Token* new_tokens = shared_realloc_safe(lexer->tokens, sizeof(Token) * new_capacity, "core", "unknown_function", 101);
        if (!new_tokens) {
            return 0;  // Memory allocation failed
        }
        lexer->tokens = new_tokens;
        lexer->token_capacity = new_capacity;
    }
    
    // Create the new token
    Token* token = &lexer->tokens[lexer->token_count];
    token->type = type;
    token->line = line;
    token->column = column;
    
    // Copy the text content
    if (text) {
        token->text = (text ? strdup(text) : NULL);
        if (!token->text) {
            return 0;  // Memory allocation failed
        }
    } else {
        token->text = NULL;
    }
    
    // Initialize the data union based on token type
    switch (type) {
        case TOKEN_NUMBER:
            token->data.number_value = text ? atof(text) : 0.0;
            break;
        case TOKEN_STRING:
            token->data.string_value = text ? (text ? strdup(text) : NULL) : NULL;
            break;
        case TOKEN_BOOL:
            if (text && (strcmp(text, "True") == 0 || strcmp(text, "true") == 0)) {
                token->data.bool_value = 1;
            } else {
                token->data.bool_value = 0;
            }
            break;
        default:
            // For other token types, data is not used
            break;
    }
    
    lexer->token_count++;
    return 1;
}

/**
 * @brief Check if we've reached the end of the source code
 * 
 * @param lexer The lexer to check
 * @return 1 if at end, 0 otherwise
 */
static int lexer_is_at_end(Lexer* lexer) {
    if (!lexer || !lexer->source) {
        return 1;  // Consider at end if no source
    }
    return (size_t)lexer->current >= strlen(lexer->source);
}

/**
 * @brief Get the current character from the source
 * 
 * @param lexer The lexer to get the character from
 * @return The current character, or '\0' if at end
 */
static char lexer_current_char(Lexer* lexer) {
    if (!lexer || !lexer->source || lexer_is_at_end(lexer)) {
        return '\0';
    }
    return lexer->source[lexer->current];
}

/**
 * @brief Get the next character from the source
 * 
 * @param lexer The lexer to get the character from
 * @return The next character, or '\0' if at end
 */
static char lexer_next_char(Lexer* lexer) {
    if (!lexer || !lexer->source || (size_t)(lexer->current + 1) >= strlen(lexer->source)) {
        return '\0';
    }
    return lexer->source[lexer->current + 1];
}

/**
 * @brief Advance the lexer to the next character
 * 
 * @param lexer The lexer to advance
 */
static void lexer_advance(Lexer* lexer) {
    if (!lexer_is_at_end(lexer)) {
        if (lexer->source[lexer->current] == '\n') {
            lexer->line++;
            lexer->column = 1;
        } else {
            lexer->column++;
        }
        lexer->current++;
    }
}

/**
 * @brief Check if the current character matches the expected character
 * 
 * @param lexer The lexer to check
 * @param expected The expected character
 * @return 1 if matches, 0 otherwise
 */
static int lexer_match(Lexer* lexer, char expected) {
    if (lexer_is_at_end(lexer)) {
        return 0;
    }
    if (lexer->source[lexer->current] != expected) {
        return 0;
    }
    lexer_advance(lexer);
    return 1;
}

/**
 * @brief Skip whitespace characters
 * 
 * @param lexer The lexer to skip whitespace in
 */
static void lexer_skip_whitespace(Lexer* lexer) {
    while (!lexer_is_at_end(lexer) && isspace(lexer_current_char(lexer))) {
        lexer_advance(lexer);
    }
}

/**
 * @brief Skip comments (lines starting with #)
 * 
 * @param lexer The lexer to skip comments in
 */
static void lexer_skip_comments(Lexer* lexer) {
    // Handle # style comments
    if (lexer_current_char(lexer) == '#') {
        // Skip to end of line
        while (!lexer_is_at_end(lexer) && lexer_current_char(lexer) != '\n') {
            lexer_advance(lexer);
        }
        // Also skip the newline character that ends the comment
        if (!lexer_is_at_end(lexer) && lexer_current_char(lexer) == '\n') {
            lexer_advance(lexer);
        }
    }
    // Handle // style comments
    else if (lexer_current_char(lexer) == '/' && !lexer_is_at_end(lexer) && lexer_next_char(lexer) == '/') {
        // Skip to end of line
        while (!lexer_is_at_end(lexer) && lexer_current_char(lexer) != '\n') {
            lexer_advance(lexer);
        }
        // Also skip the newline character that ends the comment
        if (!lexer_is_at_end(lexer) && lexer_current_char(lexer) == '\n') {
            lexer_advance(lexer);
        }
    }
}

/**
 * @brief Extract text from start to current position
 * 
 * @param lexer The lexer to extract text from
 * @return A newly allocated string containing the extracted text
 */
static char* lexer_extract_text(Lexer* lexer) {
    if (!lexer || !lexer->source) {
        return NULL;
    }
    int length = lexer->current - lexer->start;
    char* text = shared_malloc_safe(length + 1, "core", "unknown_function", 257);
    if (text) {
        strncpy(text, lexer->source + lexer->start, length);
        text[length] = '\0';
    }
    return text;
}

/**
 * @brief Parse a number literal
 * 
 * @param lexer The lexer to parse the number in
 */
static void lexer_parse_number(Lexer* lexer) {
    // Consume digits and underscores
    while (!lexer_is_at_end(lexer) && (isdigit(lexer_current_char(lexer)) || lexer_current_char(lexer) == '_')) {
        lexer_advance(lexer);
    }
    
    // Look for decimal point
    if (!lexer_is_at_end(lexer) && lexer_current_char(lexer) == '.' && 
        !lexer_is_at_end(lexer) && isdigit(lexer_next_char(lexer))) {
        lexer_advance(lexer);  // Consume the decimal point
        
        // Consume digits and underscores after decimal point
        while (!lexer_is_at_end(lexer) && (isdigit(lexer_current_char(lexer)) || lexer_current_char(lexer) == '_')) {
            lexer_advance(lexer);
        }
    }
    
    // Extract the number text
    char* text = lexer_extract_text(lexer);
    if (text) {
        // Remove underscores from the number text before converting to double
        char* clean_text = shared_malloc_safe((text ? strlen(text) : 0) + 1, "core", "unknown_function", 291);
        if (clean_text) {
            size_t j = 0;
            for (size_t i = 0; text[i] != '\0'; i++) {
                if (text[i] != '_') {
                    clean_text[j++] = text[i];
                }
            }
            clean_text[j] = '\0';
            
            lexer_add_token(lexer, TOKEN_NUMBER, clean_text, lexer->line, lexer->column - (text ? strlen(text) : 0));
            shared_free_safe(clean_text, "core", "unknown_function", 302);
        } else {
            // Fallback to original text if memory allocation fails
            lexer_add_token(lexer, TOKEN_NUMBER, text, lexer->line, lexer->column - (text ? strlen(text) : 0));
        }
        shared_free_safe(text, "core", "unknown_function", 307);
    }
}



/**
 * @brief Parse a string literal
 * 
 * @param lexer The lexer to parse the string in
 */
static void lexer_parse_string(Lexer* lexer) {
    char quote_char = lexer_current_char(lexer);
    lexer_advance(lexer);  // Consume the opening quote
    
    // Build the string content with proper escape sequence processing
    char* result = shared_malloc_safe(1024, "core", "unknown_function", 323);  // Start with reasonable buffer size
    int result_len = 0;
    int result_capacity = 1024;
    
    while (!lexer_is_at_end(lexer) && lexer_current_char(lexer) != quote_char) {
        if (lexer_current_char(lexer) == '\n') {
            // String spans multiple lines - this is an error
            lexer_add_token(lexer, TOKEN_ERROR, "Unterminated string", lexer->line, lexer->column);
            shared_free_safe(result, "core", "unknown_function", 331);
            return;
        }
        
        // Handle escape sequences
        if (lexer_current_char(lexer) == '\\' && !lexer_is_at_end(lexer)) {
            lexer_advance(lexer);  // Skip the backslash
            if (!lexer_is_at_end(lexer)) {
                char escaped_char = lexer_current_char(lexer);
                char actual_char;
                
                switch (escaped_char) {
                    case 'n': actual_char = '\n'; break;
                    case 't': actual_char = '\t'; break;
                    case 'r': actual_char = '\r'; break;
                    case '\\': actual_char = '\\'; break;
                    case '"': actual_char = '"'; break;
                    default: actual_char = escaped_char; break;
                }
                
                // Add the actual character to result
                if (result_len >= result_capacity - 1) {
                    result_capacity *= 2;
                    result = shared_realloc_safe(result, result_capacity, "core", "unknown_function", 354);
                }
                result[result_len++] = actual_char;
                lexer_advance(lexer);
            }
        } else {
            // Add regular character to result
            if (result_len >= result_capacity - 1) {
                result_capacity *= 2;
                result = shared_realloc_safe(result, result_capacity, "core", "unknown_function", 363);
            }
            result[result_len++] = lexer_current_char(lexer);
            lexer_advance(lexer);
        }
    }
    
    if (lexer_is_at_end(lexer)) {
        // String was not terminated
        lexer_add_token(lexer, TOKEN_ERROR, "Unterminated string", lexer->line, lexer->column);
        shared_free_safe(result, "core", "unknown_function", 373);
        return;
    }
    
    // Null terminate the result
    result[result_len] = '\0';
    lexer_add_token(lexer, TOKEN_STRING, result, lexer->line, lexer->column - result_len - 1);
    shared_free_safe(result, "core", "unknown_function", 380);
    
    lexer_advance(lexer);  // Consume the closing quote
}

/**
 * @brief Parse an identifier or keyword
 * 
 * @param lexer The lexer to parse the identifier in
 */
static void lexer_parse_identifier(Lexer* lexer) {
    // Consume alphanumeric characters and underscores
    while (!lexer_is_at_end(lexer) && 
           (isalnum(lexer_current_char(lexer)) || lexer_current_char(lexer) == '_')) {
        lexer_advance(lexer);
    }
    
    // Extract the identifier text
    char* text = lexer_extract_text(lexer);
    if (text) {
        // Check if it's a keyword
        TokenType type = TOKEN_IDENTIFIER;
        
        if (strcmp(text, "if") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "else") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "while") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "for") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "in") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "func") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "function") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "class") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "self") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "extends") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "super") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "return") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "let") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "const") == 0) type = TOKEN_CONST;
        else if (strcmp(text, "macro") == 0) type = TOKEN_MACRO;
        else if (strcmp(text, "template") == 0) type = TOKEN_TEMPLATE;
        else if (strcmp(text, "expand") == 0) type = TOKEN_EXPAND;
        else if (strcmp(text, "comptime") == 0) type = TOKEN_COMPTIME;
        else if (strcmp(text, "async") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "await") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "True") == 0 || strcmp(text, "true") == 0) type = TOKEN_BOOL;
        else if (strcmp(text, "False") == 0 || strcmp(text, "false") == 0) type = TOKEN_BOOL;
        else if (strcmp(text, "Null") == 0 || strcmp(text, "null") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "and") == 0) type = TOKEN_AND;
        else if (strcmp(text, "or") == 0) type = TOKEN_OR;
        else if (strcmp(text, "not") == 0) type = TOKEN_NOT;
        else if (strcmp(text, "match") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "case") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "default") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "try") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "catch") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "throw") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "root") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "end") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "use") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "from") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "as") == 0) type = TOKEN_KEYWORD;
        else if (strcmp(text, "public") == 0) type = TOKEN_KEYWORD;
        
        lexer_add_token(lexer, type, text, lexer->line, lexer->column - (text ? strlen(text) : 0));
        shared_free_safe(text, "core", "unknown_function", 439);
    }
}

/**
 * @brief Scan a single token from the source code
 * 
 * @param lexer The lexer to scan with
 * @return 1 if a token was successfully scanned, 0 if at end or error
 */
static int lexer_scan_token(Lexer* lexer) {
    lexer->start = lexer->current;
    
    // Skip whitespace and comments in a loop to handle consecutive comments
    while (!lexer_is_at_end(lexer)) {
        int initial_pos = lexer->current;
        lexer_skip_whitespace(lexer);
        lexer_skip_comments(lexer);
        // If position didn't change, we're done skipping
        if (lexer->current == initial_pos) {
            break;
        }
    }
    
    // Update start position after skipping
    lexer->start = lexer->current;
    
    if (lexer_is_at_end(lexer)) {
        return 0;  // No more tokens
    }
    
    char c = lexer_current_char(lexer);
    
    // Handle single-character tokens
    switch (c) {
        case '(':
            lexer_advance(lexer);
            lexer_add_token(lexer, TOKEN_LEFT_PAREN, "(", lexer->line, lexer->column - 1);
            break;
            
        case ')':
            lexer_advance(lexer);
            lexer_add_token(lexer, TOKEN_RIGHT_PAREN, ")", lexer->line, lexer->column - 1);
            break;
            
        case '{':
            lexer_advance(lexer);
            lexer_add_token(lexer, TOKEN_LEFT_BRACE, "{", lexer->line, lexer->column - 1);
            break;
            
        case '}':
            lexer_advance(lexer);
            lexer_add_token(lexer, TOKEN_RIGHT_BRACE, "}", lexer->line, lexer->column - 1);
            break;
            
        case '[':
            lexer_advance(lexer);
            lexer_add_token(lexer, TOKEN_LEFT_BRACKET, "[", lexer->line, lexer->column - 1);
            break;
            
        case ']':
            lexer_advance(lexer);
            lexer_add_token(lexer, TOKEN_RIGHT_BRACKET, "]", lexer->line, lexer->column - 1);
            break;
            
        case ';':
            lexer_advance(lexer);
            lexer_add_token(lexer, TOKEN_SEMICOLON, ";", lexer->line, lexer->column - 1);
            break;
            
        case ':':
            lexer_advance(lexer);
            lexer_add_token(lexer, TOKEN_COLON, ":", lexer->line, lexer->column - 1);
            break;
            
        case ',':
            lexer_advance(lexer);
            lexer_add_token(lexer, TOKEN_COMMA, ",", lexer->line, lexer->column - 1);
            break;
            

            
        case '.':
            if (lexer_next_char(lexer) == '.') {
                lexer_advance(lexer);
                lexer_advance(lexer);
                lexer_add_token(lexer, TOKEN_DOT_DOT, "..", lexer->line, lexer->column - 2);
            } else {
                lexer_advance(lexer);
                lexer_add_token(lexer, TOKEN_DOT, ".", lexer->line, lexer->column - 1);
            }
            break;
            
        case '+':
            lexer_advance(lexer);
            if (lexer_match(lexer, '=')) {
                lexer_add_token(lexer, TOKEN_PLUS_ASSIGN, "+=", lexer->line, lexer->column - 2);
            } else {
                lexer_add_token(lexer, TOKEN_PLUS, "+", lexer->line, lexer->column - 1);
            }
            break;
            
        case '-':
            lexer_advance(lexer);
            if (lexer_match(lexer, '>')) {
                lexer_add_token(lexer, TOKEN_RETURN_ARROW, "->", lexer->line, lexer->column - 2);
            } else if (lexer_match(lexer, '=')) {
                lexer_add_token(lexer, TOKEN_MINUS_ASSIGN, "-=", lexer->line, lexer->column - 2);
            } else {
                lexer_add_token(lexer, TOKEN_MINUS, "-", lexer->line, lexer->column - 1);
            }
            break;
            
        case '*':
            lexer_advance(lexer);
            if (lexer_match(lexer, '*')) {
                lexer_add_token(lexer, TOKEN_POWER, "**", lexer->line, lexer->column - 2);
            } else if (lexer_match(lexer, '=')) {
                lexer_add_token(lexer, TOKEN_MULTIPLY_ASSIGN, "*=", lexer->line, lexer->column - 2);
            } else {
                lexer_add_token(lexer, TOKEN_MULTIPLY, "*", lexer->line, lexer->column - 1);
            }
            break;
            
        case '/':
            lexer_advance(lexer);
            if (lexer_match(lexer, '=')) {
                lexer_add_token(lexer, TOKEN_DIVIDE_ASSIGN, "/=", lexer->line, lexer->column - 2);
            } else {
                lexer_add_token(lexer, TOKEN_DIVIDE, "/", lexer->line, lexer->column - 1);
            }
            break;
            
        case '%':
            lexer_advance(lexer);
            lexer_add_token(lexer, TOKEN_MODULO, "%", lexer->line, lexer->column - 1);
            break;
            
        case '=':
            lexer_advance(lexer);
            if (lexer_match(lexer, '=')) {
                lexer_add_token(lexer, TOKEN_EQUAL, "==", lexer->line, lexer->column - 2);
            } else if (lexer_match(lexer, '>')) {
                lexer_add_token(lexer, TOKEN_ARROW, "=>", lexer->line, lexer->column - 2);
            } else {
                lexer_add_token(lexer, TOKEN_ASSIGN, "=", lexer->line, lexer->column - 1);
            }
            break;
            
        case '!':
            lexer_advance(lexer);
            if (lexer_match(lexer, '=')) {
                lexer_add_token(lexer, TOKEN_NOT_EQUAL, "!=", lexer->line, lexer->column - 2);
            } else {
                lexer_add_token(lexer, TOKEN_NOT, "!", lexer->line, lexer->column - 1);
            }
            break;
            
        case '<':
            lexer_advance(lexer);
            if (lexer_match(lexer, '=')) {
                lexer_add_token(lexer, TOKEN_LESS_EQUAL, "<=", lexer->line, lexer->column - 2);
            } else {
                lexer_add_token(lexer, TOKEN_LESS, "<", lexer->line, lexer->column - 1);
            }
            break;
            
        case '>':
            lexer_advance(lexer);
            if (lexer_match(lexer, '=')) {
                lexer_add_token(lexer, TOKEN_GREATER_EQUAL, ">=", lexer->line, lexer->column - 2);
            } else {
                lexer_add_token(lexer, TOKEN_GREATER, ">", lexer->line, lexer->column - 1);
            }
            break;
            
        case '"':
        case '\'':
            lexer_parse_string(lexer);
            break;
            
        case '|':
            lexer_add_token(lexer, TOKEN_PIPE, "|", lexer->line, lexer->column);
            lexer_advance(lexer);
            break;
            
        case '?':
            lexer_add_token(lexer, TOKEN_QUESTION, "?", lexer->line, lexer->column);
            lexer_advance(lexer);
            break;
            
        default:
            if (isdigit(c)) {
                lexer_parse_number(lexer);
            } else if (isalpha(c) || c == '_') {
                lexer_parse_identifier(lexer);
            } else {
                // Unknown character
                char error_msg[64];
                snprintf(error_msg, sizeof(error_msg), "Unknown character '%c'", c);
                lexer_add_token(lexer, TOKEN_ERROR, error_msg, lexer->line, lexer->column);
                lexer_advance(lexer);
            }
            break;
    }
    
    return 1;
}

/**
 * @brief Scan all tokens from the source code
 * 
 * This is the main function that processes the entire source code and
 * produces a complete list of tokens. It handles all the complexity
 * of recognizing different token types and managing the token array.
 * 
 * @param lexer The lexer to use for scanning
 * @return Number of tokens found, or -1 if an error occurred
 * 
 * After calling this function, you can access the tokens through
 * lexer->tokens and lexer->token_count. The tokens are owned by
 * the lexer and will be freed when lexer_free() is called.
 */
int lexer_scan_all(Lexer* lexer) {
    if (!lexer || !lexer->source) {
        return -1;  // Invalid lexer or source
    }
    
    // Reset lexer state for fresh scanning
    lexer->start = 0;
    lexer->current = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->token_count = 0;
    
    // Scan tokens until we reach the end
    while (lexer_scan_token(lexer)) {
        // Continue scanning...
    }
    
    // Add EOF token
    lexer_add_token(lexer, TOKEN_EOF, "EOF", lexer->line, lexer->column);
    
    return lexer->token_count;
}

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
Token* lexer_scan_next(Lexer* lexer) {
    if (!lexer || !lexer->source) {
        return NULL;
    }
    
    // Scan a single token
    if (lexer_scan_token(lexer)) {
        // Return the last token that was added
        if (lexer->token_count > 0) {
            return &lexer->tokens[lexer->token_count - 1];
        }
    }
    
    return NULL;
}

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
Token* lexer_get_token(Lexer* lexer, int index) {
    if (!lexer || !lexer->tokens || index < 0 || index >= lexer->token_count) {
        return NULL;  // Invalid parameters or out of bounds
    }
    
    return &lexer->tokens[index];
}

/**
 * @brief Print all tokens for debugging purposes
 * 
 * This function outputs all tokens in a human-readable format. It's very useful
 * for debugging lexer issues or understanding how source code is being tokenized.
 * 
 * @param lexer The lexer containing the tokens to print
 */
void lexer_print_tokens(Lexer* lexer) {
    if (!lexer || !lexer->tokens) {
        printf("No tokens to print\n");
        return;
    }
    
    printf("Tokens found: %d\n", lexer->token_count);
    for (int i = 0; i < lexer->token_count; i++) {
        Token* token = &lexer->tokens[i];
        printf("%d: ", i + 1);
    
    switch (token->type) {
            case TOKEN_EOF:
                printf("End of File");
                break;
        case TOKEN_NUMBER:
                printf("Number(%.6f)", token->data.number_value);
                break;
            case TOKEN_STRING:
                printf("String(\"%s\")", token->data.string_value ? token->data.string_value : "");
                break;
            case TOKEN_BOOL:
                printf("Boolean(%s)", token->data.bool_value ? "True" : "False");
                break;
            case TOKEN_IDENTIFIER:
                printf("Identifier(\"%s\")", token->text ? token->text : "");
                break;
            case TOKEN_KEYWORD:
                printf("Keyword(\"%s\")", token->text ? token->text : "");
            break;
        case TOKEN_PLUS:
            printf("Plus");
            break;
        case TOKEN_MINUS:
            printf("Minus");
            break;
        case TOKEN_MULTIPLY:
            printf("Multiply");
            break;
        case TOKEN_DIVIDE:
            printf("Divide");
            break;
            case TOKEN_MODULO:
                printf("Modulo");
                break;
            case TOKEN_POWER:
                printf("Power");
                break;
            case TOKEN_EQUAL:
                printf("Equal");
                break;
            case TOKEN_NOT_EQUAL:
                printf("Not Equal");
                break;
            case TOKEN_LESS:
                printf("Less Than");
                break;
            case TOKEN_GREATER:
                printf("Greater Than");
                break;
            case TOKEN_LESS_EQUAL:
                printf("Less Equal");
                break;
            case TOKEN_GREATER_EQUAL:
                printf("Greater Equal");
                break;
            case TOKEN_AND:
                printf("Logical AND");
                break;
            case TOKEN_OR:
                printf("Logical OR");
                break;
            case TOKEN_NOT:
                printf("Logical NOT");
                break;
            case TOKEN_ASSIGN:
                printf("Assign");
                break;
            case TOKEN_PLUS_ASSIGN:
                printf("Plus Assign");
                break;
            case TOKEN_MINUS_ASSIGN:
                printf("Minus Assign");
                break;
            case TOKEN_MULTIPLY_ASSIGN:
                printf("Multiply Assign");
                break;
            case TOKEN_DIVIDE_ASSIGN:
                printf("Divide Assign");
                break;
            case TOKEN_LEFT_PAREN:
                printf("Left Parenthesis");
                break;
            case TOKEN_RIGHT_PAREN:
                printf("Right Parenthesis");
                break;
            case TOKEN_LEFT_BRACE:
                printf("Left Brace");
                break;
            case TOKEN_RIGHT_BRACE:
                printf("Right Brace");
                break;
            case TOKEN_LEFT_BRACKET:
                printf("Left Bracket");
                break;
            case TOKEN_RIGHT_BRACKET:
                printf("Right Bracket");
                break;
            case TOKEN_SEMICOLON:
                printf("Semicolon");
                break;
            case TOKEN_COLON:
                printf("Colon");
                break;
            case TOKEN_COMMA:
                printf("Comma");
                break;
            case TOKEN_DOT:
                printf("Dot");
                break;
            case TOKEN_DOT_DOT:
                printf("Range(..)");
                break;
            case TOKEN_ARROW:
                printf("Arrow(=>)");
                break;
            case TOKEN_ERROR:
                printf("Error(\"%s\")", token->text ? token->text : "");
            break;
        default:
            printf("Unknown");
            break;
    }
        
        printf(" (Line %d, Column %d)\n", token->line, token->column);
    }
}

/**
 * @brief Check if the lexer encountered any errors during scanning
 * 
 * This function returns whether any TOKEN_ERROR tokens were produced during
 * tokenization. It's useful for determining if the source code is valid.
 * 
 * @param lexer The lexer to check
 * @return 1 if errors occurred, 0 if no errors
 */
int lexer_has_errors(Lexer* lexer) {
    if (!lexer || !lexer->tokens) {
        return 0;  // No tokens to check
    }
    
    // Check if any tokens have the ERROR type
    for (int i = 0; i < lexer->token_count; i++) {
        if (lexer->tokens[i].type == TOKEN_ERROR) {
            return 1;  // Found an error token
        }
    }
    
    return 0;  // No errors found
}

/**
 * @brief Get the total number of tokens found
 * 
 * This function returns the count of tokens that have been successfully
 * scanned from the source code.
 * 
 * @param lexer The lexer to use
 * @return Number of tokens found
 */
int lexer_get_token_count(Lexer* lexer) {
    if (!lexer) {
        return 0;
    }
    
    return lexer->token_count;
}