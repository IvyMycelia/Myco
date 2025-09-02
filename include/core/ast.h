#ifndef AST_H
#define AST_H

#include <stddef.h>

// AST Node Types
typedef enum {
    AST_NODE_NUMBER,
    AST_NODE_STRING,
    AST_NODE_BOOL,
    AST_NODE_NULL,
    AST_NODE_IDENTIFIER,
    AST_NODE_BINARY_OP,
    AST_NODE_UNARY_OP,
    AST_NODE_ASSIGNMENT,
    AST_NODE_FUNCTION_CALL,
    AST_NODE_VARIABLE_DECLARATION,
    AST_NODE_IF_STATEMENT,
    AST_NODE_WHILE_LOOP,
    AST_NODE_FOR_LOOP,
    AST_NODE_BLOCK,
    AST_NODE_RETURN,
    AST_NODE_BREAK,
    AST_NODE_CONTINUE,
    AST_NODE_TRY_CATCH,
    AST_NODE_SWITCH,
    AST_NODE_MATCH,
    AST_NODE_SPORE,
    AST_NODE_SPORE_CASE,
    AST_NODE_CLASS,
    AST_NODE_FUNCTION,
    AST_NODE_ARRAY_LITERAL,
    AST_NODE_ARRAY_ACCESS,
    AST_NODE_MEMBER_ACCESS,
    AST_NODE_FUNCTION_CALL_EXPR,
    AST_NODE_IMPORT,
    AST_NODE_USE,
    AST_NODE_MODULE,
    AST_NODE_PACKAGE,
    AST_NODE_ERROR
} ASTNodeType;

// Binary Operators
typedef enum {
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_MODULO,
    OP_POWER,
    OP_EQUAL,
    OP_NOT_EQUAL,
    OP_LESS_THAN,
    OP_LESS_EQUAL,
    OP_GREATER_THAN,
    OP_GREATER_EQUAL,
    OP_LOGICAL_AND,
    OP_LOGICAL_OR,
    OP_LOGICAL_XOR,
    OP_BITWISE_AND,
    OP_BITWISE_OR,
    OP_BITWISE_XOR,
    OP_LEFT_SHIFT,
    OP_RIGHT_SHIFT,
    OP_RANGE,
    OP_RANGE_INCLUSIVE,
    OP_RANGE_STEP
} BinaryOperator;

// Unary Operators
typedef enum {
    OP_POSITIVE,
    OP_NEGATIVE,
    OP_LOGICAL_NOT,
    OP_BITWISE_NOT,
    OP_DEREFERENCE,
    OP_ADDRESS_OF
} UnaryOperator;

// AST Node Structure
typedef struct ASTNode {
    ASTNodeType type;
    union {
        // Literal values
        double number_value;
        char* string_value;
        int bool_value;
        char* identifier_value;
        
        // Operators
        struct {
            BinaryOperator op;
            struct ASTNode* left;
            struct ASTNode* right;
            struct ASTNode* step;  // For range with step (start..end..step)
        } binary;
        
        struct {
            UnaryOperator op;
            struct ASTNode* operand;
        } unary;
        
        // Assignment
        struct {
            char* variable_name;
            struct ASTNode* value;
        } assignment;
        
        // Function call
        struct {
            char* function_name;
            struct ASTNode** arguments;
            size_t argument_count;
        } function_call;
        
        // Variable declaration
        struct {
            char* variable_name;
            char* type_name;
            struct ASTNode* initial_value;
            int is_mutable;
        } variable_declaration;
        
        // Control flow
        struct {
            struct ASTNode* condition;
            struct ASTNode* then_block;
            struct ASTNode* else_block;
            struct ASTNode* else_if_chain;  // Chain of else if statements
        } if_statement;
        
        struct {
            struct ASTNode* condition;
            struct ASTNode* body;
        } while_loop;
        
        struct {
            char* iterator_name;
            struct ASTNode* collection;
            struct ASTNode* body;
        } for_loop;
        
        // Block of statements
        struct {
            struct ASTNode** statements;
            size_t statement_count;
        } block;
        
        // Return statement
        struct {
            struct ASTNode* value;
        } return_statement;
        
        // Try-catch
        struct {
            struct ASTNode* try_block;
            char* catch_variable;
            struct ASTNode* catch_block;
            struct ASTNode* finally_block;
        } try_catch;
        
        // Switch statement
        struct {
            struct ASTNode* expression;
            struct ASTNode** cases;
            size_t case_count;
            struct ASTNode* default_case;
        } switch_statement;
        
        // Match expression
        struct {
            struct ASTNode* expression;
            struct ASTNode** patterns;
            size_t pattern_count;
        } match;
        
        // Spore statement (pattern matching)
        struct {
            struct ASTNode* expression;           // Value to match against
            struct ASTNode** cases;              // Array of case nodes
            size_t case_count;                   // Number of cases
            struct ASTNode* root_case;           // Default/root case
        } spore;
        
        // Spore case (individual case in a spore statement)
        struct {
            struct ASTNode* pattern;              // Pattern to match against
            struct ASTNode* body;                 // Body to execute if matched
            int is_lambda;                        // 1 if lambda style (=>), 0 if block style (:)
        } spore_case;
        
        // Class definition
        struct {
            char* class_name;
            char* parent_class;
            struct ASTNode* body;
        } class_definition;
        
        // Function definition
        struct {
            char* function_name;
            struct ASTNode** parameters;
            size_t parameter_count;
            char* return_type;
            struct ASTNode* body;
        } function_definition;
        
        // Array literal
        struct {
            struct ASTNode** elements;
            size_t element_count;
        } array_literal;
        
        // Array access
        struct {
            struct ASTNode* array;      // The array being accessed
            struct ASTNode* index;      // The index expression
        } array_access;
        
        // Member access
        struct {
            struct ASTNode* object;     // The object being accessed
            char* member_name;          // The member name
        } member_access;
        
        // Function call on expression (e.g., obj.method(args), func(args))
        struct {
            struct ASTNode* function;   // The function expression (can be identifier, member access, etc.)
            struct ASTNode** arguments; // The arguments
            size_t argument_count;      // Number of arguments
        } function_call_expr;
        
        // Import statement
        struct {
            char* module_name;
            char* alias;
        } import_statement;
        
        // Use statement
        struct {
            char* library_name;
            char* alias;
            char** specific_items;
            char** specific_aliases;
            size_t item_count;
        } use_statement;
        
        // Module definition
        struct {
            char* module_name;
            struct ASTNode* body;
        } module_definition;
        
        // Package definition
        struct {
            char* package_name;
            struct ASTNode* body;
        } package_definition;
        
        // Error node
        struct {
            char* error_message;
        } error_node;
    } data;
    
    // Source location information
    int line;
    int column;
    
    // Memory management
    struct ASTNode* next;  // For linked list management
} ASTNode;

