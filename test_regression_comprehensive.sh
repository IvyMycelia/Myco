#!/bin/bash

# Comprehensive Regression Testing Script
# Tests all optimization tiers and verifies correctness

set -e

echo "=== Myco Comprehensive Regression Testing ==="
echo "Testing all optimization tiers and verifying correctness"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test configuration
MYCO_BIN="./bin/myco"
TEST_FILES=(
    "pass.myco"
    "benchmark/ultimate_performance.myco"
    "benchmark/micro/tier_transitions.myco"
    "benchmark/micro/guard_overhead.myco"
    "benchmark/micro/deoptimization_cost.myco"
)

# Performance thresholds
PERFORMANCE_THRESHOLD_MS=1000  # Maximum acceptable execution time in ms
MEMORY_THRESHOLD_MB=100        # Maximum acceptable memory usage in MB

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
PERFORMANCE_FAILURES=0
MEMORY_FAILURES=0

# Function to run a test
run_test() {
    local test_file="$1"
    local test_name="$2"
    local expected_exit_code="${3:-0}"
    
    echo "Running test: $test_name"
    echo "File: $test_file"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    # Measure execution time and memory usage
    start_time=$(date +%s%3N)
    
    if command -v valgrind >/dev/null 2>&1; then
        # Run with valgrind for memory checking
        valgrind_output=$(valgrind --leak-check=full --error-exitcode=1 $MYCO_BIN "$test_file" 2>&1)
        exit_code=$?
        memory_usage=$(echo "$valgrind_output" | grep "total heap usage" | sed 's/.*total heap usage: \([0-9,]*\) allocs.*/\1/' | tr -d ',')
    else
        # Run without valgrind
        $MYCO_BIN "$test_file" >/dev/null 2>&1
        exit_code=$?
        memory_usage="unknown"
    fi
    
    end_time=$(date +%s%3N)
    duration_ms=$((end_time - start_time))
    
    # Check exit code
    if [ $exit_code -eq $expected_exit_code ]; then
        echo -e "  ${GREEN}✓${NC} Exit code: $exit_code (expected: $expected_exit_code)"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "  ${RED}✗${NC} Exit code: $exit_code (expected: $expected_exit_code)"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    
    # Check performance
    if [ $duration_ms -gt $PERFORMANCE_THRESHOLD_MS ]; then
        echo -e "  ${YELLOW}⚠${NC} Performance: ${duration_ms}ms (threshold: ${PERFORMANCE_THRESHOLD_MS}ms)"
        PERFORMANCE_FAILURES=$((PERFORMANCE_FAILURES + 1))
    else
        echo -e "  ${GREEN}✓${NC} Performance: ${duration_ms}ms (threshold: ${PERFORMANCE_THRESHOLD_MS}ms)"
    fi
    
    # Check memory usage
    if [ "$memory_usage" != "unknown" ] && [ $memory_usage -gt $MEMORY_THRESHOLD_MB ]; then
        echo -e "  ${YELLOW}⚠${NC} Memory: ${memory_usage}MB (threshold: ${MEMORY_THRESHOLD_MB}MB)"
        MEMORY_FAILURES=$((MEMORY_FAILURES + 1))
    else
        echo -e "  ${GREEN}✓${NC} Memory: ${memory_usage}MB (threshold: ${MEMORY_THRESHOLD_MB}MB)"
    fi
    
    echo ""
}

# Function to test optimization tiers
test_optimization_tiers() {
    echo "=== Testing Optimization Tiers ==="
    echo ""
    
    # Test with different optimization levels
    for tier in "ast" "bytecode" "jit" "specialized" "vectorized"; do
        echo "Testing tier: $tier"
        
        # Run pass.myco with specific tier
        if $MYCO_BIN --tier="$tier" pass.myco >/dev/null 2>&1; then
            echo -e "  ${GREEN}✓${NC} Tier $tier working"
        else
            echo -e "  ${RED}✗${NC} Tier $tier failed"
            FAILED_TESTS=$((FAILED_TESTS + 1))
        fi
        TOTAL_TESTS=$((TOTAL_TESTS + 1))
    done
    
    echo ""
}

# Function to test dual execution
test_dual_execution() {
    echo "=== Testing Dual Execution (AST vs JIT) ==="
    echo ""
    
    # Run AST version
    echo "Running AST version..."
    $MYCO_BIN --tier=ast pass.myco > ast_output.txt 2>&1
    ast_exit_code=$?
    
    # Run JIT version
    echo "Running JIT version..."
    $MYCO_BIN --tier=jit pass.myco > jit_output.txt 2>&1
    jit_exit_code=$?
    
    # Compare outputs
    if [ $ast_exit_code -eq $jit_exit_code ]; then
        if diff ast_output.txt jit_output.txt >/dev/null 2>&1; then
            echo -e "  ${GREEN}✓${NC} AST and JIT outputs match"
            PASSED_TESTS=$((PASSED_TESTS + 1))
        else
            echo -e "  ${RED}✗${NC} AST and JIT outputs differ"
            FAILED_TESTS=$((FAILED_TESTS + 1))
        fi
    else
        echo -e "  ${RED}✗${NC} AST and JIT exit codes differ ($ast_exit_code vs $jit_exit_code)"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    # Cleanup
    rm -f ast_output.txt jit_output.txt
    
    echo ""
}

# Function to run performance benchmarks
run_performance_benchmarks() {
    echo "=== Running Performance Benchmarks ==="
    echo ""
    
    # Run ultimate performance benchmark
    run_test "benchmark/ultimate_performance.myco" "Ultimate Performance Benchmark"
    
    # Run micro-benchmarks
    for micro_test in benchmark/micro/*.myco; do
        if [ -f "$micro_test" ]; then
            test_name=$(basename "$micro_test" .myco)
            run_test "$micro_test" "Micro-benchmark: $test_name"
        fi
    done
}

# Main test execution
echo "Starting comprehensive regression testing..."
echo ""

# Test basic functionality
echo "=== Basic Functionality Tests ==="
echo ""

for test_file in "${TEST_FILES[@]}"; do
    if [ -f "$test_file" ]; then
        test_name=$(basename "$test_file" .myco)
        run_test "$test_file" "$test_name"
    else
        echo -e "${YELLOW}Warning:${NC} Test file $test_file not found"
    fi
done

# Test optimization tiers
test_optimization_tiers

# Test dual execution
test_dual_execution

# Run performance benchmarks
run_performance_benchmarks

# Summary
echo "=== Test Summary ==="
echo "Total tests: $TOTAL_TESTS"
echo -e "Passed: ${GREEN}$PASSED_TESTS${NC}"
echo -e "Failed: ${RED}$FAILED_TESTS${NC}"
echo -e "Performance failures: ${YELLOW}$PERFORMANCE_FAILURES${NC}"
echo -e "Memory failures: ${YELLOW}$MEMORY_FAILURES${NC}"
echo ""

# Calculate success rate
if [ $TOTAL_TESTS -gt 0 ]; then
    success_rate=$((PASSED_TESTS * 100 / TOTAL_TESTS))
    echo "Success rate: $success_rate%"
    
    if [ $success_rate -eq 100 ]; then
        echo -e "${GREEN}All tests passed!${NC}"
        exit 0
    else
        echo -e "${RED}Some tests failed!${NC}"
        exit 1
    fi
else
    echo -e "${RED}No tests were run!${NC}"
    exit 1
fi
