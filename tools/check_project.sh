#!/bin/bash

# Myco v2.0 Project Health Check Script
# This script checks the project structure and build status

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Project root directory
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

echo -e "${BLUE}Myco v2.0 Project Health Check${NC}"
echo "======================================"
echo "Project root: $PROJECT_ROOT"
echo ""

# Function to check if directory exists
check_directory() {
    local dir="$1"
    local description="$2"
    
    if [ -d "$dir" ]; then
        echo -e "${GREEN}✓${NC} $description: $dir"
        return 0
    else
        echo -e "${RED}✗${NC} $description: $dir (MISSING)"
        return 1
    fi
}

# Function to check if file exists
check_file() {
    local file="$1"
    local description="$2"
    
    if [ -f "$file" ]; then
        echo -e "${GREEN}✓${NC} $description: $file"
        return 0
    else
        echo -e "${RED}✗${NC} $description: $file (MISSING)"
        return 1
    fi
}

# Function to count files in directory
count_files() {
    local dir="$1"
    local pattern="$2"
    local count=0
    
    if [ -d "$dir" ]; then
        count=$(find "$dir" -name "$pattern" | wc -l)
    fi
    
    echo "$count"
}

# Function to check build status
check_build() {
    echo -e "\n${BLUE}Build Status Check${NC}"
    echo "------------------"
    
    if [ -d "build" ] && [ -d "bin" ]; then
        local obj_files=$(count_files "build" "*.o")
        local bin_files=$(count_files "bin" "*")
        
        echo -e "${GREEN}✓${NC} Build directories exist"
        echo "  - Object files: $obj_files"
        echo "  - Binary files: $bin_files"
        
        if [ -f "bin/myco" ]; then
            echo -e "${GREEN}✓${NC} Main executable exists"
            
            # Check if executable runs
            if ./bin/myco --version >/dev/null 2>&1; then
                echo -e "${GREEN}✓${NC} Executable runs successfully"
            else
                echo -e "${YELLOW}⚠${NC} Executable exists but may have issues"
            fi
        else
            echo -e "${YELLOW}⚠${NC} Main executable not found (run 'make' first)"
        fi
    else
        echo -e "${YELLOW}⚠${NC} Build directories not found (run 'make' first)"
    fi
}

# Function to check source code statistics
check_source_stats() {
    echo -e "\n${BLUE}Source Code Statistics${NC}"
    echo "----------------------"
    
    local c_files=$(count_files "src" "*.c")
    local h_files=$(count_files "include" "*.h")
    local total_lines=0
    
    echo "  - C source files: $c_files"
    echo "  - Header files: $h_files"
    
    # Count lines of code
    if [ -d "src" ]; then
        total_lines=$(find src -name "*.c" -exec wc -l {} + | tail -1 | awk '{print $1}')
        echo "  - Total lines of C code: $total_lines"
    fi
    
    if [ -d "include" ]; then
        local header_lines=$(find include -name "*.h" -exec wc -l {} + | tail -1 | awk '{print $1}')
        echo "  - Total lines of headers: $header_lines"
    fi
}

# Function to check dependencies
check_dependencies() {
    echo -e "\n${BLUE}Dependency Check${NC}"
    echo "------------------"
    
    local missing_deps=0
    
    # Check for required tools
    local tools=("gcc" "make" "git")
    
    for tool in "${tools[@]}"; do
        if command -v "$tool" >/dev/null 2>&1; then
            local version=$("$tool" --version 2>/dev/null | head -1 | cut -d' ' -f2- | cut -d' ' -f1)
            echo -e "${GREEN}✓${NC} $tool: $version"
        else
            echo -e "${RED}✗${NC} $tool: Not found"
            ((missing_deps++))
        fi
    done
    
    # Check for optional development tools
    local dev_tools=("clang" "valgrind" "cppcheck" "doxygen")
    
    echo -e "\nOptional development tools:"
    for tool in "${dev_tools[@]}"; do
        if command -v "$tool" >/dev/null 2>&1; then
            local version=$("$tool" --version 2>/dev/null | head -1 | cut -d' ' -f2- | cut -d' ' -f1)
            echo -e "${GREEN}✓${NC} $tool: $version"
        else
            echo -e "${YELLOW}⚠${NC} $tool: Not found (run 'make dev-setup' to install)"
        fi
    done
    
    if [ $missing_deps -gt 0 ]; then
        echo -e "\n${RED}Missing required dependencies: $missing_deps${NC}"
        return 1
    fi
}

