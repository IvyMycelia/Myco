# Myco Documentation Automation

This directory contains scripts to automatically generate comprehensive documentation for the Myco programming language.

## Overview

The documentation system extracts information directly from the Myco source code and generates:

- **API Reference** - All built-in functions and libraries
- **Syntax Reference** - Complete language syntax guide
- **Examples** - Working code examples with explanations
- **Changelog** - Version history from git commits
- **Validation Report** - Test results for all code examples

## Scripts

### `extract_functions.py`
Extracts function signatures and documentation from C source files.
- Scans `src/core/interpreter/`, `src/libs/`, and `src/core/` directories
- Identifies built-in functions and their parameters
- Generates categorized API reference

### `extract_syntax.py`
Extracts syntax patterns from lexer and parser source code.
- Analyzes token definitions and AST node types
- Identifies keywords and language constructs
- Generates comprehensive syntax reference

### `generate_examples.py`
Creates working code examples from existing Myco files.
- Scans `examples/`, `tests/`, `benchmark/`, and `codesoc_website/` directories
- Extracts code patterns and examples
- Generates categorized examples documentation

### `generate_changelog.py`
Generates changelog from git commit history.
- Analyzes commit messages to categorize changes
- Groups changes by type (Added, Fixed, Changed, etc.)
- Generates version history and release notes

### `validate_examples.py`
Tests all code examples to ensure they work correctly.
- Extracts code blocks from markdown files
- Runs each example through the Myco interpreter
- Generates validation report with success/failure status

### `generate_all_docs.py`
Master script that runs all documentation generation scripts.
- Executes all scripts in the correct order
- Generates main documentation index
- Provides summary of generation results

## Usage

### Generate All Documentation
```bash
# Run the master script
python3 docs/scripts/generate_all_docs.py

# Or use make
make -f docs/scripts/Makefile docs
```

### Generate Specific Documentation
```bash
# API reference only
python3 docs/scripts/extract_functions.py

# Syntax reference only  
python3 docs/scripts/extract_syntax.py

# Examples only
python3 docs/scripts/generate_examples.py

# Changelog only
python3 docs/scripts/generate_changelog.py

# Validate examples only
python3 docs/scripts/validate_examples.py
```

### Using Make
```bash
# Generate all documentation
make -f docs/scripts/Makefile all

# Generate specific sections
make -f docs/scripts/Makefile api
make -f docs/scripts/Makefile syntax
make -f docs/scripts/Makefile examples
make -f docs/scripts/Makefile changelog
make -f docs/scripts/Makefile validate

# Clean generated files
make -f docs/scripts/Makefile clean

# Show help
make -f docs/scripts/Makefile help
```

## Output

All generated documentation is saved to `docs/generated/`:

- `README.md` - Main documentation index
- `API_REFERENCE.md` - Built-in functions and libraries
- `SYNTAX_REFERENCE.md` - Language syntax guide
- `EXAMPLES.md` - Working code examples
- `CHANGELOG.md` - Version history
- `RELEASE_NOTES.md` - Current version changes
- `VALIDATION_REPORT.md` - Example test results
- `functions.json` - Function data (JSON)
- `syntax_data.json` - Syntax data (JSON)
- `commits.json` - Commit categorization (JSON)
- `validation_results.json` - Test results (JSON)

## Requirements

- Python 3.6+
- Git (for changelog generation)
- Myco interpreter (for example validation)
- Access to Myco source code

## Customization

### Adding New Function Categories
Edit `extract_functions.py` and modify the `generate_api_reference()` function to add new categories or change categorization logic.

### Adding New Syntax Patterns
Edit `extract_syntax.py` and modify the `generate_syntax_reference()` function to add new syntax sections or examples.

### Adding New Example Types
Edit `generate_examples.py` and modify the `generate_examples_documentation()` function to add new example categories or patterns.

### Customizing Changelog Format
Edit `generate_changelog.py` and modify the `generate_changelog()` function to change the changelog format or categorization.

## Integration

The documentation system can be integrated into:

- **CI/CD pipelines** - Run on every commit
- **Release process** - Generate docs before releases
- **Development workflow** - Update docs during development
- **Website generation** - Use generated markdown for websites

## Maintenance

- **Regular updates** - Run documentation generation regularly
- **Example validation** - Ensure all examples continue to work
- **Source synchronization** - Keep scripts in sync with code changes
- **Format consistency** - Maintain consistent documentation format
