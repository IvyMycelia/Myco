#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include <sys/stat.h>
#ifndef _WIN32
#include <unistd.h>
#endif

// Forward declarations for functions defined later in this file
static struct ASTNode* find_function_in_module(struct ASTNode* mod, const char* name);
long long eval_expression(struct ASTNode* ast);
void eval_evaluate(struct ASTNode* ast);
static struct ASTNode* resolve_module(const char* alias);
static long long eval_user_function_call(struct ASTNode* fn, struct ASTNode* args_node);

// ANSI color codes
#define RED "\033[31m"
#define RESET "\033[0m"

// Error code structure: 0xMY[SEV][MOD][ERR]
// SEV: Severity (0=Info, 1=Warning, 2=Error, F=Fatal)
// MOD: Module (0=Runtime, 1=Math, 2=Type, 3=Syntax, 4=IO)
// ERR: Specific error code

// Severity levels
//    Info - Probably a log or something lmao
#define SEV_INFO    0x00
//    Warning - You have something wrong with the program but it ain't super bad
//    Current Functionality: Continue
#define SEV_WARNING 0x01
//    Error - Current Functionality:
//    On Interpret: Continue
//    On Build&Run: It'll just break
#define SEV_ERROR   0x02
//    Fatal Error - Current Functionality:
//    On Interpret: Continue TODO must exit program
//    On Build&Run: It'll just break
#define SEV_FATAL   0x0F

// Modules
#define MOD_RUNTIME 0x00
#define MOD_MATH    0x01
#define MOD_TYPE    0x02
#define MOD_SYNTAX  0x03
#define MOD_IO      0x04

// Specific error codes
#define ERR_NONE             0x00
#define ERR_DIVISION_BY_ZERO 0x01
#define ERR_MODULO_BY_ZERO   0x02
#define ERR_UNDEFINED_VAR    0x03
#define ERR_TYPE_MISMATCH    0x04
#define ERR_INVALID_OP       0x05
#define ERR_RECURSION        0x06
#define ERR_FUNC_CALL        0x07
#define ERR_BAD_MEMORY       0x08
#define ERR_INPUT_FAILED     0x09
#define ERR_INVALID_INPUT    0x0A

// Combined error codes
#define ERROR_DIVISION_BY_ZERO   ((SEV_ERROR << 16) | (MOD_MATH << 8) | ERR_DIVISION_BY_ZERO)
#define ERROR_MODULO_BY_ZERO     ((SEV_ERROR << 16) | (MOD_MATH << 8) | ERR_MODULO_BY_ZERO)
#define ERROR_UNDEFINED_VAR      ((SEV_ERROR << 16) | (MOD_RUNTIME << 8) | ERR_UNDEFINED_VAR)
#define ERROR_TYPE_MISMATCH      ((SEV_ERROR << 16) | (MOD_TYPE << 8) | ERR_TYPE_MISMATCH)
#define ERROR_INVALID_OP         ((SEV_ERROR << 16) | (MOD_RUNTIME << 8) | ERR_INVALID_OP)
#define ERROR_RECURSION          ((SEV_ERROR << 16) | (MOD_RUNTIME << 8) | ERR_RECURSION)
#define ERROR_FUNC_CALL          ((SEV_ERROR << 16) | (MOD_RUNTIME << 8) | ERR_FUNC_CALL)
#define ERROR_BAD_MEMORY         ((SEV_FATAL << 16) | (MOD_RUNTIME << 8) | ERR_BAD_MEMORY)
#define ERROR_INPUT_FAILED       ((SEV_ERROR << 16) | (MOD_IO << 8) | ERR_INPUT_FAILED)
#define ERROR_INVALID_INPUT      ((SEV_ERROR << 16) | (MOD_IO << 8) | ERR_INVALID_INPUT)

// Error messages
static const char* error_messages[] = {
    "No error",
    "Division by zero",
    "Modulo by zero",
    "Undefined variable",
    "Type mismatch",
    "Invalid operation",
    "Recursion error",
    "Function call error",
    "Bad memory access"
};

// Helper to check if a string is a string literal (starts and ends with ")
static int is_string_literal(const char* text) {
    if (!text) return 0;
    size_t len = strlen(text);
    return len >= 2 && text[0] == '"' && text[len-1] == '"';
}

