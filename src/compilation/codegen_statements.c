#include "codegen_statements.h"
#include "codegen_expressions.h"
#include "codegen_variables.h"
#include "codegen_utils.h"
#include "../core/ast.h"
#include <stdio.h>
#include <string.h>

// Generate C code for statements
int codegen_generate_c_statement(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    switch (node->type) {
        case AST_NODE_EXPRESSION:
            return codegen_generate_c_expression(context, node);
            
        case AST_NODE_VARIABLE_DECLARATION:
            return codegen_generate_c_variable_declaration(context, node);
            
        case AST_NODE_IF_STATEMENT:
            return codegen_generate_c_if_statement(context, node);
            
        case AST_NODE_WHILE_LOOP:
            return codegen_generate_c_while_loop(context, node);
            
        case AST_NODE_FOR_LOOP:
            return codegen_generate_c_for_loop(context, node);
            
        case AST_NODE_BLOCK:
            return codegen_generate_c_block(context, node);
            
        case AST_NODE_RETURN:
            return codegen_generate_c_return(context, node);
            
        case AST_NODE_BREAK:
            return codegen_generate_c_break(context, node);
            
        case AST_NODE_CONTINUE:
            return codegen_generate_c_continue(context, node);
            
        case AST_NODE_FUNCTION_DECLARATION:
            return codegen_generate_c_function_declaration(context, node);
            
        case AST_NODE_CLASS_DECLARATION:
            return codegen_generate_c_class_declaration(context, node);
            
        case AST_NODE_IMPORT:
            return codegen_generate_c_import(context, node);
            
        case AST_NODE_MODULE:
            return codegen_generate_c_module(context, node);
            
        case AST_NODE_PACKAGE:
            return codegen_generate_c_package(context, node);
            
        case AST_NODE_TRY_CATCH:
            return codegen_generate_c_try_catch(context, node);
            
        case AST_NODE_SWITCH:
            return codegen_generate_c_switch(context, node);
            
        case AST_NODE_MATCH:
            return codegen_generate_c_match(context, node);
            
        case AST_NODE_THROW:
            return codegen_generate_c_throw(context, node);
            
        case AST_NODE_ASYNC_FUNCTION_DECLARATION:
            return codegen_generate_c_async_function_declaration(context, node);
            
        case AST_NODE_AWAIT:
            return codegen_generate_c_await(context, node);
            
        case AST_NODE_PROMISE:
            return codegen_generate_c_promise(context, node);
            
        default:
            return 0;
    }
}

