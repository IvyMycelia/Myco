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
    int type_checking;
    char* output_file;
    char* include_paths[100];
    int include_path_count;
    char* library_paths[100];
    int library_path_count;
    char* defines[100];
    int define_count;
} CompilerConfig;

// Variable scope entry
typedef struct {
    char* original_name;
    char* c_name;
    int scope_level;
    int is_declared;
} VariableScopeEntry;

// Variable scope stack
typedef struct {
    VariableScopeEntry* entries;
    int capacity;
    int count;
    int current_scope_level;
} VariableScopeStack;

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
    VariableScopeStack* variable_scope;
    // Track imported libraries to avoid duplicates
    char* imported_libraries[100];
    int imported_library_count;
    // Track current variable name being declared (for context-aware code generation)
    const char* current_variable_name;
    // Type checker context for accurate type inference
    void* type_context;  // TypeCheckerContext* - using void* to avoid circular includes
} CodeGenContext;

// Compiler initialization and cleanup
CompilerConfig* compiler_config_create(void);
void compiler_config_free(CompilerConfig* config);
void compiler_config_set_target(CompilerConfig* config, TargetArchitecture target);
void compiler_config_set_optimization(CompilerConfig* config, OptimizationLevel level);
void compiler_config_set_output(CompilerConfig* config, const char* output_file);
void compiler_config_set_type_checking(CompilerConfig* config, int enable);
void compiler_config_set_debug_info(CompilerConfig* config, int enable);
void compiler_config_set_strict_mode(CompilerConfig* config, int enable);
void compiler_config_add_include_path(CompilerConfig* config, const char* path);
void compiler_config_add_library_path(CompilerConfig* config, const char* path);
void compiler_config_add_define(CompilerConfig* config, const char* define);

// Code generation context management
CodeGenContext* codegen_context_create(CompilerConfig* config, FILE* output);
void codegen_context_free(CodeGenContext* context);
void codegen_context_reset(CodeGenContext* context);

// Variable scoping system
VariableScopeStack* variable_scope_create(void);
void variable_scope_free(VariableScopeStack* scope);
void variable_scope_enter(VariableScopeStack* scope);
void variable_scope_exit(VariableScopeStack* scope);
char* variable_scope_get_c_name(VariableScopeStack* scope, const char* original_name);
char* variable_scope_declare_variable(VariableScopeStack* scope, const char* original_name);
int variable_scope_is_declared(VariableScopeStack* scope, const char* original_name);

// C code generation
int compiler_generate_c(CompilerConfig* config, ASTNode* ast, const char* output_file);
int compiler_compile_to_binary(CompilerConfig* config, const char* c_file, const char* binary_file);
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
int codegen_generate_c_spore(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_block(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_return(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_break(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_continue(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_throw(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_function_declaration(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_async_function_declaration(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_await(CodeGenContext* context, ASTNode* node);
int codegen_generate_c_promise(CodeGenContext* context, ASTNode* node);
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

// Type checking and validation
int compiler_type_check(ASTNode* ast);

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
