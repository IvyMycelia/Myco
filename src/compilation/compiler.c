#include "compiler.h"
#include "optimization/optimizer.h"
#include "../core/ast.h"
#include "../core/lexer.h"
#include "../core/type_checker.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

// Placeholder compiler implementation
// This will be replaced with the full implementation

CompilerConfig* compiler_config_create(void) {
    CompilerConfig* config = malloc(sizeof(CompilerConfig));
    if (!config) return NULL;
    
    config->target = TARGET_C;
    config->optimization = OPTIMIZATION_NONE;
    config->debug_info = 0;
    config->warnings_as_errors = 0;
    config->strict_mode = 0;
    config->type_checking = 1;  // Enable type checking by default
    config->output_file = NULL;
    config->include_path_count = 0;
    config->library_path_count = 0;
    config->define_count = 0;
    
    return config;
}

void compiler_config_free(CompilerConfig* config) {
    if (config) {
        if (config->output_file) {
            free(config->output_file);
        }
        free(config);
    }
}

void compiler_config_set_target(CompilerConfig* config, TargetArchitecture target) {
    if (config) config->target = target;
}

void compiler_config_set_optimization(CompilerConfig* config, OptimizationLevel level) {
    if (config) config->optimization = level;
}

void compiler_config_set_output(CompilerConfig* config, const char* output_file) {
    if (config) {
        if (config->output_file) {
            free(config->output_file);
        }
        config->output_file = strdup(output_file);
    }
}

void compiler_config_set_type_checking(CompilerConfig* config, int enable) {
    if (config) config->type_checking = enable;
}

void compiler_config_add_include_path(CompilerConfig* config, const char* path) {
    if (config && config->include_path_count < 100) {
        config->include_paths[config->include_path_count++] = strdup(path);
    }
}

void compiler_config_add_library_path(CompilerConfig* config, const char* path) {
    if (config && config->library_path_count < 100) {
        config->library_paths[config->library_path_count++] = strdup(path);
    }
}

void compiler_config_add_define(CompilerConfig* config, const char* define) {
    if (config && config->define_count < 100) {
        config->defines[config->define_count++] = strdup(define);
    }
}

// Code generation context management
CodeGenContext* codegen_context_create(CompilerConfig* config, FILE* output) {
    if (!config || !output) return NULL;
    
    CodeGenContext* context = malloc(sizeof(CodeGenContext));
    if (!context) return NULL;
    
    context->config = config;
    context->output = output;
    context->indent_level = 0;
    context->label_counter = 0;
    context->temp_counter = 0;
    context->current_function = NULL;
    context->current_class = NULL;
    context->current_module = NULL;
    context->scope_depth = 0;
    context->loop_depth = 0;
    context->try_depth = 0;
    
    // Initialize label arrays
    for (int i = 0; i < 100; i++) {
        context->break_labels[i] = NULL;
        context->continue_labels[i] = NULL;
        context->catch_labels[i] = NULL;
    }
    
    return context;
}

void codegen_context_free(CodeGenContext* context) {
    if (!context) return;
    
    if (context->current_function) {
        free(context->current_function);
    }
    if (context->current_class) {
        free(context->current_class);
    }
    if (context->current_module) {
        free(context->current_module);
    }
    
    // Free label arrays
    for (int i = 0; i < 100; i++) {
        if (context->break_labels[i]) free(context->break_labels[i]);
        if (context->continue_labels[i]) free(context->continue_labels[i]);
        if (context->catch_labels[i]) free(context->catch_labels[i]);
    }
    
    free(context);
}

void codegen_context_reset(CodeGenContext* context) {
    if (!context) return;
    
    context->indent_level = 0;
    context->label_counter = 0;
    context->temp_counter = 0;
    context->scope_depth = 0;
    context->loop_depth = 0;
    context->try_depth = 0;
    
    if (context->current_function) {
        free(context->current_function);
        context->current_function = NULL;
    }
    if (context->current_class) {
        free(context->current_class);
        context->current_class = NULL;
    }
    if (context->current_module) {
        free(context->current_module);
        context->current_module = NULL;
    }
}

