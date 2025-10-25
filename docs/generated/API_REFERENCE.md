# API Reference

This document contains all built-in functions available in Myco.

## Core Functions

### adaptive_executor_execute()

**Signature:** `Value adaptive_executor_execute(AdaptiveExecutor* executor, 
                               Interpreter* interpreter, 
                               ASTNode* node)`

**Parameters:**
- `executor` (AdaptiveExecutor*)
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `adaptive_executor.c`

---

### adaptive_executor_execute_ast()

**Signature:** `Value adaptive_executor_execute_ast(AdaptiveExecutor* executor, 
                                  Interpreter* interpreter, 
                                  ASTNode* node)`

**Parameters:**
- `executor` (AdaptiveExecutor*)
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `adaptive_executor.c`

---

### adaptive_executor_execute_bytecode()

**Signature:** `Value adaptive_executor_execute_bytecode(AdaptiveExecutor* executor, 
                                       Interpreter* interpreter, 
                                       ASTNode* node)`

**Parameters:**
- `executor` (AdaptiveExecutor*)
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `adaptive_executor.c`

---

### adaptive_executor_execute_jit()

**Signature:** `Value adaptive_executor_execute_jit(AdaptiveExecutor* executor, 
                                  Interpreter* interpreter, 
                                  ASTNode* node)`

**Parameters:**
- `executor` (AdaptiveExecutor*)
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `adaptive_executor.c`

---

### adaptive_executor_execute_specialized()

**Signature:** `Value adaptive_executor_execute_specialized(AdaptiveExecutor* executor, 
                                          Interpreter* interpreter, 
                                          ASTNode* node)`

**Parameters:**
- `executor` (AdaptiveExecutor*)
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `adaptive_executor.c`

---

### bytecode_execute()

**Signature:** `Value bytecode_execute(BytecodeProgram* program, Interpreter* interpreter, int debug)`

**Parameters:**
- `program` (BytecodeProgram*)
- `interpreter` (Interpreter*)
- `debug` (int)

**Source:** `bytecode_vm.c`

---

### bytecode_execute_function_bytecode()

**Signature:** `Value bytecode_execute_function_bytecode(Interpreter* interpreter, BytecodeFunction* func, Value* args, int arg_count, BytecodeProgram* program)`

**Parameters:**
- `interpreter` (Interpreter*)
- `func` (BytecodeFunction*)
- `args` (Value*)
- `arg_count` (int)
- `program` (BytecodeProgram*)

**Source:** `bytecode_vm.c`

---

### create_class_instance()