// AST Node Creation Functions
ASTNode* ast_create_number(double value, int line, int column);
ASTNode* ast_create_string(const char* value, int line, int column);
ASTNode* ast_create_bool(int value, int line, int column);
ASTNode* ast_create_null(int line, int column);
ASTNode* ast_create_identifier(const char* name, int line, int column);
ASTNode* ast_create_binary_op(BinaryOperator op, ASTNode* left, ASTNode* right, int line, int column);
ASTNode* ast_create_range_with_step(ASTNode* start, ASTNode* end, ASTNode* step, int line, int column);
ASTNode* ast_create_unary_op(UnaryOperator op, ASTNode* operand, int line, int column);
ASTNode* ast_create_assignment(const char* variable, ASTNode* value, int line, int column);
ASTNode* ast_create_function_call(const char* name, ASTNode** args, size_t arg_count, int line, int column);
ASTNode* ast_create_function_call_expr(ASTNode* function, ASTNode** args, size_t arg_count, int line, int column);
ASTNode* ast_create_variable_declaration(const char* name, const char* type, ASTNode* initial_value, int is_mutable, int line, int column);
ASTNode* ast_create_array_access(ASTNode* array, ASTNode* index, int line, int column);
ASTNode* ast_create_member_access(ASTNode* object, const char* member_name, int line, int column);
ASTNode* ast_create_if_statement(ASTNode* condition, ASTNode* then_block, ASTNode* else_block, ASTNode* else_if_chain, int line, int column);
ASTNode* ast_create_while_loop(ASTNode* condition, ASTNode* body, int line, int column);
ASTNode* ast_create_for_loop(const char* iterator, ASTNode* collection, ASTNode* body, int line, int column);
ASTNode* ast_create_block(ASTNode** statements, size_t statement_count, int line, int column);
ASTNode* ast_create_return(ASTNode* value, int line, int column);
ASTNode* ast_create_try_catch(ASTNode* try_block, const char* catch_var, ASTNode* catch_block, ASTNode* finally_block, int line, int column);
ASTNode* ast_create_switch(ASTNode* expression, ASTNode** cases, size_t case_count, ASTNode* default_case, int line, int column);
ASTNode* ast_create_match(ASTNode* expression, ASTNode** patterns, size_t pattern_count, int line, int column);
ASTNode* ast_create_spore(ASTNode* expression, ASTNode** cases, size_t case_count, ASTNode* root_case, int line, int column);
ASTNode* ast_create_spore_case(ASTNode* pattern, ASTNode* body, int is_lambda, int line, int column);
ASTNode* ast_create_class(const char* name, const char* parent, ASTNode* body, int line, int column);
ASTNode* ast_create_function(const char* name, ASTNode** params, size_t param_count, const char* return_type, ASTNode* body, int line, int column);
ASTNode* ast_create_array_literal(ASTNode** elements, size_t element_count, int line, int column);
ASTNode* ast_create_import(const char* module, const char* alias, int line, int column);
ASTNode* ast_create_use(const char* library, const char* alias, char** specific_items, char** specific_aliases, size_t item_count, int line, int column);
ASTNode* ast_create_module(const char* name, ASTNode* body, int line, int column);
ASTNode* ast_create_package(const char* name, ASTNode* body, int line, int column);
ASTNode* ast_create_error_node(const char* error_message, int line, int column);

// AST Node Management Functions
void ast_free(ASTNode* node);
void ast_free_tree(ASTNode* root);
ASTNode* ast_clone(ASTNode* node);
ASTNode* ast_clone_tree(ASTNode* root);

// AST Utility Functions
void ast_print(ASTNode* node, int indent);
void ast_print_tree(ASTNode* root);
const char* ast_node_type_to_string(ASTNodeType type);
const char* binary_op_to_string(BinaryOperator op);
const char* unary_op_to_string(UnaryOperator op);

// AST Validation Functions
int ast_validate(ASTNode* node);
int ast_validate_tree(ASTNode* root);

#endif // AST_H