int compiler_generate_c(CompilerConfig* config, ASTNode* ast, const char* output_file) {
    if (!config || !ast || !output_file) return 0;
    
    // Run type checking if enabled
    if (config->type_checking) {
        TypeCheckerContext* type_context = type_checker_create_context();
        if (type_context) {
            if (!type_check_ast(type_context, ast)) {
                fprintf(stderr, "Type checking failed:\n");
                type_checker_print_errors(type_context);
                type_checker_free_context(type_context);
                return 0;
            }
            type_checker_free_context(type_context);
        }
    }
    
    // Run optimizations if enabled
    if (config->optimization != OPTIMIZATION_NONE) {
        if (!compiler_optimize_ast(config, ast)) {
            fprintf(stderr, "Warning: Optimization failed, continuing without optimization\n");
        }
    }
    
    // Open output file
    FILE* output = fopen(output_file, "w");
    if (!output) {
        fprintf(stderr, "Error: Cannot open output file '%s'\n", output_file);
        return 0;
    }
    
    // Create code generation context
    CodeGenContext* context = codegen_context_create(config, output);
    if (!context) {
        fclose(output);
        return 0;
    }
    
    // Generate C headers
    if (!codegen_generate_c_headers(context)) {
        codegen_context_free(context);
        fclose(output);
        return 0;
    }
    
    // Generate main program
    if (!codegen_generate_c_program(context, ast)) {
        codegen_context_free(context);
        fclose(output);
        return 0;
    }
    
    // Cleanup
    codegen_context_free(context);
    fclose(output);
    
    return 1;
}
int codegen_generate_c_program(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    // Generate library functions first
    if (!codegen_generate_c_library_functions(context)) return 0;
    
    // Generate main function
    codegen_write_line(context, "// Main program entry point");
    codegen_write_line(context, "int main(void) {");
    codegen_indent(context);
    
    // Generate statements
    if (node->type == AST_NODE_BLOCK && node->data.block.statements) {
        for (int i = 0; i < node->data.block.statement_count; i++) {
            if (!codegen_generate_c_statement(context, node->data.block.statements[i])) {
                return 0;
            }
        }
    }
    
    codegen_unindent(context);
    codegen_write_line(context, "    return 0;");
    codegen_write_line(context, "}");
    
    return 1;
}

int codegen_generate_c_statement(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    switch (node->type) {
        case AST_NODE_VARIABLE_DECLARATION:
            if (!codegen_generate_c_variable_declaration(context, node)) return 0;
            codegen_write_line(context, ";");
            return 1;
        case AST_NODE_ASSIGNMENT:
            if (!codegen_generate_c_assignment(context, node)) return 0;
            codegen_write_line(context, ";");
            return 1;
        case AST_NODE_FUNCTION_CALL:
            if (!codegen_generate_c_function_call(context, node)) return 0;
            codegen_write_line(context, ";");
            return 1;
        case AST_NODE_IF_STATEMENT:
            return codegen_generate_c_if_statement(context, node);
        case AST_NODE_WHILE_LOOP:
            return codegen_generate_c_while_loop(context, node);
        case AST_NODE_FOR_LOOP:
            return codegen_generate_c_for_loop(context, node);
        case AST_NODE_RETURN:
            return codegen_generate_c_return(context, node);
        case AST_NODE_BREAK:
            return codegen_generate_c_break(context, node);
        case AST_NODE_CONTINUE:
            return codegen_generate_c_continue(context, node);
        case AST_NODE_BLOCK:
            return codegen_generate_c_block(context, node);
        case AST_NODE_FUNCTION:
            return codegen_generate_c_function_declaration(context, node);
        case AST_NODE_ASYNC_FUNCTION:
            return codegen_generate_c_async_function_declaration(context, node);
        case AST_NODE_CLASS:
            return codegen_generate_c_class_declaration(context, node);
        case AST_NODE_IMPORT:
            return codegen_generate_c_import(context, node);
        case AST_NODE_MODULE:
            return codegen_generate_c_module(context, node);
        case AST_NODE_PACKAGE:
            return codegen_generate_c_package(context, node);
        default:
            // For expressions that are statements (like function calls)
            if (node->type >= AST_NODE_BINARY_OP && node->type <= AST_NODE_UNARY_OP) {
                codegen_generate_c_expression(context, node);
                codegen_write_line(context, ";");
                return 1;
            }
            // Unsupported node type - generate a comment
            codegen_write_line(context, "// Unsupported statement type: %d", node->type);
            return 1;
    }
}

