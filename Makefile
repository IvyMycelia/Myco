# Myco v2.0 - High-Performance Programming Language
# Makefile for building the complete project

# Compiler and flags
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -O2
DEBUG_CFLAGS = -std=c99 -Wall -Wextra -pedantic -g -O0 -DDEBUG
RELEASE_CFLAGS = -std=c99 -Wall -Wextra -pedantic -O3 -DNDEBUG

# Platform detection
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    PLATFORM = macos
    CFLAGS += -DMYCO_PLATFORM_MACOS=1
    CFLAGS += -DMYCO_PLATFORM_LINUX=0
    CFLAGS += -DMYCO_PLATFORM_WINDOWS=0
else ifeq ($(UNAME_S),Linux)
    PLATFORM = linux
    CFLAGS += -DMYCO_PLATFORM_MACOS=0
    CFLAGS += -DMYCO_PLATFORM_LINUX=1
    CFLAGS += -DMYCO_PLATFORM_WINDOWS=0
else ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
    PLATFORM = windows
    CFLAGS += -DMYCO_PLATFORM_MACOS=0
    CFLAGS += -DMYCO_PLATFORM_LINUX=0
    CFLAGS += -DMYCO_PLATFORM_WINDOWS=1
else
    PLATFORM = unknown
    CFLAGS += -DMYCO_PLATFORM_MACOS=0
    CFLAGS += -DMYCO_PLATFORM_LINUX=0
    CFLAGS += -DMYCO_PLATFORM_WINDOWS=0
endif

# Architecture detection
UNAME_M := $(shell uname -m)
ifeq ($(UNAME_M),x86_64)
    ARCH = x86_64
    CFLAGS += -DMYCO_ARCH_X86_64=1
    CFLAGS += -DMYCO_ARCH_ARM64=0
    CFLAGS += -DMYCO_ARCH_ARM=0
else ifeq ($(UNAME_M),aarch64)
    ARCH = arm64
    CFLAGS += -DMYCO_ARCH_X86_64=0
    CFLAGS += -DMYCO_ARCH_ARM64=1
    CFLAGS += -DMYCO_ARCH_ARM=0
else ifeq ($(UNAME_M),armv7l)
    ARCH = arm
    CFLAGS += -DMYCO_ARCH_X86_64=0
    CFLAGS += -DMYCO_ARCH_ARM64=0
    CFLAGS += -DMYCO_ARCH_ARM=1
else
    ARCH = unknown
    CFLAGS += -DMYCO_ARCH_X86_64=0
    CFLAGS += -DMYCO_ARCH_ARM64=0
    CFLAGS += -DMYCO_ARCH_ARM=0
endif

# Compiler detection
ifeq ($(CC),gcc)
    CFLAGS += -DMYCO_COMPILER_GCC=1
    CFLAGS += -DMYCO_COMPILER_CLANG=0
    CFLAGS += -DMYCO_COMPILER_MSVC=0
else ifeq ($(CC),clang)
    CFLAGS += -DMYCO_COMPILER_GCC=0
    CFLAGS += -DMYCO_COMPILER_CLANG=1
    CFLAGS += -DMYCO_COMPILER_MSVC=0
else ifeq ($(CC),cl)
    CFLAGS += -DMYCO_COMPILER_GCC=0
    CFLAGS += -DMYCO_COMPILER_CLANG=0
    CFLAGS += -DMYCO_COMPILER_MSVC=1
else
    CFLAGS += -DMYCO_COMPILER_GCC=0
    CFLAGS += -DMYCO_COMPILER_CLANG=0
    CFLAGS += -DMYCO_COMPILER_MSVC=0
endif

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = tests
DOCS_DIR = docs
EXAMPLES_DIR = examples
TOOLS_DIR = tools

