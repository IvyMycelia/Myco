#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include "ast.h"
#include <stddef.h>

// Type system definitions
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_NULL,
    TYPE_ARRAY,
    TYPE_HASH_MAP,
    TYPE_SET,
    TYPE_FUNCTION,
    TYPE_CLASS,
    TYPE_UNION,
    TYPE_OPTIONAL,
    TYPE_ANY,
    TYPE_UNKNOWN,
    TYPE_ERROR
} MycoTypeKind;

// Type representation
typedef struct MycoType {
    MycoTypeKind kind;
    union {
        // Array type: element type
        struct MycoType* element_type;
        
        // Function type: parameter types and return type
        struct {
            struct MycoType** parameter_types;
            size_t parameter_count;
            struct MycoType* return_type;
        } function_type;
        
        // Class type: class name
        char* class_name;
        
        // Union type: possible types
        struct {
            struct MycoType** types;
            size_t type_count;
        } union_type;
        
        // Optional type: wrapped type
        struct MycoType* optional_type;
    } data;
    
    // Source location for error reporting
    int line;
    int column;
} MycoType;

// Type environment for tracking variable types
typedef struct TypeEnvironment {
    struct TypeEnvironment* parent;
    char** variable_names;
    MycoType** variable_types;
    size_t variable_count;
    size_t variable_capacity;
} TypeEnvironment;

// Type checker context
typedef struct TypeCheckerContext {
    TypeEnvironment* current_environment;
    MycoType** function_return_types;
    char** function_names;
    size_t function_count;
    size_t function_capacity;
    int error_count;
    char** error_messages;
    size_t error_capacity;
} TypeCheckerContext;

// Type checker functions
TypeCheckerContext* type_checker_create_context(void);
void type_checker_free_context(TypeCheckerContext* context);

// Type creation and management
MycoType* type_create(MycoTypeKind kind, int line, int column);
MycoType* type_create_array(MycoType* element_type, int line, int column);
MycoType* type_create_function(MycoType** parameter_types, size_t parameter_count, 
                               MycoType* return_type, int line, int column);
MycoType* type_create_class(const char* class_name, int line, int column);
MycoType* type_create_union(MycoType** types, size_t type_count, int line, int column);
MycoType* type_create_optional(MycoType* wrapped_type, int line, int column);
MycoType* type_clone(MycoType* type);
void type_free(MycoType* type);

// Type checking functions
int type_check_ast(TypeCheckerContext* context, ASTNode* node);
int type_check_expression(TypeCheckerContext* context, ASTNode* node, MycoType** result_type);
int type_check_statement(TypeCheckerContext* context, ASTNode* node);
int type_check_variable_declaration(TypeCheckerContext* context, ASTNode* node);
int type_check_assignment(TypeCheckerContext* context, ASTNode* node);
int type_check_function(TypeCheckerContext* context, ASTNode* node);
int type_check_class(TypeCheckerContext* context, ASTNode* node);
int type_check_block(TypeCheckerContext* context, ASTNode* node);

// Type inference
MycoType* type_infer_expression(TypeCheckerContext* context, ASTNode* node);
MycoType* type_infer_literal(ASTNode* node);
MycoType* type_infer_binary_op(TypeCheckerContext* context, ASTNode* node);
MycoType* type_infer_unary_op(TypeCheckerContext* context, ASTNode* node);
MycoType* type_infer_function_call(TypeCheckerContext* context, ASTNode* node);
MycoType* type_infer_array_literal(TypeCheckerContext* context, ASTNode* node);
MycoType* type_infer_member_access(TypeCheckerContext* context, ASTNode* node);

// Type compatibility
int type_is_compatible(MycoType* expected, MycoType* actual);
int type_is_assignable(MycoType* target, MycoType* source);
int type_is_strictly_compatible(MycoType* expected, MycoType* actual);
int type_is_equal(MycoType* type1, MycoType* type2);

// Type environment management
TypeEnvironment* type_environment_create(TypeEnvironment* parent);
void type_environment_free(TypeEnvironment* env);
int type_environment_add_variable(TypeEnvironment* env, const char* name, MycoType* type);
MycoType* type_environment_lookup_variable(TypeEnvironment* env, const char* name);
int type_environment_update_variable(TypeEnvironment* env, const char* name, MycoType* type);

// Error reporting
void type_checker_add_error(TypeCheckerContext* context, const char* message, int line, int column);
void type_checker_print_errors(TypeCheckerContext* context);

// Utility functions
const char* type_kind_to_string(MycoTypeKind kind);
const char* type_to_string(MycoType* type);
MycoType* type_parse_string(const char* type_string, int line, int column);

#endif // TYPE_CHECKER_H