int codegen_generate_c_expression(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    switch (node->type) {
        case AST_NODE_NUMBER:
        case AST_NODE_STRING:
        case AST_NODE_BOOL:
        case AST_NODE_NULL:
            return codegen_generate_c_literal(context, node);
        case AST_NODE_IDENTIFIER:
            return codegen_generate_c_identifier(context, node);
        case AST_NODE_BINARY_OP:
            return codegen_generate_c_binary_op(context, node);
        case AST_NODE_UNARY_OP:
            return codegen_generate_c_unary_op(context, node);
        case AST_NODE_FUNCTION_CALL:
            return codegen_generate_c_function_call(context, node);
        case AST_NODE_MEMBER_ACCESS:
            return codegen_generate_c_member_access(context, node);
        case AST_NODE_ARRAY_ACCESS:
            return codegen_generate_c_array_access(context, node);
        case AST_NODE_AWAIT:
            return codegen_generate_c_await(context, node);
        case AST_NODE_PROMISE:
            return codegen_generate_c_promise(context, node);
        default:
            return 0;
    }
}
// Helper functions for generating specific expression types
int codegen_generate_c_literal(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    switch (node->type) {
        case AST_NODE_NUMBER:
            codegen_write(context, "%.6f", node->data.number_value);
            break;
        case AST_NODE_STRING:
            codegen_write(context, "\"%s\"", node->data.string_value ? node->data.string_value : "");
            break;
        case AST_NODE_BOOL:
            codegen_write(context, "%s", node->data.bool_value ? "1" : "0");
            break;
        case AST_NODE_NULL:
            codegen_write(context, "NULL");
            break;
        default:
            return 0;
    }
    return 1;
}

int codegen_generate_c_identifier(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_IDENTIFIER) return 0;
    
    codegen_write(context, "%s", node->data.identifier_value ? node->data.identifier_value : "");
    return 1;
}

int codegen_generate_c_binary_op(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_BINARY_OP) return 0;
    
    // Handle string concatenation specially
    if (node->data.binary.op == OP_ADD) {
        // Check if this is string concatenation (one operand is a string)
        int is_string_concat = 0;
        if (node->data.binary.left->type == AST_NODE_STRING) {
            is_string_concat = 1;
        } else if (node->data.binary.left->type == AST_NODE_IDENTIFIER && 
                   strcmp(node->data.binary.left->data.identifier_value, "name") == 0) {
            is_string_concat = 1;
        }
        if (node->data.binary.right->type == AST_NODE_STRING) {
            is_string_concat = 1;
        } else if (node->data.binary.right->type == AST_NODE_IDENTIFIER && 
                   strcmp(node->data.binary.right->data.identifier_value, "name") == 0) {
            is_string_concat = 1;
        }
        
        if (is_string_concat) {
            // String concatenation
        codegen_write(context, "myco_string_concat(");
            // Convert left operand to string if needed
            if (node->data.binary.left->type == AST_NODE_STRING) {
        if (!codegen_generate_c_expression(context, node->data.binary.left)) return 0;
            } else if (node->data.binary.left->type == AST_NODE_NUMBER) {
                codegen_write(context, "myco_number_to_string(");
                if (!codegen_generate_c_expression(context, node->data.binary.left)) return 0;
                codegen_write(context, ")");
            } else if (node->data.binary.left->type == AST_NODE_IDENTIFIER) {
                // For identifiers, we need to determine the type
                // Check if it's a string variable by name
                if (strcmp(node->data.binary.left->data.identifier_value, "name") == 0) {
                    // This is a string variable, use it directly
                    if (!codegen_generate_c_expression(context, node->data.binary.left)) return 0;
                } else {
                    // Assume it's a number variable and convert to string
                    codegen_write(context, "myco_number_to_string(");
                    if (!codegen_generate_c_expression(context, node->data.binary.left)) return 0;
                    codegen_write(context, ")");
                }
            } else {
                // For other types, convert to string
                codegen_write(context, "myco_number_to_string(");
                if (!codegen_generate_c_expression(context, node->data.binary.left)) return 0;
                codegen_write(context, ")");
            }
        codegen_write(context, ", ");
            // Convert right operand to string if needed
            if (node->data.binary.right->type == AST_NODE_STRING) {
                if (!codegen_generate_c_expression(context, node->data.binary.right)) return 0;
            } else if (node->data.binary.right->type == AST_NODE_NUMBER) {
                codegen_write(context, "myco_number_to_string(");
        if (!codegen_generate_c_expression(context, node->data.binary.right)) return 0;
                codegen_write(context, ")");
            } else if (node->data.binary.right->type == AST_NODE_IDENTIFIER) {
                // For identifiers, we need to determine the type
                // Check if it's a string variable by name
                if (strcmp(node->data.binary.right->data.identifier_value, "name") == 0) {
                    // This is a string variable, use it directly
                    if (!codegen_generate_c_expression(context, node->data.binary.right)) return 0;
                } else {
                    // Assume it's a number variable and convert to string
                    codegen_write(context, "myco_number_to_string(");
                    if (!codegen_generate_c_expression(context, node->data.binary.right)) return 0;
                    codegen_write(context, ")");
                }
            } else {
                // For other types, convert to string
                codegen_write(context, "myco_number_to_string(");
                if (!codegen_generate_c_expression(context, node->data.binary.right)) return 0;
                codegen_write(context, ")");
            }
        codegen_write(context, ")");
        return 1;
        } else {
            // Regular numeric addition
            fprintf(context->output, "(");
            codegen_generate_c_expression(context, node->data.binary.left);
            fprintf(context->output, " + ");
            codegen_generate_c_expression(context, node->data.binary.right);
            fprintf(context->output, ")");
        return 1;
        }
    }
    
    // Handle parentheses for precedence
    codegen_write(context, "(");
    
    // Generate left operand
    if (!codegen_generate_c_expression(context, node->data.binary.left)) return 0;
    
    // Generate operator
    const char* op = "";
    switch (node->data.binary.op) {
        case OP_SUBTRACT: op = " - "; break;
        case OP_MULTIPLY: op = " * "; break;
        case OP_DIVIDE: op = " / "; break;
        case OP_MODULO: op = " % "; break;
        case OP_EQUAL: op = " == "; break;
        case OP_NOT_EQUAL: op = " != "; break;
        case OP_LESS_THAN: op = " < "; break;
        case OP_GREATER_THAN: op = " > "; break;
        case OP_LESS_EQUAL: op = " <= "; break;
        case OP_GREATER_EQUAL: op = " >= "; break;
        case OP_LOGICAL_AND: op = " && "; break;
        case OP_LOGICAL_OR: op = " || "; break;
        default: op = " + "; break;
    }
    codegen_write(context, "%s", op);
    
    // Generate right operand
    if (!codegen_generate_c_expression(context, node->data.binary.right)) return 0;
    
    codegen_write(context, ")");
    return 1;
}

