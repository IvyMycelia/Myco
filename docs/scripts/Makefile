# Myco Documentation Makefile
# Automates the generation of all documentation

.PHONY: all clean docs validate examples api syntax changelog

# Default target
all: docs

# Generate all documentation
docs: api syntax examples changelog validate
	@echo "All documentation generated successfully!"

# Generate API reference
api:
	@echo "Generating API reference..."
	python3 docs/scripts/extract_functions.py

# Generate syntax reference  
syntax:
	@echo "Generating syntax reference..."
	python3 docs/scripts/extract_syntax.py

# Generate examples
examples:
	@echo "Generating examples..."
	python3 docs/scripts/generate_examples.py

# Generate changelog
changelog:
	@echo "Generating changelog..."
	python3 docs/scripts/generate_changelog.py

# Validate all examples
validate:
	@echo "Validating examples..."
	python3 docs/scripts/validate_examples.py

# Generate everything
generate-all:
	@echo "Generating all documentation..."
	python3 docs/scripts/generate_all_docs.py

# Clean generated files
clean:
	@echo "Cleaning generated documentation..."
	rm -rf docs/generated/*
	@echo "Generated documentation cleaned!"

# Install dependencies (if needed)
install-deps:
	@echo "No external dependencies required for documentation generation"

# Help
help:
	@echo "Myco Documentation Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  all          - Generate all documentation (default)"
	@echo "  docs         - Generate all documentation"
	@echo "  api          - Generate API reference"
	@echo "  syntax       - Generate syntax reference"
	@echo "  examples     - Generate examples"
	@echo "  changelog    - Generate changelog"
	@echo "  validate     - Validate all examples"
	@echo "  generate-all - Run master generation script"
	@echo "  clean        - Remove generated files"
	@echo "  help         - Show this help"