# Function to check git status
check_git_status() {
    echo -e "\n${BLUE}Git Repository Status${NC}"
    echo "----------------------"
    
    if [ -d ".git" ]; then
        local branch=$(git branch --show-current 2>/dev/null || echo "unknown")
        local commit=$(git rev-parse --short HEAD 2>/dev/null || echo "unknown")
        local status=$(git status --porcelain | wc -l)
        
        echo -e "${GREEN}✓${NC} Git repository found"
        echo "  - Branch: $branch"
        echo "  - Commit: $commit"
        echo "  - Uncommitted changes: $status"
        
        if [ $status -gt 0 ]; then
            echo -e "${YELLOW}⚠${NC} You have uncommitted changes"
        fi
    else
        echo -e "${YELLOW}⚠${NC} Not a git repository"
    fi
}

# Function to run basic tests
run_basic_tests() {
    echo -e "\n${BLUE}Basic Functionality Tests${NC}"
    echo "---------------------------"
    
    local test_count=0
    local passed=0
    
    # Test 1: Check if we can compile
    ((test_count++))
    echo -n "Test $test_count: Compilation... "
    if make clean >/dev/null 2>&1 && make >/dev/null 2>&1; then
        echo -e "${GREEN}PASSED${NC}"
        ((passed++))
    else
        echo -e "${RED}FAILED${NC}"
    fi
    
    # Test 2: Check if executable works
    ((test_count++))
    echo -n "Test $test_count: Executable functionality... "
    if [ -f "bin/myco" ] && ./bin/myco --version >/dev/null 2>&1; then
        echo -e "${GREEN}PASSED${NC}"
        ((passed++))
    else
        echo -e "${RED}FAILED${NC}"
    fi
    
    # Test 3: Check help output
    ((test_count++))
    echo -n "Test $test_count: Help functionality... "
    if [ -f "bin/myco" ] && ./bin/myco --help >/dev/null 2>&1; then
        echo -e "${GREEN}PASSED${NC}"
        ((passed++))
    else
        echo -e "${RED}FAILED${NC}"
    fi
    
    echo -e "\nTest Results: $passed/$test_count tests passed"
    
    if [ $passed -eq $test_count ]; then
        echo -e "${GREEN}All basic tests passed!${NC}"
    else
        echo -e "${RED}Some tests failed. Check the output above.${NC}"
        return 1
    fi
}

# Main execution
main() {
    cd "$PROJECT_ROOT"
    
    local errors=0
    
    echo -e "\n${BLUE}Project Structure Check${NC}"
    echo "------------------------"
    
    # Check required directories
    check_directory "src" "Source code directory" || ((errors++))
    check_directory "include" "Header files directory" || ((errors++))
    check_directory "tests" "Test files directory" || ((errors++))
    check_directory "docs" "Documentation directory" || ((errors++))
    check_directory "examples" "Example programs directory" || ((errors++))
    check_directory "tools" "Development tools directory" || ((errors++))
    
    echo -e "\n${BLUE}Required Files Check${NC}"
    echo "---------------------"
    
    # Check required files
    check_file "Makefile" "Build system" || ((errors++))
    check_file "README.md" "Project documentation" || ((errors++))
    check_file "include/myco.h" "Main header file" || ((errors++))
    check_file "src/main.c" "Main source file" || ((errors++))
    
    # Check source files
    check_file "src/lexer.c" "Lexer implementation" || ((errors++))
    check_file "src/ast.c" "AST implementation" || ((errors++))
    check_file "include/lexer.h" "Lexer header" || ((errors++))
    check_file "include/ast.h" "AST header" || ((errors++))
    
    # Check dependencies
    check_dependencies || ((errors++))
    
    # Check git status
    check_git_status
    
    # Check source statistics
    check_source_stats
    
    # Check build status
    check_build
    
    # Run basic tests if requested
    if [ "$1" = "--test" ]; then
        run_basic_tests || ((errors++))
    fi
    
    echo -e "\n${BLUE}Summary${NC}"
    echo "-------"
    
    if [ $errors -eq 0 ]; then
        echo -e "${GREEN}✓ Project structure is healthy!${NC}"
        echo -e "You can now run 'make' to build the project."
    else
        echo -e "${RED}✗ Found $errors issue(s) with the project structure.${NC}"
        echo -e "Please fix the issues above before proceeding."
    fi
    
    echo -e "\nNext steps:"
    echo "1. Run 'make' to build the project"
    echo "2. Run 'make test' to run tests"
    echo "3. Run './bin/myco --help' to see available options"
    echo "4. Check docs/DEVELOPMENT_PLAN.md for development roadmap"
    
    exit $errors
}

# Run main function with all arguments
main "$@"
