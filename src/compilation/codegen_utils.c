#include "codegen_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// Write formatted output to the context
void codegen_write(CodeGenContext* context, const char* format, ...) {
    if (!context || !format) return;
    
    va_list args;
    va_start(args, format);
    
    // Calculate the required buffer size
    va_list args_copy;
    va_copy(args_copy, args);
    int size = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);
    
    if (size < 0) {
        va_end(args);
        return;
    }
    
    // Ensure we have enough space
    if (context->output_length + size + 1 >= context->output_capacity) {
        context->output_capacity = (context->output_length + size + 1) * 2;
        context->output = realloc(context->output, context->output_capacity);
        if (!context->output) {
            va_end(args);
            return;
        }
    }
    
    // Write the formatted string
    vsnprintf(context->output + context->output_length, 
              context->output_capacity - context->output_length, 
              format, args);
    context->output_length += size;
    
    va_end(args);
}

// Write a string to the context
void codegen_write_string(CodeGenContext* context, const char* str) {
    if (!context || !str) return;
    
    size_t len = strlen(str);
    if (context->output_length + len + 1 >= context->output_capacity) {
        context->output_capacity = (context->output_length + len + 1) * 2;
        context->output = realloc(context->output, context->output_capacity);
        if (!context->output) return;
    }
    
    strcpy(context->output + context->output_length, str);
    context->output_length += len;
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
    for (int i = 0; i < context->indentation_level; i++) {
        codegen_write_string(context, "    ");
    }
}

// Increase indentation
void codegen_indent_increase(CodeGenContext* context) {
    context->indentation_level++;
}

// Decrease indentation
void codegen_indent_decrease(CodeGenContext* context) {
    if (context->indentation_level > 0) {
        context->indentation_level--;
    }
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