// Simple variable environment: dynamic array of variable names and values
typedef struct {
    char* name;
    long long value;
} VarEntry;

static VarEntry* var_env = NULL;
static int var_env_size = 0;
static int var_env_capacity = 0;

// Simple module alias mapping
typedef struct {
    char* alias;
    ASTNode* module_ast;
} ModuleEntry;
static ModuleEntry* modules = NULL;
static int modules_size = 0;
static int modules_cap = 0;

// Base directory for resolving relative module paths
static char base_dir[1024] = "";
void eval_set_base_dir(const char* dir) {
    if (!dir) { base_dir[0] = '\0'; return; }
    size_t n = strlen(dir);
    if (n >= sizeof(base_dir)) n = sizeof(base_dir) - 1;
    strncpy(base_dir, dir, n);
    base_dir[n] = '\0';
}

static void compute_full_path(const char* path, char* out, size_t out_size) {
    const char* rel = path;
    if (rel[0] == '.' && rel[1] == '/') rel = rel + 2;
    if (base_dir[0]) snprintf(out, out_size, "%s/%s", base_dir, rel);
    else snprintf(out, out_size, "%s", rel);
}

// Global function registry
typedef struct {
    char* name;
    ASTNode* func_ast; // points to AST_FUNC node
} FuncEntry;
static FuncEntry* functions = NULL;
static int functions_size = 0;
static int functions_cap = 0;

static void register_function(const char* name, ASTNode* fn) {
    if (!name || !fn) return;
    if (functions_size >= functions_cap) {
        functions_cap = functions_cap ? functions_cap * 2 : 8;
        functions = (FuncEntry*)realloc(functions, functions_cap * sizeof(FuncEntry));
    }
    functions[functions_size].name = strdup(name);
    functions[functions_size].func_ast = fn;
    functions_size++;
}

static ASTNode* find_function_global(const char* name) {
    if (!name) return NULL;
    for (int i = functions_size - 1; i >= 0; i--) {
        if (strcmp(functions[i].name, name) == 0) return functions[i].func_ast;
    }
    // fallback: search modules directly
    for (int mi = 0; mi < modules_size; mi++) {
        ASTNode* fn = find_function_in_module(modules[mi].module_ast, name);
        if (fn) return fn;
    }
    return NULL;
}

// Error handling state
static int in_try_block = 0;
static int in_catch_block = 0; // NEW: track when evaluating catch body
static int error_occurred = 0;
static int error_value = 0;
static int error_printed = 0;

// Function return handling
static int return_flag = 0;
static long long return_value = 0;

// Global variables for tracking state
static int loop_counter = 0;

// Add global variable for current line
static int current_line = 1;

// Helper function to get error description
static const char* get_error_description(int error_code) {
    // Map error codes to descriptions
    switch (error_code) {
        case ERROR_DIVISION_BY_ZERO: return "division by zero";
        case ERROR_MODULO_BY_ZERO:   return "modulo by zero";
        case ERROR_UNDEFINED_VAR:    return "undefined variable";
        case ERROR_TYPE_MISMATCH:    return "type mismatch";
        case ERROR_INVALID_OP:       return "invalid operation";
        case ERROR_RECURSION:        return "recursion error";
        case ERROR_FUNC_CALL:        return "function call error";
        case ERROR_BAD_MEMORY:       return "bad memory access";
        case ERROR_INPUT_FAILED:     return "input failed";
        case ERROR_INVALID_INPUT:    return "invalid input";
        default:                     return "unknown error";
    }
}

// Helper function to format error message with proper error code
static void format_error_message(int error_code, int line, char* buffer, size_t size) {
    if (in_catch_block) {
        // For catch blocks: just the description
        snprintf(buffer, size, "%s%s%s", RED, get_error_description(error_code), RESET);
    } else {
        // Regular runtime report: include line
        const char* desc = get_error_description(error_code);
        char cap[256];
        strncpy(cap, desc, sizeof(cap) - 1);
        cap[sizeof(cap) - 1] = '\0';
        if (cap[0]) cap[0] = (char)toupper((unsigned char)cap[0]);
        snprintf(buffer, size, "%sLine %d: %s%s", RED, line, cap, RESET);
    }
}

