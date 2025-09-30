#ifndef CODEGEN_EXPRESSIONS_H
#define CODEGEN_EXPRESSIONS_H

#include "compilation.h"
#include "core/ast.h"

// Expression generation functions
int codegen_generate_c_expression(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_literal(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_identifier(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_binary_op(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_unary_op(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_assignment(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_function_call(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_member_access(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_array_literal(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_hash_map_literal(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_set_literal(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_lambda(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_array_access(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_await(CodeGenContext* context, ASTNode* node);

// Utility functions for expression generation
const char* get_placeholder_function_return_type(const char* func_name);

#endif // CODEGEN_EXPRESSIONS_H
