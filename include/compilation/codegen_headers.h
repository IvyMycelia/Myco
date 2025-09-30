#ifndef CODEGEN_HEADERS_H
#define CODEGEN_HEADERS_H

#include "compiler.h"

// Header generation functions
int codegen_generate_c_headers(CodeGenContext* context);
int codegen_generate_c_type_definitions(CodeGenContext* context);
int codegen_generate_c_function_declarations(CodeGenContext* context);
int codegen_generate_c_library_functions(CodeGenContext* context);
int codegen_generate_c_stdlib_headers(CodeGenContext* context);
int codegen_generate_c_library_includes(CodeGenContext* context);

#endif // CODEGEN_HEADERS_H