// Helper function to set error state
static void set_error(int error_code) {
    error_occurred = 1;
    error_value = error_code;
    if (!in_try_block && !error_printed) {
        char error_msg[256];
        format_error_message(error_code, current_line, error_msg, sizeof(error_msg));
        fprintf(stderr, "%s\n", error_msg);
        error_printed = 1;
    }
}

// Helper function to handle error in catch block
static void handle_catch_error(int error_code) {
    char error_msg[256];
    format_error_message(error_code, current_line, error_msg, sizeof(error_msg));
    printf("%s\n", error_msg);
}

// Helper function to reset error state
static void reset_error_state() {
    error_occurred = 0;
    error_value = 0;
    error_printed = 0;
}

// Helper function to get a variable's value from the environment
static long long get_var_value(const char* name) {
    // Special case for loop counter
    if (strcmp(name, "i") == 0) {
        return loop_counter;
    }
    
    // Special case for error variable in catch block
    if (in_try_block && error_occurred && strcmp(name, "err") == 0) {
        return error_value;
    }
    
    // Check if variable exists
    for (int i = var_env_size - 1; i >= 0; i--) {
        if (strcmp(var_env[i].name, name) == 0) {
            return var_env[i].value;
        }
    }
    
    // If variable not found, return undefined variable error
    set_error(ERROR_UNDEFINED_VAR);
    return ERROR_UNDEFINED_VAR;
}

// Helper function to set a variable's value in the environment
static void set_var_value(const char* name, long long value) {
    // Special case for loop counter
    if (strcmp(name, "i") == 0) {
        loop_counter = value;
        return;
    }
    
    // Check if variable already exists
    for (int i = var_env_size - 1; i >= 0; i--) {
        if (strcmp(var_env[i].name, name) == 0) {
            var_env[i].value = value;
            return;
        }
    }
    
    // Variable not found, add it
    if (var_env_size >= var_env_capacity) {
        var_env_capacity = (var_env_capacity == 0) ? 10 : var_env_capacity * 2;
        var_env = realloc(var_env, var_env_capacity * sizeof(VarEntry));
        if (!var_env) {
            // Handle memory allocation error
            error_occurred = 1;
            error_value = ERROR_BAD_MEMORY;
            return;
        }
    }
    
    // Allocate new variable entry
    var_env[var_env_size].name = strdup(name);
    if (!var_env[var_env_size].name) {
        // Handle memory allocation error
        error_occurred = 1;
        error_value = ERROR_BAD_MEMORY;
        return;
    }
    
    var_env[var_env_size].value = value;
    var_env_size++;
}

