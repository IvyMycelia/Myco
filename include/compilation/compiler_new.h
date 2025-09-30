#ifndef COMPILER_NEW_H
#define COMPILER_NEW_H

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

// Code generation context
typedef struct {
    char* output;
    size_t output_capacity;
    size_t output_length;
    int indentation_level;
    VariableScopeStack* variable_scope;
} CodeGenContext;

// Forward declaration for VariableScopeStack
typedef struct VariableScopeStack VariableScopeStack;

// Main compiler functions
int compile_myco_to_c(ASTNode* ast, CompilerConfig* config, char** output);
int codegen_generate_c_program(CodeGenContext* context, ASTNode* node);

// Context management
CodeGenContext* codegen_context_create(void);
void codegen_context_free(CodeGenContext* context);
const char* codegen_get_output(CodeGenContext* context);
size_t codegen_get_output_length(CodeGenContext* context);

// Compiler configuration
CompilerConfig* compiler_config_create(void);
void compiler_config_free(CompilerConfig* config);
void compiler_config_set_target(CompilerConfig* config, TargetArchitecture target);
void compiler_config_set_optimization(CompilerConfig* config, OptimizationLevel level);
void compiler_config_set_output(CompilerConfig* config, const char* output_file);
void compiler_config_set_type_checking(CompilerConfig* config, int enable);
void compiler_config_add_include_path(CompilerConfig* config, const char* path);
void compiler_config_add_library_path(CompilerConfig* config, const char* path);
void compiler_config_add_define(CompilerConfig* config, const char* define);

// Error reporting
void compiler_report_error(const char* message, int line, int column);
void compiler_report_warning(const char* message, int line, int column);
void compiler_report_info(const char* message, int line, int column);

// Utility functions
const char* target_architecture_to_string(TargetArchitecture target);
const char* optimization_level_to_string(OptimizationLevel level);

#endif // COMPILER_NEW_H
