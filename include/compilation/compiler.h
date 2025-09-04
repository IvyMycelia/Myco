#ifndef COMPILER_H
#define COMPILER_H

#include "ast.h"
#include <stdio.h>

// Target architectures
typedef enum {
    TARGET_C,
    TARGET_X86_64,
    TARGET_ARM64,
    TARGET_WASM,
    TARGET_BYTECODE
} TargetArchitecture;

// Optimization levels
typedef enum {
    OPTIMIZATION_NONE,
    OPTIMIZATION_BASIC,
    OPTIMIZATION_AGGRESSIVE,
    OPTIMIZATION_SIZE
} OptimizationLevel;

// Compiler configuration
typedef struct {
    TargetArchitecture target;
    OptimizationLevel optimization;
    int debug_info;
    int warnings_as_errors;
    int strict_mode;
    char* output_file;
    char* include_paths[100];
    int include_path_count;
    char* library_paths[100];
    int library_path_count;
    char* defines[100];
    int define_count;
} CompilerConfig;

// Code generation context
typedef struct {
    CompilerConfig* config;
    FILE* output;
    int indent_level;
    int label_counter;
    int temp_counter;
    char* current_function;
    char* current_class;
    char* current_module;
    int scope_depth;
    int loop_depth;
    int try_depth;
    char* break_labels[100];
    char* continue_labels[100];
    char* catch_labels[100];
} CodeGenContext;

// Compiler initialization and cleanup
CompilerConfig* compiler_config_create(void);
void compiler_config_free(CompilerConfig* config);
void compiler_config_set_target(CompilerConfig* config, TargetArchitecture target);
void compiler_config_set_optimization(CompilerConfig* config, OptimizationLevel level);
void compiler_config_set_output(CompilerConfig* config, const char* output_file);
void compiler_config_add_include_path(CompilerConfig* config, const char* path);
void compiler_config_add_library_path(CompilerConfig* config, const char* path);
void compiler_config_add_define(CompilerConfig* config, const char* define);

// Code generation context management
CodeGenContext* codegen_context_create(CompilerConfig* config, FILE* output);
void codegen_context_free(CodeGenContext* context);
void codegen_context_reset(CodeGenContext* context);

// C code generation
int compiler_generate_c(CompilerConfig* config, ASTNode* ast, const char* output_file);
int codegen_generate_c_program(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_statement(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_expression(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_binary_op(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_unary_op(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_assignment(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_function_call(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_variable_declaration(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_if_statement(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_while_loop(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_for_loop(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_try_catch(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_switch(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_match(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_block(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_return(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_break(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_continue(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_throw(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_function_declaration(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_class_declaration(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_import(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_module(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_package(CodeGenContext* context, ASTNode* node);

// Helper functions for specific expression types
int codegen_generate_c_literal(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_identifier(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_member_access(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_array_access(CodeGenContext* context, ASTNode* node);

// Assembly code generation
int compiler_generate_assembly(CompilerConfig* config, ASTNode* ast, const char* output_file);
int codegen_generate_x86_64(CodeGenContext* context, ASTNode* ast);
int codegen_generate_arm64(CodeGenContext* context, ASTNode* ast);
int codegen_generate_wasm(CodeGenContext* context, ASTNode* ast);

// Bytecode generation
int compiler_generate_bytecode(CompilerConfig* config, ASTNode* ast, const char* output_file);
int codegen_generate_bytecode_program(CodeGenContext* context, ASTNode* node);
int codegen_generate_bytecode_statement(CodeGenContext* context, ASTNode* node);
int codegen_generate_bytecode_expression(CodeGenContext* context, ASTNode* node);

// Optimization passes
int compiler_optimize_ast(CompilerConfig* config, ASTNode* ast);
ASTNode* optimizer_constant_folding(ASTNode* node);
ASTNode* optimizer_dead_code_elimination(ASTNode* node);
ASTNode* optimizer_common_subexpression_elimination(ASTNode* node);
ASTNode* optimizer_function_inlining(ASTNode* node);
ASTNode* optimizer_loop_optimization(ASTNode* node);

// Type checking and validation
int compiler_type_check(ASTNode* ast);
int type_check_node(ASTNode* node);
int type_check_expression(ASTNode* node);
int type_check_statement(ASTNode* node);
int type_check_function(ASTNode* node);
int type_check_class(ASTNode* node);

// Error reporting
void compiler_report_error(const char* message, int line, int column);
void compiler_report_warning(const char* message, int line, int column);
void compiler_report_info(const char* message, int line, int column);

// Utility functions
void codegen_indent(CodeGenContext* context);
void codegen_unindent(CodeGenContext* context);
void codegen_write(CodeGenContext* context, const char* format, ...);
void codegen_write_line(CodeGenContext* context, const char* format, ...);
char* codegen_generate_label(CodeGenContext* context, const char* prefix);
char* codegen_generate_temp(CodeGenContext* context, const char* prefix);
const char* target_architecture_to_string(TargetArchitecture target);
const char* optimization_level_to_string(OptimizationLevel level);

// Header generation
int codegen_generate_c_headers(CodeGenContext* context);
int codegen_generate_c_stdlib_headers(CodeGenContext* context);
int codegen_generate_c_type_definitions(CodeGenContext* context);
int codegen_generate_c_function_declarations(CodeGenContext* context);

// Library linking
int codegen_generate_c_library_includes(CodeGenContext* context);
int codegen_generate_c_library_functions(CodeGenContext* context);

#endif // COMPILER_H