# Source files (recursive search through all subdirectories)
SRC_FILES = $(shell find $(SRC_DIR) -name "*.c")
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Header files
HEADER_FILES = $(wildcard $(INCLUDE_DIR)/*.h)

# Main executable
MAIN_EXECUTABLE = $(BIN_DIR)/myco
TEST_EXECUTABLE = $(BIN_DIR)/myco_test

# Libraries
LIBS = -lm -lcurl -L/opt/homebrew/opt/libmicrohttpd/lib -lmicrohttpd

# Default target
.PHONY: all
all: $(MAIN_EXECUTABLE)

# Debug build
.PHONY: debug
debug: CFLAGS = $(DEBUG_CFLAGS)
debug: $(MAIN_EXECUTABLE)

# Release build
.PHONY: release
release: CFLAGS = $(RELEASE_CFLAGS)
release: $(MAIN_EXECUTABLE)

# Create build directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Main executable
$(MAIN_EXECUTABLE): $(OBJ_FILES) | $(BIN_DIR)
	@echo "Linking $@..."
	$(CC) $(OBJ_FILES) -o $@ $(LIBS)
	@echo "Build complete: $@"

# Object files (handle subdirectories)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADER_FILES) | $(BUILD_DIR)
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/core -I$(INCLUDE_DIR)/compilation -I$(INCLUDE_DIR)/runtime -I$(INCLUDE_DIR)/cli -I$(INCLUDE_DIR)/utils -I/opt/homebrew/opt/libmicrohttpd/include -c $< -o $@

# Test executable
.PHONY: test
test: $(TEST_EXECUTABLE)

$(TEST_EXECUTABLE): $(OBJ_FILES) | $(BIN_DIR)
	@echo "Building test executable..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -DTESTING $(OBJ_FILES) -o $@ $(LIBS)

# Run tests
.PHONY: run-tests
run-tests: test
	@echo "Running tests..."
	./$(TEST_EXECUTABLE)

# Clean build artifacts
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_DIR)
	rm -f *.o *.a *.so *.dylib

# Clean everything including generated files
.PHONY: distclean
distclean: clean
	@echo "Cleaning everything..."
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_DIR)
	rm -f *.o *.a *.so *.dylib
	rm -f tags cscope.out
	find . -name "*.swp" -delete
	find . -name "*.swo" -delete
	find . -name "*~" -delete

# Install (system-wide)
.PHONY: install
install: $(MAIN_EXECUTABLE)
	@echo "Installing Myco..."
	sudo cp $(MAIN_EXECUTABLE) /usr/local/bin/
	sudo chmod +x /usr/local/bin/myco
	@echo "Installation complete. Run 'myco --version' to verify."

# Uninstall
.PHONY: uninstall
uninstall:
	@echo "Uninstalling Myco..."
	sudo rm -f /usr/local/bin/myco
	@echo "Uninstallation complete."

# Development tools
.PHONY: tags
tags:
	@echo "Generating tags..."
	ctags -R $(SRC_DIR) $(INCLUDE_DIR)

.PHONY: cscope
cscope:
	@echo "Generating cscope database..."
	find $(SRC_DIR) $(INCLUDE_DIR) -name "*.c" -o -name "*.h" > cscope.files
	cscope -b -q -k

# Documentation
.PHONY: docs
docs:
	@echo "Generating documentation..."
	@if command -v doxygen >/dev/null 2>&1; then \
		doxygen Doxyfile; \
	else \
		echo "Doxygen not found. Install it to generate documentation."; \
	fi

# Format code
.PHONY: format
format:
	@echo "Formatting code..."
	@if command -v clang-format >/dev/null 2>&1; then \
		find $(SRC_DIR) $(INCLUDE_DIR) -name "*.c" -o -name "*.h" | xargs clang-format -i; \
	else \
		echo "clang-format not found. Install it to format code."; \
	fi

# Static analysis
.PHONY: analyze
analyze:
	@echo "Running static analysis..."
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --std=c99 $(SRC_DIR) $(INCLUDE_DIR); \
	else \
		echo "cppcheck not found. Install it to run static analysis."; \
	fi

# Memory check
.PHONY: memcheck
memcheck: $(MAIN_EXECUTABLE)
	@echo "Running memory check..."
	@if command -v valgrind >/dev/null 2>&1; then \
		valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./$(MAIN_EXECUTABLE) --version; \
	else \
		echo "valgrind not found. Install it to run memory checks."; \
	fi

# Performance profiling
.PHONY: profile
profile: $(MAIN_EXECUTABLE)
	@echo "Running performance profiling..."
	@if command -v gprof >/dev/null 2>&1; then \
		$(CC) $(CFLAGS) -pg -I$(INCLUDE_DIR) $(SRC_FILES) -o $(BIN_DIR)/myco_profile $(LIBS); \
		./$(BIN_DIR)/myco_profile --version; \
		gprof $(BIN_DIR)/myco_profile > profile.txt; \
		echo "Profile results saved to profile.txt"; \
	else \
		echo "gprof not found. Install it to run performance profiling."; \
	fi

# Package creation
.PHONY: package
package: release
	@echo "Creating package..."
	@VERSION=$$(grep 'MYCO_VERSION_STRING' $(INCLUDE_DIR)/myco.h | cut -d'"' -f2); \
	PACKAGE_NAME="myco-$$VERSION-$(PLATFORM)-$(ARCH)"; \
	mkdir -p $$PACKAGE_NAME; \
	cp -r $(BIN_DIR) $(INCLUDE_DIR) $(DOCS_DIR) $(EXAMPLES_DIR) $$PACKAGE_NAME/; \
	cp README.md LICENSE Makefile $$PACKAGE_NAME/; \
	tar -czf $$PACKAGE_NAME.tar.gz $$PACKAGE_NAME; \
	rm -rf $$PACKAGE_NAME; \
	echo "Package created: $$PACKAGE_NAME.tar.gz"

# Development setup
.PHONY: dev-setup
dev-setup:
	@echo "Setting up development environment..."
	@if command -v brew >/dev/null 2>&1; then \
		echo "Installing development tools with Homebrew..."; \
		brew install ctags cscope doxygen clang-format cppcheck valgrind; \
	elif command -v apt-get >/dev/null 2>&1; then \
		echo "Installing development tools with apt..."; \
		sudo apt-get update && sudo apt-get install -y ctags cscope doxygen clang-format cppcheck valgrind; \
	elif command -v yum >/dev/null 2>&1; then \
		echo "Installing development tools with yum..."; \
		sudo yum install -y ctags cscope doxygen clang-format cppcheck valgrind; \
	else \
		echo "Package manager not found. Please install development tools manually."; \
	fi

# Show build information
.PHONY: info
info:
	@echo "Myco v2.0 Build Information"
	@echo "============================"
	@echo "Platform: $(PLATFORM)"
	@echo "Architecture: $(ARCH)"
	@echo "Compiler: $(CC)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "Source files: $(SRC_FILES)"
	@echo "Header files: $(HEADER_FILES)"
	@echo "Build directory: $(BUILD_DIR)"
	@echo "Binary directory: $(BIN_DIR)"

# Help
.PHONY: help
help:
	@echo "Myco v2.0 Makefile Help"
	@echo "========================"
	@echo "Available targets:"
	@echo "  all          - Build the project (default)"
	@echo "  debug        - Build with debug symbols"
	@echo "  release      - Build optimized release version"
	@echo "  test         - Build test executable"
	@echo "  run-tests    - Build and run tests"
	@echo "  clean        - Remove build artifacts"
	@echo "  distclean    - Remove everything"
	@echo "  install      - Install system-wide"
	@echo "  uninstall    - Remove system-wide installation"
	@echo "  tags         - Generate ctags"
	@echo "  cscope       - Generate cscope database"
	@echo "  docs         - Generate documentation"
	@echo "  format       - Format code with clang-format"
	@echo "  analyze      - Run static analysis"
	@echo "  memcheck     - Run memory checks"
	@echo "  profile      - Run performance profiling"
	@echo "  package      - Create distribution package"
	@echo "  dev-setup    - Install development tools"
	@echo "  info         - Show build information"
	@echo "  help         - Show this help message"

# Dependencies
$(BUILD_DIR)/main.o: $(INCLUDE_DIR)/myco.h
$(BUILD_DIR)/ast.o: $(INCLUDE_DIR)/ast.h
$(BUILD_DIR)/lexer.o: $(INCLUDE_DIR)/lexer.h
$(BUILD_DIR)/parser.o: $(INCLUDE_DIR)/parser.h $(INCLUDE_DIR)/ast.h $(INCLUDE_DIR)/lexer.h
$(BUILD_DIR)/interpreter.o: $(INCLUDE_DIR)/interpreter.h $(INCLUDE_DIR)/ast.h
$(BUILD_DIR)/compiler.o: $(INCLUDE_DIR)/compiler.h $(INCLUDE_DIR)/ast.h
$(BUILD_DIR)/memory.o: $(INCLUDE_DIR)/memory.h

# Phony targets that don't create files
.PHONY: all debug release test run-tests clean distclean install uninstall
.PHONY: tags cscope docs format analyze memcheck profile package dev-setup info help