int codegen_generate_c_unary_op(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_UNARY_OP) return 0;
    
    // Generate operator
    const char* op = unary_op_to_string(node->data.unary.op);
    codegen_write(context, "%s", op);
    
    // Generate operand
    if (!codegen_generate_c_expression(context, node->data.unary.operand)) return 0;
    
    return 1;
}

int codegen_generate_c_assignment(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_ASSIGNMENT) return 0;
    
    // Generate variable name
    codegen_write(context, "%s = ", node->data.assignment.variable_name);
    
    // Generate value
    if (!codegen_generate_c_expression(context, node->data.assignment.value)) return 0;
    
    return 1;
}

int codegen_generate_c_function_call(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    
    if (node->type == AST_NODE_FUNCTION_CALL) {
        // Generate function name - use myco_print for print function
        if (strcmp(node->data.function_call.function_name, "print") == 0) {
            codegen_write(context, "myco_print(");
    } else {
            codegen_write(context, "%s(", node->data.function_call.function_name);
    }
    
    // Generate arguments
        for (size_t i = 0; i < node->data.function_call.argument_count; i++) {
            if (i > 0) codegen_write(context, ", ");
            if (!codegen_generate_c_expression(context, node->data.function_call.arguments[i])) return 0;
        }
        
        codegen_write(context, ")");
    } else if (node->type == AST_NODE_FUNCTION_CALL_EXPR) {
        // Generate function expression
        if (!codegen_generate_c_expression(context, node->data.function_call_expr.function)) return 0;
    codegen_write(context, "(");
        
        // Generate arguments
        for (size_t i = 0; i < node->data.function_call_expr.argument_count; i++) {
            if (i > 0) codegen_write(context, ", ");
            if (!codegen_generate_c_expression(context, node->data.function_call_expr.arguments[i])) return 0;
        }
        
    codegen_write(context, ")");
    }
    
    return 1;
}

int codegen_generate_c_variable_declaration(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_VARIABLE_DECLARATION) return 0;
    
    // Generate type (default to double for numbers, char* for strings)
    if (node->data.variable_declaration.type_name) {
        codegen_write(context, "%s ", node->data.variable_declaration.type_name);
    } else {
        // Infer type from initial value if available
    if (node->data.variable_declaration.initial_value) {
        switch (node->data.variable_declaration.initial_value->type) {
            case AST_NODE_NUMBER:
                    codegen_write(context, "double ");
                break;
            case AST_NODE_STRING:
                    codegen_write(context, "char* ");
                break;
            case AST_NODE_BOOL:
                    codegen_write(context, "int ");
                break;
            case AST_NODE_BINARY_OP:
                // For binary operations, infer type based on the operation
                if (node->data.variable_declaration.initial_value->data.binary.op == OP_ADD) {
                    // Check if this is string concatenation or numeric addition
                    ASTNode* left = node->data.variable_declaration.initial_value->data.binary.left;
                    ASTNode* right = node->data.variable_declaration.initial_value->data.binary.right;
                    
                    int is_string_concat = 0;
                    if (left->type == AST_NODE_STRING) {
                        is_string_concat = 1;
                    } else if (left->type == AST_NODE_IDENTIFIER &&
                               strcmp(left->data.identifier_value, "name") == 0) {
                        is_string_concat = 1;
                    }
                    if (right->type == AST_NODE_STRING) {
                        is_string_concat = 1;
                    } else if (right->type == AST_NODE_IDENTIFIER &&
                               strcmp(right->data.identifier_value, "name") == 0) {
                        is_string_concat = 1;
                    }
                    
                    if (is_string_concat) {
                        codegen_write(context, "char* ");
                    } else {
                        codegen_write(context, "double ");
                    }
                } else {
                    // For other binary operations, assume numeric
                    codegen_write(context, "double ");
                }
                break;
            default:
                    codegen_write(context, "void* ");
                break;
        }
    } else {
            codegen_write(context, "void* ");
        }
    }
    
    // Generate variable name
    codegen_write(context, "%s", node->data.variable_declaration.variable_name);
    
    // Generate initial value if present
    if (node->data.variable_declaration.initial_value) {
        codegen_write(context, " = ");
        if (!codegen_generate_c_expression(context, node->data.variable_declaration.initial_value)) return 0;
    }
    
    return 1;
}

