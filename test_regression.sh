#!/bin/bash

# Comprehensive Regression Testing Script
# Tests that optimization produces identical results to AST interpreter

set -e

echo "=== COMPREHENSIVE REGRESSION TESTING ==="
echo "Testing that optimization produces identical results to AST interpreter"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test files
TEST_FILES=(
    "pass.myco"
    "test_dual_execution.myco"
    "benchmark/performance_suite.myco"
    "benchmark/basic_arithmetic.myco"
)

# Results tracking
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Function to run a test
run_test() {
    local test_file="$1"
    local test_name="$2"
    
    echo "Testing: $test_name"
    echo "File: $test_file"
    echo "----------------------------------------"
    
    # Run with optimization enabled (default)
    echo "Running with optimization enabled..."
    ./bin/myco "$test_file" > "test_output_optimized.txt" 2>&1
    local optimized_exit_code=$?
    
    # Run with optimization disabled
    echo "Running with optimization disabled..."
    MYCO_OPTIMIZE=0 ./bin/myco "$test_file" > "test_output_unoptimized.txt" 2>&1
    local unoptimized_exit_code=$?
    
    # Compare exit codes
    if [ $optimized_exit_code -ne $unoptimized_exit_code ]; then
        echo -e "${RED}FAILED: Exit codes differ${NC}"
        echo "Optimized exit code: $optimized_exit_code"
        echo "Unoptimized exit code: $unoptimized_exit_code"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return 1
    fi
    
    # Compare outputs
    if diff -q "test_output_optimized.txt" "test_output_unoptimized.txt" > /dev/null; then
        echo -e "${GREEN}PASSED: Outputs are identical${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}FAILED: Outputs differ${NC}"
        echo "Differences:"
        diff "test_output_optimized.txt" "test_output_unoptimized.txt" || true
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return 1
    fi
    
    echo ""
    return 0
}

# Function to run performance test
run_performance_test() {
    local test_file="$1"
    local test_name="$2"
    
    echo "Performance Test: $test_name"
    echo "File: $test_file"
    echo "----------------------------------------"
    
    # Run with optimization enabled
    echo "Running with optimization enabled..."
    local start_time=$(date +%s%3N)
    ./bin/myco "$test_file" > "perf_output_optimized.txt" 2>&1
    local optimized_exit_code=$?
    local end_time=$(date +%s%3N)
    local optimized_time=$((end_time - start_time))
    
    # Run with optimization disabled
    echo "Running with optimization disabled..."
    start_time=$(date +%s%3N)
    MYCO_OPTIMIZE=0 ./bin/myco "$test_file" > "perf_output_unoptimized.txt" 2>&1
    unoptimized_exit_code=$?
    end_time=$(date +%s%3N)
    local unoptimized_time=$((end_time - start_time))
    
    # Calculate speedup
    local speedup=$(echo "scale=2; $unoptimized_time / $optimized_time" | bc -l 2>/dev/null || echo "N/A")
    
    echo "Optimized time: ${optimized_time}ms"
    echo "Unoptimized time: ${unoptimized_time}ms"
    echo "Speedup: ${speedup}x"
    
    # Check if outputs are identical
    if diff -q "perf_output_optimized.txt" "perf_output_unoptimized.txt" > /dev/null; then
        echo -e "${GREEN}PASSED: Performance test outputs are identical${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}FAILED: Performance test outputs differ${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    
    echo ""
}

# Main test execution
echo "Starting regression tests..."
echo ""

# Run functional tests
for test_file in "${TEST_FILES[@]}"; do
    if [ -f "$test_file" ]; then
        TOTAL_TESTS=$((TOTAL_TESTS + 1))
        test_name=$(basename "$test_file" .myco)
        run_test "$test_file" "$test_name"
    else
        echo -e "${YELLOW}WARNING: Test file $test_file not found${NC}"
    fi
done

# Run performance tests
echo "=== PERFORMANCE TESTS ==="
echo ""

PERF_FILES=(
    "benchmark/performance_suite.myco"
    "benchmark/basic_arithmetic.myco"
)

for test_file in "${PERF_FILES[@]}"; do
    if [ -f "$test_file" ]; then
        TOTAL_TESTS=$((TOTAL_TESTS + 1))
        test_name=$(basename "$test_file" .myco)
        run_performance_test "$test_file" "$test_name"
    else
        echo -e "${YELLOW}WARNING: Performance test file $test_file not found${NC}"
    fi
done

# Cleanup
rm -f test_output_optimized.txt test_output_unoptimized.txt
rm -f perf_output_optimized.txt perf_output_unoptimized.txt

# Summary
echo "=== REGRESSION TEST SUMMARY ==="
echo "Total tests: $TOTAL_TESTS"
echo -e "Passed: ${GREEN}$PASSED_TESTS${NC}"
echo -e "Failed: ${RED}$FAILED_TESTS${NC}"

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}ALL TESTS PASSED! No regressions detected.${NC}"
    exit 0
else
    echo -e "${RED}REGRESSIONS DETECTED! Please fix before proceeding.${NC}"
    exit 1
fi
