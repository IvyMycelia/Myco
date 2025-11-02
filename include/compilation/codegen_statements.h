#ifndef CODEGEN_STATEMENTS_H
#define CODEGEN_STATEMENTS_H

#include "compiler.h"
#include "../core/ast.h"

// Statement generation functions
int codegen_generate_c_statement(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_variable_declaration(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_if_statement(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_while_loop(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_for_loop(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_block(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_return(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_break(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_continue(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_function_declaration(CodeGenContext* context, ASTNode* node, const char* override_name);
int codegen_generate_c_class_declaration(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_import(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_module(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_package(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_try_catch(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_switch(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_match(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_throw(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_async_function_declaration(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_await(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_promise(CodeGenContext* context, ASTNode* node);

#endif // CODEGEN_STATEMENTS_H