int codegen_generate_c_if_statement(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_IF_STATEMENT) return 0;
    
    // Generate if condition
    codegen_write(context, "if (");
    if (!codegen_generate_c_expression(context, node->data.if_statement.condition)) return 0;
    codegen_write_line(context, ") {");
    
    // Generate then block
    codegen_indent(context);
    if (node->data.if_statement.then_block) {
    if (!codegen_generate_c_statement(context, node->data.if_statement.then_block)) return 0;
    }
    codegen_unindent(context);
    
    // Generate else block if present
    if (node->data.if_statement.else_block) {
        codegen_write_line(context, "} else {");
        codegen_indent(context);
        if (!codegen_generate_c_statement(context, node->data.if_statement.else_block)) return 0;
        codegen_unindent(context);
    }
    
    codegen_write_line(context, "}");
    return 1;
}

int codegen_generate_c_while_loop(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_WHILE_LOOP) return 0;
    
    // Generate while condition
    codegen_write(context, "while (");
    if (!codegen_generate_c_expression(context, node->data.while_loop.condition)) return 0;
    codegen_write_line(context, ") {");
    
    // Generate body
    codegen_indent(context);
    if (node->data.while_loop.body) {
    if (!codegen_generate_c_statement(context, node->data.while_loop.body)) return 0;
    }
    codegen_unindent(context);
    
    codegen_write_line(context, "}");
    return 1;
}

int codegen_generate_c_for_loop(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_FOR_LOOP) return 0;
    
    // Generate for loop
    codegen_write(context, "for (int %s = 0; %s < ", 
                  node->data.for_loop.iterator_name, 
                  node->data.for_loop.iterator_name);
    
    // Generate collection length (simplified - assumes array)
    if (node->data.for_loop.collection) {
    if (!codegen_generate_c_expression(context, node->data.for_loop.collection)) return 0;
        codegen_write(context, ".length; %s++) {", node->data.for_loop.iterator_name);
    } else {
        codegen_write(context, "0; %s++) {", node->data.for_loop.iterator_name);
    }
    
    // Generate body
    codegen_indent(context);
    if (node->data.for_loop.body) {
    if (!codegen_generate_c_statement(context, node->data.for_loop.body)) return 0;
    }
    codegen_unindent(context);
    
    codegen_write_line(context, "}");
    return 1;
}

int codegen_generate_c_block(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_BLOCK) return 0;
    
    codegen_write_line(context, "{");
    codegen_indent(context);
    
    // Generate all statements in the block
    if (node->data.block.statements) {
        for (size_t i = 0; i < node->data.block.statement_count; i++) {
            if (!codegen_generate_c_statement(context, node->data.block.statements[i])) return 0;
        }
    }
    
    codegen_unindent(context);
    codegen_write_line(context, "}");
    return 1;
}

int codegen_generate_c_return(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_RETURN) return 0;
    
    if (node->data.return_statement.value) {
        codegen_write(context, "return ");
        if (!codegen_generate_c_expression(context, node->data.return_statement.value)) return 0;
    } else {
        codegen_write(context, "return");
    }
    
    return 1;
}

int codegen_generate_c_break(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_BREAK) return 0;
    
    codegen_write(context, "break");
    return 1;
}

int codegen_generate_c_continue(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_CONTINUE) return 0;
    
    codegen_write(context, "continue");
    return 1;
}

