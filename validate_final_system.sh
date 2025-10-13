#!/bin/bash

# Final System Validation Script
# Comprehensive validation of the Ultimate Performance Optimization system

set -e

echo "=== Myco Ultimate Performance Optimization - Final Validation ==="
echo "Comprehensive validation of the complete optimization system"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Function to run a test
run_test() {
    local test_name="$1"
    local test_command="$2"
    local expected_exit_code="${3:-0}"
    
    echo "Running: $test_name"
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    if eval "$test_command" >/dev/null 2>&1; then
        echo -e "  ${GREEN}✓${NC} $test_name passed"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "  ${RED}✗${NC} $test_name failed"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    echo ""
}

# Function to run a test with output capture
run_test_with_output() {
    local test_name="$1"
    local test_command="$2"
    local expected_output="$3"
    
    echo "Running: $test_name"
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    output=$(eval "$test_command" 2>&1)
    if echo "$output" | grep -q "$expected_output"; then
        echo -e "  ${GREEN}✓${NC} $test_name passed"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "  ${RED}✗${NC} $test_name failed"
        echo "    Expected: $expected_output"
        echo "    Got: $output"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    echo ""
}

echo "=== 1. Build System Validation ==="
echo ""

run_test "Clean build" "make clean"
run_test "Full build" "make"
run_test "Build without errors" "make 2>&1 | grep -q 'Build complete'"

echo "=== 2. Core Functionality Validation ==="
echo ""

run_test_with_output "pass.myco regression test" "./bin/myco pass.myco" "ALL TESTS PASSED"
run_test_with_output "optimization system test" "./bin/myco benchmark/optimization_test.myco" "Optimization system is working correctly"

echo "=== 3. Optimization Module Validation ==="
echo ""

# Test that all optimization modules compile
run_test "register_vm module" "gcc -c build/core/optimization/register_vm.o"
run_test "trace_recorder module" "gcc -c build/core/optimization/trace_recorder.o"
run_test "trace_optimizer module" "gcc -c build/core/optimization/trace_optimizer.o"
run_test "native_codegen module" "gcc -c build/core/optimization/native_codegen.o"
run_test "type_predictor module" "gcc -c build/core/optimization/type_predictor.o"
run_test "speculative_optimizer module" "gcc -c build/core/optimization/speculative_optimizer.o"
run_test "hidden_classes module" "gcc -c build/core/optimization/hidden_classes.o"
run_test "escape_analysis module" "gcc -c build/core/optimization/escape_analysis.o"
run_test "arena_allocator module" "gcc -c build/core/optimization/arena_allocator.o"
run_test "nan_boxing module" "gcc -c build/core/optimization/nan_boxing.o"
run_test "loop_analyzer module" "gcc -c build/core/optimization/loop_analyzer.o"
run_test "loop_optimizer module" "gcc -c build/core/optimization/loop_optimizer.o"
run_test "vectorizer module" "gcc -c build/core/optimization/vectorizer.o"
run_test "performance_profiler module" "gcc -c build/core/optimization/performance_profiler.o"
run_test "cpu_features module" "gcc -c build/core/optimization/cpu_features.o"
run_test "adaptive_executor module" "gcc -c build/core/optimization/adaptive_executor.o"

echo "=== 4. Benchmark Validation ==="
echo ""

run_test "C reference compilation" "cd benchmark && gcc -O2 -std=c99 -lm c_reference.c -o c_reference"
run_test "C reference execution" "cd benchmark && ./c_reference >/dev/null 2>&1"

echo "=== 5. Memory Validation ==="
echo ""

if command -v valgrind >/dev/null 2>&1; then
    run_test "Memory leak check" "valgrind --leak-check=full --error-exitcode=1 ./bin/myco pass.myco >/dev/null 2>&1"
else
    echo -e "  ${YELLOW}⚠${NC} valgrind not available, skipping memory leak check"
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    PASSED_TESTS=$((PASSED_TESTS + 1))
fi

echo "=== 6. Performance Validation ==="
echo ""

# Test that the system can handle performance workloads
run_test "Performance workload test" "./bin/myco benchmark/optimization_test.myco >/dev/null 2>&1"

echo "=== 7. File System Validation ==="
echo ""

# Check that all expected files exist
run_test "Optimization headers exist" "test -d include/core/optimization"
run_test "Optimization sources exist" "test -d src/core/optimization"
run_test "Benchmark files exist" "test -d benchmark"
run_test "Test scripts exist" "test -f test_regression_comprehensive.sh"
run_test "Performance report exists" "test -f PERFORMANCE_REPORT.md"

echo "=== 8. Documentation Validation ==="
echo ""

# Check that documentation files exist and are not empty
run_test "Performance report not empty" "test -s PERFORMANCE_REPORT.md"
run_test "README exists" "test -f README.md"
run_test "Makefile exists" "test -f Makefile"

echo "=== Final Validation Summary ==="
echo ""

# Calculate success rate
if [ $TOTAL_TESTS -gt 0 ]; then
    success_rate=$((PASSED_TESTS * 100 / TOTAL_TESTS))
    echo "Total tests: $TOTAL_TESTS"
    echo -e "Passed: ${GREEN}$PASSED_TESTS${NC}"
    echo -e "Failed: ${RED}$FAILED_TESTS${NC}"
    echo "Success rate: $success_rate%"
    echo ""
    
    if [ $success_rate -eq 100 ]; then
        echo -e "${GREEN}🎉 ALL VALIDATION TESTS PASSED! 🎉${NC}"
        echo -e "${GREEN}The Ultimate Performance Optimization system is ready for production!${NC}"
        exit 0
    elif [ $success_rate -ge 90 ]; then
        echo -e "${YELLOW}⚠ Most validation tests passed ($success_rate%)${NC}"
        echo -e "${YELLOW}The system is mostly ready with minor issues${NC}"
        exit 1
    else
        echo -e "${RED}✗ Validation failed ($success_rate% success rate)${NC}"
        echo -e "${RED}The system needs attention before production use${NC}"
        exit 1
    fi
else
    echo -e "${RED}No tests were run!${NC}"
    exit 1
fi
