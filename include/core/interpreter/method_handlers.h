#ifndef METHOD_HANDLERS_H
#define METHOD_HANDLERS_H

#include "../interpreter.h"
#include <stddef.h>

// ============================================================================
// METHOD DISPATCH FUNCTIONS
// ============================================================================

// Main method dispatcher
Value handle_method_call(Interpreter* interpreter, ASTNode* call_node, Value object);

// Super method handling
Value handle_super_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name);

// ============================================================================
// LIBRARY METHOD HANDLERS
// ============================================================================

// Data structure method handlers
Value handle_tree_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_graph_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_heap_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_queue_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_stack_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);

// Server method handlers
Value handle_server_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_request_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_response_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_route_group_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);
Value handle_server_library_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object);

#endif // METHOD_HANDLERS_H