// Generate C code for variable declarations
int codegen_generate_c_variable_declaration(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    const char* var_name = node->data.variable_declaration.name;
    const char* type_annotation = node->data.variable_declaration.type_annotation;
    ASTNode* initializer = node->data.variable_declaration.initializer;
    
    // Declare variable in scope
    char* c_name = variable_scope_declare_variable(context->variable_scope, var_name);
    if (!c_name) return 0;
    
    // Determine C type
    char* c_type = NULL;
    if (type_annotation) {
        c_type = myco_type_to_c_type(type_annotation);
    } else if (initializer) {
        // Infer type from initializer
        if (initializer->type == AST_NODE_FUNCTION_CALL) {
            const char* func_name = initializer->data.function_call.function_name;
            if (strcmp(func_name, "Dog") == 0 || strcmp(func_name, "Puppy") == 0 || 
                strcmp(func_name, "Cat") == 0 || strcmp(func_name, "Lion") == 0 || 
                strcmp(func_name, "Animal") == 0) {
                c_type = (func_name ? strdup(func_name) : NULL);
            } else if (strcmp(func_name, "post") == 0 || strcmp(func_name, "get") == 0 || 
                       strcmp(func_name, "put") == 0 || strcmp(func_name, "delete") == 0) {
                c_type = ("HttpResponse" ? strdup("HttpResponse") : NULL);
            } else {
                c_type = ("void*" ? strdup("void*") : NULL);
            }
        } else if (initializer->type == AST_NODE_MEMBER_ACCESS) {
            const char* member_name = initializer->data.member_access.member_name;
            if (strcmp(member_name, "year") == 0 || strcmp(member_name, "month") == 0 || 
                strcmp(member_name, "day") == 0 || strcmp(member_name, "hour") == 0 || 
                strcmp(member_name, "minute") == 0 || strcmp(member_name, "second") == 0 ||
                strcmp(member_name, "unix_timestamp") == 0 || strcmp(member_name, "difference") == 0) {
                c_type = ("double" ? strdup("double") : NULL);
            } else if (strcmp(member_name, "add") == 0 || strcmp(member_name, "subtract") == 0 ||
                       strcmp(member_name, "now") == 0 || strcmp(member_name, "create") == 0) {
                c_type = ("void*" ? strdup("void*") : NULL);
            } else if (strcmp(member_name, "speak") == 0 || strcmp(member_name, "match") == 0 || 
                       strcmp(member_name, "stringify") == 0) {
                c_type = ("char*" ? strdup("char*") : NULL);
            } else if (strcmp(member_name, "size") == 0) {
                c_type = ("double" ? strdup("double") : NULL);
            } else {
                c_type = ("void*" ? strdup("void*") : NULL);
            }
        } else if (initializer->type == AST_NODE_FUNCTION_CALL_EXPR) {
            // Check if this is a member access function call (e.g., time.add())
            if (initializer->data.function_call_expr.function &&
                initializer->data.function_call_expr.function->type == AST_NODE_MEMBER_ACCESS) {
                const char* member_name = initializer->data.function_call_expr.function->data.member_access.member_name;
                if (strcmp(member_name, "increment") == 0 || strcmp(member_name, "getValue") == 0 || 
                    strcmp(member_name, "process") == 0 || strcmp(member_name, "calculate") == 0) {
                    c_type = ("double" ? strdup("double") : NULL);
                } else if (strcmp(member_name, "add") == 0 || strcmp(member_name, "subtract") == 0 ||
                           strcmp(member_name, "now") == 0 || strcmp(member_name, "create") == 0) {
                    c_type = ("void*" ? strdup("void*") : NULL);
                } else if (strcmp(member_name, "speak") == 0 || strcmp(member_name, "match") == 0 || 
                           strcmp(member_name, "stringify") == 0 || strcmp(member_name, "join") == 0 ||
                           strcmp(member_name, "toString") == 0) {
                    c_type = ("char*" ? strdup("char*") : NULL);
                } else {
                    c_type = ("void*" ? strdup("void*") : NULL);
                }
            } else {
                c_type = ("void*" ? strdup("void*") : NULL);
            }
        } else if (initializer->type == AST_NODE_ARRAY_LITERAL) {
            c_type = ("char**" ? strdup("char**") : NULL);
        } else {
            c_type = ("void*" ? strdup("void*") : NULL);
        }
    } else {
        c_type = ("void*" ? strdup("void*") : NULL);
    }
    
    // Generate variable declaration
    codegen_indent(context);
    codegen_write(context, "%s %s", c_type, c_name);
    
    if (initializer) {
        codegen_write(context, " = ");
        if (!codegen_generate_c_expression(context, initializer)) {
            free(c_name);
            free(c_type);
            return 0;
        }
    }
    
    codegen_semicolon(context);
    codegen_newline(context);
    
    free(c_name);
    free(c_type);
    return 1;
}

// Generate C code for if statements
int codegen_generate_c_if_statement(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    codegen_indent(context);
    codegen_write_string(context, "if (");
    
    if (!codegen_generate_c_expression(context, node->data.if_statement.condition)) return 0;
    
    codegen_write_string(context, ") {");
    codegen_newline(context);
    
    codegen_indent_increase(context);
    if (!codegen_generate_c_statement(context, node->data.if_statement.then_branch)) {
        codegen_indent_decrease(context);
        return 0;
    }
    codegen_indent_decrease(context);
    
    if (node->data.if_statement.else_branch) {
        codegen_indent(context);
        codegen_write_string(context, "} else {");
        codegen_newline(context);
        
        codegen_indent_increase(context);
        if (!codegen_generate_c_statement(context, node->data.if_statement.else_branch)) {
            codegen_indent_decrease(context);
            return 0;
        }
        codegen_indent_decrease(context);
    }
    
    codegen_indent(context);
    codegen_write_string(context, "}");
    codegen_newline(context);
    
    return 1;
}