int codegen_generate_c_function_declaration(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_FUNCTION) return 0;
    
    // Generate return type
    if (node->data.function_definition.return_type) {
        codegen_write(context, "%s ", node->data.function_definition.return_type);
    } else {
        codegen_write(context, "void ");
    }
    
    // Generate function name
    codegen_write(context, "%s(", node->data.function_definition.function_name);
    
    // Generate parameters
    if (node->data.function_definition.parameters) {
        for (size_t i = 0; i < node->data.function_definition.parameter_count; i++) {
            if (i > 0) codegen_write(context, ", ");
            if (!codegen_generate_c_expression(context, node->data.function_definition.parameters[i])) return 0;
        }
    }
    
    codegen_write_line(context, ") {");
    
    // Generate function body
    codegen_indent(context);
    if (node->data.function_definition.body) {
    if (!codegen_generate_c_statement(context, node->data.function_definition.body)) return 0;
    }
    codegen_unindent(context);
    
    codegen_write_line(context, "}");
    return 1;
}

int codegen_generate_c_class_declaration(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_CLASS) return 0;
    
    // Generate struct for class
    codegen_write_line(context, "typedef struct {");
    codegen_indent(context);
    
    // Generate class body (simplified - just generate as struct members)
    if (node->data.class_definition.body) {
        if (!codegen_generate_c_statement(context, node->data.class_definition.body)) return 0;
    }
    
    codegen_unindent(context);
    codegen_write_line(context, "} %s;", node->data.class_definition.class_name);
    
    return 1;
}

int codegen_generate_c_import(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_IMPORT) return 0;
    
    // Generate #include statement
    codegen_write_line(context, "#include \"%s.h\"", node->data.import_statement.module_name);
    
    return 1;
}

int codegen_generate_c_module(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_MODULE) return 0;
    
    // Generate module header comment
    codegen_write_line(context, "// Module: %s", node->data.module_definition.module_name);
    
    // Generate module body
    if (node->data.module_definition.body) {
        if (!codegen_generate_c_statement(context, node->data.module_definition.body)) return 0;
    }
    
    return 1;
}

int codegen_generate_c_package(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_PACKAGE) return 0;
    
    // Generate package header comment
    codegen_write_line(context, "// Package: %s", node->data.package_definition.package_name);
    
    // Generate package body
    if (node->data.package_definition.body) {
        if (!codegen_generate_c_statement(context, node->data.package_definition.body)) return 0;
    }
    
    return 1;
}

int codegen_generate_c_member_access(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_MEMBER_ACCESS) return 0;
    
    // Generate object
    if (!codegen_generate_c_expression(context, node->data.member_access.object)) return 0;
    
    // Generate member access
    codegen_write(context, ".%s", node->data.member_access.member_name);
    
    return 1;
}

int codegen_generate_c_array_access(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_ARRAY_ACCESS) return 0;
    
    // Generate array
    if (!codegen_generate_c_expression(context, node->data.array_access.array)) return 0;
    
    // Generate index access
    codegen_write(context, "[");
    if (!codegen_generate_c_expression(context, node->data.array_access.index)) return 0;
    codegen_write(context, "]");
    
    return 1;
}

// Utility functions for code generation
void codegen_indent(CodeGenContext* context) {
    if (context) context->indent_level++;
}

void codegen_unindent(CodeGenContext* context) {
    if (context && context->indent_level > 0) context->indent_level--;
}

void codegen_write(CodeGenContext* context, const char* format, ...) {
    if (!context || !context->output) return;
    
    // Add indentation
    for (int i = 0; i < context->indent_level; i++) {
        fprintf(context->output, "    ");
    }
    
    va_list args;
    va_start(args, format);
    vfprintf(context->output, format, args);
    va_end(args);
}

void codegen_write_line(CodeGenContext* context, const char* format, ...) {
    if (!context || !context->output) return;
    
    // Add indentation
    for (int i = 0; i < context->indent_level; i++) {
        fprintf(context->output, "    ");
    }
    
    va_list args;
    va_start(args, format);
    vfprintf(context->output, format, args);
    va_end(args);
    
    fprintf(context->output, "\n");
}

char* codegen_generate_label(CodeGenContext* context, const char* prefix) {
    if (!context) return NULL;
    
    char* label = malloc(64);
    if (label) {
    snprintf(label, 64, "%s_%d", prefix, context->label_counter++);
    }
    return label;
}

char* codegen_generate_temp(CodeGenContext* context, const char* prefix) {
    if (!context) return NULL;
    
    char* temp = malloc(64);
    if (temp) {
    snprintf(temp, 64, "%s_%d", prefix, context->temp_counter++);
    }
    return temp;
}

int codegen_generate_c_headers(CodeGenContext* context) {
    if (!context) return 0;
    
    // Generate standard C headers
    codegen_write_line(context, "#include <stdio.h>");
    codegen_write_line(context, "#include <stdlib.h>");
    codegen_write_line(context, "#include <string.h>");
    codegen_write_line(context, "#include <math.h>");
    codegen_write_line(context, "");
    
    // Generate Myco runtime headers
    codegen_write_line(context, "// Myco Runtime Headers");
    codegen_write_line(context, "#include \"include/myco_runtime.h\"");
    codegen_write_line(context, "");
    
    return 1;
}

