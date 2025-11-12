# Myco v2.0 - High-Performance Programming Language
# Makefile for building the complete project

# Compiler and flags
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -O2
DEBUG_CFLAGS = -std=c99 -Wall -Wextra -pedantic -g -O0 -DDEBUG
RELEASE_CFLAGS = -std=c99 -Wall -Wextra -pedantic -O3 -DNDEBUG
# AddressSanitizer flags for memory debugging
ASAN_CFLAGS = -std=c99 -Wall -Wextra -pedantic -g -O1 -fsanitize=address -fno-omit-frame-pointer -fno-common

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

# Source files (recursive search through all subdirectories, excluding LSP and experimental codegen split files)
SRC_FILES = $(shell find $(SRC_DIR) -name "*.c" ! -path "*/lsp/*" \
    ! -name "codegen_statements.c" \
    ! -name "codegen_variables.c" \
    ! -name "codegen_utils.c" \
    ! -name "codegen_headers.c" \
    ! -name "compiler_new.c" \
    ! -name "error_handling.c" \
    ! -name "error_system.c")
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# LSP source files (separate from main build)
LSP_SRC_FILES = $(shell find $(SRC_DIR)/lsp -name "*.c")
LSP_OBJ_FILES = $(LSP_SRC_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Header files
HEADER_FILES = $(wildcard $(INCLUDE_DIR)/*.h)

# Main executable
MAIN_EXECUTABLE = $(BIN_DIR)/myco
TEST_EXECUTABLE = $(BIN_DIR)/myco_test
LSP_EXECUTABLE = $(BIN_DIR)/myco-lsp

# Libraries - No external dependencies (using custom implementations)
# SDL2 is optional for graphics library - enhanced cross-platform detection
SDL2_CFLAGS := $(shell pkg-config --cflags sdl2 2>/dev/null || sdl2-config --cflags 2>/dev/null || echo "")
SDL2_LIBS := $(shell pkg-config --libs sdl2 2>/dev/null || sdl2-config --libs 2>/dev/null || echo "-lSDL2")

# SDL_ttf for font rendering (text editor support)
SDL2_TTF_CFLAGS := $(shell pkg-config --cflags SDL2_ttf 2>/dev/null || echo "")
SDL2_TTF_LIBS := $(shell pkg-config --libs SDL2_ttf 2>/dev/null || echo "-lSDL2_ttf")

# Platform-specific SDL2 paths and configuration
ifeq ($(PLATFORM),macos)
    # macOS: Check Homebrew paths first, then system paths
    ifeq ($(shell [ -d /opt/homebrew/include/SDL2 ] && echo exists),exists)
        SDL2_CFLAGS := -I/opt/homebrew/include -D_THREAD_SAFE
        SDL2_LIBS := -L/opt/homebrew/lib -lSDL2
        SDL2_TTF_CFLAGS := -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE
        SDL2_TTF_LIBS := -L/opt/homebrew/lib -lSDL2_ttf
        # Also add parent directory for SDL_ttf.h
        SDL2_TTF_CFLAGS := $(SDL2_TTF_CFLAGS) -I/opt/homebrew/include
    else ifeq ($(shell [ -d /usr/local/include/SDL2 ] && echo exists),exists)
        SDL2_CFLAGS := -I/usr/local/include -D_THREAD_SAFE
        SDL2_LIBS := -L/usr/local/lib -lSDL2
        SDL2_TTF_CFLAGS := -I/usr/local/include/SDL2 -D_THREAD_SAFE
        SDL2_TTF_LIBS := -L/usr/local/lib -lSDL2_ttf
        # Also add parent directory for SDL_ttf.h
        SDL2_TTF_CFLAGS := $(SDL2_TTF_CFLAGS) -I/usr/local/include
    else
        # Fallback to pkg-config/sdl2-config result (includes parent directory)
        SDL2_CFLAGS := $(SDL2_CFLAGS) -D_THREAD_SAFE
        SDL2_TTF_CFLAGS := $(SDL2_TTF_CFLAGS) -D_THREAD_SAFE
    endif
    LIBS = -lm $(SDL2_LIBS) $(SDL2_TTF_LIBS) -framework OpenGL -lssl -lcrypto -lpthread
    INCLUDE_FLAGS = $(SDL2_CFLAGS) $(SDL2_TTF_CFLAGS)
else ifeq ($(PLATFORM),linux)
    # Linux: Standard package manager paths
    # pkg-config should handle most cases, but add common fallbacks
    ifeq ($(SDL2_CFLAGS),)
        SDL2_CFLAGS := -I/usr/include -D_REENTRANT
        SDL2_LIBS := -lSDL2
        SDL2_TTF_CFLAGS := -I/usr/include/SDL2 -D_REENTRANT
        SDL2_TTF_LIBS := -lSDL2_ttf
    else
        SDL2_CFLAGS := $(SDL2_CFLAGS) -D_REENTRANT
        SDL2_TTF_CFLAGS := $(SDL2_TTF_CFLAGS) -D_REENTRANT
    endif
    LIBS = -lm $(SDL2_LIBS) $(SDL2_TTF_LIBS) -framework OpenGL -lssl -lcrypto -lpthread
    INCLUDE_FLAGS = $(SDL2_CFLAGS) $(SDL2_TTF_CFLAGS)
else ifeq ($(PLATFORM),windows)
    # Windows (MinGW/MSYS2): Check common installation paths
    ifeq ($(shell [ -d /mingw64/include/SDL2 ] && echo exists),exists)
        SDL2_CFLAGS := -I/mingw64/include -Dmain=SDL_main
        SDL2_LIBS := -L/mingw64/lib -lSDL2 -lmingw32
        SDL2_TTF_CFLAGS := -I/mingw64/include/SDL2 -Dmain=SDL_main
        SDL2_TTF_LIBS := -L/mingw64/lib -lSDL2_ttf -lmingw32
    else ifeq ($(shell [ -d /usr/include/SDL2 ] && echo exists),exists)
        SDL2_CFLAGS := -I/usr/include -Dmain=SDL_main
        SDL2_LIBS := -L/usr/lib -lSDL2 -lmingw32
        SDL2_TTF_CFLAGS := -I/usr/include/SDL2 -Dmain=SDL_main
        SDL2_TTF_LIBS := -L/usr/lib -lSDL2_ttf -lmingw32
    else
        # Fallback to pkg-config/sdl2-config result (includes parent directory)
        SDL2_CFLAGS := $(SDL2_CFLAGS) -Dmain=SDL_main
        SDL2_LIBS := $(SDL2_LIBS) -lmingw32
        SDL2_TTF_CFLAGS := $(SDL2_TTF_CFLAGS) -Dmain=SDL_main
        SDL2_TTF_LIBS := $(SDL2_TTF_LIBS) -lmingw32
    endif
    LIBS = -lm $(SDL2_LIBS) $(SDL2_TTF_LIBS) -framework OpenGL -lssl -lcrypto -lpthread
    INCLUDE_FLAGS = $(SDL2_CFLAGS) $(SDL2_TTF_CFLAGS)
else
    # Unknown platform: use pkg-config/sdl2-config result
    LIBS = -lm $(SDL2_LIBS) $(SDL2_TTF_LIBS) -framework OpenGL -lssl -lcrypto -lpthread
    INCLUDE_FLAGS = $(SDL2_CFLAGS) $(SDL2_TTF_CFLAGS)

# Optional bgfx backend (Phase 1 scaffold)
# Enable with: make BGFX=1 BGFX_INCLUDE="-I/path/to/bgfx/include -I/path/to/bx/include -I/path/to/bimg/include" BGFX_LIBS="-L... -lbgfx -lbx -lbimg"
ifeq ($(BGFX),1)
    CFLAGS += -DMYCO_USE_BGFX=1
    ifdef BGFX_INCLUDE
        INCLUDE_FLAGS += $(BGFX_INCLUDE)
    endif
    ifdef BGFX_LIBS
        LIBS += $(BGFX_LIBS)
    endif
endif
endif

# Default target
.PHONY: all
all: $(MAIN_EXECUTABLE)

# Debug build
.PHONY: debug
debug: CFLAGS = $(DEBUG_CFLAGS)
debug: $(MAIN_EXECUTABLE)

# AddressSanitizer build for memory debugging
.PHONY: asan
asan: CFLAGS = $(ASAN_CFLAGS)
asan: LIBS += -fsanitize=address
asan: $(MAIN_EXECUTABLE)

# Release build
.PHONY: release
release: CFLAGS = $(RELEASE_CFLAGS)
release: $(MAIN_EXECUTABLE)

# LSP server build
.PHONY: lsp
lsp: $(LSP_EXECUTABLE)

# Create build directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Main executable
$(MAIN_EXECUTABLE): $(OBJ_FILES) $(BUILD_DIR)/compilation/codegen_statements.o $(BUILD_DIR)/compilation/codegen_variables.o $(BUILD_DIR)/compilation/codegen_utils.o $(BUILD_DIR)/compilation/codegen_headers.o | $(BIN_DIR)
	@echo "Linking $@..."
	$(CC) $(OBJ_FILES) $(BUILD_DIR)/compilation/codegen_statements.o $(BUILD_DIR)/compilation/codegen_variables.o $(BUILD_DIR)/compilation/codegen_utils.o $(BUILD_DIR)/compilation/codegen_headers.o -o $@ $(LIBS)
	@echo "Build complete: $@"

# LSP executable
$(LSP_EXECUTABLE): $(LSP_OBJ_FILES) $(BUILD_DIR)/core/interpreter/interpreter_main.o $(BUILD_DIR)/core/lexer.o $(BUILD_DIR)/core/parser.o $(BUILD_DIR)/core/ast.o $(BUILD_DIR)/core/type_checker.o $(BUILD_DIR)/core/environment.o $(BUILD_DIR)/core/error_handling.o $(BUILD_DIR)/core/error_system.o $(BUILD_DIR)/core/jit_compiler.o $(BUILD_DIR)/runtime/memory.o $(BUILD_DIR)/runtime/myco_runtime.o $(BUILD_DIR)/libs/json.o $(BUILD_DIR)/libs/sets.o $(BUILD_DIR)/libs/math.o $(BUILD_DIR)/libs/builtin_libs.o $(BUILD_DIR)/libs/array.o $(BUILD_DIR)/libs/maps.o $(BUILD_DIR)/libs/string.o $(BUILD_DIR)/libs/server/server.o $(BUILD_DIR)/libs/stacks.o $(BUILD_DIR)/libs/dir.o $(BUILD_DIR)/libs/graphs.o $(BUILD_DIR)/libs/time.o $(BUILD_DIR)/libs/http.o $(BUILD_DIR)/libs/trees.o $(BUILD_DIR)/libs/file.o $(BUILD_DIR)/libs/queues.o $(BUILD_DIR)/libs/heaps.o $(BUILD_DIR)/libs/regex.o $(BUILD_DIR)/compilation/optimization/optimizer.o $(BUILD_DIR)/compilation/compiler.o $(BUILD_DIR)/compilation/compiler_new.o $(BUILD_DIR)/compilation/codegen_expressions.o $(BUILD_DIR)/compilation/codegen_statements.o $(BUILD_DIR)/compilation/codegen_variables.o $(BUILD_DIR)/compilation/codegen_utils.o $(BUILD_DIR)/compilation/codegen_headers.o | $(BIN_DIR)
	@echo "Linking $@..."
	$(CC) $(LSP_OBJ_FILES) $(BUILD_DIR)/core/interpreter/interpreter_main.o $(BUILD_DIR)/core/lexer.o $(BUILD_DIR)/core/parser.o $(BUILD_DIR)/core/ast.o $(BUILD_DIR)/core/type_checker.o $(BUILD_DIR)/core/environment.o $(BUILD_DIR)/core/error_handling.o $(BUILD_DIR)/core/error_system.o $(BUILD_DIR)/core/jit_compiler.o $(BUILD_DIR)/runtime/memory.o $(BUILD_DIR)/runtime/myco_runtime.o $(BUILD_DIR)/libs/json.o $(BUILD_DIR)/libs/sets.o $(BUILD_DIR)/libs/math.o $(BUILD_DIR)/libs/builtin_libs.o $(BUILD_DIR)/libs/array.o $(BUILD_DIR)/libs/maps.o $(BUILD_DIR)/libs/string.o $(BUILD_DIR)/libs/server/server.o $(BUILD_DIR)/libs/stacks.o $(BUILD_DIR)/libs/dir.o $(BUILD_DIR)/libs/graphs.o $(BUILD_DIR)/libs/time.o $(BUILD_DIR)/libs/http.o $(BUILD_DIR)/libs/trees.o $(BUILD_DIR)/libs/file.o $(BUILD_DIR)/libs/queues.o $(BUILD_DIR)/libs/heaps.o $(BUILD_DIR)/libs/regex.o $(BUILD_DIR)/compilation/optimization/optimizer.o $(BUILD_DIR)/compilation/compiler.o $(BUILD_DIR)/compilation/compiler_new.o $(BUILD_DIR)/compilation/codegen_expressions.o $(BUILD_DIR)/compilation/codegen_statements.o $(BUILD_DIR)/compilation/codegen_variables.o $(BUILD_DIR)/compilation/codegen_utils.o $(BUILD_DIR)/compilation/codegen_headers.o -o $@ $(LIBS)
	@echo "LSP server build complete: $@"

# Object files (handle subdirectories)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADER_FILES) | $(BUILD_DIR)
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/core -I$(INCLUDE_DIR)/compilation -I$(INCLUDE_DIR)/runtime -I$(INCLUDE_DIR)/cli -I$(INCLUDE_DIR)/utils -I$(INCLUDE_DIR)/lsp $(INCLUDE_FLAGS) -c $< -o $@

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

# Debian-specific setup
.PHONY: debian-setup
debian-setup:
	@echo "Setting up Myco dependencies for Debian..."
	@echo "Installing required libraries..."
	sudo apt-get update
	sudo apt-get install -y build-essential libcurl4-openssl-dev libmicrohttpd-dev libreadline-dev zlib1g-dev
	@echo "Dependencies installed. Run 'make clean && make' to rebuild."

# Check dependencies
.PHONY: check-deps
check-deps:
	@echo "Checking Myco dependencies..."
	@echo "Required libraries:"
	@echo -n "libcurl: "; pkg-config --exists libcurl && echo "✓ Found" || echo "✗ Missing"
	@echo -n "libmicrohttpd: "; pkg-config --exists libmicrohttpd && echo "✓ Found" || echo "✗ Missing"
	@echo -n "libreadline: "; pkg-config --exists readline && echo "✓ Found" || echo "✗ Missing"
	@echo -n "zlib: "; pkg-config --exists zlib && echo "✓ Found" || echo "✗ Missing"

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

# Version management
.PHONY: version version-patch version-minor version-major version-auto version-status
version:
	@echo "Current Myco Version:"
	@./bin/myco --version

version-patch:
	@python3 scripts/version_manager.py patch
	@echo "Patch version bumped"

version-minor:
	@python3 scripts/version_manager.py minor
	@echo "Minor version bumped"

version-major:
	@python3 scripts/version_manager.py major
	@echo "Major version bumped"

version-auto:
	@python3 scripts/version_manager.py auto
	@echo "Version auto-bumped"

version-status:
	@python3 scripts/version_manager.py status

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
	@echo "  version      - Show current version"
	@echo "  version-patch - Bump patch version (bug fixes)"
	@echo "  version-minor - Bump minor version (new features)"
	@echo "  version-major - Bump major version (breaking changes)"
	@echo "  version-auto - Auto-detect and bump version"
	@echo "  version-status - Show version and recent changes"
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