**Signature:** `Value create_class_instance(Interpreter* interpreter, Value* class_value, ASTNode* call_node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `class_value` (Value*)
- `call_node` (ASTNode*)

**Source:** `value_functions.c`

---

### create_class_instance()

**Signature:** `Value create_class_instance(Interpreter* interpreter, Value* class_value, ASTNode* call_node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `class_value` (Value*)
- `call_node` (ASTNode*)

**Source:** `value_functions.c`

---

### create_class_instance_from_args()

**Signature:** `Value create_class_instance_from_args(Interpreter* interpreter, Value* class_value, Value* args, size_t arg_count)`

**Parameters:**
- `interpreter` (Interpreter*)
- `class_value` (Value*)
- `args` (Value*)
- `arg_count` (size_t)

**Source:** `value_functions.c`

---

### create_class_instance_from_args()

**Signature:** `Value create_class_instance_from_args(Interpreter* interpreter, Value* class_value, Value* args, size_t arg_count)`

**Parameters:**
- `interpreter` (Interpreter*)
- `class_value` (Value*)
- `args` (Value*)
- `arg_count` (size_t)

**Source:** `value_functions.c`

---

### environment_get()

**Signature:** `Value environment_get(Environment* env, const char* name)`

**Parameters:**
- `env` (Environment*)
- `name` (const char*)

**Source:** `environment.c`

---

### eval_binary()

**Signature:** `Value eval_binary(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_expressions.c`

---

### eval_binary()

**Signature:** `Value eval_binary(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_expressions.c`

---

### eval_block()

**Signature:** `Value eval_block(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_control_flow.c`

---

### eval_block()

**Signature:** `Value eval_block(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_control_flow.c`

---

### eval_for_loop()

**Signature:** `Value eval_for_loop(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_control_flow.c`

---

### eval_for_loop()

**Signature:** `Value eval_for_loop(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_control_flow.c`

---

### eval_if_statement()

**Signature:** `Value eval_if_statement(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_control_flow.c`

---

### eval_if_statement()

**Signature:** `Value eval_if_statement(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_control_flow.c`

---

### eval_node()

**Signature:** `Value eval_node(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_core.c`

---

### eval_node()

**Signature:** `Value eval_node(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_core.c`

---

### eval_return_statement()

**Signature:** `Value eval_return_statement(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_control_flow.c`

---

### eval_return_statement()

**Signature:** `Value eval_return_statement(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_control_flow.c`

---

### eval_throw_statement()

**Signature:** `Value eval_throw_statement(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_control_flow.c`

---

### eval_throw_statement()

**Signature:** `Value eval_throw_statement(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_control_flow.c`

---

### eval_try_catch()

**Signature:** `Value eval_try_catch(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_control_flow.c`

---

### eval_try_catch()

**Signature:** `Value eval_try_catch(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_control_flow.c`

---

### eval_unary()

**Signature:** `Value eval_unary(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_expressions.c`

---

### eval_unary()

**Signature:** `Value eval_unary(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_expressions.c`

---

### eval_while_loop()

**Signature:** `Value eval_while_loop(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_control_flow.c`

---

### eval_while_loop()

**Signature:** `Value eval_while_loop(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_control_flow.c`

---

### evaluate_pure_function()

**Signature:** `Value evaluate_pure_function(CompileTimeEvaluator* evaluator, struct Interpreter* interpreter, 
                            const char* function_name, ASTNode** arguments, size_t arg_count)`

**Parameters:**
- `evaluator` (CompileTimeEvaluator*)
- `interpreter` (struct Interpreter*)
- `function_name` (const char*)
- `arguments` (ASTNode**)
- `arg_count` (size_t)

**Source:** `compile_time.c`

---

### execute_myco_function()

**Signature:** `Value execute_myco_function(Interpreter* interpreter, Value function, Value* arg1, Value* arg2)`

**Parameters:**
- `interpreter` (Interpreter*)
- `function` (Value)
- `arg1` (Value*)
- `arg2` (Value*)

**Source:** `server.c`

---

### execute_myco_function_1()

**Signature:** `Value execute_myco_function_1(Interpreter* interpreter, Value function, Value* arg1)`

**Parameters:**
- `interpreter` (Interpreter*)
- `function` (Value)
- `arg1` (Value*)

**Source:** `server.c`

---

### execute_myco_function_3()

**Signature:** `Value execute_myco_function_3(Interpreter* interpreter, Value function, Value* arg1, Value* arg2, Value* arg3)`

**Parameters:**
- `interpreter` (Interpreter*)
- `function` (Value)
- `arg1` (Value*)
- `arg2` (Value*)
- `arg3` (Value*)

**Source:** `server.c`

---

### fast_create_boolean()

**Signature:** `Value fast_create_boolean(BytecodeProgram* program, int val)`

**Parameters:**
- `program` (BytecodeProgram*)
- `val` (int)

**Source:** `bytecode_vm.c`

---

### fast_create_null()

**Signature:** `Value fast_create_null(BytecodeProgram* program)`

**Parameters:**
- `program` (BytecodeProgram*)

**Source:** `bytecode_vm.c`

---

### fast_create_number()

**Signature:** `Value fast_create_number(BytecodeProgram* program, double val)`

**Parameters:**
- `program` (BytecodeProgram*)
- `val` (double)

**Source:** `bytecode_vm.c`

---

### find_method_in_inheritance_chain()

**Signature:** `Value find_method_in_inheritance_chain(Interpreter* interpreter, Value* class_value, const char* method_name)`

**Parameters:**
- `interpreter` (Interpreter*)
- `class_value` (Value*)
- `method_name` (const char*)

**Source:** `value_functions.c`

---

### find_method_in_inheritance_chain()

**Signature:** `Value find_method_in_inheritance_chain(Interpreter* interpreter, Value* class_value, const char* method_name)`

**Parameters:**
- `interpreter` (Interpreter*)
- `class_value` (Value*)
- `method_name` (const char*)

**Source:** `value_functions.c`

---

### handle_database_collection_method_call()

**Signature:** `Value handle_database_collection_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_database_collection_method_call()

**Signature:** `Value handle_database_collection_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_db_method_call()

**Signature:** `Value handle_db_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_db_method_call()

**Signature:** `Value handle_db_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_graph_method_call()

**Signature:** `Value handle_graph_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_graph_method_call()

**Signature:** `Value handle_graph_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_heap_method_call()

**Signature:** `Value handle_heap_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_heap_method_call()

**Signature:** `Value handle_heap_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_method_call()

**Signature:** `Value handle_method_call(Interpreter* interpreter, ASTNode* call_node, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `object` (Value)

**Source:** `method_dispatch.c`

---

### handle_method_call()

**Signature:** `Value handle_method_call(Interpreter* interpreter, ASTNode* call_node, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `object` (Value)

**Source:** `method_dispatch.c`

---

### handle_queue_method_call()

**Signature:** `Value handle_queue_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_queue_method_call()

**Signature:** `Value handle_queue_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_request_method_call()

**Signature:** `Value handle_request_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_request_method_call()

**Signature:** `Value handle_request_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_response_method_call()

**Signature:** `Value handle_response_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_response_method_call()

**Signature:** `Value handle_response_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_route_group_method_call()

**Signature:** `Value handle_route_group_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_route_group_method_call()

**Signature:** `Value handle_route_group_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_stack_method_call()

**Signature:** `Value handle_stack_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_stack_method_call()

**Signature:** `Value handle_stack_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_super_method_call()

**Signature:** `Value handle_super_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)

**Source:** `method_dispatch.c`

---

### handle_super_method_call()

**Signature:** `Value handle_super_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)

**Source:** `method_dispatch.c`

---

### handle_tree_method_call()

**Signature:** `Value handle_tree_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_tree_method_call()

**Signature:** `Value handle_tree_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_web_method_call()

**Signature:** `Value handle_web_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_web_method_call()

**Signature:** `Value handle_web_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### inline_function_execution()

**Signature:** `Value inline_function_execution(Interpreter* interpreter, BytecodeFunction* func, Value* args, int arg_count, BytecodeProgram* program)`

**Parameters:**
- `interpreter` (Interpreter*)
- `func` (BytecodeFunction*)
- `args` (Value*)
- `arg_count` (int)
- `program` (BytecodeProgram*)

**Source:** `bytecode_vm.c`

---

### interpreter_execute()

**Signature:** `Value interpreter_execute(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_core.c`

---

### interpreter_execute()

**Signature:** `Value interpreter_execute(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `eval_core.c`

---

### interpreter_execute_assignment()

**Signature:** `Value interpreter_execute_assignment(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_assignment()

**Signature:** `Value interpreter_execute_assignment(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_binary_op()

**Signature:** `Value interpreter_execute_binary_op(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_binary_op()

**Signature:** `Value interpreter_execute_binary_op(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_block()

**Signature:** `Value interpreter_execute_block(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_block()

**Signature:** `Value interpreter_execute_block(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_break()

**Signature:** `Value interpreter_execute_break(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_break()

**Signature:** `Value interpreter_execute_break(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_bytecode()

**Signature:** `Value interpreter_execute_bytecode(Interpreter* interpreter, BytecodeProgram* bytecode)`

**Parameters:**
- `interpreter` (Interpreter*)
- `bytecode` (BytecodeProgram*)

**Source:** `bytecode_vm.c`

---

### interpreter_execute_class_declaration()

**Signature:** `Value interpreter_execute_class_declaration(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_class_declaration()

**Signature:** `Value interpreter_execute_class_declaration(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_compiled_function()

**Signature:** `Value interpreter_execute_compiled_function(Interpreter* interpreter, const char* function_name, Value* args, size_t arg_count)`

**Parameters:**
- `interpreter` (Interpreter*)
- `function_name` (const char*)
- `args` (Value*)
- `arg_count` (size_t)

**Source:** `interpreter_main.c`

---

### interpreter_execute_compiled_function()

**Signature:** `Value interpreter_execute_compiled_function(Interpreter* interpreter, const char* function_name, Value* args, size_t arg_count)`

**Parameters:**
- `interpreter` (Interpreter*)
- `function_name` (const char*)
- `args` (Value*)
- `arg_count` (size_t)

**Source:** `interpreter_main.c`

---

### interpreter_execute_continue()

**Signature:** `Value interpreter_execute_continue(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_continue()

**Signature:** `Value interpreter_execute_continue(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_expression()

**Signature:** `Value interpreter_execute_expression(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_expression()

**Signature:** `Value interpreter_execute_expression(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_for_loop()

**Signature:** `Value interpreter_execute_for_loop(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_for_loop()

**Signature:** `Value interpreter_execute_for_loop(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_function_call()

**Signature:** `Value interpreter_execute_function_call(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_function_call()

**Signature:** `Value interpreter_execute_function_call(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_function_declaration()

**Signature:** `Value interpreter_execute_function_declaration(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_function_declaration()

**Signature:** `Value interpreter_execute_function_declaration(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_if_statement()

**Signature:** `Value interpreter_execute_if_statement(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_if_statement()

**Signature:** `Value interpreter_execute_if_statement(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_import()

**Signature:** `Value interpreter_execute_import(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_import()

**Signature:** `Value interpreter_execute_import(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_match()

**Signature:** `Value interpreter_execute_match(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_match()

**Signature:** `Value interpreter_execute_match(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_module()

**Signature:** `Value interpreter_execute_module(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_module()

**Signature:** `Value interpreter_execute_module(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_package()

**Signature:** `Value interpreter_execute_package(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_package()

**Signature:** `Value interpreter_execute_package(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_program()

**Signature:** `Value interpreter_execute_program(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_program()

**Signature:** `Value interpreter_execute_program(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_return()

**Signature:** `Value interpreter_execute_return(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_return()

**Signature:** `Value interpreter_execute_return(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_statement()

**Signature:** `Value interpreter_execute_statement(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_statement()

**Signature:** `Value interpreter_execute_statement(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_switch()

**Signature:** `Value interpreter_execute_switch(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_switch()

**Signature:** `Value interpreter_execute_switch(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_throw()

**Signature:** `Value interpreter_execute_throw(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_throw()

**Signature:** `Value interpreter_execute_throw(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_try_catch()

**Signature:** `Value interpreter_execute_try_catch(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_try_catch()

**Signature:** `Value interpreter_execute_try_catch(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_unary_op()

**Signature:** `Value interpreter_execute_unary_op(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_unary_op()

**Signature:** `Value interpreter_execute_unary_op(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_variable_declaration()

**Signature:** `Value interpreter_execute_variable_declaration(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_variable_declaration()

**Signature:** `Value interpreter_execute_variable_declaration(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_while_loop()

**Signature:** `Value interpreter_execute_while_loop(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### interpreter_execute_while_loop()

**Signature:** `Value interpreter_execute_while_loop(Interpreter* interpreter, ASTNode* node)`

**Parameters:**
- `interpreter` (Interpreter*)
- `node` (ASTNode*)

**Source:** `interpreter_main.c`

---

### micro_jit_execute_function()

**Signature:** `Value micro_jit_execute_function(JitCompiledFunction* compiled_func,
                                Value* args,
                                size_t arg_count,
                                Interpreter* interpreter)`

**Parameters:**
- `compiled_func` (JitCompiledFunction*)
- `args` (Value*)
- `arg_count` (size_t)
- `interpreter` (Interpreter*)

**Source:** `micro_jit.c`

---

### nan_boxing_create_boolean()

**Signature:** `Value nan_boxing_create_boolean(int boolean)`

**Parameters:**
- `boolean` (int)

**Source:** `nan_boxing.c`

---

### nan_boxing_create_function()

**Signature:** `Value nan_boxing_create_function(void* function)`

**Parameters:**
- `function` (void*)

**Source:** `nan_boxing.c`

---

### nan_boxing_create_integer()

**Signature:** `Value nan_boxing_create_integer(int32_t integer)`

**Parameters:**
- `integer` (int32_t)

**Source:** `nan_boxing.c`

---

### nan_boxing_create_null()

**Signature:** `Value nan_boxing_create_null(void)`

**Parameters:**
- `void` (Value)

**Source:** `nan_boxing.c`

---

### nan_boxing_create_number()

**Signature:** `Value nan_boxing_create_number(double number)`

**Parameters:**
- `number` (double)

**Source:** `nan_boxing.c`

---

### nan_boxing_from_value()

**Signature:** `Value nan_boxing_from_value(NanBoxingContext* context, const Value* value)`

**Parameters:**
- `context` (NanBoxingContext*)
- `value` (const Value*)

**Source:** `nan_boxing.c`

---

### parse_form_body()

**Signature:** `Value parse_form_body(const char* body)`

**Parameters:**
- `body` (const char*)

**Source:** `server.c`

---

### register_execute()

**Signature:** `Value register_execute(RegisterContext* context, Interpreter* interpreter)`

**Parameters:**
- `context` (RegisterContext*)
- `interpreter` (Interpreter*)

**Source:** `register_vm.c`

---

### register_execute_instruction()

**Signature:** `Value register_execute_instruction(RegisterContext* context, Interpreter* interpreter, RegisterInstruction instr)`

**Parameters:**
- `context` (RegisterContext*)
- `interpreter` (Interpreter*)
- `instr` (RegisterInstruction)

**Source:** `register_vm.c`

---

### safe_arithmetic_operation()

**Signature:** `Value safe_arithmetic_operation(Value* left, Value* right, char operation, Interpreter* interpreter, int line, int column)`

**Parameters:**
- `left` (Value*)
- `right` (Value*)
- `operation` (char)
- `interpreter` (Interpreter*)
- `line` (int)
- `column` (int)

**Source:** `error_handling.c`

---

### safe_comparison_operation()

**Signature:** `Value safe_comparison_operation(Value* left, Value* right, char operation, Interpreter* interpreter, int line, int column)`

**Parameters:**
- `left` (Value*)
- `right` (Value*)
- `operation` (char)
- `interpreter` (Interpreter*)
- `line` (int)
- `column` (int)

**Source:** `error_handling.c`

---

### session_get_value()

**Signature:** `Value session_get_value(Session* session, const char* key)`

**Parameters:**
- `session` (Session*)
- `key` (const char*)

**Source:** `web.c`

---

### template_get_variable()

**Signature:** `Value template_get_variable(const char* var_name, Value* context)`

**Parameters:**
- `var_name` (const char*)
- `context` (Value*)

**Source:** `web.c`

---

### value_add()

**Signature:** `Value value_add(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_add()

**Signature:** `Value value_add(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_bitwise_and()

**Signature:** `Value value_bitwise_and(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_bitwise_and()

**Signature:** `Value value_bitwise_and(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_bitwise_not()

**Signature:** `Value value_bitwise_not(Value* a)`

**Parameters:**
- `a` (Value*)

**Source:** `value_arithmetic.c`

---

### value_bitwise_not()

**Signature:** `Value value_bitwise_not(Value* a)`

**Parameters:**
- `a` (Value*)

**Source:** `value_arithmetic.c`

---

### value_bitwise_or()

**Signature:** `Value value_bitwise_or(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_bitwise_or()

**Signature:** `Value value_bitwise_or(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_bitwise_xor()

**Signature:** `Value value_bitwise_xor(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_bitwise_xor()

**Signature:** `Value value_bitwise_xor(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_clone()

**Signature:** `Value value_clone(Value* value)`

**Parameters:**
- `value` (Value*)

**Source:** `value_conversions.c`

---

### value_clone()

**Signature:** `Value value_clone(Value* value)`

**Parameters:**
- `value` (Value*)

**Source:** `value_conversions.c`

---

### value_create_async_function()

**Signature:** `Value value_create_async_function(const char* name, ASTNode** params, size_t param_count, const char* return_type, ASTNode* body, Environment* captured_env)`

**Parameters:**
- `name` (const char*)
- `params` (ASTNode**)
- `param_count` (size_t)
- `return_type` (const char*)
- `body` (ASTNode*)
- `captured_env` (Environment*)

**Source:** `value_functions.c`

---

### value_create_async_function()

**Signature:** `Value value_create_async_function(const char* name, ASTNode** params, size_t param_count, const char* return_type, ASTNode* body, Environment* captured_env)`

**Parameters:**
- `name` (const char*)
- `params` (ASTNode**)
- `param_count` (size_t)
- `return_type` (const char*)
- `body` (ASTNode*)
- `captured_env` (Environment*)

**Source:** `value_functions.c`

---

### value_create_boolean()

**Signature:** `Value value_create_boolean(int value)`

**Parameters:**
- `value` (int)

**Source:** `value_primitives.c`

---

### value_create_boolean()

**Signature:** `Value value_create_boolean(int value)`

**Parameters:**
- `value` (int)

**Source:** `value_primitives.c`

---

### value_create_class()

**Signature:** `Value value_create_class(const char* name, const char* parent_name, ASTNode* class_body, Environment* class_env)`

**Parameters:**
- `name` (const char*)
- `parent_name` (const char*)
- `class_body` (ASTNode*)
- `class_env` (Environment*)

**Source:** `value_functions.c`

---

### value_create_class()

**Signature:** `Value value_create_class(const char* name, const char* parent_name, ASTNode* class_body, Environment* class_env)`

**Parameters:**
- `name` (const char*)
- `parent_name` (const char*)
- `class_body` (ASTNode*)
- `class_env` (Environment*)

**Source:** `value_functions.c`

---

### value_create_error()

**Signature:** `Value value_create_error(const char* message, int code)`

**Parameters:**
- `message` (const char*)
- `code` (int)

**Source:** `interpreter_main.c`

---

### value_create_error()

**Signature:** `Value value_create_error(const char* message, int code)`

**Parameters:**
- `message` (const char*)
- `code` (int)

**Source:** `interpreter_main.c`

---

### value_create_function()

**Signature:** `Value value_create_function(ASTNode* body, ASTNode** params, size_t param_count, const char* return_type, Environment* captured_env)`

**Parameters:**
- `body` (ASTNode*)
- `params` (ASTNode**)
- `param_count` (size_t)
- `return_type` (const char*)
- `captured_env` (Environment*)

**Source:** `value_functions.c`

---

### value_create_function()

**Signature:** `Value value_create_function(ASTNode* body, ASTNode** params, size_t param_count, const char* return_type, Environment* captured_env)`

**Parameters:**
- `body` (ASTNode*)
- `params` (ASTNode**)
- `param_count` (size_t)
- `return_type` (const char*)
- `captured_env` (Environment*)

**Source:** `value_functions.c`

---

### value_create_hash_map()

**Signature:** `Value value_create_hash_map(size_t initial_capacity)`

**Parameters:**
- `initial_capacity` (size_t)

**Source:** `value_collections.c`

---

### value_create_hash_map()

**Signature:** `Value value_create_hash_map(size_t initial_capacity)`

**Parameters:**
- `initial_capacity` (size_t)

**Source:** `value_collections.c`

---

### value_create_immutable_number()

**Signature:** `Value value_create_immutable_number(double value)`

**Parameters:**
- `value` (double)

**Source:** `value_primitives.c`

---

### value_create_immutable_number()

**Signature:** `Value value_create_immutable_number(double value)`

**Parameters:**
- `value` (double)

**Source:** `value_primitives.c`

---

### value_create_module()

**Signature:** `Value value_create_module(const char* name, void* exports)`

**Parameters:**
- `name` (const char*)
- `exports` (void*)

**Source:** `value_functions.c`

---

### value_create_module()

**Signature:** `Value value_create_module(const char* name, void* exports)`

**Parameters:**
- `name` (const char*)
- `exports` (void*)

**Source:** `value_functions.c`

---

### value_create_null()

**Signature:** `Value value_create_null(void)`

**Parameters:**
- `void` (Value)

**Source:** `value_primitives.c`

---

### value_create_null()

**Signature:** `Value value_create_null(void)`

**Parameters:**
- `void` (Value)

**Source:** `value_primitives.c`

---

### value_create_number()

**Signature:** `Value value_create_number(double value)`

**Parameters:**
- `value` (double)

**Source:** `value_primitives.c`

---

### value_create_number()

**Signature:** `Value value_create_number(double value)`

**Parameters:**
- `value` (double)

**Source:** `value_primitives.c`

---

### value_create_optimized()

**Signature:** `Value value_create_optimized(ValueType type, uint8_t flags)`

**Parameters:**
- `type` (ValueType)
- `flags` (uint8_t)

**Source:** `value_primitives.c`

---

### value_create_optimized()

**Signature:** `Value value_create_optimized(ValueType type, uint8_t flags)`

**Parameters:**
- `type` (ValueType)
- `flags` (uint8_t)

**Source:** `value_primitives.c`

---

### value_create_promise()

**Signature:** `Value value_create_promise(Value resolved_value, int is_resolved, Value error_value)`

**Parameters:**
- `resolved_value` (Value)
- `is_resolved` (int)
- `error_value` (Value)

**Source:** `value_functions.c`

---

### value_create_promise()

**Signature:** `Value value_create_promise(Value resolved_value, int is_resolved, Value error_value)`

**Parameters:**
- `resolved_value` (Value)
- `is_resolved` (int)
- `error_value` (Value)

**Source:** `value_functions.c`

---

### value_create_range()

**Signature:** `Value value_create_range(double start, double end, double step, int inclusive)`

**Parameters:**
- `start` (double)
- `end` (double)
- `step` (double)
- `inclusive` (int)

**Source:** `value_primitives.c`

---

### value_create_range()

**Signature:** `Value value_create_range(double start, double end, double step, int inclusive)`

**Parameters:**
- `start` (double)
- `end` (double)
- `step` (double)
- `inclusive` (int)

**Source:** `value_primitives.c`

---

### value_create_set()

**Signature:** `Value value_create_set(size_t initial_capacity)`

**Parameters:**
- `initial_capacity` (size_t)

**Source:** `value_collections.c`

---

### value_create_set()

**Signature:** `Value value_create_set(size_t initial_capacity)`

**Parameters:**
- `initial_capacity` (size_t)

**Source:** `value_collections.c`

---

### value_divide()

**Signature:** `Value value_divide(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_divide()

**Signature:** `Value value_divide(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_equal()

**Signature:** `Value value_equal(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_equal()

**Signature:** `Value value_equal(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_function_call()

**Signature:** `Value value_function_call(Value* func, Value* args, size_t arg_count, Interpreter* interpreter, int line, int column)`

**Parameters:**
- `func` (Value*)
- `args` (Value*)
- `arg_count` (size_t)
- `interpreter` (Interpreter*)
- `line` (int)
- `column` (int)

**Source:** `value_functions.c`

---

### value_function_call()

**Signature:** `Value value_function_call(Value* func, Value* args, size_t arg_count, Interpreter* interpreter, int line, int column)`

**Parameters:**
- `func` (Value*)
- `args` (Value*)
- `arg_count` (size_t)
- `interpreter` (Interpreter*)
- `line` (int)
- `column` (int)

**Source:** `value_functions.c`

---

### value_function_call_with_self()

**Signature:** `Value value_function_call_with_self(Value* func, Value* args, size_t arg_count, Interpreter* interpreter, Value* self, int line, int column)`

**Parameters:**
- `func` (Value*)
- `args` (Value*)
- `arg_count` (size_t)
- `interpreter` (Interpreter*)
- `self` (Value*)
- `line` (int)
- `column` (int)

**Source:** `value_functions.c`

---

### value_function_call_with_self()

**Signature:** `Value value_function_call_with_self(Value* func, Value* args, size_t arg_count, Interpreter* interpreter, Value* self, int line, int column)`

**Parameters:**
- `func` (Value*)
- `args` (Value*)
- `arg_count` (size_t)
- `interpreter` (Interpreter*)
- `self` (Value*)
- `line` (int)
- `column` (int)

**Source:** `value_functions.c`

---

### value_greater_equal()

**Signature:** `Value value_greater_equal(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_greater_equal()

**Signature:** `Value value_greater_equal(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_greater_than()

**Signature:** `Value value_greater_than(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_greater_than()

**Signature:** `Value value_greater_than(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_hash_map_get()

**Signature:** `Value value_hash_map_get(Value* map, Value key)`

**Parameters:**
- `map` (Value*)
- `key` (Value)

**Source:** `value_collections.c`

---

### value_hash_map_get()

**Signature:** `Value value_hash_map_get(Value* map, Value key)`

**Parameters:**
- `map` (Value*)
- `key` (Value)

**Source:** `value_collections.c`

---

### value_left_shift()

**Signature:** `Value value_left_shift(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_left_shift()

**Signature:** `Value value_left_shift(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_less_equal()

**Signature:** `Value value_less_equal(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_less_equal()

**Signature:** `Value value_less_equal(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_less_than()

**Signature:** `Value value_less_than(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_less_than()

**Signature:** `Value value_less_than(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_logical_and()

**Signature:** `Value value_logical_and(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_logical_and()

**Signature:** `Value value_logical_and(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_logical_not()

**Signature:** `Value value_logical_not(Value* a)`

**Parameters:**
- `a` (Value*)

**Source:** `value_arithmetic.c`

---

### value_logical_not()

**Signature:** `Value value_logical_not(Value* a)`

**Parameters:**
- `a` (Value*)

**Source:** `value_arithmetic.c`

---

### value_logical_or()

**Signature:** `Value value_logical_or(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_logical_or()

**Signature:** `Value value_logical_or(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_logical_xor()

**Signature:** `Value value_logical_xor(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_logical_xor()

**Signature:** `Value value_logical_xor(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_modulo()

**Signature:** `Value value_modulo(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_modulo()

**Signature:** `Value value_modulo(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_multiply()

**Signature:** `Value value_multiply(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_multiply()

**Signature:** `Value value_multiply(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_not_equal()

**Signature:** `Value value_not_equal(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_not_equal()

**Signature:** `Value value_not_equal(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_power()

**Signature:** `Value value_power(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_power()

**Signature:** `Value value_power(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_right_shift()

**Signature:** `Value value_right_shift(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_right_shift()

**Signature:** `Value value_right_shift(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_stack_peek()

**Signature:** `Value value_stack_peek(void)`

**Parameters:**
- `void` (Value)

**Source:** `bytecode_vm.c`

---

### value_stack_pop()

**Signature:** `Value value_stack_pop(void)`

**Parameters:**
- `void` (Value)

**Source:** `bytecode_vm.c`

---

### value_subtract()

**Signature:** `Value value_subtract(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_subtract()

**Signature:** `Value value_subtract(Value* a, Value* b)`

**Parameters:**
- `a` (Value*)
- `b` (Value*)

**Source:** `value_arithmetic.c`

---

### value_to_boolean()

**Signature:** `Value value_to_boolean(Value* value)`

**Parameters:**
- `value` (Value*)

**Source:** `value_conversions.c`

---

### value_to_boolean()

**Signature:** `Value value_to_boolean(Value* value)`

**Parameters:**
- `value` (Value*)

**Source:** `value_conversions.c`

---

### value_to_number()

**Signature:** `Value value_to_number(Value* value)`

**Parameters:**
- `value` (Value*)

**Source:** `value_conversions.c`

---

### value_to_number()

**Signature:** `Value value_to_number(Value* value)`

**Parameters:**
- `value` (Value*)

**Source:** `value_conversions.c`

---

## String Functions

### eval_string_concat_compile_time()

**Signature:** `Value eval_string_concat_compile_time(CompileTimeEvaluator* evaluator, struct Interpreter* interpreter,
                                     ASTNode* expression)`

**Parameters:**
- `evaluator` (CompileTimeEvaluator*)
- `interpreter` (struct Interpreter*)
- `expression` (ASTNode*)

**Source:** `compile_time.c`

---

### fast_create_string()

**Signature:** `Value fast_create_string(BytecodeProgram* program, const char* str)`

**Parameters:**
- `program` (BytecodeProgram*)
- `str` (const char*)

**Source:** `bytecode_vm.c`

---

### fast_string_concat()

**Signature:** `Value fast_string_concat(const char* str1, const char* str2)`

**Parameters:**
- `str1` (const char*)
- `str2` (const char*)

**Source:** `bytecode_vm.c`

---

### nan_boxing_create_string()

**Signature:** `Value nan_boxing_create_string(char* string)`

**Parameters:**
- `string` (char*)

**Source:** `nan_boxing.c`

---

### parse_query_string()

**Signature:** `Value parse_query_string(const char* query_string)`

**Parameters:**
- `query_string` (const char*)

**Source:** `server.c`

---

### value_create_cached_string()

**Signature:** `Value value_create_cached_string(const char* value)`

**Parameters:**
- `value` (const char*)

**Source:** `value_primitives.c`

---

### value_create_cached_string()

**Signature:** `Value value_create_cached_string(const char* value)`

**Parameters:**
- `value` (const char*)

**Source:** `value_primitives.c`

---

### value_create_string()

**Signature:** `Value value_create_string(const char* value)`

**Parameters:**
- `value` (const char*)

**Source:** `value_primitives.c`

---

### value_create_string()

**Signature:** `Value value_create_string(const char* value)`

**Parameters:**
- `value` (const char*)

**Source:** `value_primitives.c`

---

### value_to_string()

**Signature:** `Value value_to_string(Value* value)`

**Parameters:**
- `value` (Value*)

**Source:** `value_conversions.c`

---

### value_to_string()

**Signature:** `Value value_to_string(Value* value)`

**Parameters:**
- `value` (Value*)

**Source:** `value_conversions.c`

---

## Array Functions

### nan_boxing_create_array()

**Signature:** `Value nan_boxing_create_array(Value** array)`

**Parameters:**
- `array` (Value**)

**Source:** `nan_boxing.c`

---

### safe_array_access()

**Signature:** `Value safe_array_access(Value* array, int index, Interpreter* interpreter, int line, int column)`

**Parameters:**
- `array` (Value*)
- `index` (int)
- `interpreter` (Interpreter*)
- `line` (int)
- `column` (int)

**Source:** `error_handling.c`

---

### value_array_get()

**Signature:** `Value value_array_get(Value* array, size_t index)`

**Parameters:**
- `array` (Value*)
- `index` (size_t)

**Source:** `value_collections.c`

---

### value_array_get()

**Signature:** `Value value_array_get(Value* array, size_t index)`

**Parameters:**
- `array` (Value*)
- `index` (size_t)

**Source:** `value_collections.c`

---

### value_array_pop()

**Signature:** `Value value_array_pop(Value* array)`

**Parameters:**
- `array` (Value*)

**Source:** `value_collections.c`

---

### value_array_pop()

**Signature:** `Value value_array_pop(Value* array)`

**Parameters:**
- `array` (Value*)

**Source:** `value_collections.c`

---

### value_create_array()

**Signature:** `Value value_create_array(size_t initial_capacity)`

**Parameters:**
- `initial_capacity` (size_t)

**Source:** `value_collections.c`

---

### value_create_array()

**Signature:** `Value value_create_array(size_t initial_capacity)`

**Parameters:**
- `initial_capacity` (size_t)

**Source:** `value_collections.c`

---

### value_create_pooled_array()

**Signature:** `Value value_create_pooled_array(size_t initial_capacity)`

**Parameters:**
- `initial_capacity` (size_t)

**Source:** `value_primitives.c`

---

### value_create_pooled_array()

**Signature:** `Value value_create_pooled_array(size_t initial_capacity)`

**Parameters:**
- `initial_capacity` (size_t)

**Source:** `value_primitives.c`

---

### value_set_to_array()

**Signature:** `Value value_set_to_array(Value* set)`

**Parameters:**
- `set` (Value*)

**Source:** `value_collections.c`

---

### value_set_to_array()

**Signature:** `Value value_set_to_array(Value* set)`

**Parameters:**
- `set` (Value*)

**Source:** `value_collections.c`

---

## Object Functions

### create_request_object()

**Signature:** `Value create_request_object(MycoRequest* request)`

**Parameters:**
- `request` (MycoRequest*)

**Source:** `server.c`

---

### create_response_object()

**Signature:** `Value create_response_object(MycoResponse* response)`

**Parameters:**
- `response` (MycoResponse*)

**Source:** `server.c`

---

### nan_boxing_create_object()

**Signature:** `Value nan_boxing_create_object(void* object)`

**Parameters:**
- `object` (void*)

**Source:** `nan_boxing.c`

---

### safe_object_access()

**Signature:** `Value safe_object_access(Value* object, const char* property, Interpreter* interpreter, int line, int column)`

**Parameters:**
- `object` (Value*)
- `property` (const char*)
- `interpreter` (Interpreter*)
- `line` (int)
- `column` (int)

**Source:** `error_handling.c`

---

### value_create_object()

**Signature:** `Value value_create_object(size_t initial_capacity)`

**Parameters:**
- `initial_capacity` (size_t)

**Source:** `value_collections.c`

---

### value_create_object()

**Signature:** `Value value_create_object(size_t initial_capacity)`

**Parameters:**
- `initial_capacity` (size_t)

**Source:** `value_collections.c`

---

### value_object_get()

**Signature:** `Value value_object_get(Value* obj, const char* key)`

**Parameters:**
- `obj` (Value*)
- `key` (const char*)

**Source:** `value_collections.c`

---

### value_object_get()

**Signature:** `Value value_object_get(Value* obj, const char* key)`

**Parameters:**
- `obj` (Value*)
- `key` (const char*)

**Source:** `value_collections.c`

---

## HTTP Server

### handle_http_method_call()

**Signature:** `Value handle_http_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_http_method_call()

**Signature:** `Value handle_http_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_server_library_method_call()

**Signature:** `Value handle_server_library_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_server_library_method_call()

**Signature:** `Value handle_server_library_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_server_method_call()

**Signature:** `Value handle_server_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

### handle_server_method_call()

**Signature:** `Value handle_server_method_call(Interpreter* interpreter, ASTNode* call_node, const char* method_name, Value object)`

**Parameters:**
- `interpreter` (Interpreter*)
- `call_node` (ASTNode*)
- `method_name` (const char*)
- `object` (Value)

**Source:** `library_methods.c`

---

## JSON Operations

### json_to_myco_value()

**Signature:** `Value json_to_myco_value(const JsonValue* json_value)`

**Parameters:**
- `json_value` (const JsonValue*)

**Source:** `json.c`

---

### parse_json_body()

**Signature:** `Value parse_json_body(const char* body)`

**Parameters:**
- `body` (const char*)

**Source:** `server.c`

---

## Time Functions

### compile_time_eval()

**Signature:** `Value compile_time_eval(CompileTimeEvaluator* evaluator, struct Interpreter* interpreter, 
                       ASTNode* expression)`

**Parameters:**
- `evaluator` (CompileTimeEvaluator*)
- `interpreter` (struct Interpreter*)
- `expression` (ASTNode*)

**Source:** `compile_time.c`

---

### create_time_value()

**Signature:** `Value create_time_value(time_t timestamp)`

**Parameters:**
- `timestamp` (time_t)

**Source:** `time.c`

---

### eval_arithmetic_compile_time()

**Signature:** `Value eval_arithmetic_compile_time(CompileTimeEvaluator* evaluator, struct Interpreter* interpreter,
                                ASTNode* expression)`

**Parameters:**
- `evaluator` (CompileTimeEvaluator*)
- `interpreter` (struct Interpreter*)
- `expression` (ASTNode*)

**Source:** `compile_time.c`

---

### eval_boolean_compile_time()

**Signature:** `Value eval_boolean_compile_time(CompileTimeEvaluator* evaluator, struct Interpreter* interpreter,
                               ASTNode* expression)`

**Parameters:**
- `evaluator` (CompileTimeEvaluator*)
- `interpreter` (struct Interpreter*)
- `expression` (ASTNode*)

**Source:** `compile_time.c`

---