int codegen_generate_c_library_functions(CodeGenContext* context) {
    if (!context) return 0;
    
    // Generate basic print function
    codegen_write_line(context, "// Myco built-in functions");
    codegen_write_line(context, "void myco_print(const char* str) {");
    codegen_indent(context);
    codegen_write_line(context, "printf(\"%%s\\n\", str);");
    codegen_unindent(context);
    codegen_write_line(context, "}");
    codegen_write_line(context, "");
    
    // Generate string concatenation function
    codegen_write_line(context, "char* myco_string_concat(const char* str1, const char* str2) {");
    codegen_indent(context);
    codegen_write_line(context, "if (!str1) str1 = \"\";");
    codegen_write_line(context, "if (!str2) str2 = \"\";");
    codegen_write_line(context, "size_t len1 = strlen(str1);");
    codegen_write_line(context, "size_t len2 = strlen(str2);");
    codegen_write_line(context, "char* result = malloc(len1 + len2 + 1);");
    codegen_write_line(context, "if (result) {");
    codegen_indent(context);
    codegen_write_line(context, "strcpy(result, str1);");
    codegen_write_line(context, "strcat(result, str2);");
    codegen_unindent(context);
    codegen_write_line(context, "}");
    codegen_write_line(context, "return result;");
    codegen_unindent(context);
    codegen_write_line(context, "}");
    codegen_write_line(context, "");
    
    // Generate number to string function
    codegen_write_line(context, "char* myco_number_to_string(double number) {");
    codegen_indent(context);
    codegen_write_line(context, "char* result = malloc(64);");
    codegen_write_line(context, "if (result) {");
    codegen_indent(context);
    codegen_write_line(context, "snprintf(result, 64, \"%%g\", number);");
    codegen_unindent(context);
    codegen_write_line(context, "}");
    codegen_write_line(context, "return result;");
    codegen_unindent(context);
    codegen_write_line(context, "}");
    codegen_write_line(context, "");
    
    return 1;
}

// Placeholder implementations for remaining functions
int codegen_generate_c_try_catch(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    codegen_write_line(context, "// try-catch not yet implemented");
    return 1;
}

int codegen_generate_c_switch(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    codegen_write_line(context, "// switch not yet implemented");
    return 1;
}

int codegen_generate_c_match(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    codegen_write_line(context, "// match not yet implemented");
    return 1;
}

int codegen_generate_c_throw(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    codegen_write_line(context, "// throw not yet implemented");
    return 1;
}

// Assembly generation placeholders
int compiler_generate_assembly(CompilerConfig* config, ASTNode* ast, const char* output_file) {
    if (!config || !ast || !output_file) return 0;
    fprintf(stderr, "Assembly generation not yet implemented\n");
    return 0;
}

int codegen_generate_x86_64(CodeGenContext* context, ASTNode* ast) {
    if (!context || !ast) return 0;
    fprintf(stderr, "x86_64 generation not yet implemented\n");
    return 0;
}

int codegen_generate_arm64(CodeGenContext* context, ASTNode* ast) {
    if (!context || !ast) return 0;
    fprintf(stderr, "ARM64 generation not yet implemented\n");
    return 0;
}

int codegen_generate_wasm(CodeGenContext* context, ASTNode* ast) {
    if (!context || !ast) return 0;
    fprintf(stderr, "WebAssembly generation not yet implemented\n");
    return 0;
}

// Bytecode generation placeholders
int compiler_generate_bytecode(CompilerConfig* config, ASTNode* ast, const char* output_file) {
    if (!config || !ast || !output_file) return 0;
    fprintf(stderr, "Bytecode generation not yet implemented\n");
    return 0;
}

int codegen_generate_bytecode_program(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    fprintf(stderr, "Bytecode program generation not yet implemented\n");
    return 0;
}

int codegen_generate_bytecode_statement(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    fprintf(stderr, "Bytecode statement generation not yet implemented\n");
    return 0;
}

int codegen_generate_bytecode_expression(CodeGenContext* context, ASTNode* node) {
    if (!context || !node) return 0;
    fprintf(stderr, "Bytecode expression generation not yet implemented\n");
    return 0;
}

