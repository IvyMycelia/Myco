#ifndef MYCO_CLI_H
#define MYCO_CLI_H

/**
 * @file cli.h
 * @brief Command Line Interface Module - handles user interaction and program control
 * 
 * The CLI module provides the user-facing interface for the Myco programming language.
 * It handles command-line argument parsing, user input, and provides an interactive
 * environment for running Myco programs.
 * 
 * Key components of the CLI module:
 * 
 * - **Argument Parser**: Processes command-line flags and options
 * - **Main Entry Point**: Program startup, initialization, and main control flow
 * - **REPL (Read-Eval-Print Loop)**: Interactive mode for testing and learning
 * - **File Processing**: Handles input files and source code strings
 * - **Output Management**: Controls where and how results are displayed
 * 
 * The CLI supports several modes of operation:
 * 
 * 1. **File Interpretation**: `myco program.myco` - runs a Myco source file
 * 2. **String Interpretation**: `myco \`code\`` - runs inline Myco code
 * 3. **Compilation**: `myco program.myco --build c` - compiles to other formats
 * 4. **Interactive Mode**: `myco` (no arguments) - starts the REPL
 * 
 * When adding new CLI features:
 * 1. **New Options**: Add to the argument parser in argument_parser.c
 * 2. **New Commands**: Extend the main processing logic in main.c
 * 3. **REPL Features**: Enhance the interactive mode in repl.c
 * 4. **Output Formats**: Add new ways to display results and errors
 * 
 * The CLI module is designed to be user-friendly while providing powerful
 * functionality for both beginners and advanced users. It includes helpful
 * error messages, usage examples, and interactive help features.
 */

// CLI components
// TODO: Add CLI headers when implemented
// #include "argument_parser.h"  // Command-line argument parsing and validation
// #include "repl.h"             // Interactive Read-Eval-Print Loop

#endif // MYCO_CLI_H
