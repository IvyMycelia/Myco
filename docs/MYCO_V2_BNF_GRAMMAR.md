# Myco v2.0 - Complete BNF Grammar Specification

**Version**: v2.0 - Language  
**Last Updated**: September 2025  
**Status**: Complete and Current

---

## **Table of Contents**

1. [Program Structure](#program-structure)
2. [Lexical Elements](#lexical-elements)
3. [Expressions](#expressions)
4. [Statements](#statements)
5. [Functions](#functions)
6. [Control Flow](#control-flow)
7. [Data Structures](#data-structures)
8. [Object-Oriented Programming](#object-oriented-programming)
9. [Functional Programming](#functional-programming)
10. [Error Handling](#error-handling)
11. [Modules and Imports](#modules-and-imports)
12. [Concurrency](#concurrency)
13. [Additional Features](#additional-features)
14. [Built-in Functions](#built-in-functions)

---

## **Program Structure**

```bnf
<program> ::= <statement_list>

<statement_list> ::= <statement>*
<statement> ::= <expression_statement> | <declaration_statement> | <control_statement> | <function_declaration> | <class_declaration> | <interface_declaration> | <module_declaration> | <package_declaration> | <return_statement> | <break_statement> | <continue_statement> | <throw_statement> | <import_statement> | <export_statement>

<block> ::= "{" <statement_list> "}" | <statement_list> "end"
```

---

## **Lexical Elements**

### **Identifiers**

```bnf
<identifier> ::= <letter> (<letter> | <digit> | "_")*
<letter> ::= "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z" | "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z"
<digit> ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"

<qualified_identifier> ::= <identifier> ("." <identifier>)*
```

### **Literals**

```bnf
<literal> ::= <number_literal> | <string_literal> | <boolean_literal> | <character_literal> | <array_literal> | <object_literal> | <tuple_literal> | <null_literal>

<number_literal> ::= <integer_literal> | <float_literal>
<integer_literal> ::= <digit>+ | "-" <digit>+ | "0x" <hex_digit>+ | "0b" <binary_digit>+ | "0o" <octal_digit>+
<float_literal> ::= <digit>+ "." <digit>* | "." <digit>+ | <digit>+ "e" <digit>+ | <digit>+ "." <digit>* "e" <digit>+
<hex_digit> ::= <digit> | "a" | "b" | "c" | "d" | "e" | "f" | "A" | "B" | "C" | "D" | "E" | "F"
<binary_digit> ::= "0" | "1"
<octal_digit> ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7"

<string_literal> ::= '"' <string_content>* '"' | "'" <string_content>* "'" | "`" <string_content>* "`"
<string_content> ::= <any_char_except_quote> | <escape_sequence> | <template_expression>
<escape_sequence> ::= "\\" <any_char> | "\\n" | "\\t" | "\\r" | "\\" | "\\'" | "\\"" | "\\x" <hex_digit> <hex_digit>
<template_expression> ::= "${" <expression> "}"

<boolean_literal> ::= "True" | "False" | "1" | "0"

<character_literal> ::= "'" <single_char> "'"
<single_char> ::= <any_char_except_single_quote> | <escape_sequence>

<array_literal> ::= "[" <expression_list> "]"
<expression_list> ::= <expression> ("," <expression>)*

<object_literal> ::= "{" <property_list> "}"
<property_list> ::= <property> ("," <property>)*
<property> ::= <identifier> ":" <expression> | <string_literal> ":" <expression> | <computed_property>

<tuple_literal> ::= "(" <expression_list> ")"

<null_literal> ::= "null" | "None"
```

### **Type Specifiers**

```bnf
<type_spec> ::= <primitive_type> | <array_type> | <object_type> | <function_type> | <union_type> | <generic_type> | <qualified_type>

<primitive_type> ::= "Int" | "Float" | "Bool" | "String" | "Void"

<array_type> ::= "[" <type_spec> "]" | "[" <type_spec> ";" <expression> "]"

<object_type> ::= "{" <property_type_list> "}"
<property_type_list> ::= <property_type> ("," <property_type>)*
<property_type> ::= <identifier> ":" <type_spec>

<function_type> ::= "fn" "(" <parameter_type_list> ")" "->" <type_spec>
<parameter_type_list> ::= <parameter_type> ("," <parameter_type>)*
<parameter_type> ::= <identifier> (":" <type_spec>)? | <type_spec>

<union_type> ::= <type_spec> "|" <type_spec>

<generic_type> ::= <identifier> "<" <type_argument_list> ">"
<type_argument_list> ::= <type_spec> ("," <type_spec>)*

<qualified_type> ::= <identifier> "." <identifier>
```

---

## **Expressions**

### **Primary Expressions**

```bnf
<expression> ::= <primary_expression> | <binary_expression> | <unary_expression> | <assignment_expression> | <conditional_expression> | <lambda_expression>

<primary_expression> ::= <literal> | <identifier> | <parenthesized_expression> | <array_access> | <object_access> | <function_call> | <method_call> | <constructor_call>

<parenthesized_expression> ::= "(" <expression> ")"

<array_access> ::= <identifier> "[" <expression> "]"

<object_access> ::= <identifier> "." <identifier> | <identifier> "[" <expression> "]"

<function_call> ::= <identifier> "(" <argument_list> ")"
<argument_list> ::= <expression> ("," <expression>)*

<method_call> ::= <expression> "." <identifier> "(" <argument_list> ")"

<constructor_call> ::= <identifier> "(" <argument_list> ")"
```

### **Binary Expressions**

```bnf
<binary_expression> ::= <expression> <binary_operator> <expression>

<binary_operator> ::= "+" | "-" | "*" | "/" | "%" | "**" | "==" | "!=" | "<" | ">" | "<=" | ">=" | "&&" | "||" | "^^" | "&" | "|" | "^" | "<<" | ">>" | ".." | "..="
```

### **Unary Expressions**

```bnf
<unary_expression> ::= <unary_operator> <expression>

<unary_operator> ::= "+" | "-" | "!" | "~" | "*" | "&"
```

### **Assignment Expressions**

```bnf
<assignment_expression> ::= <identifier> <assignment_operator> <expression>

<assignment_operator> ::= "=" | "+=" | "-=" | "*=" | "/=" | "%=" | "**=" | "&=" | "|=" | "^=" | "<<=" | ">>="
```

### **Conditional Expressions**

```bnf
<conditional_expression> ::= <expression> "?" <expression> ":" <expression>
```

### **Lambda Expressions**

```bnf
<lambda_expression> ::= "fn" "(" <parameter_list> ")" ("->" <type_spec>)? ":" <expression> | "fn" "(" <parameter_list> ")" ("->" <type_spec>)? <block>
```

---

## **Statements**

### **Expression Statements**

```bnf
<expression_statement> ::= <expression> ";"
```

### **Declaration Statements**

```bnf
<declaration_statement> ::= <variable_declaration> | <constant_declaration> | <type_declaration>

<variable_declaration> ::= ("let" | "var") ("mut")? <identifier> (":" <type_spec>)? ("=" <expression>)? ";"

<constant_declaration> ::= "const" <identifier> ":" <type_spec> "=" <expression> ";"

<type_declaration> ::= "type" <identifier> "=" <type_spec> ";"
```

### **Control Statements**

```bnf
<control_statement> ::= <if_statement> | <switch_statement> | <loop_statement> | <try_statement> | <pick_statement>

<if_statement> ::= "if" <expression> ":" <block> <else_clause>?

<else_clause> ::= "else" ":" <block> | "else" "if" <expression> ":" <block> <else_clause>?

<switch_statement> ::= "switch" <expression> ":" <switch_cases> "end"

<switch_cases> ::= <case_statement>* <default_statement>?

<case_statement> ::= "case" <case_pattern> ":" <case_body>

<case_pattern> ::= <expression> | <range_pattern> | <multiple_pattern>

<range_pattern> ::= <expression> ".." <expression>

<multiple_pattern> ::= <expression> ("," <expression>)*

<case_body> ::= <statement>*

<default_statement> ::= "default" ":" <default_body>

<default_body> ::= <statement>*

<loop_statement> ::= <for_statement> | <while_statement> | <do_while_statement> | <loop_statement>

<for_statement> ::= "for" <for_pattern> "in" <expression> ":" <block>

<for_pattern> ::= <identifier> | "(" <identifier> "," <identifier> ")" | <array_pattern>

<array_pattern> ::= "[" <pattern_element> ("," <pattern_element>)* "]"

<pattern_element> ::= <identifier> | "..." <identifier>

<while_statement> ::= "while" <expression> ":" <block>

<do_while_statement> ::= "do" ":" <block> "while" <expression> ";"

<loop_statement> ::= "loop" ":" <block>

<try_statement> ::= "try" ":" <block> <catch_clause>+ <finally_clause>?

<catch_clause> ::= "catch" <catch_pattern> ":" <block>

<catch_pattern> ::= <identifier> | <type_spec> "as" <identifier>

<finally_clause> ::= "finally" ":" <block>

<pick_statement> ::= "pick" <expression> ":" <pick_cases> "root"

<pick_cases> ::= <pick_case>*

<pick_case> ::= "case" <pattern> ("if" <expression>)? "=>" <expression> ";" | "case" <pattern> ("if" <expression>)? ":" <block>
```

## **Functions**

### **Function Declaration**

```bnf
<function_declaration> ::= "func" <identifier> "(" <parameter_list> ")" ("->" <type_spec>)? ":" <block>

<parameter_list> ::= <parameter> ("," <parameter>)*

<parameter> ::= <identifier> (":" <type_spec>)? ("=" <expression>)? | "..." <identifier> (":" <type_spec>)?
```

### **Class Declaration**

```bnf
<class_declaration> ::= "class" <identifier> <inheritance>? ":" <class_body>

<inheritance> ::= "extends" <identifier> | "implements" <identifier> ("," <identifier>)*

<class_body> ::= <class_member>*

<class_member> ::= <field_declaration> | <method_declaration> | <constructor_declaration> | <static_declaration>

<field_declaration> ::= ("let" | "var") ("mut")? <identifier> ":" <type_spec> ("=" <expression>)? ";"

<method_declaration> ::= "func" <identifier> "(" <parameter_list> ")" ("->" <type_spec>)? ":" <block>

<constructor_declaration> ::= "func" "__init__" "(" <parameter_list> ")" ":" <block>

<static_declaration> ::= "static" <method_declaration>
```

### **Interface Declaration**

```bnf
<interface_declaration> ::= "interface" <identifier> ":" <interface_body>

<interface_body> ::= <interface_member>*

<interface_member> ::= <method_signature> | <property_signature>

<method_signature> ::= "func" <identifier> "(" <parameter_list> ")" ("->" <type_spec>)? ";"

<property_signature> ::= <identifier> ":" <type_spec> ";"
```

### **Module Declaration**

```bnf
<module_declaration> ::= "module" <identifier> ";" <module_body>

<module_body> ::= <module_member>*

<module_member> ::= <function_declaration> | <class_declaration> | <export_statement>

<export_statement> ::= "export" <function_declaration> | "export" <class_declaration> | "export" <identifier> ";"
```

### **Package Declaration**

```bnf
<package_declaration> ::= "package" <identifier> ";" <package_body>

<package_body> ::= <package_member>*

<package_member> ::= <dependency_declaration> | <config_declaration> | <module_declaration>

<dependency_declaration> ::= "dependencies" ":" <dependency_list> "end"

<dependency_list> ::= <dependency>*

<dependency> ::= '"' <identifier> '"' ":" '"' <version> '"' ";"

<config_declaration> ::= "config" ":" <config_list> "end"

<config_list> ::= <config_item>*

<config_item> ::= <identifier> ":" <expression> ";"
```

### **Import Statements**

```bnf
<import_statement> ::= <import_declaration> | <import_specific> | <import_wildcard>

<import_declaration> ::= "import" <identifier> ("as" <identifier>)? ";"

<import_specific> ::= "import" "{" <import_list> "}" "from" <identifier> ";"

<import_list> ::= <identifier> ("," <identifier>)*

<import_wildcard> ::= "import" "*" "from" <identifier> ";"
```

### **Other Statements**

```bnf
<return_statement> ::= "return" <expression>? ";"

<break_statement> ::= "break" <identifier>? ";"

<continue_statement> ::= "continue" <identifier>? ";"

<throw_statement> ::= "throw" <expression> ";"
```

---

## **Functional Programming**

### **Pattern Matching**

```bnf
<pattern> ::= <literal_pattern> | <identifier_pattern> | <tuple_pattern> | <array_pattern> | <object_pattern> | <wildcard_pattern>

<literal_pattern> ::= <literal>

<identifier_pattern> ::= <identifier>

<tuple_pattern> ::= "(" <pattern> ("," <pattern>)* ")"

<array_pattern> ::= "[" <pattern> ("," <pattern>)* "]"

<object_pattern> ::= "{" <object_pattern_property> ("," <object_pattern_property>)* "}"

<object_pattern_property> ::= <identifier> ":" <pattern> | <identifier> | "..." <identifier>

<wildcard_pattern> ::= "_"
```

### **Higher-Order Functions**

```bnf
<map_expression> ::= <expression> "." "map" "(" <lambda_expression> ")"

<filter_expression> ::= <expression> "." "filter" "(" <lambda_expression> ")"

<reduce_expression> ::= <expression> "." "reduce" "(" <expression> "," <lambda_expression> ")"

<forEach_expression> ::= <expression> "." "forEach" "(" <lambda_expression> ")"
```

---

## **Concurrency**

### **Threads**

```bnf
<thread_creation> ::= "Thread" "::" "new" "(" <lambda_expression> ("," <expression>)* ")"

<thread_management> ::= <identifier> "." <thread_method> "(" <argument_list> ")"

<thread_method> ::= "start" | "join" | "detach"

<thread_pool> ::= "ThreadPool" "::" "new" "(" <expression> ")"
```

### **Async/Await**

```bnf
<async_function> ::= "async" <function_declaration>

<await_expression> ::= "await" <expression>

<async_expression> ::= <expression> "." "catch" "(" <lambda_expression> ")"
```

### **Channels**

```bnf
<channel_creation> ::= "Channel" "::" "new" "(" ")"

<channel_operation> ::= <identifier> "." <channel_method> "(" <argument_list> ")"

<channel_method> ::= "send" | "recv" | "recv_timeout"
```

---

## **Additional Features**

### **Metaprogramming**

```bnf
<macro_declaration> ::= "macro" <identifier> "(" <macro_parameter_list> ")" ":" <macro_body>

<macro_parameter_list> ::= <identifier> ("," <identifier>)*

<macro_body> ::= <statement>*

<macro_usage> ::= <identifier> "(" <argument_list> ")"
```

### **Reflection**

```bnf
<reflection_expression> ::= "reflect" "::" <reflection_method> "(" <argument_list> ")"

<reflection_method> ::= "type_of" | "fields_of" | "methods_of"
```

### **Foreign Function Interface**

```bnf
<extern_declaration> ::= "extern" <string_literal> <function_declaration>
```

---

## **Built-in Functions**

### **Core Functions**

```bnf
<core_function> ::= "print" | "input" | "len" | "type" | "str" | "int" | "float" | "bool"
```

### **Array Functions**

```bnf
<array_function> ::= "push" | "pop" | "shift" | "unshift" | "slice" | "splice" | "reverse" | "sort" | "join" | "indexOf" | "lastIndexOf" | "includes"
```

### **String Functions**

```bnf
<string_function> ::= "split" | "trim" | "replace" | "toLowerCase" | "toUpperCase" | "substring" | "charAt" | "startsWith" | "endsWith"
```

### **Object Functions**

```bnf
<object_function> ::= "keys" | "values" | "entries" | "hasOwnProperty" | "assign" | "freeze" | "seal"
```

### **Math Functions**

```bnf
<math_function> ::= "abs" | "pow" | "sqrt" | "floor" | "ceil" | "round" | "min" | "max" | "random" | "sin" | "cos" | "tan" | "log" | "exp"
```

### **Utility Functions**

```bnf
<utility_function> ::= "debug" | "assert" | "clone" | "deepClone" | "isArray" | "isObject" | "isFunction" | "isNumber" | "isString" | "isBoolean"
```

---

## **Complete Program Example**

```bnf
<complete_program> ::= <package_declaration>? <import_statement>* <module_declaration>*

# Example Myco v2.0 program structure:
# 1. Package declaration (optional)
# 2. Import statements
# 3. Module declarations
# 4. Main program logic
# 5. Function and class definitions
```

---

## **Language Features Summary**

### **Core Language Features**

- **Variables & Types**: Static typing with type inference, all primitive types, user-defined types
- **Control Flow**: If-else, switch/case with patterns, loops (for, while, do-while, loop), try-catch-finally
- **Functions**: First-class functions, closures, higher-order functions, recursion, variadic parameters
- **Data Structures**: Arrays, objects, tuples, sets, linked lists, binary trees with full method support
- **Object-Oriented**: Classes, inheritance, interfaces, polymorphism, encapsulation, static members
- **Functional Programming**: Lambda functions, pattern matching, map/filter/reduce, higher-order functions
- **Error Handling**: Try-catch blocks, custom error types, Result type, exception handling
- **Modules & Imports**: Module system, package management, dependency resolution
- **Concurrency**: Threads, async/await, channels, thread pools
- **Additional Features**: Metaprogramming, reflection, FFI, macros

### **Performance Features**

- **Static Typing**: Compile-time type checking
- **Zero-Cost Abstractions**: No runtime overhead for abstractions
- **Memory Safety**: Automatic memory management with bounds checking
- **Optimization**: Multiple optimization levels, dead code elimination, inlining

### **Platform Support**

- **Cross-Platform**: Windows, macOS, Linux, WebAssembly, embedded systems
- **Native Code**: AOT compilation to native binaries
- **Web Support**: WebAssembly compilation and JavaScript interop
- **Embedded**: ARM Cortex-M, RISC-V, x86 embedded support

---

## **Grammar Notes**

1. **Whitespace**: Myco v2.0 is whitespace-insensitive except within string literals
2. **Semicolons**: All statements must end with semicolons (except in blocks)
3. **Type System**: Type system with static checking and type inference
4. **Backward Compatibility**: Maintains compatibility with Myco v1.x syntax where possible
5. **Memory Management**: Automatic memory management with zero-cost abstractions
6. **Performance**: Optimized for high performance with C-level speed targets
7. **Comments**: Use # for single-line comments
8. **Types**: Use Int, Float, Bool, String instead of i32, f32, bool, str
9. **Pattern Matching**: Use pick/case/root instead of match

---

## **Future Extensions**

### **v2.0 Features**

- **Complete Language**: Full feature parity with modern programming languages
- **High Performance**: 90%+ of C performance across all benchmarks
- **Production Ready**: Production-grade reliability and tooling
- **Cross-Platform**: Universal compatibility across all major platforms

### **Planned for v2.1+**

- **Additional Optimizations**: LLVM backend, more aggressive optimizations
- **Package Ecosystem**: Community package registry and dependency management
- **IDE Support**: Full language server protocol and IDE integration
- **Performance Profiling**: Profiling and optimization tools

---

*This BNF grammar specification is current as of Myco v2.0 and provides the complete syntax foundation for the language. All language features are defined according to this specification.*
