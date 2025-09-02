# Myco v2.0 - Commenting Guide

**Version**: v2.0  
**Last Updated**: August 2025  
**Status**: Complete and Current

---

## **Overview**

This document explains the comprehensive commenting approach used throughout the Myco v2.0 codebase. Every file, function, and important code section has been documented with professional, detailed comments to make the codebase as navigable and maintainable as possible.

---

## **Commenting Philosophy**

### **Goals**

- **Maximum Navigability**: Developers can quickly find what they need
- **Professional Quality**: Comments read like technical documentation
- **Comprehensive Coverage**: No important concept goes unexplained
- **Learning-Friendly**: New developers can understand the system quickly
- **Maintenance Support**: Future developers can modify code confidently

### **Principles**

- **Simple and Clear**: Avoid overly complex language
- **Detailed but Concise**: Provide complete information without verbosity
- **Professional Tone**: Write as if for a technical audience
- **Plentiful**: Err on the side of over-documentation rather than under-documentation

---

## **Comment Types Used**

### **1. File Header Comments**

Every header file includes a comprehensive file header comment that explains:

- What the file does
- Key responsibilities
- How it fits into the overall system
- When and how to use it
- What to look for when adding new features

**Example**:

```c
/**
 * @file lexer.h
 * @brief Lexical Analysis Module - converts source code into tokens
 * 
 * The lexer (also called a tokenizer) is the first step in processing Myco source code.
 * It reads the raw text character by character and groups them into meaningful units
 * called "tokens". These tokens represent the basic building blocks of the language:
 * 
 * Examples of tokens:
 * - Keywords: 'if', 'while', 'func', 'return'
 * - Identifiers: variable names, function names
 * - Literals: numbers (42, 3.14), strings ("hello"), booleans (True, False)
 * - Operators: +, -, *, /, ==, !=, &&, ||
 * - Punctuation: (, ), {, }, ;, :
 * 
 * When adding new language features, you'll often need to:
 * 1. Add new token types to the TokenType enum
 * 2. Update the lexer logic to recognize new patterns
 * 3. Handle any special parsing requirements
 */
```

### **2. Function Documentation Comments**

Every function includes detailed documentation explaining:

- What the function does
- Parameters and their purpose
- Return values and their meaning
- Usage examples when helpful
- Side effects and important notes

**Example**:

```c
/**
 * @brief Initialize a new lexer with source code
 * 
 * This function sets up a lexer to process the given source code. It initializes
 * all internal state variables and prepares the lexer to start tokenization.
 * 
 * @param source The source code string to tokenize (must not be NULL)
 * @return A pointer to the initialized lexer, or NULL if allocation failed
 * 
 * Usage example:
 * @code
 * Lexer* lexer = lexer_initialize("let x = 42;");
 * if (lexer) {
 *     // Use the lexer...
 *     lexer_free(lexer);
 * }
 * @endcode
 */
```

### **3. Inline Comments**

Important code sections include inline comments explaining:

- What the code is doing
- Why it's doing it that way
- Important implementation details
- Performance considerations
- Edge cases being handled

**Example**:

```c
// Initialize lexer state
parser->lexer = lexer;                    // Store reference to the lexer
parser->current_token = NULL;             // Current token being processed
parser->previous_token = NULL;            // Previously processed token
parser->current_position = 0;             // Current position in token stream
parser->error_count = 0;                  // Number of parsing errors encountered
parser->error_message = NULL;             // Description of the last error
parser->error_line = 0;                   // Line number where error occurred
parser->error_column = 0;                 // Column number where error occurred
```

### **4. Section Header Comments**

Logical sections of code are separated with descriptive headers:

```c
// Statement parsing functions

/**
 * @brief Parse an expression statement
 * 
 * Expression statements are expressions followed by a semicolon or newline.
 * They're the most common type of statement in Myco programs.
 * 
 * @param parser The parser to use
 * @return AST node representing the expression statement
 */
```

### **5. TODO Comments**

Placeholder implementations include TODO comments explaining what needs to be implemented:

```c
// TODO: Implement full token scanning logic
// This placeholder implementation just creates a basic token structure
```

---

## **Module-Specific Commenting**

### **Core Module (`src/core/`)**

- **lexer.h/c**: Comprehensive explanation of tokenization process
- **parser.h/c**: Detailed parsing strategy and operator precedence
- **ast.h/c**: AST structure and node types
- **interpreter.h/c**: Execution model and value system

### **Compilation Module (`src/compilation/`)**

