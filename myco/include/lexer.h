#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_EOF,
    TOKEN_NUMBER,
    TOKEN_FLOAT,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,
    TOKEN_OPERATOR,
    TOKEN_ASSIGN,
    TOKEN_QUESTION,
    TOKEN_COLON,
    TOKEN_SEMICOLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_COMMA,
    TOKEN_FUNC,
    TOKEN_LET,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_FOR,
    TOKEN_WHILE,
    TOKEN_END,
    TOKEN_RETURN,
    TOKEN_SWITCH,
    TOKEN_CASE,
    TOKEN_DEFAULT,
    TOKEN_TRY,
    TOKEN_CATCH,
    TOKEN_PRINT,
    TOKEN_UPRINT,
    TOKEN_TYPE_MARKER,
    TOKEN_STRING_TYPE,
    TOKEN_IN,
    TOKEN_USE,
    TOKEN_AS,
    TOKEN_PATH,
    TOKEN_DOT,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LAMBDA,
    TOKEN_ARROW,
    TOKEN_TRUE,
    TOKEN_FALSE,
    
    // NEW: v2.0 Type System Tokens
    TOKEN_TYPE_INT,           // int
    TOKEN_TYPE_FLOAT,         // float  
    TOKEN_TYPE_BOOL,          // bool
    TOKEN_TYPE_STRING,        // string
    TOKEN_TYPE_ARRAY,         // array
    TOKEN_TYPE_TUPLE,         // tuple
    TOKEN_TYPE_DICT,          // dict
    TOKEN_TYPE_SET,           // set
    TOKEN_TYPE_NULL,          // null
    TOKEN_TYPE_ANY,           // any
    TOKEN_TYPE_FUNCTION,      // function
    TOKEN_TYPE_OBJECT,        // object
    TOKEN_TYPE_CLASS,         // class
    TOKEN_TYPE_ENUM,          // enum
    TOKEN_TYPE_BYTE,          // byte
    TOKEN_TYPE_BYTES,         // bytes
    
    // NEW: Generic Type Tokens
    TOKEN_LT,                 // <
    TOKEN_GT,                 // >
    TOKEN_UNION,              // |
    TOKEN_INTERSECTION,       // &
    TOKEN_OPTIONAL,           // ?
    TOKEN_DOT_DOT,            // ..
} MycoTokenType;

typedef struct {
    MycoTokenType type;
    char* text;
    int line;
} Token;

// Function prototypes
Token* lexer_tokenize(const char* source);
void lexer_free_tokens(Token* tokens);

#endif // LEXER_H 