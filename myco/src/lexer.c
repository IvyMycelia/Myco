/**
 * @file lexer.c
 * @brief Myco Language Lexer - Tokenizes source code into tokens
 * @version 1.0.0
 * @author Myco Development Team
 * 
 * This file implements the lexical analysis phase of the Myco interpreter.
 * It converts raw source code into a stream of tokens that the parser can process.
 * 
 * Key Features:
 * - Supports all Myco language tokens (keywords, operators, identifiers, etc.)
 * - Handles comments (# single-line, multi-line comments)
 * - Processes strings, numbers, and paths
 * - Maintains line number tracking for error reporting
 * - Memory-efficient token generation
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"
#include "memory_tracker.h"

/*******************************************************************************
 * KEYWORD IDENTIFICATION
 ******************************************************************************/

/**
 * @brief Determines if an identifier is a Myco language keyword
 * @param text The identifier text to check
 * @return The appropriate token type (keyword or identifier)
 * 
 * This function checks if a given identifier matches any of the Myco language
 * keywords and returns the appropriate token type. If no keyword match is found,
 * it returns TOKEN_IDENTIFIER.
 */
static MycoTokenType get_keyword_type(const char* text) {
    if (strcmp(text, "func") == 0) return TOKEN_FUNC;
    if (strcmp(text, "let") == 0) return TOKEN_LET;
    if (strcmp(text, "if") == 0) return TOKEN_IF;
    if (strcmp(text, "else") == 0) return TOKEN_ELSE;
    if (strcmp(text, "for") == 0) return TOKEN_FOR;
    if (strcmp(text, "while") == 0) return TOKEN_WHILE;
    if (strcmp(text, "end") == 0) return TOKEN_END;
    if (strcmp(text, "return") == 0) return TOKEN_RETURN;
    if (strcmp(text, "switch") == 0) return TOKEN_SWITCH;
    if (strcmp(text, "case") == 0) return TOKEN_CASE;
    if (strcmp(text, "default") == 0) return TOKEN_DEFAULT;
    if (strcmp(text, "try") == 0) return TOKEN_TRY;
    if (strcmp(text, "catch") == 0) return TOKEN_CATCH;
    if (strcmp(text, "print") == 0) return TOKEN_PRINT;
    if (strcmp(text, "uprint") == 0) return TOKEN_UPRINT;
    if (strcmp(text, "in") == 0) return TOKEN_IN;
    if (strcmp(text, "use") == 0) return TOKEN_USE;
    if (strcmp(text, "as") == 0) return TOKEN_AS;
    
    // NEW: v2.0 Type System Keywords
    if (strcmp(text, "int") == 0) return TOKEN_TYPE_INT;
    if (strcmp(text, "float") == 0) return TOKEN_TYPE_FLOAT;
    if (strcmp(text, "bool") == 0) return TOKEN_TYPE_BOOL;
    if (strcmp(text, "string") == 0) return TOKEN_TYPE_STRING;
    if (strcmp(text, "array") == 0) return TOKEN_TYPE_ARRAY;
    if (strcmp(text, "tuple") == 0) return TOKEN_TYPE_TUPLE;
    if (strcmp(text, "dict") == 0) return TOKEN_TYPE_DICT;
    if (strcmp(text, "set") == 0) return TOKEN_TYPE_SET;
    if (strcmp(text, "null") == 0) return TOKEN_TYPE_NULL;
    if (strcmp(text, "any") == 0) return TOKEN_TYPE_ANY;
    if (strcmp(text, "function") == 0) return TOKEN_TYPE_FUNCTION;
    if (strcmp(text, "object") == 0) return TOKEN_TYPE_OBJECT;
    if (strcmp(text, "class") == 0) return TOKEN_TYPE_CLASS;
    if (strcmp(text, "enum") == 0) return TOKEN_TYPE_ENUM;
    if (strcmp(text, "byte") == 0) return TOKEN_TYPE_BYTE;
    if (strcmp(text, "bytes") == 0) return TOKEN_TYPE_BYTES;
    
    // Legacy type support (for backward compatibility)
    if (strcmp(text, "str") == 0) return TOKEN_STRING_TYPE;
    if (strcmp(text, "obj") == 0) return TOKEN_TYPE_OBJECT;
    
    return TOKEN_IDENTIFIER;
}

