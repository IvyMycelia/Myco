#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef enum {
    AST_FUNC, AST_LET, AST_IF, AST_FOR, AST_WHILE, AST_RETURN,
    AST_SWITCH, AST_CASE, AST_DEFAULT, AST_TRY, AST_CATCH, AST_PRINT, AST_UPRINT,
    AST_EXPR, AST_BLOCK, AST_DOT, AST_ASSIGN, AST_ARRAY_LITERAL,
    AST_ARRAY_ACCESS, AST_ARRAY_ASSIGN, AST_OBJECT_LITERAL,
    AST_OBJECT_ACCESS, AST_OBJECT_ASSIGN, AST_OBJECT_BRACKET_ACCESS,
    AST_OBJECT_BRACKET_ASSIGN, AST_LAMBDA, AST_TERNARY,
    
    // NEW: v2.0 Type System AST Nodes
    AST_TYPE_ANNOTATION,      // x: int
    AST_GENERIC_TYPE,         // array<int>
    AST_UNION_TYPE,           // int | string
    AST_INTERSECTION_TYPE,    // int & string
    AST_OPTIONAL_TYPE,        // string?
    AST_FUNCTION_TYPE,        // function<int, string>
    AST_TUPLE_TYPE,           // tuple<int, string>
    AST_DICT_TYPE,            // dict<string, int>
    AST_SET_TYPE,             // set<int>
    AST_ARRAY_TYPE,           // array<int>
    AST_OBJECT_TYPE,          // object
    AST_CLASS_TYPE,           // class
    AST_ENUM_TYPE,            // enum
    AST_BYTE_TYPE,            // byte
    AST_BYTES_TYPE,           // bytes
    AST_NULL_TYPE,            // null
    AST_ANY_TYPE,             // any
    
    // NEW: v2.0 Primitive Type AST Nodes
    AST_TYPE_INT,             // int
    AST_TYPE_FLOAT,           // float
    AST_TYPE_BOOL,            // bool
    AST_TYPE_STRING,          // string
    
    // NEW: v2.0 Collection Type AST Nodes
    AST_TYPE_ARRAY,           // array
    AST_TYPE_TUPLE,           // tuple
    AST_TYPE_DICT,            // dict
    AST_TYPE_SET,             // set
    
    // NEW: v2.0 Special Type AST Nodes
    AST_TYPE_NULL,            // null
    AST_TYPE_ANY,             // any
    AST_TYPE_FUNCTION,        // function
    AST_TYPE_OBJECT,          // object
    AST_TYPE_CLASS,           // class
    AST_TYPE_ENUM,            // enum
    AST_TYPE_BYTE,            // byte
    AST_TYPE_BYTES            // bytes
} ASTNodeType;

// Enhanced for loop types
typedef enum {
    AST_FOR_RANGE,        // for i in 1..10:
    AST_FOR_ARRAY,        // for item in array:
    AST_FOR_STEP,         // for i in 1:10:2:
    AST_FOR_DOWNTO        // for i in 10:1:-1:
} ForLoopType;

typedef struct ASTNode {
    ASTNodeType type;
    char* text;  // For identifiers, numbers, strings, etc.
    char* implicit_function;  // NEW: stores function name for implicit calls
    struct ASTNode* children;  // Array of child nodes
    int child_count;
    struct ASTNode* next;  // For linked list of statements
    int line;  // Add line number field
    
    // Enhanced for loop support
    ForLoopType for_type; // Specific for loop variant (only used when type == AST_FOR)
    
    // NEW: v2.0 Type System Support
    ASTNodeType type_annotation;  // Type annotation for this node
    char* type_text;              // Type text (e.g., "int", "string")
} ASTNode;

// Function prototypes
ASTNode* parser_parse(Token* tokens);
void parser_free_ast(ASTNode* ast);

#endif // PARSER_H 