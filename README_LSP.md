# Myco Language Server

This directory contains the Language Server Protocol (LSP) implementation for the Myco programming language.

## Features

- **Syntax Highlighting**: Full support for Myco syntax highlighting
- **Code Completion**: Intelligent code completion for functions, variables, and types
- **Error Detection**: Real-time error detection and reporting
- **Hover Information**: Detailed information on hover over symbols
- **Go to Definition**: Navigate to symbol definitions
- **Find References**: Find all references to a symbol
- **Document Symbols**: List all symbols in a document
- **Workspace Symbols**: Search symbols across the entire workspace

## Installation

### VS Code

1. Copy the `.vscode` directory to your project root
2. Ensure `bin/myco-lsp` is executable
3. Open your `.myco` files in VS Code

### Other IDEs

The LSP server can be used with any IDE that supports the Language Server Protocol:

- **Neovim**: Use with `nvim-lspconfig`
- **Emacs**: Use with `lsp-mode`
- **Sublime Text**: Use with `LSP` package
- **Atom**: Use with `atom-languageclient`

## Configuration

### VS Code Settings

```json
{
    "myco.languageServerPath": "./bin/myco-lsp",
    "myco.enableLanguageServer": true,
    "files.associations": {
        "*.myco": "myco"
    }
}
```

### LSP Client Configuration

```json
{
    "command": "./bin/myco-lsp",
    "args": ["--debug"],
    "filetypes": ["myco"],
    "rootPatterns": ["*.myco", "myco.json"]
}
```

## Usage

1. Start your IDE with LSP support
2. Open a `.myco` file
3. The language server will automatically start
4. Enjoy features like syntax highlighting, completion, and error detection

## Debug Mode

Run the language server in debug mode to see detailed logging:

```bash
bin/myco-lsp --debug
```

## Development

To rebuild the language server:

```bash
make lsp
```

## Troubleshooting

- Ensure the language server binary is executable
- Check that all dependencies are installed
- Verify the LSP client configuration
- Check the debug output for error messages
