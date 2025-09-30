#include "compiler.h"
#include "codegen_variables.h"
#include "codegen_expressions.h"
#include "codegen_statements.h"
#include "codegen_headers.h"
#include "codegen_utils.h"
#include "optimization/optimizer.h"
#include "../core/ast.h"
#include "../core/lexer.h"
#include "../core/type_checker.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Main compiler function
int compile_myco_to_c(ASTNode* ast, CompilerConfig* config, char** output) {
    if (!ast || !config || !output) return 0;
    
    // Create code generation context
    CodeGenContext context;
    context.output = malloc(10000);
    context.output_capacity = 10000;
    context.output_length = 0;
    context.indentation_level = 0;
    context.variable_scope = variable_scope_create();
    
    if (!context.output || !context.variable_scope) {
        if (context.output) free(context.output);
        if (context.variable_scope) variable_scope_free(context.variable_scope);
        return 0;
    }
    
    // Generate C code
    if (!codegen_generate_c_program(&context, ast)) {
        free(context.output);
        variable_scope_free(context.variable_scope);
        return 0;
    }
    
    // Set output
    *output = context.output;
    
    // Clean up
    variable_scope_free(context.variable_scope);
    
    return 1;
}

// Generate C code for a complete program
int codegen_generate_c_program(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    // Generate headers
    if (!codegen_generate_c_headers(context)) return 0;
    
    // Generate main function
    codegen_write_string(context, "int main() {\n");
    codegen_indent_increase(context);
    
    // Generate program statements
    for (size_t i = 0; i < node->data.program.statement_count; i++) {
        if (!codegen_generate_c_statement(context, node->data.program.statements[i])) {
            codegen_indent_decrease(context);
            return 0;
        }
    }
    
    codegen_indent_decrease(context);
    codegen_write_string(context, "    return 0;\n");
    codegen_write_string(context, "}\n");
    
    return 1;
}

// Create a new code generation context
CodeGenContext* codegen_context_create(void) {
    CodeGenContext* context = malloc(sizeof(CodeGenContext));
    if (!context) return NULL;
    
    context->output = malloc(10000);
    context->output_capacity = 10000;
    context->output_length = 0;
    context->indentation_level = 0;
    context->variable_scope = variable_scope_create();
    
    if (!context->output || !context->variable_scope) {
        if (context->output) free(context->output);
        if (context->variable_scope) variable_scope_free(context->variable_scope);
        free(context);
        return NULL;
    }
    
    return context;
}

// Free a code generation context
void codegen_context_free(CodeGenContext* context) {
    if (!context) return;
    
    if (context->output) free(context->output);
    if (context->variable_scope) variable_scope_free(context->variable_scope);
    free(context);
}

// Get the generated C code
const char* codegen_get_output(CodeGenContext* context) {
    if (!context || !context->output) return NULL;
    return context->output;
}

// Get the length of the generated C code
size_t codegen_get_output_length(CodeGenContext* context) {
    if (!context) return 0;
    return context->output_length;
}
