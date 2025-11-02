#ifndef CODEGEN_UTILS_H
#define CODEGEN_UTILS_H

#include "compiler.h"
#include <stdarg.h>

// Utility functions for code generation
void codegen_write(CodeGenContext* context, const char* format, ...);
void codegen_write_string(CodeGenContext* context, const char* str);
char* myco_type_to_c_type(const char* myco_type);
const char* get_placeholder_function_return_type(const char* func_name);

// Indentation functions
void codegen_indent(CodeGenContext* context);
void codegen_indent_increase(CodeGenContext* context);
void codegen_indent_decrease(CodeGenContext* context);
void codegen_unindent(CodeGenContext* context);
void codegen_write_line(CodeGenContext* context, const char* line);

// Punctuation functions
void codegen_newline(CodeGenContext* context);
void codegen_semicolon(CodeGenContext* context);
void codegen_comma(CodeGenContext* context);
void codegen_open_paren(CodeGenContext* context);
void codegen_close_paren(CodeGenContext* context);
void codegen_open_brace(CodeGenContext* context);
void codegen_close_brace(CodeGenContext* context);
void codegen_open_bracket(CodeGenContext* context);
void codegen_close_bracket(CodeGenContext* context);

#endif // CODEGEN_UTILS_H