- **compiler.h/c**: Compilation pipeline and code generation
- **codegen/**: Target-specific code generation strategies
- **optimization/**: AST optimization passes

### **Runtime Module (`src/runtime/`)**

- **memory.h/c**: Memory management strategies and safety features
- **garbage_collector.h/c**: Automatic memory cleanup
- **type_system.h/c**: Runtime type checking

### **CLI Module (`src/cli/`)**

- **main.c**: Command-line interface and program flow
- **argument_parser.c**: Option parsing and validation
- **repl.c**: Interactive mode implementation

### **Utils Module (`src/utils/`)**

- **error_handling.c**: Centralized error management
- **logging.c**: Debug output and diagnostics
- **file_utils.c**: File I/O operations

---

## **Commenting Standards**

### **Formatting**

- Use Doxygen-style comments for function documentation
- Include `@brief`, `@param`, `@return` tags consistently
- Use `@code` and `@endcode` for usage examples
- Keep inline comments concise but informative

### **Content Guidelines**

- **Explain the "what"**: What does this code do?
- **Explain the "why"**: Why is it implemented this way?
- **Explain the "how"**: How does it work internally?
- **Provide context**: How does it fit into the larger system?
- **Include examples**: Show how to use the functionality
- **Note limitations**: Mention any constraints or edge cases

### **Language Style**

- Write in clear, professional English
- Use active voice when possible
- Be specific and avoid vague language
- Include technical details that matter
- Explain concepts that might not be obvious

---

## **Finding Information**

### **Quick Navigation**

- **File Headers**: Start here to understand what a file does
- **Function Comments**: Find specific functionality
- **Inline Comments**: Understand implementation details
- **Section Headers**: Navigate to relevant code sections

### **Common Patterns**

- **"When adding new..."**: Look for guidance on extending functionality
- **"This function..."**: Find detailed explanations of behavior
- **"TODO:"**: Identify incomplete implementations
- **"Examples:"**: Find usage patterns and examples

---

## **Adding New Comments**

### **For New Files**

1. **File Header**: Explain the file's purpose and responsibilities
2. **Module Context**: How it fits into the overall system
3. **Usage Guidelines**: When and how to use this functionality
4. **Extension Points**: How to add new features

### **For New Functions**

1. **Purpose**: What does this function accomplish?
2. **Parameters**: What does each parameter do?
3. **Return Value**: What does the function return?
4. **Usage Example**: How should it be called?
5. **Side Effects**: What changes does it make?

### **For New Code Sections**

1. **Section Header**: What is this group of functions doing?
2. **Implementation Notes**: Important details about the approach
3. **Edge Cases**: Special situations being handled
4. **Performance Notes**: Any performance considerations

---

## **Benefits of This Approach**

### **For Developers**

- **Faster Onboarding**: New team members can understand the system quickly
- **Easier Navigation**: Find specific functionality without searching
- **Better Understanding**: Comprehend complex algorithms and data structures
- **Confident Modifications**: Know what can be changed safely

### **For Maintenance**

- **Reduced Bugs**: Understanding prevents mistakes
- **Easier Debugging**: Clear explanations help identify issues
- **Better Testing**: Understanding leads to better test coverage
- **Faster Development**: Less time spent figuring out how things work

### **For Project Health**

- **Knowledge Preservation**: Critical information is documented
- **Quality Assurance**: Clear expectations for code quality
- **Team Collaboration**: Shared understanding across the team
- **Long-term Maintainability**: Code remains understandable over time

---

## **Examples of Good Comments**

### **Complex Algorithm Explanation**

```c
/**
 * @brief Parse logical OR expressions (lowest precedence)
 * 
 * Logical OR expressions use the 'or' keyword and have the lowest precedence
 * among logical operators. This allows expressions like:
 * a or b and c  (parsed as: a or (b and c))
 * 
 * The parser implements this using the Pratt parsing technique, where
 * each precedence level has its own parsing function. This function
 * handles the lowest precedence level and calls higher precedence
 * functions for its operands.
 * 
 * @param parser The parser to use
 * @return AST node representing the logical OR expression
 */
```

### **Data Structure Documentation**

```c
/**
 * @brief Represents a single token in the source code
 * 
 * Tokens are the fundamental units that the lexer produces from
 * source code. Each token represents a meaningful piece of the
 * language: a keyword, identifier, literal, operator, etc.
 * 
 * The token includes position information (line and column) for
 * accurate error reporting, and stores the actual text that was
 * tokenized for debugging and display purposes.
 */
typedef struct {
    TokenType type;        // What kind of token this is
    char* text;            // The actual text that was tokenized
    int line;              // Line number in source file (1-based)
    int column;            // Column number in source file (1-based)
    union {
        double number_value;    // Numeric value for TOKEN_NUMBER
        char* string_value;     // String value for TOKEN_STRING
        int bool_value;         // Boolean value for TOKEN_BOOL
    } data;
} Token;
```

### **Configuration Documentation**

```c
/**
 * @brief Memory management configuration
 * 
 * These constants define the default sizes and limits for Myco's
 * memory management system. They control how much memory is allocated
 * initially and how many memory pools/arenas can be created.
 * 
 * These values can be tuned based on the target system and application
 * requirements. Larger values provide better performance but use more memory.
 */
#define MYCO_DEFAULT_POOL_SIZE (1024 * 1024)  // Default memory pool size: 1MB
#define MYCO_DEFAULT_ARENA_SIZE (1024 * 1024) // Default memory arena size: 1MB
#define MYCO_MAX_POOL_COUNT 100               // Maximum number of memory pools
#define MYCO_MAX_ARENA_COUNT 100              // Maximum number of memory arenas
```

---

## **Conclusion**

The comprehensive commenting approach used in Myco v2.0 transforms the codebase from a collection of code files into a self-documenting, navigable system. Every developer who works with this code will find:

- **Clear understanding** of what each component does
- **Confident navigation** to find specific functionality
- **Comprehensive guidance** on how to extend the system
- **Professional documentation** that supports long-term maintenance

This commenting strategy ensures that Myco remains accessible, maintainable, and developer-friendly as it grows and evolves. The investment in thorough documentation pays dividends in development speed, code quality, and team productivity.