// Helper function to check if a variable exists
static int var_exists(const char* name) {
    for (int i = 0; i < var_env_size; i++) {
        if (strcmp(var_env[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

// Helper: is this integer an error code of our 0xMY[SEV][MOD][ERR] scheme?
static int is_error_code(int value) {
    int sev = (value >> 16) & 0xFF;
    return sev == 0x02 || sev == 0x0F; // ERROR or FATAL
}

// Helper function to format error value for printing
static void format_error_value(int value, char* buffer, size_t size) {
    if (is_error_code(value)) {
        const char* desc = get_error_description(value);
        char lower_desc[256];
        strncpy(lower_desc, desc, sizeof(lower_desc) - 1);
        lower_desc[0] = tolower(lower_desc[0]);
        snprintf(buffer, size, "%s%s%s", RED, lower_desc, RESET);
    } else {
        snprintf(buffer, size, "%d", value);
    }
}

static ASTNode* load_and_parse_module(const char* path) {
    char full[2048];
    compute_full_path(path, full, sizeof(full));
    FILE* f = fopen(full, "r");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buf = (char*)malloc(sz + 1);
    if (!buf) { fclose(f); return NULL; }
    fread(buf, 1, sz, f); buf[sz] = '\0'; fclose(f);
    Token* toks = lexer_tokenize(buf);
    free(buf);
    if (!toks) return NULL;
    ASTNode* mod = parser_parse(toks);
    lexer_free_tokens(toks);
    return mod;
}

static void register_module(const char* alias, ASTNode* ast) {
    if (modules_size >= modules_cap) {
        modules_cap = modules_cap ? modules_cap * 2 : 4;
        modules = (ModuleEntry*)realloc(modules, modules_cap * sizeof(ModuleEntry));
    }
    modules[modules_size].alias = strdup(alias);
    modules[modules_size].module_ast = ast;
    modules_size++;
    // Register all functions from the module
    for (int i = 0; i < ast->child_count; i++) {
        ASTNode* n = &ast->children[i];
        if (n->type == AST_FUNC && n->text) {
            register_function(n->text, n);
        }
    }
}

static ASTNode* find_function_in_module(ASTNode* mod, const char* name) {
    if (!mod) return NULL;
    for (int i = 0; i < mod->child_count; i++) {
        ASTNode* n = &mod->children[i];
        if (n->type == AST_FUNC && n->text && strcmp(n->text, name) == 0) return n;
    }
    return NULL;
}

static ASTNode* resolve_module(const char* alias) {
    for (int i = 0; i < modules_size; i++) {
        if (strcmp(modules[i].alias, alias) == 0) return modules[i].module_ast;
    }
    return NULL;
}

// Interpret a user-defined function call: evaluate args, bind params, execute body, capture return
static long long eval_user_function_call(ASTNode* fn, ASTNode* args_node) {
    if (!fn) return 0;
    // find body index
    int body_index = -1;
    for (int i = 0; i < fn->child_count; i++) {
        if (fn->children[i].type == AST_BLOCK) { body_index = i; break; }
    }
    if (body_index < 0) return 0;
    // collect parameter names (AST_EXPR before body, excluding type markers like 'int')
    int param_indices[16]; int param_count = 0;
    for (int i = 0; i < body_index && param_count < 16; i++) {
        if (fn->children[i].type == AST_EXPR && fn->children[i].text && strcmp(fn->children[i].text, "int") != 0) {
            param_indices[param_count++] = i;
        }
    }
    // evaluate arguments
    long long argvals[16]; int argn = args_node ? args_node->child_count : 0;
    for (int i = 0; i < argn && i < 16; i++) {
        argvals[i] = eval_expression(&args_node->children[i]);
        if (error_occurred) return 0;
    }
    // new scope snapshot
    int old_env = var_env_size;
    // bind params
    for (int i = 0; i < param_count && i < argn; i++) {
        const char* pname = fn->children[param_indices[i]].text;
        set_var_value(pname, argvals[i]);
    }
    // execute
    int saved_return_flag = return_flag; long long saved_return_value = return_value;
    return_flag = 0; return_value = 0;
    eval_evaluate(&fn->children[body_index]);
    long long rv = return_value;
    // restore return state
    return_flag = saved_return_flag; return_value = saved_return_value;
    // restore scope
    for (int i = var_env_size - 1; i >= old_env; i--) {
        free(var_env[i].name);
    }
    var_env_size = old_env;
    return rv;
}

long long eval_expression(ASTNode* ast) {
    if (!ast) return 0;
    if (error_occurred) return 0;

    // Update current line for expression-level errors
    if (ast->line > 0) {
        current_line = ast->line;
    }

    if (ast->type == AST_EXPR && ast->text) {
        if (strcmp(ast->text, "+") == 0 || strcmp(ast->text, "-") == 0 ||
            strcmp(ast->text, "*") == 0 || strcmp(ast->text, "/") == 0 ||
            strcmp(ast->text, "%") == 0 || strcmp(ast->text, "==") == 0 ||
            strcmp(ast->text, "!=") == 0 || strcmp(ast->text, "<") == 0 ||
            strcmp(ast->text, ">") == 0 || strcmp(ast->text, "<=") == 0 ||
            strcmp(ast->text, ">=") == 0) {
            long long left = eval_expression(&ast->children[0]);
            if (error_occurred) return 0;
            long long right = eval_expression(&ast->children[1]);
            if (error_occurred) return 0;
            if (strcmp(ast->text, "+") == 0) return left + right;
            if (strcmp(ast->text, "-") == 0) return left - right;
            if (strcmp(ast->text, "*") == 0) return left * right;
            if (strcmp(ast->text, "/") == 0) {
                if (right == 0) { set_error(ERROR_DIVISION_BY_ZERO); return 0; }
                return left / right;
            }
            if (strcmp(ast->text, "%") == 0) {
                if (right == 0) { set_error(ERROR_MODULO_BY_ZERO); return 0; }
                return left % right;
            }
            if (strcmp(ast->text, "==") == 0) return left == right;
            if (strcmp(ast->text, "!=") == 0) return left != right;
            if (strcmp(ast->text, "<") == 0) return left < right;
            if (strcmp(ast->text, ">") == 0) return left > right;
            if (strcmp(ast->text, "<=") == 0) return left <= right;
            if (strcmp(ast->text, ">=") == 0) return left >= right;
        } else if (strcmp(ast->text, "call") == 0) {
            if (ast->child_count >= 2) {
                const char* name = ast->children[0].text; // function identifier
                // Built-in input(prompt): prompt user and map to an action code
                if (name && strcmp(name, "input") == 0) {
                    // Choose input/output stream
                    FILE* inp = stdin;
                    FILE* outp = stdout;
#ifndef _WIN32
                    FILE* opened_tty = NULL;
                    // Prefer interactive stdin; else try a TTY device on POSIX
                    if (!isatty(STDIN_FILENO)) {
                        const char* tty_env = getenv("MYCO_TTY");
                        if (tty_env && tty_env[0]) opened_tty = fopen(tty_env, "r+");
#ifdef L_ctermid
                        if (!opened_tty) { char tty_path[L_ctermid]; ctermid(tty_path); if (tty_path[0]) opened_tty = fopen(tty_path, "r+"); }
#endif
                        if (!opened_tty) opened_tty = fopen("/dev/tty", "r+");
                        if (opened_tty) { inp = opened_tty; outp = opened_tty; }
                    }
#endif

                    // Print prompt if provided
                    if (ast->children[1].child_count > 0) {
                        ASTNode* a0 = &ast->children[1].children[0];
                        if (a0->type == AST_EXPR && a0->text && is_string_literal(a0->text)) {
                            size_t len = strlen(a0->text);
                            if (len >= 2) {
                                fwrite(a0->text + 1, 1, len - 2, outp);
                                fflush(outp);
                            }
                        }
                    }

                    char buf[256];
                    if (!fgets(buf, sizeof(buf), inp)) {
#ifndef _WIN32
                        if (inp != stdin) fclose(inp);
#endif
                        error_occurred = 1;
                        error_value = ERROR_INPUT_FAILED;
                        return ERROR_INPUT_FAILED;
                    }
#ifndef _WIN32
                    if (inp != stdin) fclose(inp);
#endif

                    // Trim whitespace
                    size_t bl = strlen(buf);
                    while (bl && (buf[bl-1] == '\n' || buf[bl-1] == '\r' || buf[bl-1] == ' ' || buf[bl-1] == '\t')) { buf[--bl] = '\0'; }
                    size_t start = 0; while (start < bl && (buf[start] == ' ' || buf[start] == '\t')) start++;
                    if (start > 0) memmove(buf, buf + start, bl - start + 1);
                    bl = strlen(buf);
                    if (bl == 0) {
                        error_occurred = 1;
                        error_value = ERROR_INVALID_INPUT;
                        return ERROR_INVALID_INPUT;
                    }

                    // If numeric, return it
                    int all_digits = 1;
                    for (size_t i = 0; i < bl; i++) if (buf[i] < '0' || buf[i] > '9') { all_digits = 0; break; }
                    if (all_digits) {
                        long long val = strtoll(buf, NULL, 10);
                        return (int)val;
                    }
                    // Map commands
                    for (size_t i = 0; i < bl; i++) buf[i] = (char)tolower((unsigned char)buf[i]);
                    if (strcmp(buf, "left") == 0) return 1;
                    if (strcmp(buf, "right") == 0) return 2;
                    if (strcmp(buf, "up") == 0) return 3;
                    if (strcmp(buf, "down") == 0) return 4;
                    if (strcmp(buf, "attack") == 0) return 5;
                    error_occurred = 1;
                    error_value = ERROR_INVALID_INPUT;
                    return ERROR_INVALID_INPUT;
                }

                // No built-in factorial; all function calls must resolve to user-defined or imported functions
                // Alias-wrapped call: alias(function(args)) -> resolve in module and interpret
                {
                    ASTNode* mod_ast = resolve_module(name);
                    if (mod_ast && ast->children[1].child_count == 1) {
                        ASTNode* inner = &ast->children[1].children[0];
                        if (inner->type == AST_EXPR && inner->text && strcmp(inner->text, "call") == 0 && inner->child_count >= 2) {
                            const char* inner_name = inner->children[0].text;
                            ASTNode* fn = find_function_in_module(mod_ast, inner_name);
                            if (fn) {
                                return eval_user_function_call(fn, &inner->children[1]);
                            }
                        }
                    }
                }
                // Global or module-level function call by name
                {
                    // search modules
                    for (int mi = 0; mi < modules_size; mi++) {
                        ASTNode* fn = find_function_in_module(modules[mi].module_ast, name);
                        if (fn) return eval_user_function_call(fn, &ast->children[1]);
                    }
                    // search registered (including current file) functions
                    ASTNode* gfn = find_function_global(name);
                    if (gfn) return eval_user_function_call(gfn, &ast->children[1]);
                }
            }
        } else {
            if (strcmp(ast->text, "i") == 0) return loop_counter;
            char* end; long long v = strtoll(ast->text, &end, 10);
            if (*end == '\0') return (long long)v;
            long long varv = get_var_value(ast->text);
            if (varv == 0 && !var_exists(ast->text)) { set_error(ERROR_UNDEFINED_VAR); return 0; }
            return varv;
        }
    }
    return 0;
}

void eval_evaluate(ASTNode* ast) {
    if (!ast) return;
    
    // Update current line from AST node
    if (ast->line > 0) {
        current_line = ast->line;
    }

    switch (ast->type) {
        case AST_TRY: {
            // Save current variable environment size
            int old_var_env_size = var_env_size;
            
            // Set try block flag
            in_try_block = 1;
            
            // Reset error state for try block
            reset_error_state();
            
            // Evaluate try body
            for (int i = 0; i < ast->children[0].child_count; i++) {
                eval_evaluate(&ast->children[0].children[i]);
                if (error_occurred) {
                    break;
                }
            }
            
            // Reset try block flag
            in_try_block = 0;
            
            // If error occurred in try block, evaluate catch block
            if (error_occurred) {
                // Save the error that occurred
                int caught_error = error_value;
                
                // Reset error state for catch block
                reset_error_state();
                
                // Set the error variable with the caught error code
                set_var_value(ast->children[1].text, caught_error);
                
                // Mark that we are in catch for printing behavior
                in_catch_block = 1;
                // Evaluate catch body
                eval_evaluate(&ast->children[2]);
                in_catch_block = 0;
                // After catch block, reset error state since error was handled
                reset_error_state();
            }
            
            // Restore variable environment to state before try block
            for (int i = var_env_size - 1; i >= old_var_env_size; i--) {
                free(var_env[i].name);
            }
            var_env_size = old_var_env_size;
            break;
        }
        case AST_FUNC: {
            // Register function for later calls
            if (ast->text) register_function(ast->text, ast);
            break;
        }
        case AST_LET: {
            // Evaluate the right-hand side and assign to the variable
            long long value = eval_expression(&ast->children[1]);
            if (!error_occurred) {  // Only set if no error occurred
            set_var_value(ast->children[0].text, value);
            }
            break;
        }
        case AST_PRINT: {
            // Evaluate and print each argument
            for (int i = 0; i < ast->child_count; i++) {
                if (ast->children[i].type == AST_EXPR) {
                    // If it's a string literal, print it without quotes
                    if (is_string_literal(ast->children[i].text)) {
                        size_t len = strlen(ast->children[i].text);
                        printf("%.*s", (int)(len-2), ast->children[i].text+1);
                    } else {
                        // Otherwise evaluate the expression
                        long long value = eval_expression(&ast->children[i]);
                        // If evaluation produced an error code, print the error message instead of 0
                        if (is_error_code((int)value)) {
                            char msg[256];
                            format_error_message((int)value, current_line, msg, sizeof(msg));
                            printf("%s", msg);
                            // reset for further prints in same line
                            reset_error_state();
                        } else if (!error_occurred) {
                            // print as 64-bit signed
                            printf("%lld", value);
                        }
                    }
                    if (i < ast->child_count - 1 && !error_occurred) {
                        printf(" ");
                    }
                }
            }
            if (!error_occurred) {
                printf("\n");
            }
            break;
        }
        case AST_IF: {
            // Evaluate condition
            int condition = eval_expression(&ast->children[0]);
            if (!error_occurred) {  // Only proceed if no error occurred
            if (condition) {
                // Execute if body
                eval_evaluate(&ast->children[1]);
            } else if (ast->child_count > 2) {
                // Execute else body if it exists
                eval_evaluate(&ast->children[2]);
                }
            }
            break;
        }
        case AST_FOR: {
            // Get range start and end
            int start = eval_expression(&ast->children[1]);
            if (error_occurred) break;
            int end = eval_expression(&ast->children[2]);
            if (error_occurred) break;
            
            // Execute loop body for each value
            for (loop_counter = start; loop_counter <= end; loop_counter++) {
                eval_evaluate(&ast->children[3]);
                if (error_occurred) {
                    break;
                }
            }
            break;
        }
        case AST_SWITCH: {
            // Evaluate switch expression
            int value = eval_expression(&ast->children[0]);
            if (!error_occurred) {  // Only proceed if no error occurred
            // Find matching case
            int found_match = 0;
            for (int i = 0; i < ast->children[1].child_count; i++) {
                ASTNode* case_node = &ast->children[1].children[i];
                if (case_node->type == AST_CASE) {
                    int case_value = eval_expression(&case_node->children[0]);
                        if (!error_occurred && value == case_value) {
                        eval_evaluate(&case_node->children[1]);
                        found_match = 1;
                        break;
                    }
                    } else if (case_node->type == AST_DEFAULT && !found_match && !error_occurred) {
                    eval_evaluate(&case_node->children[0]);
                    }
                }
            }
            break;
        }
        case AST_BLOCK: {
            // handle 'use' directive blocks or execute normal blocks
            if (ast->text && strcmp(ast->text, "use") == 0 && ast->child_count == 2) {
                const char* path = ast->children[0].text;
                const char* alias = ast->children[1].text;
                // Compute full path and module dir
                char full[2048]; compute_full_path(path, full, sizeof(full));
                ASTNode* mod = load_and_parse_module(path);
                if (mod) {
                    register_module(alias, mod);
                    // Push base_dir to module's directory for nested imports and top-level init
                    char saved_base[1024]; strncpy(saved_base, base_dir, sizeof(saved_base)-1); saved_base[sizeof(saved_base)-1] = '\0';
                    char* last_slash = strrchr(full, '/');
                    if (last_slash) {
                        size_t n = (size_t)(last_slash - full);
                        char mod_dir[1024]; if (n >= sizeof(mod_dir)) n = sizeof(mod_dir)-1; memcpy(mod_dir, full, n); mod_dir[n] = '\0';
                        eval_set_base_dir(mod_dir);
                    }
                    // Evaluate module top-level (non-function) statements to init state
                    for (int i = 0; i < mod->child_count; i++) {
                        ASTNode* child = &mod->children[i];
                        if (child->type != AST_FUNC) {
                            eval_evaluate(child);
                            if (error_occurred && in_try_block) break;
                        }
                    }
                    // Restore base_dir
                    eval_set_base_dir(saved_base);
                }
                else {
                    char msg[256];
                    set_error(ERROR_FUNC_CALL);
                    format_error_message(error_value, ast->line, msg, sizeof(msg));
                    fprintf(stderr, "%s\n", msg);
                    reset_error_state();
                }
            } else {
                // Execute each child statement in the block
                for (int i = 0; i < ast->child_count; i++) {
                    eval_evaluate(&ast->children[i]);
                    if ((error_occurred && in_try_block) || return_flag) break;
                }
            }
            break;
        }
        case AST_RETURN: {
            long long value = 0;
            if (ast->child_count > 0) {
                value = eval_expression(&ast->children[0]);
            }
            return_value = value;
            return_flag = 1;
            break;
        }
    }

    // Evaluate children for non-control flow nodes (exclude FUNC as well)
    if (ast->type != AST_IF && ast->type != AST_FOR && 
        ast->type != AST_SWITCH && ast->type != AST_TRY &&
        ast->type != AST_FUNC && ast->type != AST_BLOCK && ast->type != AST_RETURN) {
        for (int i = 0; i < ast->child_count; i++) {
            eval_evaluate(&ast->children[i]);
            if (return_flag) break;
        }
    }

    // Evaluate next node in linked list
    if (ast->next) {
        eval_evaluate(ast->next);
    }
} 