#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// AST Node Creation Functions
ASTNode* ast_create_number(double value, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_NUMBER;
    node->data.number_value = value;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_bool(int value, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_BOOL;
    node->data.bool_value = value;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_null(int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_NULL;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_string(const char* value, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_STRING;
    node->data.string_value = strdup(value);
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_identifier(const char* name, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_IDENTIFIER;
    node->data.identifier_value = strdup(name);
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_typed_parameter(const char* name, const char* type, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_TYPED_PARAMETER;
    node->data.typed_parameter.parameter_name = strdup(name);
    node->data.typed_parameter.parameter_type = strdup(type);
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_binary_op(BinaryOperator op, ASTNode* left, ASTNode* right, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_BINARY_OP;
    node->data.binary.op = op;
    node->data.binary.left = left;
    node->data.binary.right = right;
    node->data.binary.step = NULL;  // Initialize step to NULL for regular binary ops
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_range_with_step(ASTNode* start, ASTNode* end, ASTNode* step, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_BINARY_OP;
    node->data.binary.op = OP_RANGE_STEP;
    node->data.binary.left = start;
    node->data.binary.right = end;
    node->data.binary.step = step;  // Store step in a custom field
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_unary_op(UnaryOperator op, ASTNode* operand, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_UNARY_OP;
    node->data.unary.op = op;
    node->data.unary.operand = operand;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_assignment(const char* variable, ASTNode* value, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_ASSIGNMENT;
    node->data.assignment.variable_name = strdup(variable);
    node->data.assignment.value = value;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_function_call(const char* name, ASTNode** args, size_t arg_count, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_FUNCTION_CALL;
    node->data.function_call.function_name = strdup(name);
    if (!node->data.function_call.function_name) {
        free(node);
        return NULL;
    }
    node->data.function_call.arguments = args;
    node->data.function_call.argument_count = arg_count;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_variable_declaration(const char* name, const char* type, ASTNode* initial_value, int is_mutable, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_VARIABLE_DECLARATION;
    node->data.variable_declaration.variable_name = strdup(name);
    node->data.variable_declaration.type_name = type ? strdup(type) : NULL;
    node->data.variable_declaration.initial_value = initial_value;
    node->data.variable_declaration.is_mutable = is_mutable;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_if_statement(ASTNode* condition, ASTNode* then_block, ASTNode* else_block, ASTNode* else_if_chain, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_IF_STATEMENT;
    node->data.if_statement.condition = condition;
    node->data.if_statement.then_block = then_block;
    node->data.if_statement.else_block = else_block;
    node->data.if_statement.else_if_chain = else_if_chain;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_while_loop(ASTNode* condition, ASTNode* body, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_WHILE_LOOP;
    node->data.while_loop.condition = condition;
    node->data.while_loop.body = body;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_for_loop(const char* iterator, ASTNode* collection, ASTNode* body, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_FOR_LOOP;
    node->data.for_loop.iterator_name = strdup(iterator);
    node->data.for_loop.collection = collection;
    node->data.for_loop.body = body;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_block(ASTNode** statements, size_t statement_count, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_BLOCK;
    node->data.block.statements = statements;
    node->data.block.statement_count = statement_count;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_return(ASTNode* value, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_RETURN;
    node->data.return_statement.value = value;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_throw(ASTNode* value, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_THROW;
    node->data.throw_statement.value = value;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_try_catch(ASTNode* try_block, const char* catch_var, ASTNode* catch_block, ASTNode* finally_block, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_TRY_CATCH;
    node->data.try_catch.try_block = try_block;
    node->data.try_catch.catch_variable = catch_var ? strdup(catch_var) : NULL;
    node->data.try_catch.catch_block = catch_block;
    node->data.try_catch.finally_block = finally_block;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_switch(ASTNode* expression, ASTNode** cases, size_t case_count, ASTNode* default_case, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_SWITCH;
    node->data.switch_statement.expression = expression;
    node->data.switch_statement.cases = cases;
    node->data.switch_statement.case_count = case_count;
    node->data.switch_statement.default_case = default_case;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_match(ASTNode* expression, ASTNode** patterns, size_t pattern_count, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_MATCH;
    node->data.match.expression = expression;
    node->data.match.patterns = patterns;
    node->data.match.pattern_count = pattern_count;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_spore(ASTNode* expression, ASTNode** cases, size_t case_count, ASTNode* root_case, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_SPORE;
    node->data.spore.expression = expression;
    node->data.spore.cases = cases;
    node->data.spore.case_count = case_count;
    node->data.spore.root_case = root_case;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_spore_case(ASTNode* pattern, ASTNode* body, int is_lambda, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_SPORE_CASE;
    node->data.spore_case.pattern = pattern;
    node->data.spore_case.body = body;
    node->data.spore_case.is_lambda = is_lambda;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_class(const char* name, const char* parent, ASTNode* body, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_CLASS;
    node->data.class_definition.class_name = strdup(name);
    node->data.class_definition.parent_class = parent ? strdup(parent) : NULL;
    node->data.class_definition.body = body;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_function(const char* name, ASTNode** params, size_t param_count, const char* return_type, ASTNode* body, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_FUNCTION;
    node->data.function_definition.function_name = strdup(name);
    node->data.function_definition.parameters = params;
    node->data.function_definition.parameter_count = param_count;
    node->data.function_definition.return_type = return_type ? strdup(return_type) : NULL;
    node->data.function_definition.body = body;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_lambda(ASTNode** params, size_t param_count, const char* return_type, ASTNode* body, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_LAMBDA;
    node->data.lambda.parameters = params;
    node->data.lambda.parameter_count = param_count;
    node->data.lambda.return_type = return_type ? strdup(return_type) : NULL;
    node->data.lambda.body = body;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_array_literal(ASTNode** elements, size_t element_count, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_ARRAY_LITERAL;
    node->data.array_literal.elements = elements;
    node->data.array_literal.element_count = element_count;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_hash_map_literal(ASTNode** keys, ASTNode** values, size_t pair_count, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_HASH_MAP_LITERAL;
    node->data.hash_map_literal.keys = keys;
    node->data.hash_map_literal.values = values;
    node->data.hash_map_literal.pair_count = pair_count;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_set_literal(ASTNode** elements, size_t element_count, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_SET_LITERAL;
    node->data.set_literal.elements = elements;
    node->data.set_literal.element_count = element_count;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_array_access(ASTNode* array, ASTNode* index, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_ARRAY_ACCESS;
    node->data.array_access.array = array;
    node->data.array_access.index = index;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_function_call_expr(ASTNode* function, ASTNode** args, size_t arg_count, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_FUNCTION_CALL_EXPR;
    node->data.function_call_expr.function = function;
    node->data.function_call_expr.arguments = args;
    node->data.function_call_expr.argument_count = arg_count;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_member_access(ASTNode* object, const char* member_name, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_MEMBER_ACCESS;
    node->data.member_access.object = object;
    node->data.member_access.member_name = strdup(member_name);
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_import(const char* module, const char* alias, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_IMPORT;
    node->data.import_statement.module_name = strdup(module);
    node->data.import_statement.alias = alias ? strdup(alias) : NULL;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_use(const char* library, const char* alias, char** specific_items, char** specific_aliases, size_t item_count, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_USE;
    node->data.use_statement.library_name = strdup(library);
    node->data.use_statement.alias = alias ? strdup(alias) : NULL;
    node->data.use_statement.item_count = item_count;
    
    if (specific_items && item_count > 0) {
        node->data.use_statement.specific_items = malloc(item_count * sizeof(char*));
        if (!node->data.use_statement.specific_items) {
            free(node);
            return NULL;
        }
        for (size_t i = 0; i < item_count; i++) {
            node->data.use_statement.specific_items[i] = strdup(specific_items[i]);
        }
    } else {
        node->data.use_statement.specific_items = NULL;
    }
    
    if (specific_aliases && item_count > 0) {
        node->data.use_statement.specific_aliases = malloc(item_count * sizeof(char*));
        if (!node->data.use_statement.specific_aliases) {
            // Clean up specific_items if we allocated them
            if (node->data.use_statement.specific_items) {
                for (size_t i = 0; i < item_count; i++) {
                    free(node->data.use_statement.specific_items[i]);
                }
                free(node->data.use_statement.specific_items);
            }
            free(node);
            return NULL;
        }
        for (size_t i = 0; i < item_count; i++) {
            node->data.use_statement.specific_aliases[i] = specific_aliases[i] ? strdup(specific_aliases[i]) : NULL;
        }
    } else {
        node->data.use_statement.specific_aliases = NULL;
    }
    
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_module(const char* name, ASTNode* body, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_MODULE;
    node->data.module_definition.module_name = strdup(name);
    node->data.module_definition.body = body;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_package(const char* name, ASTNode* body, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_PACKAGE;
    node->data.package_definition.package_name = strdup(name);
    node->data.package_definition.body = body;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

// AST Node Management Functions
void ast_free(ASTNode* node) {
    if (!node) return;
    
    // Free node-specific data
    switch (node->type) {
        case AST_NODE_STRING:
        case AST_NODE_IDENTIFIER:
            free(node->data.string_value);
            break;
            
        case AST_NODE_TYPED_PARAMETER:
            free(node->data.typed_parameter.parameter_name);
            free(node->data.typed_parameter.parameter_type);
            break;
            
        case AST_NODE_BOOL:
            // Booleans don't need cleanup
            break;
            
        case AST_NODE_BINARY_OP:
            ast_free(node->data.binary.left);
            ast_free(node->data.binary.right);
            if (node->data.binary.step) {
                ast_free(node->data.binary.step);
            }
            break;
            
        case AST_NODE_UNARY_OP:
            ast_free(node->data.unary.operand);
            break;
            
        case AST_NODE_ASSIGNMENT:
            free(node->data.assignment.variable_name);
            ast_free(node->data.assignment.value);
            break;
            
        case AST_NODE_FUNCTION_CALL:
            free(node->data.function_call.function_name);
            for (size_t i = 0; i < node->data.function_call.argument_count; i++) {
                ast_free(node->data.function_call.arguments[i]);
            }
            free(node->data.function_call.arguments);
            break;
            
        case AST_NODE_VARIABLE_DECLARATION:
            free(node->data.variable_declaration.variable_name);
            if (node->data.variable_declaration.type_name) {
                free(node->data.variable_declaration.type_name);
            }
            if (node->data.variable_declaration.initial_value) {
                ast_free(node->data.variable_declaration.initial_value);
            }
            break;
            
        case AST_NODE_IF_STATEMENT:
            ast_free(node->data.if_statement.condition);
            ast_free(node->data.if_statement.then_block);
            if (node->data.if_statement.else_block) {
                ast_free(node->data.if_statement.else_block);
            }
            break;
            
        case AST_NODE_WHILE_LOOP:
            ast_free(node->data.while_loop.condition);
            ast_free(node->data.while_loop.body);
            break;
            
        case AST_NODE_FOR_LOOP:
            free(node->data.for_loop.iterator_name);
            ast_free(node->data.for_loop.collection);
            ast_free(node->data.for_loop.body);
            break;
            
        case AST_NODE_BLOCK:
            for (size_t i = 0; i < node->data.block.statement_count; i++) {
                ast_free(node->data.block.statements[i]);
            }
            free(node->data.block.statements);
            break;
            
        case AST_NODE_RETURN:
            if (node->data.return_statement.value) {
                ast_free(node->data.return_statement.value);
            }
            break;
            
        case AST_NODE_TRY_CATCH:
            ast_free(node->data.try_catch.try_block);
            if (node->data.try_catch.catch_variable) {
                free(node->data.try_catch.catch_variable);
            }
            if (node->data.try_catch.catch_block) {
                ast_free(node->data.try_catch.catch_block);
            }
            if (node->data.try_catch.finally_block) {
                ast_free(node->data.try_catch.finally_block);
            }
            break;
            
        case AST_NODE_SWITCH:
            ast_free(node->data.switch_statement.expression);
            for (size_t i = 0; i < node->data.switch_statement.case_count; i++) {
                ast_free(node->data.switch_statement.cases[i]);
            }
            free(node->data.switch_statement.cases);
            if (node->data.switch_statement.default_case) {
                ast_free(node->data.switch_statement.default_case);
            }
            break;
            
        case AST_NODE_MATCH:
            ast_free(node->data.match.expression);
            for (size_t i = 0; i < node->data.match.pattern_count; i++) {
                ast_free(node->data.match.patterns[i]);
            }
            free(node->data.match.patterns);
            break;
            
        case AST_NODE_SPORE:
            ast_free(node->data.spore.expression);
            for (size_t i = 0; i < node->data.spore.case_count; i++) {
                ast_free(node->data.spore.cases[i]);
            }
            free(node->data.spore.cases);
            if (node->data.spore.root_case) {
                ast_free(node->data.spore.root_case);
            }
            break;
            
        case AST_NODE_SPORE_CASE:
            ast_free(node->data.spore_case.pattern);
            ast_free(node->data.spore_case.body);
            break;
            
        case AST_NODE_ERROR:
            free(node->data.error_node.error_message);
            break;
            
        case AST_NODE_CLASS:
            free(node->data.class_definition.class_name);
            if (node->data.class_definition.parent_class) {
                free(node->data.class_definition.parent_class);
            }
            ast_free(node->data.class_definition.body);
            break;
            
        case AST_NODE_FUNCTION:
            free(node->data.function_definition.function_name);
            for (size_t i = 0; i < node->data.function_definition.parameter_count; i++) {
                ast_free(node->data.function_definition.parameters[i]);
            }
            free(node->data.function_definition.parameters);
            if (node->data.function_definition.return_type) {
                free(node->data.function_definition.return_type);
            }
            ast_free(node->data.function_definition.body);
            break;
            
        case AST_NODE_LAMBDA:
            for (size_t i = 0; i < node->data.lambda.parameter_count; i++) {
                ast_free(node->data.lambda.parameters[i]);
            }
            free(node->data.lambda.parameters);
            if (node->data.lambda.return_type) {
                free(node->data.lambda.return_type);
            }
            ast_free(node->data.lambda.body);
            break;
            
        case AST_NODE_ARRAY_LITERAL:
            for (size_t i = 0; i < node->data.array_literal.element_count; i++) {
                ast_free(node->data.array_literal.elements[i]);
            }
            free(node->data.array_literal.elements);
            break;
            
        case AST_NODE_ARRAY_ACCESS:
            ast_free(node->data.array_access.array);
            ast_free(node->data.array_access.index);
            break;
            
        case AST_NODE_MEMBER_ACCESS:
            ast_free(node->data.member_access.object);
            free(node->data.member_access.member_name);
            break;
            
        case AST_NODE_FUNCTION_CALL_EXPR:
            ast_free(node->data.function_call_expr.function);
            if (node->data.function_call_expr.arguments) {
                for (size_t i = 0; i < node->data.function_call_expr.argument_count; i++) {
                    ast_free(node->data.function_call_expr.arguments[i]);
                }
                free(node->data.function_call_expr.arguments);
            }
            break;
            
        case AST_NODE_IMPORT:
            free(node->data.import_statement.module_name);
            if (node->data.import_statement.alias) {
                free(node->data.import_statement.alias);
            }
            break;
            
        case AST_NODE_USE:
            free(node->data.use_statement.library_name);
            if (node->data.use_statement.alias) {
                free(node->data.use_statement.alias);
            }
            if (node->data.use_statement.specific_items) {
                for (size_t i = 0; i < node->data.use_statement.item_count; i++) {
                    free(node->data.use_statement.specific_items[i]);
                }
                free(node->data.use_statement.specific_items);
            }
            if (node->data.use_statement.specific_aliases) {
                for (size_t i = 0; i < node->data.use_statement.item_count; i++) {
                    if (node->data.use_statement.specific_aliases[i]) {
                        free(node->data.use_statement.specific_aliases[i]);
                    }
                }
                free(node->data.use_statement.specific_aliases);
            }
            break;
            
        case AST_NODE_MODULE:
            free(node->data.module_definition.module_name);
            ast_free(node->data.module_definition.body);
            break;
            
        case AST_NODE_PACKAGE:
            free(node->data.package_definition.package_name);
            ast_free(node->data.package_definition.body);
            break;
            
        case AST_NODE_ASYNC_FUNCTION:
            free(node->data.async_function_definition.function_name);
            for (size_t i = 0; i < node->data.async_function_definition.parameter_count; i++) {
                ast_free(node->data.async_function_definition.parameters[i]);
            }
            free(node->data.async_function_definition.parameters);
            if (node->data.async_function_definition.return_type) {
                free(node->data.async_function_definition.return_type);
            }
            ast_free(node->data.async_function_definition.body);
            break;
            
        case AST_NODE_AWAIT:
            ast_free(node->data.await_expression.expression);
            break;
            
        case AST_NODE_PROMISE:
            ast_free(node->data.promise_creation.expression);
            break;
            
        default:
            break;
    }
    
    // Free the node itself
    free(node);
}

void ast_free_tree(ASTNode* root) {
    if (!root) return;
    
    // Free all nodes in the tree
    ASTNode* current = root;
    while (current) {
        ASTNode* next = current->next;
        ast_free(current);
        current = next;
    }
}

ASTNode* ast_clone(ASTNode* node) {
    if (!node) return NULL;
    
    ASTNode* clone = malloc(sizeof(ASTNode));
    if (!clone) return NULL;
    
    // Copy basic fields
    clone->type = node->type;
    clone->line = node->line;
    clone->column = node->column;
    clone->next = NULL;
    
    // Copy node-specific data
    switch (node->type) {
        case AST_NODE_NUMBER:
            clone->data.number_value = node->data.number_value;
            break;
            
        case AST_NODE_STRING:
            clone->data.string_value = strdup(node->data.string_value);
            break;
            
        case AST_NODE_IDENTIFIER:
            clone->data.identifier_value = strdup(node->data.identifier_value);
            break;
            
        case AST_NODE_TYPED_PARAMETER:
            clone->data.typed_parameter.parameter_name = strdup(node->data.typed_parameter.parameter_name);
            clone->data.typed_parameter.parameter_type = strdup(node->data.typed_parameter.parameter_type);
            break;
            
        case AST_NODE_BINARY_OP:
            clone->data.binary.op = node->data.binary.op;
            clone->data.binary.left = ast_clone(node->data.binary.left);
            clone->data.binary.right = ast_clone(node->data.binary.right);
            break;
            
        case AST_NODE_UNARY_OP:
            clone->data.unary.op = node->data.unary.op;
            clone->data.unary.operand = ast_clone(node->data.unary.operand);
            break;
            
        case AST_NODE_RETURN:
            clone->data.return_statement.value = ast_clone(node->data.return_statement.value);
            break;
            
        case AST_NODE_BLOCK:
            clone->data.block.statement_count = node->data.block.statement_count;
            clone->data.block.statements = malloc(node->data.block.statement_count * sizeof(ASTNode*));
            for (size_t i = 0; i < node->data.block.statement_count; i++) {
                clone->data.block.statements[i] = ast_clone(node->data.block.statements[i]);
            }
            break;
            
        case AST_NODE_FUNCTION_CALL:
            clone->data.function_call.function_name = strdup(node->data.function_call.function_name);
            clone->data.function_call.argument_count = node->data.function_call.argument_count;
            clone->data.function_call.arguments = malloc(node->data.function_call.argument_count * sizeof(ASTNode*));
            for (size_t i = 0; i < node->data.function_call.argument_count; i++) {
                clone->data.function_call.arguments[i] = ast_clone(node->data.function_call.arguments[i]);
            }
            break;
            
        case AST_NODE_ASSIGNMENT:
            clone->data.assignment.variable_name = strdup(node->data.assignment.variable_name);
            clone->data.assignment.value = ast_clone(node->data.assignment.value);
            break;
            
        case AST_NODE_VARIABLE_DECLARATION:
            clone->data.variable_declaration.variable_name = strdup(node->data.variable_declaration.variable_name);
            clone->data.variable_declaration.type_name = node->data.variable_declaration.type_name ? strdup(node->data.variable_declaration.type_name) : NULL;
            clone->data.variable_declaration.initial_value = ast_clone(node->data.variable_declaration.initial_value);
            clone->data.variable_declaration.is_mutable = node->data.variable_declaration.is_mutable;
            break;
            
        // Add other cases as needed...
        default:
            // For now, just copy the basic structure
            break;
    }
    
    return clone;
}

ASTNode* ast_clone_tree(ASTNode* root) {
    if (!root) return NULL;
    
    ASTNode* cloned_root = NULL;
    ASTNode* cloned_current = NULL;
    ASTNode* current = root;
    
    while (current) {
        ASTNode* cloned = ast_clone(current);
        if (!cloned) {
            ast_free_tree(cloned_root);
            return NULL;
        }
        
        if (!cloned_root) {
            cloned_root = cloned;
            cloned_current = cloned;
        } else {
            cloned_current->next = cloned;
            cloned_current = cloned;
        }
        
        current = current->next;
    }
    
    return cloned_root;
}

// AST Utility Functions
void ast_print(ASTNode* node, int indent) {
    if (!node) return;
    
    // Print indentation
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
    
    // Print node information
    printf("%s", ast_node_type_to_string(node->type));
    
    switch (node->type) {
        case AST_NODE_NUMBER:
            printf("(%.6f)", node->data.number_value);
            break;
            
        case AST_NODE_STRING:
            printf("(\"%s\")", node->data.string_value);
            break;
            
        case AST_NODE_IDENTIFIER:
            printf("(%s)", node->data.identifier_value);
            break;
            
        case AST_NODE_TYPED_PARAMETER:
            printf("(%s: %s)", node->data.typed_parameter.parameter_name, node->data.typed_parameter.parameter_type);
            break;
            
        case AST_NODE_BINARY_OP:
            printf("(%s)", binary_op_to_string(node->data.binary.op));
            break;
            
        case AST_NODE_UNARY_OP:
            printf("(%s)", unary_op_to_string(node->data.unary.op));
            break;
            
        case AST_NODE_ASSIGNMENT:
            printf("(%s)", node->data.assignment.variable_name);
            break;
            
        case AST_NODE_FUNCTION_CALL:
            printf("(%s)", node->data.function_call.function_name);
            break;
            
        default:
            break;
    }
    
    printf(" [%d:%d]\n", node->line, node->column);
    
    // Print children
    switch (node->type) {
        case AST_NODE_BINARY_OP:
            ast_print(node->data.binary.left, indent + 1);
            ast_print(node->data.binary.right, indent + 1);
            break;
            
        case AST_NODE_UNARY_OP:
            ast_print(node->data.unary.operand, indent + 1);
            break;
            
        case AST_NODE_ASSIGNMENT:
            ast_print(node->data.assignment.value, indent + 1);
            break;
            
        case AST_NODE_FUNCTION_CALL:
            for (size_t i = 0; i < node->data.function_call.argument_count; i++) {
                ast_print(node->data.function_call.arguments[i], indent + 1);
            }
            break;
            
        case AST_NODE_IF_STATEMENT:
            ast_print(node->data.if_statement.condition, indent + 1);
            ast_print(node->data.if_statement.then_block, indent + 1);
            if (node->data.if_statement.else_block) {
                ast_print(node->data.if_statement.else_block, indent + 1);
            }
            break;
            
        case AST_NODE_WHILE_LOOP:
            ast_print(node->data.while_loop.condition, indent + 1);
            ast_print(node->data.while_loop.body, indent + 1);
            break;
            
        case AST_NODE_FOR_LOOP:
            ast_print(node->data.for_loop.collection, indent + 1);
            ast_print(node->data.for_loop.body, indent + 1);
            break;
            
        case AST_NODE_BLOCK:
            for (size_t i = 0; i < node->data.block.statement_count; i++) {
                ast_print(node->data.block.statements[i], indent + 1);
            }
            break;
            
        case AST_NODE_RETURN:
            if (node->data.return_statement.value) {
                ast_print(node->data.return_statement.value, indent + 1);
            }
            break;
            
        default:
            break;
    }
}

void ast_print_tree(ASTNode* root) {
    printf("Abstract Syntax Tree:\n");
    printf("=====================\n");
    
    ASTNode* current = root;
    while (current) {
        ast_print(current, 0);
        current = current->next;
    }
}

const char* ast_node_type_to_string(ASTNodeType type) {
    switch (type) {
        case AST_NODE_NUMBER: return "Number";
        case AST_NODE_STRING: return "String";
        case AST_NODE_BOOL: return "Bool";
        case AST_NODE_NULL: return "Null";
        case AST_NODE_IDENTIFIER: return "Identifier";
        case AST_NODE_TYPED_PARAMETER: return "TypedParameter";
        case AST_NODE_BINARY_OP: return "BinaryOp";
        case AST_NODE_UNARY_OP: return "UnaryOp";
        case AST_NODE_ASSIGNMENT: return "Assignment";
        case AST_NODE_FUNCTION_CALL: return "FunctionCall";
        case AST_NODE_VARIABLE_DECLARATION: return "VariableDeclaration";
        case AST_NODE_IF_STATEMENT: return "IfStatement";
        case AST_NODE_WHILE_LOOP: return "WhileLoop";
        case AST_NODE_FOR_LOOP: return "ForLoop";
        case AST_NODE_BLOCK: return "Block";
        case AST_NODE_RETURN: return "Return";
        case AST_NODE_BREAK: return "Break";
        case AST_NODE_CONTINUE: return "Continue";
        case AST_NODE_TRY_CATCH: return "TryCatch";
        case AST_NODE_SWITCH: return "Switch";
        case AST_NODE_MATCH: return "Match";
        case AST_NODE_SPORE: return "Spore";
        case AST_NODE_SPORE_CASE: return "SporeCase";
        case AST_NODE_CLASS: return "Class";
        case AST_NODE_FUNCTION: return "Function";
        case AST_NODE_LAMBDA: return "Lambda";
        case AST_NODE_ARRAY_LITERAL: return "ArrayLiteral";
        case AST_NODE_ARRAY_ACCESS: return "ArrayAccess";
        case AST_NODE_MEMBER_ACCESS: return "MemberAccess";
        case AST_NODE_FUNCTION_CALL_EXPR: return "FunctionCallExpr";
        case AST_NODE_IMPORT: return "Import";
        case AST_NODE_USE: return "Use";
        case AST_NODE_MODULE: return "Module";
        case AST_NODE_PACKAGE: return "Package";
        case AST_NODE_ASYNC_FUNCTION: return "AsyncFunction";
        case AST_NODE_AWAIT: return "Await";
        case AST_NODE_PROMISE: return "Promise";
        case AST_NODE_ERROR: return "Error";
        default: return "Unknown";
    }
}

const char* binary_op_to_string(BinaryOperator op) {
    switch (op) {
        case OP_ADD: return "+";
        case OP_SUBTRACT: return "-";
        case OP_MULTIPLY: return "*";
        case OP_DIVIDE: return "/";
        case OP_MODULO: return "%";
        case OP_POWER: return "**";
        case OP_EQUAL: return "==";
        case OP_NOT_EQUAL: return "!=";
        case OP_LESS_THAN: return "<";
        case OP_LESS_EQUAL: return "<=";
        case OP_GREATER_THAN: return ">";
        case OP_GREATER_EQUAL: return ">=";
        case OP_LOGICAL_AND: return "&&";
        case OP_LOGICAL_OR: return "||";
        case OP_LOGICAL_XOR: return "^^";
        case OP_BITWISE_AND: return "&";
        case OP_BITWISE_OR: return "|";
        case OP_BITWISE_XOR: return "^";
        case OP_LEFT_SHIFT: return "<<";
        case OP_RIGHT_SHIFT: return ">>";
        case OP_RANGE: return "..";
        case OP_RANGE_INCLUSIVE: return "..=";
        default: return "?";
    }
}

const char* unary_op_to_string(UnaryOperator op) {
    switch (op) {
        case OP_POSITIVE: return "+";
        case OP_NEGATIVE: return "-";
        case OP_LOGICAL_NOT: return "!";
        case OP_BITWISE_NOT: return "~";
        case OP_DEREFERENCE: return "*";
        case OP_ADDRESS_OF: return "&";
        default: return "?";
    }
}

// AST Error Node Creation Function
ASTNode* ast_create_error_node(const char* error_message, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_ERROR;
    node->data.error_node.error_message = strdup(error_message);
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

// Async/Await AST Node Creation Functions
ASTNode* ast_create_async_function(const char* name, ASTNode** params, size_t param_count, const char* return_type, ASTNode* body, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_ASYNC_FUNCTION;
    node->data.async_function_definition.function_name = strdup(name);
    node->data.async_function_definition.parameters = params;
    node->data.async_function_definition.parameter_count = param_count;
    node->data.async_function_definition.return_type = return_type ? strdup(return_type) : NULL;
    node->data.async_function_definition.body = body;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_await(ASTNode* expression, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_AWAIT;
    node->data.await_expression.expression = expression;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

ASTNode* ast_create_promise(ASTNode* expression, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NODE_PROMISE;
    node->data.promise_creation.expression = expression;
    node->line = line;
    node->column = column;
    node->next = NULL;
    
    return node;
}

// AST Validation Functions
int ast_validate(ASTNode* node) {
    if (!node) return 0;
    
    // Basic validation
    if (node->line < 0 || node->column < 0) return 0;
    
    // Type-specific validation
    switch (node->type) {
        case AST_NODE_BINARY_OP:
            if (!node->data.binary.left || !node->data.binary.right) return 0;
            if (!ast_validate(node->data.binary.left)) return 0;
            if (!ast_validate(node->data.binary.right)) return 0;
            break;
            
        case AST_NODE_UNARY_OP:
            if (!node->data.unary.operand) return 0;
            if (!ast_validate(node->data.unary.operand)) return 0;
            break;
            
        case AST_NODE_ASSIGNMENT:
            if (!node->data.assignment.variable_name) return 0;
            if (!node->data.assignment.value) return 0;
            if (!ast_validate(node->data.assignment.value)) return 0;
            break;
            
        case AST_NODE_FUNCTION_CALL:
            if (!node->data.function_call.function_name) return 0;
            if (node->data.function_call.arguments) {
                for (size_t i = 0; i < node->data.function_call.argument_count; i++) {
                    if (!ast_validate(node->data.function_call.arguments[i])) return 0;
                }
            }
            break;
            
        case AST_NODE_SPORE:
            if (!node->data.spore.expression) return 0;
            if (!ast_validate(node->data.spore.expression)) return 0;
            if (node->data.spore.cases) {
                for (size_t i = 0; i < node->data.spore.case_count; i++) {
                    if (!ast_validate(node->data.spore.cases[i])) return 0;
                }
            }
            if (node->data.spore.root_case && !ast_validate(node->data.spore.root_case)) return 0;
            break;
            
        case AST_NODE_SPORE_CASE:
            if (!node->data.spore_case.pattern) return 0;
            if (!node->data.spore_case.body) return 0;
            if (!ast_validate(node->data.spore_case.pattern)) return 0;
            if (!ast_validate(node->data.spore_case.body)) return 0;
            break;
            
        // Add other validation cases as needed...
        default:
            break;
    }
    
    return 1;
}

int ast_validate_tree(ASTNode* root) {
    if (!root) return 0;
    
    ASTNode* current = root;
    while (current) {
        if (!ast_validate(current)) return 0;
        current = current->next;
    }
    
    return 1;
}