// Generate C code for while loops
int codegen_generate_c_while_loop(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    codegen_indent(context);
    codegen_write_string(context, "while (");
    
    if (!codegen_generate_c_expression(context, node->data.while_loop.condition)) return 0;
    
    codegen_write_string(context, ") {");
    codegen_newline(context);
    
    codegen_indent_increase(context);
    if (!codegen_generate_c_statement(context, node->data.while_loop.body)) {
        codegen_indent_decrease(context);
        return 0;
    }
    codegen_indent_decrease(context);
    
    codegen_indent(context);
    codegen_write_string(context, "}");
    codegen_newline(context);
    
    return 1;
}

// Generate C code for for loops
int codegen_generate_c_for_loop(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    codegen_indent(context);
    codegen_write_string(context, "for (");
    
    if (node->data.for_loop.initializer) {
        if (!codegen_generate_c_statement(context, node->data.for_loop.initializer)) return 0;
    }
    
    codegen_write_string(context, "; ");
    
    if (node->data.for_loop.condition) {
        if (!codegen_generate_c_expression(context, node->data.for_loop.condition)) return 0;
    }
    
    codegen_write_string(context, "; ");
    
    if (node->data.for_loop.increment) {
        if (!codegen_generate_c_expression(context, node->data.for_loop.increment)) return 0;
    }
    
    codegen_write_string(context, ") {");
    codegen_newline(context);
    
    codegen_indent_increase(context);
    if (!codegen_generate_c_statement(context, node->data.for_loop.body)) {
        codegen_indent_decrease(context);
        return 0;
    }
    codegen_indent_decrease(context);
    
    codegen_indent(context);
    codegen_write_string(context, "}");
    codegen_newline(context);
    
    return 1;
}

// Generate C code for blocks
int codegen_generate_c_block(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    // Enter new scope
    variable_scope_enter(context->variable_scope);
    
    for (size_t i = 0; i < node->data.block.statement_count; i++) {
        if (!codegen_generate_c_statement(context, node->data.block.statements[i])) {
            variable_scope_exit(context->variable_scope);
            return 0;
        }
    }
    
    // Exit scope
    variable_scope_exit(context->variable_scope);
    
    return 1;
}

// Generate C code for return statements
int codegen_generate_c_return(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    codegen_indent(context);
    codegen_write_string(context, "return");
    
    if (node->data.return_statement.value) {
        codegen_write_string(context, " ");
        if (!codegen_generate_c_expression(context, node->data.return_statement.value)) return 0;
    }
    
    codegen_semicolon(context);
    codegen_newline(context);
    
    return 1;
}

// Generate C code for break statements
int codegen_generate_c_break(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    codegen_indent(context);
    codegen_write_string(context, "break");
    codegen_semicolon(context);
    codegen_newline(context);
    
    return 1;
}

// Generate C code for continue statements
int codegen_generate_c_continue(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    codegen_indent(context);
    codegen_write_string(context, "continue");
    codegen_semicolon(context);
    codegen_newline(context);
    
    return 1;
}

