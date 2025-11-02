#ifndef CODEGEN_EXPRESSIONS_H
#define CODEGEN_EXPRESSIONS_H

#include "compilation.h"
#include "core/ast.h"
#include "core/type_checker.h"

// Expression generation functions
int codegen_generate_c_expression(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_literal(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_identifier(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_binary_op(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_unary_op(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_assignment(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_function_call(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_function_call_expr(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_function_literal(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_member_access(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_array_literal(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_hash_map_literal(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_set_literal(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_lambda(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_array_access(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_await(CodeGenContext* context, ASTNode* node);

// Utility functions for expression generation
const char* get_placeholder_function_return_type(const char* func_name);

// Type information access helpers (Phase 1)
MycoType* codegen_get_variable_type(CodeGenContext* context, const char* var_name);
int codegen_is_library_type(CodeGenContext* context, MycoType* type, const char** library_name);
int codegen_is_class_type(CodeGenContext* context, MycoType* type, const char** class_name);
const char* codegen_get_type_category(CodeGenContext* context, MycoType* type);

#endif // CODEGEN_EXPRESSIONS_H