/*******************************************************************************
 * CONSTANTS AND CONFIGURATION
 ******************************************************************************/

#define INITIAL_TOKEN_CAPACITY 1000  // Initial capacity for tokens

/*******************************************************************************
 * MAIN LEXER FUNCTION
 ******************************************************************************/

/**
 * @brief Tokenizes Myco source code into a stream of tokens
 * @param source The source code string to tokenize
 * @return Array of tokens, or NULL on failure
 * 
 * This is the main entry point for lexical analysis. It processes the source
 * code character by character, identifying tokens and building the token stream.
 * 
 * Token Types Handled:
 * - Keywords (func, let, if, etc.)
 * - Identifiers (variable names, function names)
 * - Literals (strings, numbers)
 * - Operators (+, -, *, /, ==, !=, etc.)
 * - Delimiters (parentheses, braces, semicolons)
 * - Comments (# and multi-line)
 * - Paths (./relative/path)
 * 
 * Memory Management:
 * - Allocates token array dynamically
 * - Each token's text is allocated separately
 * - Caller is responsible for freeing tokens with lexer_free_tokens()
 */
Token* lexer_tokenize(const char* source) {
    int capacity = INITIAL_TOKEN_CAPACITY;
    Token* tokens = (Token*)tracked_malloc(capacity * sizeof(Token), __FILE__, __LINE__, "lexer_tokenize");
    if (!tokens) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }

    int token_count = 0;
    int line = 1;
    const char* p = source;

    // Helper function to grow token array
    #define GROW_TOKENS_IF_NEEDED() do { \
        if (token_count >= capacity) { \
            capacity *= 2; \
            Token* new_tokens = (Token*)tracked_realloc(tokens, capacity * sizeof(Token), __FILE__, __LINE__, "lexer_grow_tokens"); \
            if (!new_tokens) { \
                fprintf(stderr, "Error: Memory reallocation failed\n"); \
                for (int i = 0; i < token_count; i++) { \
                    if (tokens[i].text) tracked_free(tokens[i].text, __FILE__, __LINE__, "lexer_error_cleanup"); \
                } \
                tracked_free(tokens, __FILE__, __LINE__, "lexer_error_cleanup"); \
                return NULL; \
            } \
            tokens = new_tokens; \
        } \
    } while(0)

    while (*p) {
        // Skip whitespace
        if (isspace(*p)) {
            if (*p == '\n') line++;
            p++;
            continue;
        }

        // Skip single-line comments (starting with #)
        if (*p == '#') {
            while (*p && *p != '\n') p++;
            continue;
        }

        // Skip multi-line comments (enclosed in /* and */)
        if (*p == '/' && *(p + 1) == '*') {
            p += 2; // Skip /*
            while (*p && !(*p == '*' && *(p + 1) == '/')) {
                if (*p == '\n') line++;
                p++;
            }
            if (*p) p += 2; // Skip */
            continue;
        }

        // PATH starting with ./
        if (*p == '.' && *(p + 1) == '/') {
            GROW_TOKENS_IF_NEEDED();
            const char* start = p;
            p += 2;
            while (*p && !isspace(*p)) p++;
            int len = p - start;
            char* text = (char*)tracked_malloc(len + 1, __FILE__, __LINE__, "lexer_path_token");
            strncpy(text, start, len); text[len] = '\0';
            tokens[token_count].type = TOKEN_PATH;
            tokens[token_count].text = text;
            tokens[token_count].line = line;
            token_count++;
            continue;
        }

        // PATH starting with identifier followed by / (relative path)
        if (isalpha(*p) || *p == '_') {
            const char* start = p;
            // Check if this looks like a path (identifier followed by /)
            while (isalnum(*p) || *p == '_') p++;
            if (*p == '/') {
                // This is a path, consume the entire path
                while (*p && !isspace(*p)) p++;
                int len = p - start;
                char* text = (char*)tracked_malloc(len + 1, __FILE__, __LINE__, "lexer_path_token");
                strncpy(text, start, len); text[len] = '\0';
                
                GROW_TOKENS_IF_NEEDED();
                tokens[token_count].type = TOKEN_PATH;
                tokens[token_count].text = text;
                tokens[token_count].line = line;
                token_count++;
                continue;
            } else {
                // Not a path, reset to start and handle as regular identifier
                p = start;
            }
        }

        // Dot: member access or float literal starting with decimal point
        if (*p == '.' && *(p + 1) != '/' && *(p + 1) != '.') {
            // Check if this is a float literal starting with decimal point
            if (isdigit(*(p + 1))) {
                GROW_TOKENS_IF_NEEDED();
                const char* start = p;
                p++; // Skip decimal point
                // Parse fractional part
                while (isdigit(*p)) p++;
                
                int len = p - start;
                char* text = (char*)tracked_malloc(len + 1, __FILE__, __LINE__, "lexer_float_token");
                strncpy(text, start, len);
                text[len] = '\0';
                
                tokens[token_count].type = TOKEN_FLOAT;
                tokens[token_count].text = text;
                tokens[token_count].line = line;
                token_count++;
                continue;
            } else {
                // Check for dot-dot operator (range operator)
                if (*(p + 1) == '.') {
                    // NEW: v2.0 Range Operator
                    tokens[token_count].type = TOKEN_DOT_DOT;
                    tokens[token_count].text = tracked_strdup("..", __FILE__, __LINE__, "lexer_dot_dot");
                    p += 2; // Skip both dots
                    tokens[token_count].line = line;
                    token_count++;
                    continue;
                } else {
                    // Regular dot for member access
                    tokens[token_count].type = TOKEN_DOT;
                    tokens[token_count].text = tracked_strdup(".", __FILE__, __LINE__, "lexer_dot");
                    tokens[token_count].line = line;
                    token_count++;
                    p++;
                    continue;
                }
            }
        }

        // Multi-character operators (must come before identifier detection)
        if (strncmp(p, "and", 3) == 0 && (p[3] == ' ' || p[3] == '\t' || p[3] == '\n' || p[3] == '\r' || p[3] == '\0')) {
            GROW_TOKENS_IF_NEEDED();
            tokens[token_count].type = TOKEN_OPERATOR;
            tokens[token_count].text = tracked_strdup("and", __FILE__, __LINE__, "lexer_and");
            p += 3; // Skip 'and' completely, no need for loop increment
            tokens[token_count].line = line;
            token_count++;
            continue;
        } else if (strncmp(p, "or", 2) == 0 && (p[2] == ' ' || p[2] == '\t' || p[2] == '\n' || p[2] == '\r' || p[2] == '\0')) {
            GROW_TOKENS_IF_NEEDED();
            tokens[token_count].type = TOKEN_OPERATOR;
            tokens[token_count].text = tracked_strdup("or", __FILE__, __LINE__, "lexer_or");
            p += 2; // Skip 'or' completely, no need for loop increment
            tokens[token_count].line = line;
            token_count++;
            continue;
        }

        // Keywords and identifiers
        if (isalpha(*p) || *p == '_') {
            GROW_TOKENS_IF_NEEDED();
            const char* start = p;
            while (isalnum(*p) || *p == '_') p++;
            int len = p - start;
            char* text = (char*)tracked_malloc(len + 1, __FILE__, __LINE__, "lexer_identifier");
            strncpy(text, start, len);
            text[len] = '\0';
            


            // Use the centralized keyword recognition function
            tokens[token_count].type = get_keyword_type(text);

            tokens[token_count].text = text;
            tokens[token_count].line = line;
            token_count++;
            continue;
        }

        // Numbers (integers and floats)
        if (isdigit(*p)) {
            GROW_TOKENS_IF_NEEDED();
            const char* start = p;
            int has_decimal = 0;
            
            // Parse integer part
            while (isdigit(*p)) p++;
            
            // Check for decimal point
            if (*p == '.') {
                has_decimal = 1;
                p++; // Skip decimal point
                // Parse fractional part
                while (isdigit(*p)) p++;
            }
            
            int len = p - start;
            char* text = (char*)tracked_malloc(len + 1, __FILE__, __LINE__, "lexer_number");
            strncpy(text, start, len);
            text[len] = '\0';
            
            tokens[token_count].type = has_decimal ? TOKEN_FLOAT : TOKEN_NUMBER;
            tokens[token_count].text = text;
            tokens[token_count].line = line;
            token_count++;
            continue;
        }

        // Strings with simple escapes (\n, \t, \", \\) enclosed in double quotes
        if (*p == '"') {
            GROW_TOKENS_IF_NEEDED();
            p++; // skip opening quote
            char* buf = NULL; int cap = 0; int len = 0;
            while (*p) {
                if (*p == '"') { // closing quote
                    p++;
                    break;
                }
                char ch = *p++;
                if (ch == '\\' && *p) {
                    char esc = *p++;
                    switch (esc) {
                        case 'n': ch = '\n'; break;
                        case 't': ch = '\t'; break;
                        case '\\': ch = '\\'; break;
                        case '"': ch = '"'; break;
                        default: ch = esc; break;
                    }
                }
                if (len + 1 >= cap) { cap = cap ? cap * 2 : 32; buf = (char*)tracked_realloc(buf, cap, __FILE__, __LINE__, "lexer_string_grow"); }
                buf[len++] = ch;
            }
            if (!buf) { buf = (char*)tracked_malloc(1, __FILE__, __LINE__, "lexer_init"); buf[0] = '\0'; }
            if (*(p-1) != '"') { // no closing quote found
                fprintf(stderr, "Error: Unterminated string at line %d\n", line);
                tracked_free(tokens, __FILE__, __LINE__, "lexer_string_error");
                tracked_free(buf, __FILE__, __LINE__, "lexer_string_error");
                return NULL;
            }
            if (len + 1 >= cap) { cap = len + 1; buf = (char*)tracked_realloc(buf, cap, __FILE__, __LINE__, "lexer_string_final"); }
            buf[len] = '\0';
            tokens[token_count].type = TOKEN_STRING;
            tokens[token_count].text = buf;
            tokens[token_count].line = line;
            token_count++;
            continue;
        }

        // Operators and symbols
        // Check for multi-character operators first
        GROW_TOKENS_IF_NEEDED();
        switch (*p) {
            case '+': tokens[token_count].type = TOKEN_OPERATOR; tokens[token_count].text = tracked_strdup("+", __FILE__, __LINE__, "lexer_plus"); break;
            case '*': tokens[token_count].type = TOKEN_OPERATOR; tokens[token_count].text = tracked_strdup("*", __FILE__, __LINE__, "lexer_mult"); break;
            case '/': tokens[token_count].type = TOKEN_OPERATOR; tokens[token_count].text = tracked_strdup("/", __FILE__, __LINE__, "lexer_div"); break;
            case '%': tokens[token_count].type = TOKEN_OPERATOR; tokens[token_count].text = tracked_strdup("%", __FILE__, __LINE__, "lexer_mod"); break;
            case '?': tokens[token_count].type = TOKEN_QUESTION; tokens[token_count].text = tracked_strdup("?", __FILE__, __LINE__, "lexer_question"); break;
            case ':': tokens[token_count].type = TOKEN_COLON; tokens[token_count].text = tracked_strdup(":", __FILE__, __LINE__, "lexer_colon"); break;
            case ';': tokens[token_count].type = TOKEN_SEMICOLON; tokens[token_count].text = tracked_strdup(";", __FILE__, __LINE__, "lexer_semicolon"); break;
            case '(': tokens[token_count].type = TOKEN_LPAREN; tokens[token_count].text = tracked_strdup("(", __FILE__, __LINE__, "lexer_lparen"); break;
            case ')': tokens[token_count].type = TOKEN_RPAREN; tokens[token_count].text = tracked_strdup(")", __FILE__, __LINE__, "lexer_rparen"); break;
            case ',': tokens[token_count].type = TOKEN_COMMA; tokens[token_count].text = tracked_strdup(",", __FILE__, __LINE__, "lexer_comma"); break;
                    case '[': tokens[token_count].type = TOKEN_LBRACKET; tokens[token_count].text = tracked_strdup("[", __FILE__, __LINE__, "lexer_lbracket"); break;
        case ']': tokens[token_count].type = TOKEN_RBRACKET; tokens[token_count].text = tracked_strdup("]", __FILE__, __LINE__, "lexer_rbracket"); break;
        case '{': tokens[token_count].type = TOKEN_LBRACE; tokens[token_count].text = tracked_strdup("{", __FILE__, __LINE__, "lexer_lbrace"); break;
        case '}': tokens[token_count].type = TOKEN_RBRACE; tokens[token_count].text = tracked_strdup("}", __FILE__, __LINE__, "lexer_rbrace"); break;
            case '<':
                if (*(p + 1) == '=') {
                    tokens[token_count].type = TOKEN_OPERATOR;
                    tokens[token_count].text = tracked_strdup("<=", __FILE__, __LINE__, "lexer_operator");
                    p++;
                } else {
                    tokens[token_count].type = TOKEN_OPERATOR;
                    tokens[token_count].text = tracked_strdup("<", __FILE__, __LINE__, "lexer_operator");
                }
                break;
            case '>':
                if (*(p + 1) == '=') {
                    tokens[token_count].type = TOKEN_OPERATOR;
                    tokens[token_count].text = tracked_strdup(">=", __FILE__, __LINE__, "lexer_operator");
                    p++;
                } else {
                    tokens[token_count].type = TOKEN_OPERATOR;
                    tokens[token_count].text = tracked_strdup(">", __FILE__, __LINE__, "lexer_operator");
                }
                break;
            case '!':
                if (*(p + 1) == '=') {
                    tokens[token_count].type = TOKEN_OPERATOR;
                    tokens[token_count].text = tracked_strdup("!=", __FILE__, __LINE__, "lexer_operator");
                    p++;
                } else {
                    fprintf(stderr, "Error: Unknown character '%c' at line %d\n", *p, line);
                    tracked_free(tokens, __FILE__, __LINE__, "lexer_unknown_char");
                    return NULL;
                }
                break;
            case '=':
                if (*(p + 1) == '=') {
                    tokens[token_count].type = TOKEN_OPERATOR;
                    tokens[token_count].text = tracked_strdup("==", __FILE__, __LINE__, "lexer_operator");
                    p++;
                } else if (*(p + 1) == '>') {
                    tokens[token_count].type = TOKEN_LAMBDA;
                    tokens[token_count].text = tracked_strdup("=>", __FILE__, __LINE__, "lexer_lambda");
                    p++;
                } else {
                    tokens[token_count].type = TOKEN_ASSIGN;
                    tokens[token_count].text = tracked_strdup("=", __FILE__, __LINE__, "lexer_operator");
                }
                break;
            case '|':
                // NEW: v2.0 Union Type Operator
                tokens[token_count].type = TOKEN_UNION;
                tokens[token_count].text = tracked_strdup("|", __FILE__, __LINE__, "lexer_union");
                break;
            case '&':
                // NEW: v2.0 Intersection Type Operator
                tokens[token_count].type = TOKEN_INTERSECTION;
                tokens[token_count].text = tracked_strdup("&", __FILE__, __LINE__, "lexer_intersection");
                break;
            case '-':
                if (*(p + 1) == '>') {
                    tokens[token_count].type = TOKEN_ARROW;
                    tokens[token_count].text = tracked_strdup("->", __FILE__, __LINE__, "lexer_arrow");
                    p++;
                } else {
                    tokens[token_count].type = TOKEN_OPERATOR;
                    tokens[token_count].text = tracked_strdup("-", __FILE__, __LINE__, "lexer_minus");
                }
                break;
            default:
                fprintf(stderr, "Error: Unknown character '%c' at line %d\n", *p, line);
                tracked_free(tokens, __FILE__, __LINE__, "lexer_default_error");
                return NULL;
        }
        tokens[token_count].line = line;
        token_count++;
        p++;
    }

    // Add EOF token
    tokens[token_count].type = TOKEN_EOF;
    tokens[token_count].text = NULL;
    tokens[token_count].line = line;

    return tokens;
}

/*******************************************************************************
 * MEMORY MANAGEMENT
 ******************************************************************************/

/**
 * @brief Frees all memory allocated for tokens
 * @param tokens Array of tokens to free
 * 
 * This function properly cleans up all memory allocated during tokenization:
 * - Frees each token's text string
 * - Frees the token array itself
 * - Handles NULL tokens gracefully
 * 
 * Call this function when you're done processing tokens to prevent memory leaks.
 */
void lexer_free_tokens(Token* tokens) {
    if (!tokens) return;
    for (int i = 0; tokens[i].type != TOKEN_EOF; i++) {
        tracked_free(tokens[i].text, __FILE__, __LINE__, "lexer_free_tokens");
    }
    tracked_free(tokens, __FILE__, __LINE__, "lexer_free_tokens");
}
