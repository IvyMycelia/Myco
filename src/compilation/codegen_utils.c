#include "codegen_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// Write formatted output to the context (uses FILE* output)
void codegen_write(CodeGenContext* context, const char* format, ...) {
    if (!context || !format || !context->output) return;
    
    va_list args;
    va_start(args, format);
    vfprintf(context->output, format, args);
    va_end(args);
}

// Write a string to the context (uses FILE* output)
void codegen_write_string(CodeGenContext* context, const char* str) {
    if (!context || !str || !context->output) return;
    fputs(str, context->output);
}

// Convert Myco type to C type
char* myco_type_to_c_type(const char* myco_type) {
    if (!myco_type) return NULL;
    
    if (strcmp(myco_type, "Int") == 0) {
        return ("int" ? strdup("int") : NULL);
    } else if (strcmp(myco_type, "Float") == 0) {
        return ("double" ? strdup("double") : NULL);
    } else if (strcmp(myco_type, "String") == 0) {
        return ("char*" ? strdup("char*") : NULL);
    } else if (strcmp(myco_type, "Bool") == 0) {
        return ("int" ? strdup("int") : NULL);
    } else if (strcmp(myco_type, "Array") == 0) {
        return ("char**" ? strdup("char**") : NULL);
    } else if (strcmp(myco_type, "Object") == 0) {
        return ("void*" ? strdup("void*") : NULL);
    } else if (strcmp(myco_type, "Null") == 0) {
        return ("void*" ? strdup("void*") : NULL);
    } else if (strcmp(myco_type, "HttpResponse") == 0) {
        return ("HttpResponse" ? strdup("HttpResponse") : NULL);
    } else {
        // Default to void* for unknown types
        return ("void*" ? strdup("void*") : NULL);
    }
}

// Get placeholder function return type
const char* get_placeholder_function_return_type(const char* func_name) {
    if (!func_name) return "void*";
    
    // Method calls that return double
    if (strcmp(func_name, "increment") == 0 || 
        strcmp(func_name, "getValue") == 0 || 
        strcmp(func_name, "process") == 0 || 
        strcmp(func_name, "calculate") == 0 ||
        strcmp(func_name, "year") == 0 ||
        strcmp(func_name, "month") == 0 ||
        strcmp(func_name, "day") == 0 ||
        strcmp(func_name, "hour") == 0 ||
        strcmp(func_name, "minute") == 0 ||
        strcmp(func_name, "second") == 0 ||
        strcmp(func_name, "unix_timestamp") == 0 ||
        strcmp(func_name, "difference") == 0) {
        return "double";
    }
    
    // Method calls that return char*
    if (strcmp(func_name, "speak") == 0 ||
        strcmp(func_name, "match") == 0 ||
        strcmp(func_name, "stringify") == 0) {
        return "char*";
    }
    
    // Default to void*
    return "void*";
}

// Generate indentation
void codegen_indent(CodeGenContext* context) {
    if (!context || !context->output) return;
    for (int i = 0; i < context->indent_level; i++) {
        fputs("    ", context->output);
    }
}

// Increase indentation
void codegen_indent_increase(CodeGenContext* context) {
    if (!context) return;
    context->indent_level++;
}

// Decrease indentation
void codegen_indent_decrease(CodeGenContext* context) {
    if (!context) return;
    if (context->indent_level > 0) {
        context->indent_level--;
    }
}

// Unindent (alias for codegen_indent_decrease)
void codegen_unindent(CodeGenContext* context) {
    codegen_indent_decrease(context);
}

// Write a line with indentation and newline
void codegen_write_line(CodeGenContext* context, const char* line) {
    if (!context || !line || !context->output) return;
    codegen_indent(context);
    fputs(line, context->output);
    fputs("\n", context->output);
}

// Generate a newline
void codegen_newline(CodeGenContext* context) {
    codegen_write_string(context, "\n");
}

// Generate a semicolon
void codegen_semicolon(CodeGenContext* context) {
    codegen_write_string(context, ";");
}

// Generate a comma
void codegen_comma(CodeGenContext* context) {
    codegen_write_string(context, ", ");
}

// Generate parentheses
void codegen_open_paren(CodeGenContext* context) {
    codegen_write_string(context, "(");
}

void codegen_close_paren(CodeGenContext* context) {
    codegen_write_string(context, ")");
}

// Generate braces
void codegen_open_brace(CodeGenContext* context) {
    codegen_write_string(context, "{");
}

void codegen_close_brace(CodeGenContext* context) {
    codegen_write_string(context, "}");
}

// Generate brackets
void codegen_open_bracket(CodeGenContext* context) {
    codegen_write_string(context, "[");
}

void codegen_close_bracket(CodeGenContext* context) {
    codegen_write_string(context, "]");
}