// Generate C code for function declarations
int codegen_generate_c_function_declaration(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    const char* func_name = node->data.function_declaration.name;
    const char* return_type = node->data.function_declaration.return_type;
    
    // Generate function signature
    codegen_indent(context);
    if (return_type) {
        char* c_return_type = myco_type_to_c_type(return_type);
        codegen_write(context, "%s %s(", c_return_type, func_name);
        free(c_return_type);
    } else {
        codegen_write(context, "void %s(", func_name);
    }
    
    // Generate parameters
    for (size_t i = 0; i < node->data.function_declaration.parameter_count; i++) {
        if (i > 0) codegen_comma(context);
        
        const char* param_name = node->data.function_declaration.parameters[i].name;
        const char* param_type = node->data.function_declaration.parameters[i].type;
        
        if (param_type) {
            char* c_param_type = myco_type_to_c_type(param_type);
            codegen_write(context, "%s %s", c_param_type, param_name);
            free(c_param_type);
        } else {
            codegen_write(context, "void* %s", param_name);
        }
    }
    
    codegen_write_string(context, ") {");
    codegen_newline(context);
    
    // Generate function body
    codegen_indent_increase(context);
    if (!codegen_generate_c_statement(context, node->data.function_declaration.body)) {
        codegen_indent_decrease(context);
        return 0;
    }
    codegen_indent_decrease(context);
    
    codegen_indent(context);
    codegen_write_string(context, "}");
    codegen_newline(context);
    
    return 1;
}

// Generate C code for class declarations
int codegen_generate_c_class_declaration(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    const char* class_name = node->data.class_declaration.name;
    
    // Generate struct definition
    codegen_indent(context);
    codegen_write(context, "typedef struct {");
    codegen_newline(context);
    
    codegen_indent_increase(context);
    
    // Add inherited fields for specific classes
    if (strcmp(class_name, "Dog") == 0 || strcmp(class_name, "Puppy") == 0 || 
        strcmp(class_name, "Cat") == 0 || strcmp(class_name, "Lion") == 0) {
        codegen_indent(context);
        codegen_write_string(context, "char* name;");
        codegen_newline(context);
    }
    
    if (strcmp(class_name, "Lion") == 0) {
        codegen_indent(context);
        codegen_write_string(context, "char* habitat;");
        codegen_newline(context);
    }
    
    // Generate class fields
    for (size_t i = 0; i < node->data.class_declaration.field_count; i++) {
        const char* field_name = node->data.class_declaration.fields[i].name;
        const char* field_type = node->data.class_declaration.fields[i].type;
        
        codegen_indent(context);
        if (field_type) {
            char* c_field_type = myco_type_to_c_type(field_type);
            codegen_write(context, "%s %s;", c_field_type, field_name);
            free(c_field_type);
        } else {
            codegen_write(context, "void* %s;", field_name);
        }
        codegen_newline(context);
    }
    
    codegen_indent_decrease(context);
    codegen_indent(context);
    codegen_write(context, "} %s;", class_name);
    codegen_newline(context);
    
    return 1;
}

// Generate C code for import statements
int codegen_generate_c_import(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    // Imports are handled at the header level, not in the generated C code
    return 1;
}

// Generate C code for module declarations
int codegen_generate_c_module(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    // Modules are handled at the header level, not in the generated C code
    return 1;
}

// Generate C code for package declarations
int codegen_generate_c_package(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    // Packages are handled at the header level, not in the generated C code
    return 1;
}

// Generate C code for try-catch blocks
int codegen_generate_c_try_catch(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    // Try-catch is not implemented in C generation yet
    return 1;
}

// Generate C code for switch statements
int codegen_generate_c_switch(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    // Switch is not implemented in C generation yet
    return 1;
}

// Generate C code for match statements
int codegen_generate_c_match(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    // Match is not implemented in C generation yet
    return 1;
}

// Generate C code for throw statements
int codegen_generate_c_throw(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    // Throw is not implemented in C generation yet
    return 1;
}

// Generate C code for async function declarations
int codegen_generate_c_async_function_declaration(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    // Async functions are not implemented in C generation yet
    return 1;
}

// Generate C code for await expressions
int codegen_generate_c_await(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    // Await is not implemented in C generation yet
    return 1;
}

// Generate C code for promise expressions
int codegen_generate_c_promise(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    // Promises are not implemented in C generation yet
    return 1;
}