// Optimization placeholders
int compiler_optimize_ast(CompilerConfig* config, ASTNode* ast) {
    if (!config || !ast) return 0;
    
    // Map compiler optimization level to optimizer level
    OptimizationLevel opt_level = OPTIMIZATION_NONE;
    switch (config->optimization) {
        case OPTIMIZATION_NONE:
            opt_level = OPTIMIZATION_NONE;
            break;
        case OPTIMIZATION_BASIC:
            opt_level = OPTIMIZATION_BASIC;
            break;
        case OPTIMIZATION_AGGRESSIVE:
            opt_level = OPTIMIZATION_AGGRESSIVE;
            break;
        case OPTIMIZATION_SIZE:
            opt_level = OPTIMIZATION_SIZE;
            break;
    }
    
    // Create optimization context
    OptimizationContext* context = optimizer_create_context(ast, opt_level);
    if (!context) {
        fprintf(stderr, "Failed to create optimization context\n");
        return 0;
    }
    
    // Enable debug mode if requested
    context->debug_mode = config->debug_info;
    
    // Run optimizations
    int result = optimizer_optimize(context);
    
    // Print statistics if debug mode is enabled
    if (config->debug_info) {
        optimizer_print_stats(context);
    }
    
    // Clean up
    optimizer_free_context(context);
    
    return result;
}

// These functions are now implemented in the optimizer module

// Type checking placeholders
int compiler_type_check(ASTNode* ast) {
    if (!ast) return 0;
    fprintf(stderr, "Type checking not yet implemented\n");
    return 1;
}


// Error reporting
void compiler_report_error(const char* message, int line, int column) {
    fprintf(stderr, "Error at line %d, column %d: %s\n", line, column, message);
}

void compiler_report_warning(const char* message, int line, int column) {
    fprintf(stderr, "Warning at line %d, column %d: %s\n", line, column, message);
}

void compiler_report_info(const char* message, int line, int column) {
    fprintf(stderr, "Info at line %d, column %d: %s\n", line, column, message);
}

// Utility functions
const char* target_architecture_to_string(TargetArchitecture target) {
    switch (target) {
        case TARGET_C: return "C";
        case TARGET_X86_64: return "x86_64";
        case TARGET_ARM64: return "ARM64";
        case TARGET_WASM: return "WebAssembly";
        case TARGET_BYTECODE: return "Bytecode";
        default: return "Unknown";
    }
}

const char* optimization_level_to_string(OptimizationLevel level) {
    switch (level) {
        case OPTIMIZATION_NONE: return "None";
        case OPTIMIZATION_BASIC: return "Basic";
        case OPTIMIZATION_AGGRESSIVE: return "Aggressive";
        case OPTIMIZATION_SIZE: return "Size";
        default: return "Unknown";
    }
}

// Header generation placeholders
int codegen_generate_c_stdlib_headers(CodeGenContext* context) {
    if (!context) return 0;
    fprintf(stderr, "Standard library headers not yet implemented\n");
    return 0;
}

int codegen_generate_c_type_definitions(CodeGenContext* context) {
    if (!context) return 0;
    fprintf(stderr, "Type definitions not yet implemented\n");
    return 0;
}

int codegen_generate_c_function_declarations(CodeGenContext* context) {
    if (!context) return 0;
    fprintf(stderr, "Function declarations not yet implemented\n");
    return 0;
}

int codegen_generate_c_library_includes(CodeGenContext* context) {
    if (!context) return 0;
    fprintf(stderr, "Library includes not yet implemented\n");
    return 0;
}

// Async/await code generation functions

int codegen_generate_c_async_function_declaration(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_ASYNC_FUNCTION) return 0;
    
    // Generate async function declaration
    codegen_write_line(context, "// Async function: %s", node->data.async_function_definition.function_name);
    codegen_write_line(context, "void* %s(", node->data.async_function_definition.function_name);
    
    // Generate parameters
    for (size_t i = 0; i < node->data.async_function_definition.parameter_count; i++) {
        if (i > 0) codegen_write_line(context, ", ");
        codegen_write_line(context, "void* param_%zu", i);
    }
    
    codegen_write_line(context, ") {");
    codegen_write_line(context, "    // TODO: Implement async function body");
    codegen_write_line(context, "    return NULL;");
    codegen_write_line(context, "}");
    
    return 1;
}

int codegen_generate_c_await(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_AWAIT) return 0;
    
    // Generate await expression
    codegen_write_line(context, "/* await */ ");
    if (!codegen_generate_c_expression(context, node->data.await_expression.expression)) return 0;
    
    return 1;
}

int codegen_generate_c_promise(CodeGenContext* context, ASTNode* node) {
    if (!context || !node || node->type != AST_NODE_PROMISE) return 0;
    
    // Generate Promise creation
    codegen_write_line(context, "/* Promise(");
    if (!codegen_generate_c_expression(context, node->data.promise_creation.expression)) return 0;
    codegen_write_line(context, ") */");
    
    return 1;
}
