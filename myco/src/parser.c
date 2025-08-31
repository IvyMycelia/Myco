/**
 * @file parser.c
 * @brief Myco Language Parser - Converts tokens into Abstract Syntax Tree (AST)
 * @version 1.0.0
 * @author Myco Development Team
 * 
 * This file implements the parsing phase of the Myco interpreter.
 * It converts a stream of tokens into a structured Abstract Syntax Tree
 * that represents the program's structure and can be evaluated.
 * 
 * Parser Features:
 * - Recursive descent parsing with operator precedence
 * - AST construction for all language constructs
 * - Error handling with line number reporting
 * - Memory-efficient AST node management
 * - Support for complex expressions and statements
 * 
 * AST Node Types:
 * - AST_FUNC: Function definitions
 * - AST_LET: Variable declarations
 * - AST_IF: Conditional statements
 * - AST_FOR: Loop constructs
 * - AST_WHILE: While loops
 * - AST_RETURN: Return statements
 * - AST_EXPR: Expressions and operators
 * - AST_BLOCK: Code blocks and scopes
 * - AST_DOT: Member access (module.function)
 * - AST_ASSIGN: Variable reassignment
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"
#include "memory_tracker.h"

#define MAX_CHILDREN 100

// Forward declarations
static ASTNode* parse_expression(Token* tokens, int* current);
static ASTNode* parse_statement(Token* tokens, int* current, int token_count);
static ASTNode* parse_block(Token* tokens, int* current, int token_count);
static void deep_copy_ast_node(ASTNode* dest, ASTNode* src);

// NEW: v2.0 Type System Helper Functions

/**
 * @brief Check if a token type is a type annotation
 * @param token_type The token type to check
 * @return 1 if it's a type token, 0 otherwise
 */
static int is_type_token(MycoTokenType token_type) {
    return (token_type == TOKEN_TYPE_INT ||
            token_type == TOKEN_TYPE_FLOAT ||
            token_type == TOKEN_TYPE_BOOL ||
            token_type == TOKEN_TYPE_STRING ||
            token_type == TOKEN_TYPE_ARRAY ||
            token_type == TOKEN_TYPE_TUPLE ||
            token_type == TOKEN_TYPE_DICT ||
            token_type == TOKEN_TYPE_SET ||
            token_type == TOKEN_TYPE_NULL ||
            token_type == TOKEN_TYPE_ANY ||
            token_type == TOKEN_TYPE_FUNCTION ||
            token_type == TOKEN_TYPE_OBJECT ||
            token_type == TOKEN_TYPE_CLASS ||
            token_type == TOKEN_TYPE_ENUM ||
            token_type == TOKEN_TYPE_BYTE ||
            token_type == TOKEN_TYPE_BYTES ||
            token_type == TOKEN_STRING_TYPE); // Legacy support
}

/**
 * @brief Convert token type to AST node type
 * @param token_type The token type
 * @return The corresponding AST node type
 */
static ASTNodeType get_ast_type_from_token(MycoTokenType token_type) {
    switch (token_type) {
        case TOKEN_TYPE_INT: return AST_TYPE_INT;
        case TOKEN_TYPE_FLOAT: return AST_TYPE_FLOAT;
        case TOKEN_TYPE_BOOL: return AST_TYPE_BOOL;
        case TOKEN_TYPE_STRING: return AST_TYPE_STRING;
        case TOKEN_TYPE_ARRAY: return AST_TYPE_ARRAY;
        case TOKEN_TYPE_TUPLE: return AST_TYPE_TUPLE;
        case TOKEN_TYPE_DICT: return AST_TYPE_DICT;
        case TOKEN_TYPE_SET: return AST_TYPE_SET;
        case TOKEN_TYPE_NULL: return AST_TYPE_NULL;
        case TOKEN_TYPE_ANY: return AST_TYPE_ANY;
        case TOKEN_TYPE_FUNCTION: return AST_TYPE_FUNCTION;
        case TOKEN_TYPE_OBJECT: return AST_TYPE_OBJECT;
        case TOKEN_TYPE_CLASS: return AST_TYPE_CLASS;
        case TOKEN_TYPE_ENUM: return AST_TYPE_ENUM;
        case TOKEN_TYPE_BYTE: return AST_TYPE_BYTE;
        case TOKEN_TYPE_BYTES: return AST_TYPE_BYTES;
        case TOKEN_STRING_TYPE: return AST_TYPE_STRING; // Legacy support
        default: return AST_TYPE_ANY;
    }
}

/**
 * @brief Parse generic type parameters (e.g., array<int>)
 * @param tokens Token array
 * @param current Current token index
 * @param token_count Total token count
 * @param base_type Base type node
 * @return Updated type node with generic parameters
 */
static ASTNode* parse_generic_type(Token* tokens, int* current, int token_count, ASTNode* base_type) {
    if (tokens[*current].type != TOKEN_OPERATOR || strcmp(tokens[*current].text, "<") != 0) {
        return base_type;
    }
    
    (*current)++; // Skip '<'
    
    // Parse type parameters
    while (*current < token_count && !(tokens[*current].type == TOKEN_OPERATOR && strcmp(tokens[*current].text, ">") == 0)) {
        if (is_type_token(tokens[*current].type)) {
            ASTNode* param_type = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_generic_type_param");
            param_type->type = get_ast_type_from_token(tokens[*current].type);
            param_type->text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parse_generic_type_param_text");
            param_type->children = NULL;
            param_type->child_count = 0;
            param_type->next = NULL;
            param_type->line = tokens[*current].line;
            
            // Add as child to base type
            base_type->children = (ASTNode*)tracked_realloc(base_type->children, 
                (base_type->child_count + 1) * sizeof(ASTNode), __FILE__, __LINE__, "parse_generic_type_children");
            deep_copy_ast_node(&base_type->children[base_type->child_count], param_type);
            base_type->child_count++;
            
            (*current)++; // Skip type parameter
        } else {
            fprintf(stderr, "Error: Expected type parameter in generic type at line %d\n", tokens[*current].line);
            break;
        }
        
        // Check for comma separator
        if (tokens[*current].type == TOKEN_COMMA) {
            (*current)++; // Skip comma
        }
    }
    
    if (tokens[*current].type == TOKEN_OPERATOR && strcmp(tokens[*current].text, ">") == 0) {
        (*current)++; // Skip '>'
    } else {
        fprintf(stderr, "Error: Expected '>' to close generic type at line %d\n", tokens[*current].line);
    }
    
    return base_type;
}

// Helper function to initialize AST node fields
static void init_ast_node(ASTNode* node) {
    if (node) {
        node->implicit_function = NULL;
        node->children = NULL;
        node->child_count = 0;
        node->next = NULL;
        node->for_type = AST_FOR_RANGE;
        
        // NEW: v2.0 Type System Support
        node->type_annotation = AST_TYPE_ANY;  // Default to any type
        node->type_text = NULL;
    }
}

/*******************************************************************************
 * OPERATOR PRECEDENCE
 ******************************************************************************/

/**
 * @brief Determines the precedence level of operators for parsing
 * @param op The operator string to check
 * @return Precedence level (higher = higher precedence)
 * 
 * Operator precedence determines the order of operations in expressions.
 * Higher precedence operators are parsed first, ensuring correct
 * mathematical and logical evaluation order.
 * 
 * Precedence Levels:
 * - Level 1: Logical operators (and, or)
 * - Level 2: Equality operators (==, !=)
 * - Level 3: Comparison operators (<, >, <=, >=)
 * - Level 4: Additive operators (+, -)
 * - Level 5: Multiplicative operators (*, /, %)
 */
static int get_precedence(const char* op) {
    if (strcmp(op, "and") == 0 || strcmp(op, "or") == 0) return 1;
    if (strcmp(op, "==") == 0 || strcmp(op, "!=") == 0) return 2;
    if (strcmp(op, "<") == 0 || strcmp(op, ">") == 0 || 
        strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0) return 3;
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 4;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || strcmp(op, "%") == 0) return 5;
    return 0;
}

/*******************************************************************************
 * PRIMARY EXPRESSION PARSING
 ******************************************************************************/

/**
 * @brief Parses primary expressions (atoms) in the language
 * @param tokens Array of tokens to parse
 * @param current Pointer to current token position
 * @return AST node representing the primary expression, or NULL on error
 * 
 * Primary expressions are the basic building blocks of the language:
 * - Numbers (integers, negative numbers)
 * - Identifiers (variable names, function names)
 * - String literals
 * - Parenthesized expressions
 * - Dot expressions (member access)
 * - Function calls
 * 
 * This function handles the lowest level of expression parsing,
 * creating leaf nodes in the AST tree.
 */
static ASTNode* parse_primary(Token* tokens, int* current) {
    ASTNode* node = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_primary");
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }

    // Initialize common fields
    init_ast_node(node);

    // Set line number from current token
    node->line = tokens[*current].line;

    switch (tokens[*current].type) {
        case TOKEN_NUMBER:
        case TOKEN_FLOAT:
            node->type = AST_EXPR;
            node->text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
            node->children = NULL;
            node->child_count = 0;
            node->next = NULL;
            (*current)++;
            break;
            
        case TOKEN_TRUE:
            node->type = AST_EXPR;
            node->text = tracked_strdup("True", __FILE__, __LINE__, "parser"); // Keep as "True" for proper type detection
            node->children = NULL;
            node->child_count = 0;
            node->next = NULL;
            (*current)++;
            break;
            
        case TOKEN_FALSE:
            node->type = AST_EXPR;
            node->text = tracked_strdup("False", __FILE__, __LINE__, "parser"); // Keep as "False" for proper type detection
            node->children = NULL;
            node->child_count = 0;
            node->next = NULL;
            (*current)++;
            break;
            
        case TOKEN_TYPE_MARKER:
        case TOKEN_STRING_TYPE:
            // Treat type markers as identifiers for use in expressions
            node->type = AST_EXPR;
            node->text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
            node->children = NULL;
            node->child_count = 0;
            node->next = NULL;
            (*current)++;
            break;
            
        case TOKEN_TYPE_INT:
        case TOKEN_TYPE_FLOAT:
        case TOKEN_TYPE_BOOL:
        case TOKEN_TYPE_STRING:
        case TOKEN_TYPE_ARRAY:
        case TOKEN_TYPE_TUPLE:
        case TOKEN_TYPE_DICT:
        case TOKEN_TYPE_SET:
        case TOKEN_TYPE_NULL:
        case TOKEN_TYPE_ANY:
        case TOKEN_TYPE_FUNCTION:
        case TOKEN_TYPE_OBJECT:
        case TOKEN_TYPE_CLASS:
        case TOKEN_TYPE_ENUM:
        case TOKEN_TYPE_BYTE:
        case TOKEN_TYPE_BYTES:
            // Treat type tokens as string literals for use in expressions
            node->type = AST_EXPR;
            node->text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
            node->children = NULL;
            node->child_count = 0;
            node->next = NULL;
            (*current)++;
            break;
            
        case TOKEN_IDENTIFIER:
            // Check if this is a chained property assignment: obj.prop1.prop2... = value
            // Scan ahead to detect the pattern: identifier (.identifier)* = 
            int lookahead = *current;
            int chain_depth = 0;
            
            // Must start with identifier
            if (tokens[lookahead].type == TOKEN_IDENTIFIER) {
                lookahead++;
                chain_depth++;
                
                // Count chained properties: .identifier .identifier ...
                while (tokens[lookahead].type == TOKEN_DOT && 
                       tokens[lookahead + 1].type == TOKEN_IDENTIFIER) {
                    lookahead += 2; // Skip .identifier
                    chain_depth++;
                }
                
                // Check if this chain ends with assignment
                if (tokens[lookahead].type == TOKEN_ASSIGN && chain_depth >= 2) {
                    // This is a chained property assignment!
                    // For now, let's handle 2-level and 3-level chains
                    
                    if (chain_depth == 2) {
                        // Handle obj.prop = value (existing logic)
                        char* obj_name = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
                        char* prop_name = tracked_strdup(tokens[*current + 2].text, __FILE__, __LINE__, "parser");
                        (*current) += 3; // Skip obj.prop=
                        
                        // Parse the value expression
                        ASTNode* value_expr = parse_expression(tokens, current);
                        if (!value_expr) {
                            tracked_free(obj_name, __FILE__, __LINE__, "parse_primary_object_assign");
                            tracked_free(prop_name, __FILE__, __LINE__, "parse_primary_object_assign");
                            tracked_free(node, __FILE__, __LINE__, "parse_primary_object_assign");
                            return NULL;
                        }
                        
                        // Create object assignment node
                        node->type = AST_OBJECT_ASSIGN;
                        node->text = tracked_strdup("object_assign", __FILE__, __LINE__, "parser");
                        node->children = (ASTNode*)tracked_malloc(3 * sizeof(ASTNode), __FILE__, __LINE__, "parse_primary_object_assign");
                        node->child_count = 3;
                        node->next = NULL;
                        node->line = tokens[*current - 3].line;
                        
                        // First child: object name
                        node->children[0].type = AST_EXPR;
                        node->children[0].text = obj_name;
                        node->children[0].children = NULL;
                        node->children[0].child_count = 0;
                        node->children[0].next = NULL;
                        node->children[0].line = node->line;
                        
                        // Second child: property name
                        node->children[1].type = AST_EXPR;
                        node->children[1].text = prop_name;
                        node->children[1].children = NULL;
                        node->children[1].child_count = 0;
                        node->children[1].next = NULL;
                        node->children[1].line = node->line;
                        
                        // Third child: value expression
                        deep_copy_ast_node(&node->children[2], value_expr);
                        
                        // Clean up temporary value expression
                        parser_free_ast(value_expr);
                        break;
                    }
                    else if (chain_depth == 4) {
                        // Handle obj.prop1.prop2.prop3 = value (4-level nested)
                        char* obj_name = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
                        char* prop1_name = tracked_strdup(tokens[*current + 2].text, __FILE__, __LINE__, "parser");
                        char* prop2_name = tracked_strdup(tokens[*current + 4].text, __FILE__, __LINE__, "parser");
                        char* prop3_name = tracked_strdup(tokens[*current + 6].text, __FILE__, __LINE__, "parser");
                        (*current) += 7; // Skip obj.prop1.prop2.prop3=
                        
                        // Parse the value expression
                        ASTNode* value_expr = parse_expression(tokens, current);
                        if (!value_expr) {
                            tracked_free(obj_name, __FILE__, __LINE__, "parse_primary_4level_assign");
                            tracked_free(prop1_name, __FILE__, __LINE__, "parse_primary_4level_assign");
                            tracked_free(prop2_name, __FILE__, __LINE__, "parse_primary_4level_assign");
                            tracked_free(prop3_name, __FILE__, __LINE__, "parse_primary_4level_assign");
                            tracked_free(node, __FILE__, __LINE__, "parse_primary_4level_assign");
                            return NULL;
                        }
                        
                        // Create 4-level nested assignment node
                        // Structure: [obj_name, prop1_name, prop2_name, prop3_name, value]
                        node->type = AST_OBJECT_ASSIGN; // Reuse existing type
                        node->text = tracked_strdup("nested_assign_4", __FILE__, __LINE__, "parser");
                        node->children = (ASTNode*)tracked_malloc(5 * sizeof(ASTNode), __FILE__, __LINE__, "parse_primary_4level_assign");
                        node->child_count = 5;
                        node->next = NULL;
                        node->line = tokens[*current - 7].line;
                        
                        // First child: object name
                        node->children[0].type = AST_EXPR;
                        node->children[0].text = obj_name;
                        node->children[0].children = NULL;
                        node->children[0].child_count = 0;
                        node->children[0].next = NULL;
                        node->children[0].line = node->line;
                        
                        // Second child: first property name  
                        node->children[1].type = AST_EXPR;
                        node->children[1].text = prop1_name;
                        node->children[1].children = NULL;
                        node->children[1].child_count = 0;
                        node->children[1].next = NULL;
                        node->children[1].line = node->line;
                        
                        // Third child: second property name
                        node->children[2].type = AST_EXPR;
                        node->children[2].text = prop2_name;
                        node->children[2].children = NULL;
                        node->children[2].child_count = 0;
                        node->children[2].next = NULL;
                        node->children[2].line = node->line;
                        
                        // Fourth child: third property name
                        node->children[3].type = AST_EXPR;
                        node->children[3].text = prop3_name;
                        node->children[3].children = NULL;
                        node->children[3].child_count = 0;
                        node->children[3].next = NULL;
                        node->children[3].line = node->line;
                        
                        // Fifth child: value expression
                        deep_copy_ast_node(&node->children[4], value_expr);
                        
                        // Clean up temporary value expression
                        parser_free_ast(value_expr);
                        break;
                    }
                    else {
                        // For now, only support up to 4-level nesting
                        fprintf(stderr, "Error: Property assignment chains longer than 4 levels not yet supported at line %d\n", tokens[*current].line);
                        tracked_free(node, __FILE__, __LINE__, "parse_primary_chain_too_deep");
                        return NULL;
                    }
                }
            }
            
            // If we get here, it's not a chained assignment, so fall back to regular identifier parsing
            // First check for simple case: obj.prop = value (legacy fallback)
            if (tokens[*current + 1].type == TOKEN_DOT && 
                tokens[*current + 2].type == TOKEN_IDENTIFIER && 
                tokens[*current + 3].type == TOKEN_ASSIGN) {
                
                // This is an object property assignment: obj.prop = value
                char* obj_name = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
                char* prop_name = tracked_strdup(tokens[*current + 2].text, __FILE__, __LINE__, "parser");
                (*current) += 3; // Skip obj.prop=
                
                // Parse the value expression
                ASTNode* value_expr = parse_expression(tokens, current);
                if (!value_expr) {
                            tracked_free(obj_name, __FILE__, __LINE__, "parse_primary_object_assign");
                            tracked_free(prop_name, __FILE__, __LINE__, "parse_primary_object_assign");
                    tracked_free(node, __FILE__, __LINE__, "parse_primary_object_assign");
                    return NULL;
                }
                
                // Create object assignment node
                node->type = AST_OBJECT_ASSIGN;
                node->text = tracked_strdup("object_assign", __FILE__, __LINE__, "parser");
                node->children = (ASTNode*)tracked_malloc(3 * sizeof(ASTNode), __FILE__, __LINE__, "parse_primary_object_assign");
                node->child_count = 3;
                node->next = NULL;
                node->line = tokens[*current - 3].line;
                
                // First child: object name
                node->children[0].type = AST_EXPR;
                node->children[0].text = obj_name;
                node->children[0].children = NULL;
                node->children[0].child_count = 0;
                node->children[0].next = NULL;
                node->children[0].line = node->line;
                
                // Second child: property name
                node->children[1].type = AST_EXPR;
                node->children[1].text = prop_name;
                node->children[1].children = NULL;
                node->children[1].child_count = 0;
                node->children[1].next = NULL;
                node->children[1].line = node->line;
                
                // Third child: value expression
                deep_copy_ast_node(&node->children[2], value_expr);
                parser_free_ast(value_expr);
                break;
            }
            
            
            // Regular identifier
            node->type = AST_EXPR;
            node->text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
            node->children = NULL;
            node->child_count = 0;
            node->next = NULL;
            (*current)++;
            break;

        case TOKEN_OPERATOR:
            // Handle unary minus (negative numbers)
            if (strcmp(tokens[*current].text, "-") == 0) {
                (*current)++; // Skip '-'
                
                // Parse the number or float after the minus
                if (tokens[*current].type != TOKEN_NUMBER && tokens[*current].type != TOKEN_FLOAT) {
                    fprintf(stderr, "Error: Expected number or float after '-' at line %d\n", tokens[*current].line);
                    tracked_free(node, __FILE__, __LINE__, "parse_primary_unary_minus");
                    return NULL;
                }
                
                // Create a negative number by combining '-' and the number
                char* negative_num = (char*)tracked_malloc(strlen(tokens[*current].text) + 2, __FILE__, __LINE__, "parse_primary_unary_minus");
                sprintf(negative_num, "-%s", tokens[*current].text);
                
                node->type = AST_EXPR;
                node->text = negative_num;
                node->children = NULL;
                node->child_count = 0;
                node->next = NULL;
                (*current)++;
                break;
            } else {
                fprintf(stderr, "Error: Unexpected operator '%s' in expression at line %d\n", 
                        tokens[*current].text, tokens[*current].line);
                tracked_free(node, __FILE__, __LINE__, "parse_primary_unexpected_operator");
                return NULL;
            }

        case TOKEN_STRING:
            node->type = AST_EXPR;
            // Store the string literal with quotes to match function expectations
            char* quoted_string = (char*)tracked_malloc(strlen(tokens[*current].text) + 3, __FILE__, __LINE__, "parse_primary_string");
            if (quoted_string) {
                quoted_string[0] = '"';
                strcpy(quoted_string + 1, tokens[*current].text);
                quoted_string[strlen(tokens[*current].text) + 1] = '"';
                quoted_string[strlen(tokens[*current].text) + 2] = '\0';
                node->text = quoted_string;
            } else {
                node->text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parse_primary_string");
            }
            node->children = NULL;
            node->child_count = 0;
            node->next = NULL;
            (*current)++;
            break;



        case TOKEN_LPAREN:
            (*current)++; // Skip '('
            node = parse_expression(tokens, current);
            if (!node) return NULL;
            if (tokens[*current].type != TOKEN_RPAREN) {
                fprintf(stderr, "Error: Expected ')' at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                tracked_free(node, __FILE__, __LINE__, "parse_primary");
                return NULL;
            }
            (*current)++; // Skip ')'
            break;

        case TOKEN_LBRACKET:
            // Parse array literal: [expr1, expr2, ...]
            (*current)++; // Skip '['
            
            // Create array literal node
            node->type = AST_ARRAY_LITERAL;
            node->text = tracked_strdup("array", __FILE__, __LINE__, "parser");
            node->children = NULL;
            node->child_count = 0;
            node->next = NULL;
            
            // Parse array elements
            while (tokens[*current].type != TOKEN_RBRACKET && tokens[*current].type != TOKEN_EOF) {
                // Parse the element expression
                ASTNode* element = parse_expression(tokens, current);
                if (!element) {
                    fprintf(stderr, "Error: Failed to parse array element at line %d\n", tokens[*current].line);
                    parser_free_ast(node);
                    return NULL;
                }
                
                // Add element to array
                if (node->child_count == 0) {
                    node->children = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_primary_array_elements");
                } else {
                    ASTNode* new_children = (ASTNode*)tracked_realloc(node->children, 
                        (node->child_count + 1) * sizeof(ASTNode), __FILE__, __LINE__, "parse_primary_array_elements");
                    if (!new_children) {
                        fprintf(stderr, "Error: Memory allocation failed for array elements\n");
                        parser_free_ast(element);
                        parser_free_ast(node);
                        return NULL;
                    }
                    node->children = new_children;
                }
                
                // Copy element to children array
                deep_copy_ast_node(&node->children[node->child_count], element);
                node->child_count++;
                parser_free_ast(element);
                
                // Check for comma separator
                if (tokens[*current].type == TOKEN_COMMA) {
                    (*current)++; // Skip comma
                    // Check if there's another element after comma
                    if (tokens[*current].type == TOKEN_RBRACKET) {
                        fprintf(stderr, "Error: Trailing comma in array literal at line %d\n", tokens[*current].line);
                        parser_free_ast(node);
                        return NULL;
                    }
                } else if (tokens[*current].type != TOKEN_RBRACKET) {
                    fprintf(stderr, "Error: Expected ',' or ']' in array literal at line %d\n", tokens[*current].line);
                    parser_free_ast(node);
                    return NULL;
                }
            }
            
            // Check for closing bracket
            if (tokens[*current].type != TOKEN_RBRACKET) {
                fprintf(stderr, "Error: Expected ']' to close array literal at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                return NULL;
            }
            (*current)++; // Skip ']'
            break;

        case TOKEN_LBRACE:
            // Parse object literal: {prop1: val1, prop2: val2}
    
            (*current)++; // Skip '{'
            
            // Create object literal node
            node->type = AST_OBJECT_LITERAL;
            node->text = tracked_strdup("object", __FILE__, __LINE__, "parser");
            node->children = NULL;
            node->child_count = 0;
            node->next = NULL;
            

            
            // Parse object properties
            while (tokens[*current].type != TOKEN_RBRACE && tokens[*current].type != TOKEN_EOF) {

                
                // Parse property name (identifier)
                if (tokens[*current].type != TOKEN_IDENTIFIER) {
                    fprintf(stderr, "Error: Expected property name (identifier) in object literal at line %d\n", tokens[*current].line);
                    parser_free_ast(node);
                    return NULL;
                }
                
                // Create property name node
                ASTNode* prop_name = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_primary_object_prop_name");
                prop_name->type = AST_EXPR;
                prop_name->text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
                prop_name->children = NULL;
                prop_name->child_count = 0;
                prop_name->next = NULL;
                prop_name->line = tokens[*current].line;
                (*current)++; // Skip property name
                

                
                // Expect colon separator
                if (tokens[*current].type != TOKEN_COLON) {
                    fprintf(stderr, "Error: Expected ':' after property name in object literal at line %d\n", tokens[*current].line);
                    parser_free_ast(prop_name);
                    parser_free_ast(node);
                    return NULL;
                }
                (*current)++; // Skip ':'
                
                // Parse property value expression
                ASTNode* prop_value = parse_expression(tokens, current);
                if (!prop_value) {
                    fprintf(stderr, "Error: Failed to parse property value in object literal at line %d\n", tokens[*current].line);
                    parser_free_ast(prop_name);
                    parser_free_ast(node);
                    return NULL;
                }
                

                
                // Create property pair node (name: value)
                ASTNode* prop_pair = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_primary_object_prop_pair");
                prop_pair->type = AST_EXPR;
                prop_pair->text = tracked_strdup("prop", __FILE__, __LINE__, "parser");
                prop_pair->children = (ASTNode*)tracked_malloc(2 * sizeof(ASTNode), __FILE__, __LINE__, "parse_primary_object_prop_pair");
                prop_pair->child_count = 2;
                prop_pair->next = NULL;
                prop_pair->line = tokens[*current].line;
                
                // Copy name and value to property pair
                deep_copy_ast_node(&prop_pair->children[0], prop_name);
                deep_copy_ast_node(&prop_pair->children[1], prop_value);
                

                
                // Add property pair to object
                if (node->child_count == 0) {
                    node->children = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_primary_object_properties");
                } else {
                    ASTNode* new_children = (ASTNode*)tracked_realloc(node->children, 
                        (node->child_count + 1) * sizeof(ASTNode), __FILE__, __LINE__, "parse_primary_object_properties");
                    if (!new_children) {
                        fprintf(stderr, "Error: Memory allocation failed for object properties\n");
                        parser_free_ast(prop_name);
                        parser_free_ast(prop_value);
                        parser_free_ast(prop_pair);
                        parser_free_ast(node);
                        return NULL;
                    }
                    node->children = new_children;
                }
                
                // Copy property pair to children array
                deep_copy_ast_node(&node->children[node->child_count], prop_pair);
                node->child_count++;
                

                
                // Clean up temporary nodes
                parser_free_ast(prop_name);
                parser_free_ast(prop_value);
                parser_free_ast(prop_pair);
                
                // Check for comma separator
                if (tokens[*current].type == TOKEN_COMMA) {
                    (*current)++; // Skip comma
                    // Check if there's another property after comma
                    if (tokens[*current].type == TOKEN_RBRACE) {
                        fprintf(stderr, "Error: Trailing comma in object literal at line %d\n", tokens[*current].line);
                        parser_free_ast(node);
                        return NULL;
                    }
                } else if (tokens[*current].type != TOKEN_RBRACE) {
                    fprintf(stderr, "Error: Expected ',' or '}' in object literal at line %d\n", tokens[*current].line);
                    parser_free_ast(node);
                    return NULL;
                }
            }
            

            
            // Check for closing brace
            if (tokens[*current].type != TOKEN_RBRACE) {
                fprintf(stderr, "Error: Expected '}' to close object literal at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                return NULL;
            }
            (*current)++; // Skip '}'
            break;

        default:
            fprintf(stderr, "Error: Unexpected token '%s' in expression at line %d (token type: %d)\n", 
                    tokens[*current].text ? tokens[*current].text : "NULL", 
                    tokens[*current].line, 
                    tokens[*current].type);
            tracked_free(node, __FILE__, __LINE__, "parse_primary");
            return NULL;
    }

    // Handle dot expressions (member access) - do this BEFORE function calls
    while (tokens[*current].type == TOKEN_DOT) {
        (*current)++; // Skip '.'
        
        if (tokens[*current].type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Error: Expected identifier after '.' at line %d, got token type %d\n", 
                    tokens[*current].line, tokens[*current].type);
            parser_free_ast(node);
            return NULL;
        }
        ASTNode* dot_node = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_primary_dot");
        dot_node->type = AST_DOT;
        dot_node->text = tracked_strdup("dot", __FILE__, __LINE__, "parser");
        dot_node->children = (ASTNode*)tracked_malloc(2 * sizeof(ASTNode), __FILE__, __LINE__, "parse_primary_dot");
        dot_node->child_count = 2;
        dot_node->next = NULL;
        dot_node->line = node->line;

        // Left side (object) - move the node structure instead of copying
        dot_node->children[0] = *node;
        // Clear the original node's pointers to prevent double-free
        node->text = NULL;
        node->children = NULL;
        node->child_count = 0;

        // Right side (member name)
        dot_node->children[1].type = AST_EXPR;
        dot_node->children[1].text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
        dot_node->children[1].children = NULL;
        dot_node->children[1].child_count = 0;
        dot_node->children[1].next = NULL;
        dot_node->children[1].line = tokens[*current].line;

        (*current)++; // Skip member identifier
        

        
        // Free the original node since we're replacing it
        tracked_free(node, __FILE__, __LINE__, "parse_primary_dot_replace");
        node = dot_node;
    }

    // Handle array access - do this AFTER dot expressions but BEFORE function calls
    if (tokens[*current].type == TOKEN_LBRACKET) {
        (*current)++; // Skip '['
        
        // Parse the index expression
        ASTNode* index_expr = parse_expression(tokens, current);
        if (!index_expr) {
            fprintf(stderr, "Error: Failed to parse array index at line %d\n", tokens[*current].line);
            parser_free_ast(node);
            return NULL;
        }
        
        // Check for closing bracket
        if (tokens[*current].type != TOKEN_RBRACKET) {
            fprintf(stderr, "Error: Expected ']' after array index at line %d\n", tokens[*current].line);
            parser_free_ast(index_expr);
            parser_free_ast(node);
            return NULL;
        }
        (*current)++; // Skip ']'
        
        // Determine if this should be object bracket access or array access
        // Check if the base expression is likely an object
        int is_object_access = 0;
        if (node->type == AST_EXPR && node->text) {
            // For simple identifiers, we'll default to object access for now
            // This is a heuristic - in a full implementation we'd check the variable type
            is_object_access = 1;
        }
        
        // Create appropriate access node
        ASTNode* access_node = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_primary_bracket_access");
        if (is_object_access) {
            access_node->type = AST_OBJECT_BRACKET_ACCESS;
            access_node->text = tracked_strdup("bracket_access", __FILE__, __LINE__, "parser");
        } else {
        access_node->type = AST_ARRAY_ACCESS;
            access_node->text = tracked_strdup("access", __FILE__, __LINE__, "parser");
        }
        access_node->children = (ASTNode*)tracked_malloc(2 * sizeof(ASTNode), __FILE__, __LINE__, "parse_primary_bracket_access");
        access_node->child_count = 2;
        access_node->next = NULL;
        access_node->line = node->line;
        
        // First child is the base expression (array or object)
        deep_copy_ast_node(&access_node->children[0], node);
        // Second child is the index/key expression
        deep_copy_ast_node(&access_node->children[1], index_expr);
        
        // Clean up
        parser_free_ast(index_expr);
        tracked_free(node, __FILE__, __LINE__, "parse_primary_array_access_replace");
        node = access_node;
    }

    // Handle object bracket access - obj["property"]
    if (tokens[*current].type == TOKEN_LBRACKET && node->type == AST_DOT) {
        (*current)++; // Skip '['
        
        // Parse the property name expression (could be string literal or identifier)
        ASTNode* prop_expr = parse_expression(tokens, current);
        if (!prop_expr) {
            fprintf(stderr, "Error: Failed to parse object property name at line %d\n", tokens[*current].line);
            parser_free_ast(node);
            return NULL;
        }
        
        // Check for closing bracket
        if (tokens[*current].type != TOKEN_RBRACKET) {
            fprintf(stderr, "Error: Expected ']' after object property name at line %d\n", tokens[*current].line);
            parser_free_ast(prop_expr);
            parser_free_ast(node);
            return NULL;
        }
        (*current)++; // Skip ']'
        
        // Create object bracket access node
        ASTNode* bracket_access_node = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_primary_object_bracket_access");
        bracket_access_node->type = AST_OBJECT_BRACKET_ACCESS;
        bracket_access_node->text = tracked_strdup("bracket_access", __FILE__, __LINE__, "parser");
        bracket_access_node->children = (ASTNode*)tracked_malloc(2 * sizeof(ASTNode), __FILE__, __LINE__, "parse_primary_object_bracket_access");
        bracket_access_node->child_count = 2;
        bracket_access_node->next = NULL;
        bracket_access_node->line = node->line;
        
        // First child is the object expression (the dot expression)
        deep_copy_ast_node(&bracket_access_node->children[0], node);
        // Second child is the property name expression
        deep_copy_ast_node(&bracket_access_node->children[1], prop_expr);
        
        // Clean up
        parser_free_ast(prop_expr);
        tracked_free(node, __FILE__, __LINE__, "parse_primary_object_bracket_access_replace");
        node = bracket_access_node;
    }

    // Handle function calls - do this AFTER dot expressions
    if (tokens[*current].type == TOKEN_LPAREN) {
        (*current)++; // Skip '('
        
        // Create function call node
        ASTNode* call_node = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_primary_call");
        call_node->type = AST_EXPR;
        call_node->text = tracked_strdup("call", __FILE__, __LINE__, "parser");
        
        call_node->children = (ASTNode*)tracked_malloc(2 * sizeof(ASTNode), __FILE__, __LINE__, "parse_primary_call");
        call_node->child_count = 2;
        call_node->next = NULL;
        call_node->line = node->line;  // Preserve line number
        
        // Use deep copy to properly copy the function name node
        deep_copy_ast_node(&call_node->children[0], node);

        // Parse arguments
        call_node->children[1].type = AST_EXPR;
        call_node->children[1].text = tracked_strdup("args", __FILE__, __LINE__, "parser");
        call_node->children[1].children = NULL;
        call_node->children[1].child_count = 0;
        call_node->children[1].next = NULL;
        call_node->children[1].line = tokens[*current].line;  // Set line number for args

        while (tokens[*current].type != TOKEN_RPAREN) {
            ASTNode* arg = parse_expression(tokens, current);
            if (!arg) {
                tracked_free(call_node, __FILE__, __LINE__, "parse_primary_call_error");
                return NULL;
            }
            call_node->children[1].children = (ASTNode*)tracked_realloc(call_node->children[1].children, (call_node->children[1].child_count + 1) * sizeof(ASTNode), __FILE__, __LINE__, "parse_primary_call_args");
            // Use deep copy to properly copy the argument node
            deep_copy_ast_node(&call_node->children[1].children[call_node->children[1].child_count], arg);
            call_node->children[1].child_count++;
            tracked_free(arg, __FILE__, __LINE__, "parse_primary_call_arg");

            if (tokens[*current].type == TOKEN_COMMA) {
                (*current)++;
            } else if (tokens[*current].type != TOKEN_RPAREN) {
                fprintf(stderr, "Error: Expected ',' or ')' at line %d\n", tokens[*current].line);
                tracked_free(call_node, __FILE__, __LINE__, "parse_primary_call_syntax_error");
                return NULL;
            }
        }
        (*current)++; // Skip ')'
        
        // Free the original node since we're replacing it
        tracked_free(node, __FILE__, __LINE__, "parse_primary_call_replace");
        node = call_node;
    }

    return node;
}

// Helper function to parse expressions with operator precedence
static ASTNode* parse_expression(Token* tokens, int* current) {
    ASTNode* left = parse_primary(tokens, current);
    if (!left) {
        return NULL;
    }

    while (tokens[*current].type == TOKEN_OPERATOR) {
        char* op = tokens[*current].text;
        int op_prec = get_precedence(op);
        int op_line = tokens[*current].line;  // Store operator line number
        
        (*current)++;

        ASTNode* right = parse_primary(tokens, current);
        if (!right) {
            parser_free_ast(left);
            return NULL;
        }

        // Create operator node
        ASTNode* operator_node = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_expression_operator");
        if (!operator_node) {
            fprintf(stderr, "Error: Memory allocation failed\n");
            parser_free_ast(left);
            parser_free_ast(right);
            return NULL;
        }

        operator_node->type = AST_EXPR;
        operator_node->text = tracked_strdup(op, __FILE__, __LINE__, "parser");
        operator_node->implicit_function = NULL;  // Will be set during evaluation
        operator_node->children = (ASTNode*)tracked_malloc(2 * sizeof(ASTNode), __FILE__, __LINE__, "parse_expression_operator");
        operator_node->child_count = 2;
        operator_node->next = NULL;
        operator_node->line = op_line;  // Set operator line number
        

        
        // Move the left and right nodes directly instead of deep copying
        operator_node->children[0] = *left;
        operator_node->children[1] = *right;
        
        // Free the original nodes since we've moved their content
        tracked_free(left, __FILE__, __LINE__, "parse_expression_operator_move");
        tracked_free(right, __FILE__, __LINE__, "parse_expression_operator_move");
        
        left = operator_node;

        // Check for higher precedence operators
        while (tokens[*current].type == TOKEN_OPERATOR) {
            char* next_op = tokens[*current].text;
            int next_prec = get_precedence(next_op);
            if (next_prec <= op_prec) {
                break;
            }

            int next_op_line = tokens[*current].line;  // Store next operator line number
            (*current)++;
            ASTNode* next_right = parse_primary(tokens, current);
            if (!next_right) {
                parser_free_ast(left);
                return NULL;
            }

            // Create new operator node for higher precedence operator
            ASTNode* next_operator = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_expression_next_operator");
            if (!next_operator) {
                fprintf(stderr, "Error: Memory allocation failed\n");
                parser_free_ast(left);
                parser_free_ast(next_right);
                return NULL;
            }

            next_operator->type = AST_EXPR;
            next_operator->text = tracked_strdup(next_op, __FILE__, __LINE__, "parser");
            next_operator->implicit_function = NULL;  // Will be set during evaluation
            next_operator->children = (ASTNode*)tracked_malloc(2 * sizeof(ASTNode), __FILE__, __LINE__, "parse_expression_next_operator");
            next_operator->child_count = 2;
            next_operator->next = NULL;
            next_operator->line = next_op_line;  // Set next operator line number
            
            // Move the nodes directly instead of deep copying
            next_operator->children[0] = left->children[1];
            next_operator->children[1] = *next_right;
            
            // Free the new right node since we've moved its content
            tracked_free(next_right, __FILE__, __LINE__, "parse_expression_next_operator_move");
            
            // Update the right child of current operator by moving
            left->children[1] = *next_operator;
            
            // Free the next operator node since we've moved its content
            tracked_free(next_operator, __FILE__, __LINE__, "parse_expression_next_operator_move");
        }
    }

    // Check for ternary operator: condition ? true_expr : false_expr
    if (tokens[*current].type == TOKEN_QUESTION) {
        (*current)++; // Skip '?'
        
        ASTNode* true_expr = parse_expression(tokens, current);
        if (!true_expr) {
            parser_free_ast(left);
            return NULL;
        }
        
        if (tokens[*current].type != TOKEN_COLON) {
            fprintf(stderr, "Error: Expected ':' after '?' in ternary operator at line %d\n", tokens[*current].line);
            parser_free_ast(left);
            parser_free_ast(true_expr);
            return NULL;
        }
        (*current)++; // Skip ':'
        
        ASTNode* false_expr = parse_expression(tokens, current);
        if (!false_expr) {
            parser_free_ast(left);
            parser_free_ast(true_expr);
            return NULL;
        }
        
        // Create ternary operator node
        ASTNode* ternary_node = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_expression_ternary");
        if (!ternary_node) {
            fprintf(stderr, "Error: Memory allocation failed for ternary operator\n");
            parser_free_ast(left);
            parser_free_ast(true_expr);
            parser_free_ast(false_expr);
            return NULL;
        }
        
        // Initialize the AST node first
        init_ast_node(ternary_node);
        
        // Set ternary-specific fields
        ternary_node->type = AST_TERNARY;
        ternary_node->text = tracked_strdup("?:", __FILE__, __LINE__, "parser");
        ternary_node->children = (ASTNode*)tracked_malloc(3 * sizeof(ASTNode), __FILE__, __LINE__, "parse_expression_ternary");
        ternary_node->child_count = 3;
        ternary_node->line = tokens[*current - 3].line; // Line of the '?' token
        
        // Children: [condition, true_expr, false_expr]
        ternary_node->children[0] = *left;
        ternary_node->children[1] = *true_expr;
        ternary_node->children[2] = *false_expr;
        
        // Free the original nodes since we've moved their content
        tracked_free(left, __FILE__, __LINE__, "parse_expression_ternary_move");
        tracked_free(true_expr, __FILE__, __LINE__, "parse_expression_ternary_move");
        tracked_free(false_expr, __FILE__, __LINE__, "parse_expression_ternary_move");
        
        left = ternary_node;
    }

    return left;
}

// Helper function to parse a block of statements
static ASTNode* parse_block(Token* tokens, int* current, int token_count) {
    ASTNode* block = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_block");
    if (!block) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    block->type = AST_BLOCK;
    block->text = tracked_strdup("block", __FILE__, __LINE__, "parser");
    init_ast_node(block);

    while (tokens[*current].type != TOKEN_END && 
           tokens[*current].type != TOKEN_ELSE && 
           tokens[*current].type != TOKEN_CATCH) {
        
        // Skip any semicolons at the start
        while (tokens[*current].type == TOKEN_SEMICOLON) {
            (*current)++;
        }

        // If we hit end, break
        if (tokens[*current].type == TOKEN_END) {
            break;
        }

        ASTNode* stmt = parse_statement(tokens, current, token_count);
        if (!stmt) {
            parser_free_ast(block);
            return NULL;
        }
        
        // Allocate space for the new statement
        block->children = (ASTNode*)tracked_realloc(block->children, (block->child_count + 1) * sizeof(ASTNode), __FILE__, __LINE__, "parse_block");
        
        // Move the statement directly instead of deep copying
        block->children[block->child_count] = *stmt;
        
        // Deep copy the text to prevent corruption
        if (stmt->text) {
            block->children[block->child_count].text = tracked_strdup(stmt->text, __FILE__, __LINE__, "parser");
        }
        
        // Deep copy all children to prevent corruption
        if (stmt->children && stmt->child_count > 0) {
            block->children[block->child_count].children = (ASTNode*)tracked_malloc(stmt->child_count * sizeof(ASTNode), __FILE__, __LINE__, "parse_block");
            block->children[block->child_count].child_count = stmt->child_count;
            
            for (int j = 0; j < stmt->child_count; j++) {
                // Copy the child node
                block->children[block->child_count].children[j] = stmt->children[j];
                
                // Deep copy the child's text
                if (stmt->children[j].text) {
                    block->children[block->child_count].children[j].text = tracked_strdup(stmt->children[j].text, __FILE__, __LINE__, "parser");
                }
                
                // Deep copy the child's children recursively
                if (stmt->children[j].children && stmt->children[j].child_count > 0) {
                    block->children[block->child_count].children[j].children = (ASTNode*)tracked_malloc(stmt->children[j].child_count * sizeof(ASTNode), __FILE__, __LINE__, "parse_block");
                    block->children[block->child_count].children[j].child_count = stmt->children[j].child_count;
                    
                    for (int k = 0; k < stmt->children[j].child_count; k++) {
                        block->children[block->child_count].children[j].children[k] = stmt->children[j].children[k];
                        
                        if (stmt->children[j].children[k].text) {
                            block->children[block->child_count].children[j].children[k].text = tracked_strdup(stmt->children[j].children[k].text, __FILE__, __LINE__, "parser");
                        }
                    }
                }
            }
        }
        
        block->child_count++;
        
        // Clean up the source statement using tracked_free
        if (stmt->text) tracked_free(stmt->text, __FILE__, __LINE__, "parse_block");
        if (stmt->children) tracked_free(stmt->children, __FILE__, __LINE__, "parse_block");
        tracked_free(stmt, __FILE__, __LINE__, "parse_block");

        // Skip semicolon if present
        if (tokens[*current].type == TOKEN_SEMICOLON) {
            (*current)++;
        }

        // If we hit end, break
        if (tokens[*current].type == TOKEN_END) {
            break;
        }
    }

    // Skip the end token if we found one
    if (tokens[*current].type == TOKEN_END) {
        (*current)++;
    }

    return block;
}

// Helper function to safely copy an AST node (shallow copy)
// Recursive deep copy function for AST nodes
static void deep_copy_ast_node(ASTNode* dest, ASTNode* src) {
    dest->type = src->type;
    dest->line = src->line;
    dest->next = NULL;
    
    // Deep copy text
    if (src->text) {
        dest->text = tracked_strdup(src->text, __FILE__, __LINE__, "parser");
    } else {
        dest->text = NULL;
    }
    
    // Deep copy children recursively
    if (src->children && src->child_count > 0) {
        dest->children = (ASTNode*)tracked_malloc(src->child_count * sizeof(ASTNode), __FILE__, __LINE__, "deep_copy_ast_node");
        dest->child_count = src->child_count;
        
        for (int i = 0; i < src->child_count; i++) {
            deep_copy_ast_node(&dest->children[i], &src->children[i]);
        }
    } else {
        dest->children = NULL;
        dest->child_count = 0;
    }
}

// Helper function to parse statements
static ASTNode* parse_statement(Token* tokens, int* current, int token_count) {
    ASTNode* node = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement");
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }

    // Initialize common fields
    init_ast_node(node);


    
    // Handle 'use <path|string|identifier> as <identifier>'
    if (tokens[*current].type == TOKEN_USE) {
        int line = tokens[*current].line;
        (*current)++; // skip 'use'
        if (tokens[*current].type != TOKEN_PATH && tokens[*current].type != TOKEN_STRING && tokens[*current].type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Error: Expected module path or name after 'use' at line %d\n", tokens[*current].line);
            tracked_free(node, __FILE__, __LINE__, "parse_statement_use_error");
            return NULL;
        }
        char* path = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
        (*current)++;
        if (tokens[*current].type != TOKEN_AS) {
            fprintf(stderr, "Error: Expected 'as' after module path at line %d\n", tokens[*current].line);
            tracked_free(node, __FILE__, __LINE__, "parse_statement_use_as_error");
            tracked_free(path, __FILE__, __LINE__, "parse_statement_use_as_error");
            return NULL;
        }
        (*current)++; // skip 'as'
        if (tokens[*current].type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Error: Expected identifier after 'as' at line %d\n", tokens[*current].line);
            tracked_free(node, __FILE__, __LINE__, "parse_statement_use_alias_error");
            tracked_free(path, __FILE__, __LINE__, "parse_statement_use_alias_error");
            return NULL;
        }
        char* alias = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
        (*current)++;
        if (tokens[*current].type == TOKEN_SEMICOLON) (*current)++;
        node->type = AST_BLOCK;
        node->text = tracked_strdup("use", __FILE__, __LINE__, "parser");
        node->children = (ASTNode*)tracked_malloc(2 * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_use");
        node->child_count = 2;
        node->next = NULL;
        node->line = line;
        node->children[0].type = AST_EXPR; node->children[0].text = path; node->children[0].children = NULL; node->children[0].child_count = 0; node->children[0].next = NULL; node->children[0].line = line;
        node->children[1].type = AST_EXPR; node->children[1].text = alias; node->children[1].children = NULL; node->children[1].child_count = 0; node->children[1].next = NULL; node->children[1].line = line;
        return node;
    }

    // Handle default statements (restore original standalone handling)
    if (tokens[*current].type == TOKEN_DEFAULT) {
        (*current)++; // Skip 'default'
        if (tokens[*current].type != TOKEN_COLON) {
            fprintf(stderr, "Error: Expected ':' after default at line %d\n", tokens[*current].line);
            tracked_free(node, __FILE__, __LINE__, "parse_statement_default_error");
            return NULL;
        }
        (*current)++; // Skip ':'

        // Create default body node
        node->type = AST_DEFAULT;
        node->text = tracked_strdup("default", __FILE__, __LINE__, "parser");
        node->children = NULL;
        node->child_count = 0;
        node->next = NULL;

        // Parse the default case body as a block
        ASTNode* block = parse_block(tokens, current, token_count);
        if (!block) {
            parser_free_ast(node);
            return NULL;
        }

        // Add block as child of default node
        node->children = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_default");
        deep_copy_ast_node(&node->children[0], block);
        node->child_count = 1;

        return node;
    }

    // Now handle the specific statement types
    switch (tokens[*current].type) {
        case TOKEN_WHILE: {
            node->type = AST_WHILE;
            node->text = tracked_strdup("while", __FILE__, __LINE__, "parser");
            node->children = NULL;
            node->child_count = 0;
            node->next = NULL;
            (*current)++; // Skip 'while'

            // Parse condition
            ASTNode* condition = parse_expression(tokens, current);
            if (!condition) {
                parser_free_ast(node);
                return NULL;
            }

            if (tokens[*current].type != TOKEN_COLON) {
                fprintf(stderr, "Error: Expected ':' after while condition at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                parser_free_ast(condition);
                return NULL;
            }
            (*current)++; // Skip ':'

            // Parse while body
            ASTNode* while_body = parse_block(tokens, current, token_count);
            if (!while_body) {
                parser_free_ast(node);
                parser_free_ast(condition);
                return NULL;
            }

            // Add condition and body as children
            node->children = (ASTNode*)tracked_malloc(2 * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_while");
            node->children[0] = *condition;
            node->children[1] = *while_body;
            node->child_count = 2;
            break;
        }
        case TOKEN_IF: {
            node->type = AST_IF;
            node->text = tracked_strdup("if", __FILE__, __LINE__, "parser");
            node->children = NULL;
            node->child_count = 0;
            node->next = NULL;
            (*current)++; // Skip 'if'

            // Parse condition
            ASTNode* condition = parse_expression(tokens, current);
            if (!condition) {
                parser_free_ast(node);
                return NULL;
            }

            if (tokens[*current].type != TOKEN_COLON) {
                fprintf(stderr, "Error: Expected ':' after if condition at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                parser_free_ast(condition);
                return NULL;
            }
            (*current)++; // Skip ':'

            // Parse if body
            ASTNode* if_body = parse_block(tokens, current, token_count);
            if (!if_body) {
                parser_free_ast(node);
                parser_free_ast(condition);
                return NULL;
            }

            // Check for else clause
            ASTNode* else_body = NULL;
            if (tokens[*current].type == TOKEN_ELSE) {
                (*current)++; // Skip 'else'
                if (tokens[*current].type != TOKEN_COLON) {
                    fprintf(stderr, "Error: Expected ':' after else at line %d\n", tokens[*current].line);
                    parser_free_ast(node);
                    parser_free_ast(condition);
                    parser_free_ast(if_body);
                    return NULL;
                }
                (*current)++; // Skip ':'

                else_body = parse_block(tokens, current, token_count);
                if (!else_body) {
                    parser_free_ast(node);
                    parser_free_ast(condition);
                    parser_free_ast(if_body);
                    return NULL;
                }
            }

            // Add condition and bodies as children
            node->children = (ASTNode*)tracked_malloc((else_body ? 3 : 2) * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_if");
            node->children[0] = *condition;
            node->children[1] = *if_body;
            if (else_body) {
                node->children[2] = *else_body;
                node->child_count = 3;
            } else {
                node->child_count = 2;
            }
            break;
        }

        case TOKEN_FOR: {
            node->type = AST_FOR;
            node->for_type = AST_FOR_RANGE;  // Default to range loop
            node->text = tracked_strdup("for", __FILE__, __LINE__, "parser");
            node->children = NULL;
            node->child_count = 0;
            node->next = NULL;
            (*current)++; // Skip 'for'

            // Parse loop variable
            if (tokens[*current].type != TOKEN_IDENTIFIER) {
                fprintf(stderr, "Error: Expected loop variable at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                return NULL;
            }

            ASTNode* loop_var = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_for");
            if (!loop_var) {
                parser_free_ast(node);
                return NULL;
            }
            
            loop_var->type = AST_EXPR;
            loop_var->text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
            loop_var->children = NULL;
            loop_var->child_count = 0;
            loop_var->next = NULL;
            loop_var->for_type = AST_FOR_RANGE;  // Not used for this node type
            (*current)++; // Skip loop variable

            // Parse 'in' keyword
            if (tokens[*current].type != TOKEN_IN) {
                fprintf(stderr, "Error: Expected 'in' keyword at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                tracked_free(loop_var, __FILE__, __LINE__, "parse_statement_for");
                return NULL;
            }
            (*current)++; // Skip 'in'

            // Check if this is a range formed by a float literal ending with '.' followed by any token
            if (tokens[*current].type == TOKEN_FLOAT) {
                char* first_text = tokens[*current].text;
                int first_len = strlen(first_text);
                
                if (first_len > 0 && first_text[first_len - 1] == '.') {
                    // This is a float ending with '.', treat it as a range start

                    
                    // Create a range start node from the first part (without the trailing dot)
                    char* start_text = (char*)tracked_malloc(first_len, __FILE__, __LINE__, "parse_statement_for_start");
                    strncpy(start_text, first_text, first_len - 1);
                    start_text[first_len - 1] = '\0';
                    
                    ASTNode* range_start = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_for");
                    range_start->type = AST_EXPR;
                    range_start->text = start_text;
                    range_start->children = NULL;
                    range_start->child_count = 0;
                    range_start->next = NULL;
                    
                    // Skip the first float token
                    (*current)++;
                    
                    // Check if the next token is a dot (forming ..)
                    if (tokens[*current].type == TOKEN_DOT) {
                        (*current)++; // Skip the second dot
                    }
                    
                    // Declare range_end variable
                    ASTNode* range_end = NULL;
                    
                    // Check if the next token is a float starting with '.'
                    if (tokens[*current].type == TOKEN_FLOAT && tokens[*current].text[0] == '.') {
                        // Extract the numeric part after the dot
                        char* end_text = tracked_strdup(tokens[*current].text + 1, __FILE__, __LINE__, "parse_statement_for_end"); // Skip the leading '.'
                        
                        range_end = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_for");
                        range_end->type = AST_EXPR;
                        range_end->text = end_text;
                        range_end->children = NULL;
                        range_end->child_count = 0;
                        range_end->next = NULL;
                        
                        (*current)++; // Skip the second float token
                    } else {
                        // Parse range end normally
                        range_end = parse_expression(tokens, current);
                    }
                    if (!range_end) {
            
                        parser_free_ast(node);
                        tracked_free(loop_var, __FILE__, __LINE__, "parse_statement_for");
                        tracked_free(range_start, __FILE__, __LINE__, "parse_statement_for");
                        return NULL;
                    }
                    
                    // Parse final colon (required for all for loops)
                    if (tokens[*current].type != TOKEN_COLON) {
                        fprintf(stderr, "Error: Expected ':' after range end at line %d\n", tokens[*current].line);
                        parser_free_ast(node);
                        tracked_free(loop_var, __FILE__, __LINE__, "parse_statement_for");
                        tracked_free(range_start, __FILE__, __LINE__, "parse_statement_for");
                        tracked_free(range_end, __FILE__, __LINE__, "parse_statement_for");
                        return NULL;
                    }
                    (*current)++; // Skip final ':'
                    
                    // Parse loop body
                    ASTNode* loop_body = parse_block(tokens, current, token_count);
                    if (!loop_body) {
                        parser_free_ast(node);
                        tracked_free(loop_var, __FILE__, __LINE__, "parse_statement_for");
                        tracked_free(range_start, __FILE__, __LINE__, "parse_statement_for");
                        tracked_free(range_end, __FILE__, __LINE__, "parse_statement_for");
                        return NULL;
                    }
                    
                    // Add children: [loop_var, start, end, body]
                    node->children = (ASTNode*)tracked_malloc(4 * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_for");
                    if (!node->children) {
                        parser_free_ast(node);
                        tracked_free(loop_var, __FILE__, __LINE__, "parse_statement_for");
                        tracked_free(range_start, __FILE__, __LINE__, "parse_statement_for");
                        tracked_free(range_end, __FILE__, __LINE__, "parse_statement_for");
                        tracked_free(loop_body, __FILE__, __LINE__, "parse_statement_for");
                        return NULL;
                    }
                    
                    node->children[0] = *loop_var;
                    node->children[1] = *range_start;
                    node->children[2] = *range_end;
                    node->children[3] = *loop_body;
                    node->child_count = 4;
                    
                    // Clean up temporary nodes
                    tracked_free(loop_var, __FILE__, __LINE__, "parse_statement_for");
                    tracked_free(range_start, __FILE__, __LINE__, "parse_statement_for");
                    tracked_free(range_end, __FILE__, __LINE__, "parse_statement_for");
                    tracked_free(loop_body, __FILE__, __LINE__, "parse_statement_for");
                    
                    return node;
                }
            }
            
            // Parse range start or array variable
            ASTNode* range_start = parse_expression(tokens, current);
            if (!range_start) {
                parser_free_ast(node);
                tracked_free(loop_var, __FILE__, __LINE__, "parse_statement_for");
                return NULL;
            }

            // Check if this is an array loop (for i in array:) or range loop (for i in start..end:)
            int is_array_loop = 0;
            
            // If the range start is just an identifier, it might be an array variable
            if (range_start->type == AST_EXPR && range_start->text && 
                tokens[*current].type != TOKEN_DOT) {
                // This could be an array loop - check if the next token is a colon
                if (tokens[*current].type == TOKEN_COLON) {
                    is_array_loop = 1;
    
                }
            }
            
            if (is_array_loop) {
                // This is an array loop: for i in array:
                (*current)++; // Skip the colon
                
                // Parse loop body
                ASTNode* loop_body = parse_block(tokens, current, token_count);
                if (!loop_body) {
                parser_free_ast(node);
                tracked_free(loop_var, __FILE__, __LINE__, "parse_statement_for");
                tracked_free(range_start, __FILE__, __LINE__, "parse_statement_for");
                return NULL;
            }
                
                // Add children: [loop_var, array_var, body]
                node->children = (ASTNode*)tracked_malloc(3 * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_for");
                if (!node->children) {
                    parser_free_ast(node);
                    tracked_free(loop_var, __FILE__, __LINE__, "parse_statement_for");
                    tracked_free(range_start, __FILE__, __LINE__, "parse_statement_for");
                    tracked_free(loop_body, __FILE__, __LINE__, "parse_statement_for");
                    return NULL;
                }
                
                node->children[0] = *loop_var;
                node->children[1] = *range_start;  // This is the array variable
                node->children[2] = *loop_body;
                node->child_count = 3;
                node->for_type = AST_FOR_ARRAY;  // Mark as array loop
                
                // Clean up temporary nodes
                tracked_free(loop_var, __FILE__, __LINE__, "parse_statement_for");
                tracked_free(range_start, __FILE__, __LINE__, "parse_statement_for");
                tracked_free(loop_body, __FILE__, __LINE__, "parse_statement_for");
                
                return node;
            }
            
            // This is a range loop: for i in start..end:
            // Parse range separator (two dots)
            if (tokens[*current].type == TOKEN_DOT) {
                // Handle explicit dot-dot syntax
                (*current)++; // Skip first '.'
                
                // Check for second dot
                if (tokens[*current].type != TOKEN_DOT) {
                    fprintf(stderr, "Error: Expected '..' after range start at line %d\n", tokens[*current].line);
                    parser_free_ast(node);
                    tracked_free(loop_var, __FILE__, __LINE__, "parse_statement_for");
                    tracked_free(range_start, __FILE__, __LINE__, "parse_statement_for");
                    return NULL;
                }
                (*current)++; // Skip second '.'
            } else {
                fprintf(stderr, "Error: Expected range separator after range start at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                tracked_free(loop_var, __FILE__, __LINE__, "parse_statement_for");
                tracked_free(range_start, __FILE__, __LINE__, "parse_statement_for");
                return NULL;
            }

            // Parse range end
            ASTNode* range_end = parse_expression(tokens, current);
            if (!range_end) {
                parser_free_ast(node);
                tracked_free(loop_var, __FILE__, __LINE__, "parse_statement_for");
                tracked_free(range_start, __FILE__, __LINE__, "parse_statement_for");
                return NULL;
            }

            // Check for optional step (second colon) - but only if we're not at the final colon
            ASTNode* step = NULL;
            
            // A step separator is a colon followed by a number (not by a statement)
            // Handle both positive numbers (TOKEN_NUMBER) and negative numbers (TOKEN_OPERATOR + TOKEN_NUMBER)
            if (tokens[*current].type == TOKEN_COLON && 
                (tokens[*current + 1].type == TOKEN_NUMBER || 
                 (tokens[*current + 1].type == TOKEN_OPERATOR && tokens[*current + 1].text[0] == '-' && tokens[*current + 2].type == TOKEN_NUMBER))) {
                (*current)++; // Skip second ':'
                step = parse_expression(tokens, current);
                if (!step) {
                    parser_free_ast(node);
                    tracked_free(loop_var, __FILE__, __LINE__, "parse_statement_for");
                    tracked_free(range_start, __FILE__, __LINE__, "parse_statement_for");
                    tracked_free(range_end, __FILE__, __LINE__, "parse_statement_for");
                    return NULL;
                }
                node->for_type = AST_FOR_STEP;  // This is a step loop
            }

            // Parse final colon (required for all for loops)
            if (tokens[*current].type != TOKEN_COLON) {
                fprintf(stderr, "Error: Expected ':' after range end at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                tracked_free(loop_var, __FILE__, __LINE__, "parse_statement_for");
                tracked_free(range_start, __FILE__, __LINE__, "parse_statement_for");
                tracked_free(range_end, __FILE__, __LINE__, "parse_statement_for");
                if (step) tracked_free(step, __FILE__, __LINE__, "parse_statement_for");
                return NULL;
            }
            (*current)++; // Skip final ':'

            // Parse loop body
            ASTNode* loop_body = parse_block(tokens, current, token_count);
            if (!loop_body) {
                parser_free_ast(node);
                tracked_free(loop_var, __FILE__, __LINE__, "parse_statement_for");
                tracked_free(range_start, __FILE__, __LINE__, "parse_statement_for");
                tracked_free(range_end, __FILE__, __LINE__, "parse_statement_for");
                if (step) tracked_free(step, __FILE__, __LINE__, "parse_statement_for");
                return NULL;
            }

            // Allocate children array based on whether we have a step
            int child_count = step ? 5 : 4;
            node->children = (ASTNode*)tracked_malloc(child_count * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_for");
            if (!node->children) {
                parser_free_ast(node);
                tracked_free(loop_var, __FILE__, __LINE__, "parse_statement_for");
                tracked_free(range_start, __FILE__, __LINE__, "parse_statement_for");
                tracked_free(range_end, __FILE__, __LINE__, "parse_statement_for");
                if (step) tracked_free(step, __FILE__, __LINE__, "parse_statement_for");
                tracked_free(loop_body, __FILE__, __LINE__, "parse_statement_for");
                return NULL;
            }

            // Add children: [loop_var, start, end, step?, body]
            node->children[0] = *loop_var;
            node->children[1] = *range_start;
            node->children[2] = *range_end;
            if (step) {
                node->children[3] = *step;
                node->children[4] = *loop_body;
            } else {
            node->children[3] = *loop_body;
            }
            node->child_count = child_count;

            // Clean up individual nodes (they're now copied to children array)
            tracked_free(loop_var, __FILE__, __LINE__, "parse_statement_for");
            tracked_free(range_start, __FILE__, __LINE__, "parse_statement_for");
            tracked_free(range_end, __FILE__, __LINE__, "parse_statement_for");
            if (step) tracked_free(step, __FILE__, __LINE__, "parse_statement_for");
            tracked_free(loop_body, __FILE__, __LINE__, "parse_statement_for");
            
            break;
        }

        case TOKEN_SWITCH: {
            node->type = AST_SWITCH;
            node->text = tracked_strdup("switch", __FILE__, __LINE__, "parser");
            node->children = NULL;
            node->child_count = 0;
            node->next = NULL;
            (*current)++; // Skip 'switch'

            // Parse switch expression
            ASTNode* switch_expr = parse_expression(tokens, current);
            if (!switch_expr) {
                parser_free_ast(node);
                return NULL;
            }

            if (tokens[*current].type != TOKEN_COLON) {
                fprintf(stderr, "Error: Expected ':' after switch expression at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                parser_free_ast(switch_expr);
                return NULL;
            }
            (*current)++; // Skip ':'

            // Parse cases
            ASTNode* cases = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_switch");
            cases->type = AST_BLOCK;
            cases->text = tracked_strdup("cases", __FILE__, __LINE__, "parser");
            cases->children = NULL;
            cases->child_count = 0;
            cases->next = NULL;

            while (tokens[*current].type == TOKEN_CASE || tokens[*current].type == TOKEN_DEFAULT) {
                
                // Check if we've hit the end token
                if (tokens[*current].type == TOKEN_END) {
                    break;
                }
                
                if (tokens[*current].type == TOKEN_CASE) {
                    (*current)++; // Skip 'case'
                    ASTNode* case_expr = parse_expression(tokens, current);
                    if (!case_expr) {
                        parser_free_ast(node);
                        parser_free_ast(switch_expr);
                        parser_free_ast(cases);
                        return NULL;
                    }

                    if (tokens[*current].type != TOKEN_COLON) {
                        fprintf(stderr, "Error: Expected ':' after case expression at line %d\n", tokens[*current].line);
                        parser_free_ast(node);
                        parser_free_ast(switch_expr);
                        parser_free_ast(cases);
                        parser_free_ast(case_expr);
                        return NULL;
                    }
                    (*current)++; // Skip ':'

                    // Parse case body - special parsing for switch cases
                    ASTNode* case_body = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_switch_case_body");
                    if (!case_body) {
                        parser_free_ast(node);
                        parser_free_ast(switch_expr);
                        parser_free_ast(cases);
                        parser_free_ast(case_expr);
                        return NULL;
                    }
                    case_body->type = AST_BLOCK;
                    case_body->text = tracked_strdup("case_body", __FILE__, __LINE__, "parser");
                    case_body->children = NULL;
                    case_body->child_count = 0;
                    case_body->next = NULL;

                    // Parse case body statements until next case/default/end
                    while (tokens[*current].type != TOKEN_CASE && 
                           tokens[*current].type != TOKEN_DEFAULT &&
                           tokens[*current].type != TOKEN_END) {
                        
                        
                        // Skip any semicolons at the start
                        while (tokens[*current].type == TOKEN_SEMICOLON) {
                            (*current)++;
                        }

                        // If we hit end, break
                        if (tokens[*current].type == TOKEN_END) {
                            break;
                        }

                        ASTNode* stmt = parse_statement(tokens, current, token_count);
                        if (!stmt) {
                            parser_free_ast(case_body);
                            parser_free_ast(node);
                            parser_free_ast(switch_expr);
                            parser_free_ast(cases);
                            parser_free_ast(case_expr);
                            return NULL;
                        }
                        
                        // Add statement to case body
                        case_body->children = (ASTNode*)tracked_realloc(case_body->children, (case_body->child_count + 1) * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_switch_case_body");
                        case_body->children[case_body->child_count] = *stmt;
                        case_body->child_count++;
                        
                        // Clean up only the statement node itself, not its contents (they're now owned by case_body)
                        tracked_free(stmt, __FILE__, __LINE__, "parse_statement_switch_case_body");

                        // Skip semicolon if present
                        if (tokens[*current].type == TOKEN_SEMICOLON) {
                            (*current)++;
                        }
                    }

                    ASTNode* case_node = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_switch_case");
                    if (!case_node) {
                        return NULL;
                    }
                    case_node->type = AST_CASE;
                    case_node->text = tracked_strdup("case", __FILE__, __LINE__, "parser");
                    case_node->children = (ASTNode*)tracked_malloc(2 * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_switch_case");
                    if (!case_node->children) {
                        return NULL;
                    }
                    case_node->children[0] = *case_expr;
                    
                    // Deep copy the case body
                    case_node->children[1].type = case_body->type;
                    case_node->children[1].text = tracked_strdup(case_body->text, __FILE__, __LINE__, "parse_statement_switch_case");
                    case_node->children[1].children = NULL;
                    case_node->children[1].child_count = 0;
                    case_node->children[1].next = NULL;
                    
                    // Deep copy case body children
                    if (case_body->children && case_body->child_count > 0) {
                        case_node->children[1].children = (ASTNode*)tracked_malloc(case_body->child_count * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_switch_case");
                        if (!case_node->children[1].children) {
                            return NULL;
                        }
                        case_node->children[1].child_count = case_body->child_count;
                        
                        for (int j = 0; j < case_body->child_count; j++) {
                            // Use deep_copy_ast_node for proper recursive copying
                            deep_copy_ast_node(&case_node->children[1].children[j], &case_body->children[j]);
                        }
                    } else {
                    }
                    
                    case_node->child_count = 2;
                    case_node->next = NULL;

                    cases->children = (ASTNode*)tracked_realloc(cases->children, (cases->child_count + 1) * sizeof(ASTNode), __FILE__, __LINE__, "parse_switch_case");
                    cases->children[cases->child_count] = *case_node;
                    cases->child_count++;
                    
                    // Free the case body after copying
                    if (case_body->children) {
                        for (int j = 0; j < case_body->child_count; j++) {
                            if (case_body->children[j].text) {
                                tracked_free(case_body->children[j].text, __FILE__, __LINE__, "parse_statement_switch_case");
                            }
                        }
                        tracked_free(case_body->children, __FILE__, __LINE__, "parse_statement_switch_case");
                    }
                    if (case_body->text) {
                        tracked_free(case_body->text, __FILE__, __LINE__, "parse_statement_switch_case");
                    }
                    tracked_free(case_body, __FILE__, __LINE__, "parse_statement_switch_case");
                } else if (tokens[*current].type == TOKEN_DEFAULT) {
                    (*current)++; // Skip 'default'
                    if (tokens[*current].type != TOKEN_COLON) {
                        fprintf(stderr, "Error: Expected ':' after default at line %d\n", tokens[*current].line);
                        parser_free_ast(node);
                        parser_free_ast(switch_expr);
                        parser_free_ast(cases);
                        return NULL;
                    }
                    (*current)++; // Skip ':'

                    // Parse default body - special parsing for switch default
                    ASTNode* default_body = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_switch_default_body");
                    if (!default_body) {
                        parser_free_ast(node);
                        parser_free_ast(switch_expr);
                        parser_free_ast(cases);
                        return NULL;
                    }
                    default_body->type = AST_BLOCK;
                    default_body->text = tracked_strdup("default_body", __FILE__, __LINE__, "parser");
                    default_body->children = NULL;
                    default_body->child_count = 0;
                    default_body->next = NULL;

                    // Parse default body statements until end
                    while (tokens[*current].type != TOKEN_END) {
                        
                        // Skip any semicolons at the start
                        while (tokens[*current].type == TOKEN_SEMICOLON) {
                            (*current)++;
                        }

                        // If we hit end, break
                        if (tokens[*current].type == TOKEN_END) {
                            break;
                        }

                        ASTNode* stmt = parse_statement(tokens, current, token_count);
                        if (!stmt) {
                            parser_free_ast(default_body);
                            parser_free_ast(node);
                            parser_free_ast(switch_expr);
                            parser_free_ast(cases);
                            return NULL;
                        }
                        
                        // Add statement to default body
                        default_body->children = (ASTNode*)tracked_realloc(default_body->children, (default_body->child_count + 1) * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_switch_default_body");
                        default_body->children[default_body->child_count] = *stmt;
                        default_body->child_count++;
                        
                        // Clean up only the statement node itself, not its contents (they're now owned by default_body)
                        tracked_free(stmt, __FILE__, __LINE__, "parse_statement_switch_default_body");

                        // Skip semicolon if present
                        if (tokens[*current].type == TOKEN_SEMICOLON) {
                            (*current)++;
                        }
                    }

                    ASTNode* default_node = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_switch_default");
                    default_node->type = AST_DEFAULT;
                    default_node->text = tracked_strdup("default", __FILE__, __LINE__, "parser");
                    default_node->children = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_switch_default");
                    // Manual deep copy of the default body
                    default_node->children[0].type = default_body->type;
                    default_node->children[0].text = default_body->text ? tracked_strdup(default_body->text, __FILE__, __LINE__, "parse_statement_switch_default") : NULL;
                    default_node->children[0].children = NULL;
                    default_node->children[0].child_count = 0;
                    default_node->children[0].next = NULL;
                    default_node->children[0].line = default_body->line;
                    
                    // Deep copy default body children
                    if (default_body->children && default_body->child_count > 0) {
                        default_node->children[0].children = (ASTNode*)tracked_malloc(default_body->child_count * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_switch_default");
                        if (!default_node->children[0].children) {
                            return NULL;
                        }
                        default_node->children[0].child_count = default_body->child_count;
                        
                        for (int j = 0; j < default_body->child_count; j++) {
                            // Use deep_copy_ast_node for proper recursive copying
                            deep_copy_ast_node(&default_node->children[0].children[j], &default_body->children[j]);
                        }
                    }
                    default_node->child_count = 1;
                    default_node->next = NULL;

                    cases->children = (ASTNode*)tracked_realloc(cases->children, (cases->child_count + 1) * sizeof(ASTNode), __FILE__, __LINE__, "parse_switch_default");
                    cases->children[cases->child_count] = *default_node;
                    cases->child_count++;
                }
            }

            // Add switch expression and cases as children
            node->children = (ASTNode*)tracked_malloc(2 * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_switch");
            node->children[0] = *switch_expr;
            node->children[1] = *cases;
            node->child_count = 2;
            
            // Consume the 'end' token
            if (tokens[*current].type == TOKEN_END) {
                (*current)++;
            }
            
            break;
        }

        case TOKEN_TRY: {
            node->type = AST_TRY;
            node->text = tracked_strdup("try", __FILE__, __LINE__, "parser");
            node->children = NULL;
            node->child_count = 0;
            node->next = NULL;
            (*current)++; // Skip 'try'

            if (tokens[*current].type != TOKEN_COLON) {
                fprintf(stderr, "Error: Expected ':' after try at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                return NULL;
            }
            (*current)++; // Skip ':'

            // Parse try body
            ASTNode* try_body = parse_block(tokens, current, token_count);
            if (!try_body) {
                parser_free_ast(node);
                return NULL;
            }

            // Parse catch block
            if (tokens[*current].type != TOKEN_CATCH) {
                fprintf(stderr, "Error: Expected 'catch' at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                parser_free_ast(try_body);
                return NULL;
            }
            (*current)++; // Skip 'catch'

            // Parse error variable
            if (tokens[*current].type != TOKEN_IDENTIFIER) {
                fprintf(stderr, "Error: Expected error variable name at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                parser_free_ast(try_body);
                return NULL;
            }

            ASTNode* error_var = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_try");
            error_var->type = AST_EXPR;
            error_var->text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
            error_var->children = NULL;
            error_var->child_count = 0;
            error_var->next = NULL;
            (*current)++; // Skip error variable

            if (tokens[*current].type != TOKEN_COLON) {
                fprintf(stderr, "Error: Expected ':' after catch variable at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                parser_free_ast(try_body);
                parser_free_ast(error_var);
                return NULL;
            }
            (*current)++; // Skip ':'

            // Parse catch body
            ASTNode* catch_body = parse_block(tokens, current, token_count);
            if (!catch_body) {
                parser_free_ast(node);
                parser_free_ast(try_body);
                parser_free_ast(error_var);
                return NULL;
            }

            // Add try body, error variable, and catch body as children
            node->children = (ASTNode*)tracked_malloc(3 * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_try");
            node->children[0] = *try_body;
            node->children[1] = *error_var;
            node->children[2] = *catch_body;
            node->child_count = 3;
            break;
        }

        case TOKEN_FUNC: {
            // Handle old func syntax: func name(params): return_type: body end
            node->type = AST_FUNC;
            node->children = NULL;
            node->child_count = 0;
            node->next = NULL;
            node->line = tokens[*current].line;
            (*current)++; // Skip 'func'

            // Parse function name
            if (tokens[*current].type != TOKEN_IDENTIFIER) {
                fprintf(stderr, "Error: Expected function name at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                return NULL;
            }

            // Store the function name in the main node's text field
            node->text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
            
            ASTNode* func_name = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_func_name");
            func_name->type = AST_EXPR;
            func_name->text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
            func_name->children = NULL;
            func_name->child_count = 0;
            func_name->next = NULL;
            (*current)++; // Skip function name

            // Parse parameters
            if (tokens[*current].type != TOKEN_LPAREN) {
                fprintf(stderr, "Error: Expected '(' after function name at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                parser_free_ast(func_name);
                return NULL;
            }
            (*current)++; // Skip '('

            // Parse parameter list
            while (tokens[*current].type != TOKEN_RPAREN) {
                if (tokens[*current].type != TOKEN_IDENTIFIER) {
                    fprintf(stderr, "Error: Expected parameter name at line %d\n", tokens[*current].line);
                    parser_free_ast(node);
                    parser_free_ast(func_name);
                    return NULL;
                }

                ASTNode* param = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_func_param");
                if (!param) {
                    fprintf(stderr, "Error: Memory allocation failed\n");
                    parser_free_ast(node);
                    parser_free_ast(func_name);
                    return NULL;
                }
                param->type = AST_EXPR;
                param->text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
                param->children = NULL;
                param->child_count = 0;
                param->next = NULL;
                (*current)++; // Skip parameter name

                // Parse type annotation if present (optional for backward compatibility)
                if (tokens[*current].type == TOKEN_COLON) {
                    // Look ahead to see if this colon is followed by a type token
                    int lookahead = *current + 1;
                    if (lookahead < token_count && is_type_token(tokens[lookahead].type)) {
                        (*current)++; // Skip ':'
                        // Store type information in parameter
                        param->type_annotation = get_ast_type_from_token(tokens[*current].type);
                        param->type_text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parse_statement_func_param_type");
                        (*current)++; // Skip type
                        
                        // Check for generic type parameters
                        if (tokens[*current].type == TOKEN_OPERATOR && strcmp(tokens[*current].text, "<") == 0) {
                            // Create a temporary type node for generic parsing
                            ASTNode* temp_type = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_func_param_temp_type");
                            temp_type->type = param->type_annotation;
                            temp_type->text = tracked_strdup(param->type_text, __FILE__, __LINE__, "parse_statement_func_param_temp_type_text");
                            temp_type->children = NULL;
                            temp_type->child_count = 0;
                            temp_type->next = NULL;
                            temp_type->line = tokens[*current].line;
                            
                            temp_type = parse_generic_type(tokens, current, token_count, temp_type);
                            
                            // Update parameter with generic type info
                            param->type_annotation = temp_type->type;
                            tracked_free(param->type_text, __FILE__, __LINE__, "parse_statement_func_param_type_cleanup");
                            param->type_text = tracked_strdup(temp_type->text, __FILE__, __LINE__, "parse_statement_func_param_type_final");
                            
                            // Clean up temp type
                            tracked_free(temp_type, __FILE__, __LINE__, "parse_statement_func_param_temp_type_cleanup");
                        }
                    }
                    // If colon is not followed by a type token, leave it for function body parsing
                }
                // If no type annotation, param->type_annotation defaults to AST_TYPE_ANY

                // Add parameter to function
                node->children = (ASTNode*)tracked_realloc(node->children, (node->child_count + 1) * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_func_param");
                deep_copy_ast_node(&node->children[node->child_count], param);
                node->child_count++;

                // Skip comma if present
                if (tokens[*current].type == TOKEN_COMMA) {
                    (*current)++;
                }
            }
            (*current)++; // Skip ')'

            // Parse return type annotation if present (supports both : and -> syntax)
            if (tokens[*current].type == TOKEN_COLON || tokens[*current].type == TOKEN_ARROW) {
                (*current)++; // Skip ':' or '->'
                if (is_type_token(tokens[*current].type)) {
                    // Store return type in function name node
                    func_name->type_annotation = get_ast_type_from_token(tokens[*current].type);
                    func_name->type_text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parse_statement_func_return_type");
                    (*current)++; // Skip return type
                    
                    // Check for generic return type parameters
                    if (tokens[*current].type == TOKEN_LT) {
                        // Create a temporary type node for generic parsing
                        ASTNode* temp_type = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_func_return_temp_type");
                        temp_type->type = func_name->type_annotation;
                        temp_type->text = tracked_strdup(func_name->type_text, __FILE__, __LINE__, "parse_statement_func_return_temp_type_text");
                        temp_type->children = NULL;
                        temp_type->child_count = 0;
                        temp_type->next = NULL;
                        temp_type->line = tokens[*current].line;
                        
                        temp_type = parse_generic_type(tokens, current, token_count, temp_type);
                        
                        // Update return type with generic type info
                        func_name->type_annotation = temp_type->type;
                        tracked_free(func_name->type_text, __FILE__, __LINE__, "parse_statement_func_return_type_cleanup");
                        func_name->type_text = tracked_strdup(temp_type->text, __FILE__, __LINE__, "parse_statement_func_return_type_final");
                        
                        // Clean up temp type
                        tracked_free(temp_type, __FILE__, __LINE__, "parse_statement_func_return_temp_type_cleanup");
                    }
                } else {
                    // Return type annotation is optional, so this might be a different colon/arrow
                    // Back up and continue without return type annotation
                    (*current)--; // Go back to ':' or '->'
                    // Don't set return type annotation - it will default to AST_TYPE_ANY
                }
            }

            // Parse function body
            if (tokens[*current].type != TOKEN_COLON) {
                fprintf(stderr, "Error: Expected ':' after function signature at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                parser_free_ast(func_name);
                return NULL;
            }
            (*current)++; // Skip ':'

            ASTNode* func_body = parse_block(tokens, current, token_count);
            if (!func_body) {
                parser_free_ast(node);
                parser_free_ast(func_name);
                return NULL;
            }

            // Add function name and body as children
            node->children = (ASTNode*)tracked_realloc(node->children, (node->child_count + 2) * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_func_children");
            deep_copy_ast_node(&node->children[node->child_count], func_name);
            deep_copy_ast_node(&node->children[node->child_count + 1], func_body);
            node->child_count += 2;

            // Clean up local nodes
            tracked_free(func_name, __FILE__, __LINE__, "parse_statement_func_name_cleanup");
            break;
        }

        case TOKEN_LET: {
            node->type = AST_LET;
            node->text = tracked_strdup("let", __FILE__, __LINE__, "parser");
            node->children = NULL;
            node->child_count = 0;
            node->next = NULL;
            node->line = tokens[*current].line;
            (*current)++; // Skip 'let'

            // Parse variable name
            if (tokens[*current].type != TOKEN_IDENTIFIER) {
                // Check if this is a type token being used as a variable name
                if (is_type_token(tokens[*current].type)) {
                    fprintf(stderr, "Error: Variable name '%s' conflicts with type name at line %d\n", tokens[*current].text, tokens[*current].line);
                } else {
                fprintf(stderr, "Error: Expected variable name at line %d\n", tokens[*current].line);
                }
                parser_free_ast(node);
                return NULL;
            }

            ASTNode* var_name = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_let");
            var_name->type = AST_EXPR;
            var_name->text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
            var_name->children = NULL;
            var_name->child_count = 0;
            var_name->next = NULL;
            
            // Check if variable name conflicts with type names

            
            (*current)++; // Skip variable name

            // NEW: v2.0 Check for type annotation first
            ASTNode* type_annotation = NULL;
            if (tokens[*current].type == TOKEN_COLON) {
                (*current)++; // Skip ':'
                
                // Parse type annotation
                if (is_type_token(tokens[*current].type)) {
                    type_annotation = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_type_annotation");
                    type_annotation->type = get_ast_type_from_token(tokens[*current].type);
                    type_annotation->text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parse_statement_type_text");
                    type_annotation->children = NULL;
                    type_annotation->child_count = 0;
                    type_annotation->next = NULL;
                    type_annotation->line = tokens[*current].line;
                    (*current)++; // Skip type
                    
                    // Check for generic type parameters (e.g., array<int>)
                    if (tokens[*current].type == TOKEN_OPERATOR && strcmp(tokens[*current].text, "<") == 0) {
                        type_annotation = parse_generic_type(tokens, current, token_count, type_annotation);
                    }
                } else {
                    fprintf(stderr, "Error: Expected type annotation at line %d\n", tokens[*current].line);
                    parser_free_ast(node);
                    parser_free_ast(var_name);
                    return NULL;
                }
            }

            // Check if this is a function definition (has parentheses) or variable assignment (has =)
            if (tokens[*current].type == TOKEN_LPAREN) {
                // This is a function definition
                node->type = AST_FUNC;
                node->text = tracked_strdup(var_name->text, __FILE__, __LINE__, "parser");
                free(var_name->text);
                free(var_name);
                
                // Parse parameters
                (*current)++; // Skip '('
                while (tokens[*current].type != TOKEN_RPAREN) {
                    if (tokens[*current].type != TOKEN_IDENTIFIER) {
                        fprintf(stderr, "Error: Expected parameter name at line %d\n", tokens[*current].line);
                        parser_free_ast(node);
                        return NULL;
                    }

                    ASTNode* param = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_func_param");
                    if (!param) {
                        fprintf(stderr, "Error: Memory allocation failed\n");
                        parser_free_ast(node);
                        return NULL;
                    }
                    param->type = AST_EXPR;
                    param->text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
                    param->children = NULL;
                    param->child_count = 0;
                    param->next = NULL;
                    (*current)++; // Skip parameter name

                    // Parse type annotation if present (optional for backward compatibility)
                    if (tokens[*current].type == TOKEN_COLON) {
                        // Look ahead to see if this colon is followed by a type token
                        int lookahead = *current + 1;
                        if (lookahead < token_count && is_type_token(tokens[lookahead].type)) {
                        (*current)++; // Skip ':'
                            // Store type information in parameter
                            param->type_annotation = get_ast_type_from_token(tokens[*current].type);
                            param->type_text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parse_statement_func_param_type");
                        (*current)++; // Skip type
                            
                            // Check for generic type parameters
                            if (tokens[*current].type == TOKEN_OPERATOR && strcmp(tokens[*current].text, "<") == 0) {
                                // Create a temporary type node for generic parsing
                                ASTNode* temp_type = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_func_param_temp_type");
                                temp_type->type = param->type_annotation;
                                temp_type->text = tracked_strdup(param->type_text, __FILE__, __LINE__, "parse_statement_func_param_temp_type_text");
                                temp_type->children = NULL;
                                temp_type->child_count = 0;
                                temp_type->next = NULL;
                                temp_type->line = tokens[*current].line;
                                
                                temp_type = parse_generic_type(tokens, current, token_count, temp_type);
                                
                                // Update parameter with generic type info
                                param->type_annotation = temp_type->type;
                                tracked_free(param->type_text, __FILE__, __LINE__, "parse_statement_func_param_type_cleanup");
                                param->type_text = tracked_strdup(temp_type->text, __FILE__, __LINE__, "parse_statement_func_param_type_final");
                                
                                // Clean up temp type
                                tracked_free(temp_type, __FILE__, __LINE__, "parse_statement_func_param_temp_type_cleanup");
                            }
                        }
                        // If colon is not followed by a type token, leave it for function body parsing
                    }

                    // Add parameter to function
                    node->children = (ASTNode*)tracked_realloc(node->children, (node->child_count + 1) * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_func_param");
                    deep_copy_ast_node(&node->children[node->child_count], param);
                    node->child_count++;

                    // Skip comma if present
                    if (tokens[*current].type == TOKEN_COMMA) {
                        (*current)++;
                    }
                }
                (*current)++; // Skip ')'

                // Parse function body (supports both : and -> syntax)
                if (tokens[*current].type != TOKEN_COLON && tokens[*current].type != TOKEN_ARROW) {
                    fprintf(stderr, "Error: Expected ':' or '->' after function parameters at line %d\n", tokens[*current].line);
                    parser_free_ast(node);
                    return NULL;
                }
                (*current)++; // Skip ':' or '->'

                ASTNode* func_body = parse_block(tokens, current, token_count);
                if (!func_body) {
                    parser_free_ast(node);
                    return NULL;
                }

                // Add function body as child
                node->children = (ASTNode*)tracked_realloc(node->children, (node->child_count + 1) * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_func_body");
                deep_copy_ast_node(&node->children[node->child_count], func_body);
                node->child_count++;
            } else {
                // This is a variable assignment
            // Parse assignment operator
            if (tokens[*current].type != TOKEN_ASSIGN) {
                fprintf(stderr, "Error: Expected '=' at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                parser_free_ast(var_name);
                return NULL;
            }
            (*current)++; // Skip '='

            // Parse initial value - check if it's a lambda expression
            ASTNode* init_value = NULL;
            
            // Check if this is a lambda expression: x => expression or (x, y) => expression
            if (tokens[*current].type == TOKEN_IDENTIFIER || tokens[*current].type == TOKEN_LPAREN) {
                // Look ahead to see if this is followed by =>
                int lookahead = *current;
                int param_count = 0;
                
                if (tokens[lookahead].type == TOKEN_IDENTIFIER) {
                    // Single parameter: x => expression
                    param_count = 1;
                    lookahead++;
                } else if (tokens[lookahead].type == TOKEN_LPAREN) {
                    // Multiple parameters: (x, y) => expression
                    lookahead++; // Skip '('
                    while (tokens[lookahead].type != TOKEN_RPAREN && lookahead < token_count) {
                        if (tokens[lookahead].type == TOKEN_IDENTIFIER) {
                            param_count++;
                            lookahead++;
                            if (tokens[lookahead].type == TOKEN_COMMA) {
                                lookahead++; // Skip comma
                            }
                        } else {
                            break;
                        }
                    }
                    if (tokens[lookahead].type == TOKEN_RPAREN) {
                        lookahead++; // Skip ')'
                    }
                }
                
                // Check if next token is =>
                if (tokens[lookahead].type == TOKEN_LAMBDA) {
                    // This is a lambda expression!
                    ASTNode* lambda_node = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_lambda");
                    if (!lambda_node) {
                        fprintf(stderr, "Error: Memory allocation failed\n");
                parser_free_ast(node);
                parser_free_ast(var_name);
                        return NULL;
                    }
                    
                    lambda_node->type = AST_LAMBDA;
                    lambda_node->text = tracked_strdup("lambda", __FILE__, __LINE__, "parser");
                    lambda_node->children = (ASTNode*)tracked_malloc(2 * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_lambda_params");
                    lambda_node->child_count = 2;
                    lambda_node->next = NULL;
                    lambda_node->line = tokens[*current].line;
                    
                    // Parse parameters
                    if (param_count == 1 && tokens[*current].type == TOKEN_IDENTIFIER) {
                        // Single parameter: x
                        // First child: parameter
                        lambda_node->children[0].type = AST_EXPR;
                        lambda_node->children[0].text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
                        lambda_node->children[0].children = NULL;
                        lambda_node->children[0].child_count = 0;
                        lambda_node->children[0].next = NULL;
                        lambda_node->children[0].line = tokens[*current].line;
                        (*current)++; // Skip parameter
                    } else if (tokens[*current].type == TOKEN_LPAREN) {
                        // Multiple parameters: (x, y)
                        (*current)++; // Skip '('
                        
                                            // First child: parameter list
                    ASTNode* param_list = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_lambda_param_list");
                    param_list->type = AST_EXPR;
                    param_list->text = tracked_strdup("params", __FILE__, __LINE__, "parser");
                    param_list->children = (ASTNode*)tracked_malloc(param_count * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_lambda_param_array");
                    param_list->child_count = param_count;
                    param_list->next = NULL;
                    param_list->line = tokens[*current].line;
                    

                        
                        for (int i = 0; i < param_count; i++) {
                            if (tokens[*current].type != TOKEN_IDENTIFIER) {
                                fprintf(stderr, "Error: Expected parameter name at line %d\n", tokens[*current].line);
                                parser_free_ast(lambda_node);
                                parser_free_ast(param_list);
                    parser_free_ast(node);
                                parser_free_ast(var_name);
                                return NULL;
                            }
                            
                            param_list->children[i].type = AST_EXPR;
                            param_list->children[i].text = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
                            param_list->children[i].children = NULL;
                            param_list->children[i].child_count = 0;
                            param_list->children[i].next = NULL;
                            param_list->children[i].line = tokens[*current].line;
                            (*current)++; // Skip parameter
                            
                            if (i < param_count - 1 && tokens[*current].type == TOKEN_COMMA) {
                                (*current)++; // Skip comma
                            }
                        }
                        
                        if (tokens[*current].type != TOKEN_RPAREN) {
                            fprintf(stderr, "Error: Expected ')' after parameters at line %d\n", tokens[*current].line);
                            parser_free_ast(lambda_node);
                            parser_free_ast(param_list);
                            parser_free_ast(node);
                            parser_free_ast(var_name);
                            return NULL;
                        }
                        (*current)++; // Skip ')'
                        
                        lambda_node->children[0] = *param_list;
                        tracked_free(param_list, __FILE__, __LINE__, "parse_statement_lambda_param_list_move");
                    }
                    
                    // Skip the => token
                    if (tokens[*current].type != TOKEN_LAMBDA) {
                        fprintf(stderr, "Error: Expected '=>' after parameters at line %d\n", tokens[*current].line);
                        parser_free_ast(lambda_node);
                        parser_free_ast(node);
                        parser_free_ast(var_name);
                        return NULL;
                    }
                    (*current)++; // Skip '=>'
                    
                    // Parse lambda body (expression)
                    ASTNode* body = parse_expression(tokens, current);
                    if (!body) {
                        parser_free_ast(lambda_node);
                        parser_free_ast(node);
                        parser_free_ast(var_name);
                        return NULL;
                    }
                    
                    // Second child: body
                    lambda_node->children[1] = *body;
                    tracked_free(body, __FILE__, __LINE__, "parse_statement_lambda_body_move");
                    
                    init_value = lambda_node;
                } else {
                    // Not a lambda, parse as regular expression
                    init_value = parse_expression(tokens, current);
                }
            } else {
                // Not a lambda, parse as regular expression
                init_value = parse_expression(tokens, current);
            }
            
            if (!init_value) {
                parser_free_ast(node);
                parser_free_ast(var_name);
                return NULL;
            }

            if (tokens[*current].type != TOKEN_SEMICOLON) {
                fprintf(stderr, "Error: Expected ';' after variable declaration at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                parser_free_ast(var_name);
                parser_free_ast(init_value);
                return NULL;
            }
            (*current)++; // Skip ';'



            // Add variable name, type annotation (if present), and initial value as children
            int child_count = type_annotation ? 3 : 2;
            node->children = (ASTNode*)tracked_malloc(child_count * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_let_var");
            node->children[0] = *var_name;
            
            if (type_annotation) {
                // Store type annotation in the variable name node
                node->children[0].type_annotation = type_annotation->type;
                node->children[0].type_text = tracked_strdup(type_annotation->text, __FILE__, __LINE__, "parse_statement_let_var_type");
                
                // Add type annotation as second child
                node->children[1] = *type_annotation;
                node->children[2] = *init_value;
                node->child_count = 3;
            } else {
            node->children[1] = *init_value;
            node->child_count = 2;
            }
            }
            break;
        }

        case TOKEN_RETURN: {
            node->type = AST_RETURN;
            node->text = tracked_strdup("return", __FILE__, __LINE__, "parser");
            node->children = NULL;
            node->child_count = 0;
            node->next = NULL;
            (*current)++; // Skip 'return'

            // Check if there's a return expression or just a semicolon
            if (tokens[*current].type == TOKEN_SEMICOLON) {
                // No return value, just return;
                (*current)++; // Skip ';'
                node->child_count = 0;
            } else {
            // Parse return expression
            ASTNode* return_expr = parse_expression(tokens, current);
            if (!return_expr) {
                parser_free_ast(node);
                return NULL;
            }

            if (tokens[*current].type != TOKEN_SEMICOLON) {
                fprintf(stderr, "Error: Expected ';' after return statement at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                parser_free_ast(return_expr);
                return NULL;
            }
            (*current)++; // Skip ';'

                node->children = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_return");
            node->children[0] = *return_expr;
            node->child_count = 1;
            }
            break;
        }

        case TOKEN_PRINT: {
            node->type = AST_PRINT;
            node->text = tracked_strdup("print", __FILE__, __LINE__, "parser");
            node->children = NULL;
            node->child_count = 0;
            node->next = NULL;
            (*current)++; // Skip 'print'

            // Parse print arguments
            if (tokens[*current].type != TOKEN_LPAREN) {
                fprintf(stderr, "Error: Expected '(' after print at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                return NULL;
            }
            (*current)++; // Skip '('

            while (tokens[*current].type != TOKEN_RPAREN) {
                
                ASTNode* arg = parse_expression(tokens, current);
                if (!arg) {
                    parser_free_ast(node);
                    return NULL;
                }

                node->children = (ASTNode*)tracked_realloc(node->children, (node->child_count + 1) * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_print");
                // Move the arg node directly instead of deep copying
                node->children[node->child_count] = *arg;
                
                // Deep copy the text to prevent corruption
                if (arg->text) {
                    node->children[node->child_count].text = tracked_strdup(arg->text, __FILE__, __LINE__, "parser");
                }
                
                node->child_count++;

                if (tokens[*current].type == TOKEN_COMMA) {
                    (*current)++; // Skip ','
                } else if (tokens[*current].type != TOKEN_RPAREN) {
                    fprintf(stderr, "Error: Expected ',' or ')' at line %d\n", tokens[*current].line);
                    parser_free_ast(node);
                    return NULL;
                }
            }
            (*current)++; // Skip ')'

            if (tokens[*current].type != TOKEN_SEMICOLON) {
                fprintf(stderr, "Error: Expected ';' after print statement at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                return NULL;
            }
            (*current)++; // Skip ';'
            
            break;
        }

        case TOKEN_UPRINT: {
            node->type = AST_UPRINT;
            node->text = tracked_strdup("uprint", __FILE__, __LINE__, "parser");
            node->children = NULL;
            node->child_count = 0;
            node->next = NULL;
            (*current)++; // Skip 'uprint'

            // Parse uprint arguments
            if (tokens[*current].type != TOKEN_LPAREN) {
                fprintf(stderr, "Error: Expected '(' after uprint at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                return NULL;
            }
            (*current)++; // Skip '('

            while (tokens[*current].type != TOKEN_RPAREN) {
                
                ASTNode* arg = parse_expression(tokens, current);
                if (!arg) {
                    parser_free_ast(node);
                    return NULL;
                }

                node->children = (ASTNode*)tracked_realloc(node->children, (node->child_count + 1) * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_uprint");
                // Move the arg node directly instead of deep copying
                node->children[node->child_count] = *arg;
                
                // Deep copy the text to prevent corruption
                if (arg->text) {
                    node->children[node->child_count].text = tracked_strdup(arg->text, __FILE__, __LINE__, "parser");
                }
                
                node->child_count++;

                if (tokens[*current].type == TOKEN_COMMA) {
                    (*current)++; // Skip ','
                } else if (tokens[*current].type != TOKEN_RPAREN) {
                    fprintf(stderr, "Error: Expected ',' or ')' at line %d\n", tokens[*current].line);
                    parser_free_ast(node);
                    return NULL;
                }
            }
            (*current)++; // Skip ')'

            if (tokens[*current].type != TOKEN_SEMICOLON) {
                fprintf(stderr, "Error: Expected ';' after uprint statement at line %d\n", tokens[*current].line);
                parser_free_ast(node);
                return NULL;
            }
            (*current)++; // Skip ';'
            
            break;
        }

        case TOKEN_IDENTIFIER: {
            // Check if this is a bracket assignment statement (identifier[index] = expression)
            if (tokens[*current + 1].type == TOKEN_LBRACKET) {
                // This could be array assignment or object bracket assignment
                // We'll use a heuristic: default to object bracket assignment
                char* identifier_name = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
                int is_object_assignment = 1;  // Default to object assignment
                (*current)++; // Skip identifier
                (*current)++; // Skip '['
                
                // Parse the index expression
                ASTNode* index_expr = parse_expression(tokens, current);
                if (!index_expr) {
                    free(identifier_name);
                    parser_free_ast(node);
                    return NULL;
                }
                
                // Check for closing bracket
                if (tokens[*current].type != TOKEN_RBRACKET) {
                    fprintf(stderr, "Error: Expected ']' after bracket index at line %d\n", tokens[*current].line);
                    free(identifier_name);
                    parser_free_ast(node);
                    parser_free_ast(index_expr);
                    return NULL;
                }
                (*current)++; // Skip ']'
                
                // Check for assignment operator
                if (tokens[*current].type != TOKEN_ASSIGN) {
                    fprintf(stderr, "Error: Expected '=' after bracket index at line %d\n", tokens[*current].line);
                    free(identifier_name);
                    parser_free_ast(node);
                    parser_free_ast(index_expr);
                    return NULL;
                }
                (*current)++; // Skip '='
                
                // Parse the value expression
                ASTNode* value_expr = parse_expression(tokens, current);
                if (!value_expr) {
                    free(identifier_name);
                    parser_free_ast(node);
                    parser_free_ast(index_expr);
                    return NULL;
                }
                
                // Check for semicolon
                if (tokens[*current].type != TOKEN_SEMICOLON) {
                    fprintf(stderr, "Error: Expected ';' after array assignment at line %d\n", tokens[*current].line);
                    free(identifier_name);
                    parser_free_ast(node);
                    parser_free_ast(index_expr);
                    parser_free_ast(value_expr);
                    return NULL;
                }
                (*current)++; // Skip ';'
                
                // Create appropriate assignment node based on heuristic
                if (is_object_assignment) {
                    node->type = AST_OBJECT_BRACKET_ASSIGN;
                    node->text = tracked_strdup("bracket_assign", __FILE__, __LINE__, "parser");
                } else {
                node->type = AST_ARRAY_ASSIGN;
                    node->text = tracked_strdup("array_assign", __FILE__, __LINE__, "parser");
                }
                node->children = (ASTNode*)tracked_malloc(3 * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_array_assignment");
                node->child_count = 3;
                node->next = NULL;
                node->line = tokens[*current - 3].line; // Line of the identifier
                
                // First child: identifier name (object or array)
                node->children[0].type = AST_EXPR;
                node->children[0].text = identifier_name;
                node->children[0].children = NULL;
                node->children[0].child_count = 0;
                node->children[0].next = NULL;
                node->children[0].line = node->line;
                
                // Second child: index expression
                deep_copy_ast_node(&node->children[1], index_expr);
                
                // Third child: value expression
                deep_copy_ast_node(&node->children[2], value_expr);
                
                // Clean up
                parser_free_ast(index_expr);
                parser_free_ast(value_expr);
                break;
            }
            
            // Check if this is a chained property assignment (obj.prop1.prop2... = expression)
            // Scan ahead to detect the pattern: identifier (.identifier)* = 
            int lookahead = *current;
            int chain_depth = 0;
            
            // Must start with identifier
            if (tokens[lookahead].type == TOKEN_IDENTIFIER) {
                lookahead++;
                chain_depth++;
                
                // Count chained properties: .identifier .identifier ...
                while (lookahead < token_count &&
                       tokens[lookahead].type == TOKEN_DOT && 
                       lookahead + 1 < token_count &&
                       tokens[lookahead + 1].type == TOKEN_IDENTIFIER) {
                    lookahead += 2; // Skip .identifier
                    chain_depth++;
                }
                
                // Check if this chain ends with assignment
                if (lookahead < token_count && tokens[lookahead].type == TOKEN_ASSIGN && chain_depth >= 2) {
                    // Handle different chain depths
                    if (chain_depth == 2) {
                        // Simple assignment: obj.prop = value
                        char* obj_name = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
                        char* prop_name = tracked_strdup(tokens[*current + 2].text, __FILE__, __LINE__, "parser");
                        (*current) += 3; // Skip obj.prop=
                        
                // Parse the value expression
                ASTNode* value_expr = parse_expression(tokens, current);
                if (!value_expr) {
                    free(obj_name);
                    free(prop_name);
                    parser_free_ast(node);
                    return NULL;
                }
                
                // Check for semicolon
                        if (tokens[*current].type == TOKEN_SEMICOLON) {
                (*current)++; // Skip ';'
                        }
                
                // Create object assignment node
                node->type = AST_OBJECT_ASSIGN;
                        node->text = tracked_strdup("object_assign", __FILE__, __LINE__, "parser");
                node->children = (ASTNode*)tracked_malloc(3 * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_object_assign");
                node->child_count = 3;
                node->next = NULL;
                        node->line = tokens[*current - 3].line;
                
                // Set object name, property name, and value
                node->children[0].type = AST_EXPR;
                node->children[0].text = obj_name;
                node->children[0].children = NULL;
                node->children[0].child_count = 0;
                node->children[0].next = NULL;
                node->children[0].line = node->line;
                
                node->children[1].type = AST_EXPR;
                node->children[1].text = prop_name;
                node->children[1].children = NULL;
                node->children[1].child_count = 0;
                node->children[1].next = NULL;
                node->children[1].line = node->line;
                
                // Copy the value expression
                deep_copy_ast_node(&node->children[2], value_expr);
                parser_free_ast(value_expr);
                break;
                    }
                    else if (chain_depth == 3) {
                        // 3-level assignment: obj.prop1.prop2 = value
                        char* obj_name = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
                        char* prop1_name = tracked_strdup(tokens[*current + 2].text, __FILE__, __LINE__, "parser");
                        char* prop2_name = tracked_strdup(tokens[*current + 4].text, __FILE__, __LINE__, "parser");
                        (*current) += 5; // Skip obj.prop1.prop2=
                        
                        // Parse the value expression
                        ASTNode* value_expr = parse_expression(tokens, current);
                        if (!value_expr) {
                            free(obj_name);
                            free(prop1_name);
                            free(prop2_name);
                            parser_free_ast(node);
                            return NULL;
                        }
                        
                        // Check for semicolon
                        if (tokens[*current].type == TOKEN_SEMICOLON) {
                            (*current)++; // Skip ';'
                        }
                        
                        // Create 3-level nested assignment node
                        node->type = AST_OBJECT_ASSIGN;
                        node->text = tracked_strdup("nested_assign_3", __FILE__, __LINE__, "parser");
                        node->children = (ASTNode*)tracked_malloc(4 * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_3level_assign");
                        node->child_count = 4;
                        node->next = NULL;
                        node->line = tokens[*current - 5].line;
                        
                        // Set all property names and value
                        node->children[0].type = AST_EXPR;
                        node->children[0].text = obj_name;
                        node->children[0].children = NULL;
                        node->children[0].child_count = 0;
                        node->children[0].next = NULL;
                        node->children[0].line = node->line;
                        
                        node->children[1].type = AST_EXPR;
                        node->children[1].text = prop1_name;
                        node->children[1].children = NULL;
                        node->children[1].child_count = 0;
                        node->children[1].next = NULL;
                        node->children[1].line = node->line;
                        
                        node->children[2].type = AST_EXPR;
                        node->children[2].text = prop2_name;
                        node->children[2].children = NULL;
                        node->children[2].child_count = 0;
                        node->children[2].next = NULL;
                        node->children[2].line = node->line;
                        
                        // Copy the value expression
                        deep_copy_ast_node(&node->children[3], value_expr);
                        parser_free_ast(value_expr);
                        break;
                    }
                    else if (chain_depth == 4) {
                        // 4-level assignment: obj.prop1.prop2.prop3 = value
                        char* obj_name = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
                        char* prop1_name = tracked_strdup(tokens[*current + 2].text, __FILE__, __LINE__, "parser");
                        char* prop2_name = tracked_strdup(tokens[*current + 4].text, __FILE__, __LINE__, "parser");
                        char* prop3_name = tracked_strdup(tokens[*current + 6].text, __FILE__, __LINE__, "parser");
                        (*current) += 7; // Skip obj.prop1.prop2.prop3=
                        
                        // Parse the value expression
                        ASTNode* value_expr = parse_expression(tokens, current);
                        if (!value_expr) {
                            free(obj_name);
                            free(prop1_name);
                            free(prop2_name);
                            free(prop3_name);
                            parser_free_ast(node);
                            return NULL;
                        }
                        
                        // Check for semicolon
                        if (tokens[*current].type == TOKEN_SEMICOLON) {
                            (*current)++; // Skip ';'
                        }
                        
                        // Create 4-level nested assignment node
                        node->type = AST_OBJECT_ASSIGN;
                        node->text = tracked_strdup("nested_assign_4", __FILE__, __LINE__, "parser");
                        node->children = (ASTNode*)tracked_malloc(5 * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_4level_assign");
                        node->child_count = 5;
                        node->next = NULL;
                        node->line = tokens[*current - 7].line;
                        
                        // Set all property names and value
                        node->children[0].type = AST_EXPR;
                        node->children[0].text = obj_name;
                        node->children[0].children = NULL;
                        node->children[0].child_count = 0;
                        node->children[0].next = NULL;
                        node->children[0].line = node->line;
                        
                        node->children[1].type = AST_EXPR;
                        node->children[1].text = prop1_name;
                        node->children[1].children = NULL;
                        node->children[1].child_count = 0;
                        node->children[1].next = NULL;
                        node->children[1].line = node->line;
                        
                        node->children[2].type = AST_EXPR;
                        node->children[2].text = prop2_name;
                        node->children[2].children = NULL;
                        node->children[2].child_count = 0;
                        node->children[2].next = NULL;
                        node->children[2].line = node->line;
                        
                        node->children[3].type = AST_EXPR;
                        node->children[3].text = prop3_name;
                        node->children[3].children = NULL;
                        node->children[3].child_count = 0;
                        node->children[3].next = NULL;
                        node->children[3].line = node->line;
                        
                        // Copy the value expression
                        deep_copy_ast_node(&node->children[4], value_expr);
                        parser_free_ast(value_expr);
                        break;
                    }
                    else {
                        // Chain too deep
                        fprintf(stderr, "Error: Property assignment chains longer than 4 levels not yet supported at line %d\n", tokens[*current].line);
                        parser_free_ast(node);
                        return NULL;
                    }
                }
            }
            
            // Check if this is a regular assignment statement (identifier = expression)
            if (tokens[*current + 1].type == TOKEN_ASSIGN) {
                // This is an assignment statement
                char* var_name = tracked_strdup(tokens[*current].text, __FILE__, __LINE__, "parser");
                (*current)++; // Skip identifier
                (*current)++; // Skip '='
                
                // Parse the value expression
                ASTNode* value_expr = parse_expression(tokens, current);
                if (!value_expr) {
                    free(var_name);
                    parser_free_ast(node);
                    return NULL;
                }
                
                // Check for semicolon
                if (tokens[*current].type != TOKEN_SEMICOLON) {
                    fprintf(stderr, "Error: Expected ';' after assignment at line %d\n", tokens[*current].line);
                    free(var_name);
                    parser_free_ast(node);
                    parser_free_ast(value_expr);
                    return NULL;
                }
                (*current)++; // Skip ';'
                
                // Create assignment node
                node->type = AST_ASSIGN;
                node->text = tracked_strdup("assign", __FILE__, __LINE__, "parser");
                node->children = (ASTNode*)tracked_malloc(2 * sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_assignment");
                node->child_count = 2;
                node->next = NULL;
                node->line = tokens[*current - 2].line; // Line of the identifier
                
                // Set variable name and value
                node->children[0].type = AST_EXPR;
                node->children[0].text = var_name;
                node->children[0].children = NULL;
                node->children[0].child_count = 0;
                node->children[0].next = NULL;
                node->children[0].line = node->line;
                
                // Copy the value expression
                deep_copy_ast_node(&node->children[1], value_expr);
                parser_free_ast(value_expr);
                break;
            }
            

            
            // This might be a function call statement like "bot.send_embed(...)"
            // Save the current position to restore if this isn't a function call
            int saved_current = *current;
            
            // Try to parse as an expression (which will handle dot expressions and function calls)
            ASTNode* expr = parse_expression(tokens, current);
            if (!expr) {
                // Restore position and fall through to default
                *current = saved_current;
                goto default_case;
            }
            
            // Check if the expression ends with a semicolon (indicating it's a statement)
            if (tokens[*current].type == TOKEN_SEMICOLON) {
                (*current)++; // Skip semicolon
                
                // Create an expression statement node
                node->type = AST_EXPR;
                node->text = tracked_strdup("expr_stmt", __FILE__, __LINE__, "parser");
                node->children = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parse_statement_identifier");
                node->child_count = 1;
                node->next = NULL;
                node->line = tokens[saved_current].line;
                node->children[0] = *expr;
                break;
            } else {
                // Not a statement, restore position and fall through to default
                *current = saved_current;
                goto default_case;
            }
        }
        
        default_case:
        default:
            fprintf(stderr, "Error: Unexpected token in statement at line %d\n", tokens[*current].line);
            parser_free_ast(node);
            return NULL;
    }

    return node;
}

ASTNode* parser_parse(Token* tokens) {
    int current = 0;
    int token_count = 0;
    
    // Count tokens until EOF
    while (tokens[token_count].type != TOKEN_EOF) {
        token_count++;
    }
    ASTNode* root = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parser_parse");
    if (!root) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    root->type = AST_BLOCK;
    root->text = tracked_strdup("block", __FILE__, __LINE__, "parser");
    init_ast_node(root);

    while (tokens[current].type != TOKEN_EOF) {
        ASTNode* node = NULL;

        // Parse function declarations
        if (tokens[current].type == TOKEN_FUNC) {
            current++; // Skip 'func'
            
            if (tokens[current].type != TOKEN_IDENTIFIER) {
                fprintf(stderr, "Error: Expected function name at line %d\n", tokens[current].line);
                parser_free_ast(root);
                return NULL;
            }
            
            node = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parser_parse_func");
            if (!node) {
                fprintf(stderr, "Error: Memory allocation failed\n");
                parser_free_ast(root);
                return NULL;
            }
            node->type = AST_FUNC;
            node->text = tracked_strdup(tokens[current].text, __FILE__, __LINE__, "parser");
            if (!node->text) {
                fprintf(stderr, "Error: Memory allocation failed for function name\n");
                parser_free_ast(root);
                free(node);
                return NULL;
            }
            init_ast_node(node);
            current++; // Skip function name

            // Parse parameters
            if (tokens[current].type != TOKEN_LPAREN) {
                fprintf(stderr, "Error: Expected '(' at line %d\n", tokens[current].line);
                parser_free_ast(root);
                parser_free_ast(node);
                return NULL;
            }
            current++; // Skip '('

            while (tokens[current].type != TOKEN_RPAREN) {
                if (tokens[current].type != TOKEN_IDENTIFIER) {
                    fprintf(stderr, "Error: Expected parameter name at line %d\n", tokens[current].line);
                    parser_free_ast(root);
                    parser_free_ast(node);
                    return NULL;
                }

                ASTNode* param = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parser_parse_func_param");
                if (!param) {
                    fprintf(stderr, "Error: Memory allocation failed\n");
                    parser_free_ast(root);
                    parser_free_ast(node);
                    return NULL;
                }
                param->type = AST_EXPR;
                param->text = tracked_strdup(tokens[current].text, __FILE__, __LINE__, "parser");
                if (!param->text) {
                    fprintf(stderr, "Error: Memory allocation failed for parameter name\n");
                    parser_free_ast(root);
                    parser_free_ast(node);
                    free(param);
                    return NULL;
                }
                init_ast_node(param);
                current++; // Skip parameter name

                // Parse type annotation (optional)
                if (tokens[current].type == TOKEN_COLON) {
                    // Look ahead to see if this colon is followed by a type token
                    int lookahead = current + 1;
                    if (lookahead < token_count && is_type_token(tokens[lookahead].type)) {
                    current++; // Skip ':'
                    ASTNode* type = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parser_parse_func_type");
                    if (!type) {
                        fprintf(stderr, "Error: Memory allocation failed\n");
                        parser_free_ast(root);
                        parser_free_ast(node);
                        parser_free_ast(param);
                        return NULL;
                    }
                    type->type = AST_EXPR;
                    type->text = tracked_strdup(tokens[current].text, __FILE__, __LINE__, "parser");
                    if (!type->text) {
                        fprintf(stderr, "Error: Memory allocation failed for type name\n");
                        parser_free_ast(root);
                        parser_free_ast(node);
                        parser_free_ast(param);
                        free(type);
                        return NULL;
                    }
                    init_ast_node(type);
                    current++; // Skip type

                    // Add type as child of parameter
                    param->children = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parser_parse_func_param_type");
                    param->children[0] = *type;
                    param->child_count = 1;
                    } else {
                        // No type annotation - parameter is implicitly typed
                        param->children = NULL;
                        param->child_count = 0;
                    }
                } else {
                    // No type annotation - parameter is implicitly typed
                    param->children = NULL;
                    param->child_count = 0;
                }

                // Add parameter as child of function
                node->children = (ASTNode*)tracked_realloc(node->children, (node->child_count + 1) * sizeof(ASTNode), __FILE__, __LINE__, "parser_parse_func_param");
                node->children[node->child_count] = *param;
                node->child_count++;

                if (tokens[current].type == TOKEN_COMMA) {
                    current++; // Skip ','
                } else if (tokens[current].type != TOKEN_RPAREN) {
                    fprintf(stderr, "Error: Expected ',' or ')' at line %d\n", tokens[current].line);
                    parser_free_ast(root);
                    parser_free_ast(node);
                    return NULL;
                }
            }
            current++; // Skip ')'

            // Parse return type (optional)
            // Look ahead to see if the next token is a type marker
            if (tokens[current].type == TOKEN_COLON || tokens[current].type == TOKEN_ARROW) {
                current++; // Skip ':' or '->'
                if (!is_type_token(tokens[current].type)) {
                    // This colon/arrow is for the function body, not a return type
                    // Back up and treat as implicit return
                    current--; // Go back to colon/arrow
                    
                    // Add implicit return type
                    ASTNode* implicit_return = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parser_parse_func_implicit_return");
                    if (!implicit_return) {
                        fprintf(stderr, "Error: Memory allocation failed\n");
                    parser_free_ast(root);
                    parser_free_ast(node);
                    return NULL;
                }
                    implicit_return->type = AST_EXPR;
                    implicit_return->text = tracked_strdup("implicit", __FILE__, __LINE__, "parser");
                    init_ast_node(implicit_return);
                    
                    node->children = (ASTNode*)tracked_realloc(node->children, (node->child_count + 1) * sizeof(ASTNode), __FILE__, __LINE__, "parser_parse_func_implicit_return");
                    node->children[node->child_count] = *implicit_return;
                    node->child_count++;
                    
                    // Now consume the colon/arrow for the function body
                    if (tokens[current].type != TOKEN_COLON && tokens[current].type != TOKEN_ARROW) {
                        fprintf(stderr, "Error: Expected ':' or '->' after function declaration at line %d\n", tokens[current].line);
                        parser_free_ast(root);
                        parser_free_ast(node);
                        return NULL;
                    }
                    current++; // Skip ':' or '->'
                } else {
                    // This is a return type
                ASTNode* return_type = (ASTNode*)tracked_malloc(sizeof(ASTNode), __FILE__, __LINE__, "parser_parse_func_return_type");
                if (!return_type) {
                    fprintf(stderr, "Error: Memory allocation failed\n");
                    parser_free_ast(root);
                    parser_free_ast(node);
                    return NULL;
                }
                return_type->type = AST_EXPR;
                    return_type->text = tracked_strdup(tokens[current].text, __FILE__, __LINE__, "parser");
                if (!return_type->text) {
                    fprintf(stderr, "Error: Memory allocation failed for return type\n");
                    parser_free_ast(root);
                    parser_free_ast(node);
                    free(return_type);
                    return NULL;
                }
                    init_ast_node(return_type);
                current++; // Skip return type

                // Add return type as child of function
                node->children = (ASTNode*)tracked_realloc(node->children, (node->child_count + 1) * sizeof(ASTNode), __FILE__, __LINE__, "parser_parse_return_type");
                node->children[node->child_count] = *return_type;
                node->child_count++;

                    // After return type, we need a colon for the function body
            if (tokens[current].type != TOKEN_COLON) {
                        fprintf(stderr, "Error: Expected ':' after return type at line %d\n", tokens[current].line);
                parser_free_ast(root);
                parser_free_ast(node);
                return NULL;
            }
            current++; // Skip ':'
                }
            } else {
                // No colon or arrow found - this is an error
                fprintf(stderr, "Error: Expected ':' or '->' after function declaration at line %d\n", tokens[current].line);
                parser_free_ast(root);
                parser_free_ast(node);
                return NULL;
            }

            // Parse function body
            // The colon has already been consumed above

            ASTNode* body = parse_block(tokens, &current, token_count);
            if (!body) {
                parser_free_ast(root);
                parser_free_ast(node);
                return NULL;
            }

            // Add body as child of function
            node->children = (ASTNode*)tracked_realloc(node->children, (node->child_count + 1) * sizeof(ASTNode), __FILE__, __LINE__, "parser_parse_func_body");
            if (!node->children) {
                fprintf(stderr, "Error: Memory allocation failed\n");
                parser_free_ast(root);
                parser_free_ast(node);
                parser_free_ast(body);
                return NULL;
            }
            node->children[node->child_count] = *body;
            node->child_count++;
        }
        // Parse other statements
        else {
            node = parse_statement(tokens, &current, token_count);
            if (!node) {
                parser_free_ast(root);
                return NULL;
            }
        }

        if (node) {
            root->children = (ASTNode*)tracked_realloc(root->children, (root->child_count + 1) * sizeof(ASTNode), __FILE__, __LINE__, "parser_parse_root");
            if (!root->children) {
                fprintf(stderr, "Error: Memory allocation failed\n");
                parser_free_ast(root);
                parser_free_ast(node);
                return NULL;
            }
            root->children[root->child_count] = *node;
            root->child_count++;
        }
    }

    return root;
}

void parser_free_ast(ASTNode* node) {
    if (!node) return;
    
    // Free the next node in the linked list first
    if (node->next) {
        parser_free_ast(node->next);
        node->next = NULL;
    }
    
    // Recursively free all children
    if (node->children && node->child_count > 0) {
    for (int i = 0; i < node->child_count; i++) {
            // Free children recursively - children[i] is an ASTNode struct, not a pointer
            if (node->children[i].children && node->children[i].child_count > 0) {
                for (int j = 0; j < node->children[i].child_count; j++) {
                    if (node->children[i].children[j].text) {
                        tracked_free(node->children[i].children[j].text, __FILE__, __LINE__, "parser_free_ast");
                    }
                }
                tracked_free(node->children[i].children, __FILE__, __LINE__, "parser_free_ast");
            }
            if (node->children[i].text) {
                tracked_free(node->children[i].text, __FILE__, __LINE__, "parser_free_ast");
            }
        }
        tracked_free(node->children, __FILE__, __LINE__, "parser_free_ast");
        node->children = NULL;
    }
    
    // Free text
    if (node->text) {
        tracked_free(node->text, __FILE__, __LINE__, "parser_free_ast");
        node->text = NULL;
    }
    
    tracked_free(node, __FILE__, __LINE__, "parser_free_ast");
} 