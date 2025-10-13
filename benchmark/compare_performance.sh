#!/bin/bash

# Performance Comparison Script
# Compares Myco performance against C reference implementation

set -e

echo "=== Myco vs C Performance Comparison ==="
echo "Comparing Myco optimization system against C reference implementation"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
MYCO_BIN="../bin/myco"
C_REFERENCE="c_reference"
C_SOURCE="c_reference.c"

# Compile C reference if needed
if [ ! -f "$C_REFERENCE" ] || [ "$C_SOURCE" -nt "$C_REFERENCE" ]; then
    echo "Compiling C reference implementation..."
    gcc -O2 -std=c99 -lm "$C_SOURCE" -o "$C_REFERENCE"
    echo -e "${GREEN}✓${NC} C reference compiled successfully"
    echo ""
fi

# Function to run benchmark and extract timing
run_benchmark() {
    local name="$1"
    local command="$2"
    
    echo "Running $name..."
    
    # Run the benchmark and capture output
    output=$($command 2>&1)
    
    # Extract timing information (look for "Duration:" lines)
    duration=$(echo "$output" | grep "Duration:" | head -1 | sed 's/.*Duration: \([0-9.]*\) ms.*/\1/')
    
    if [ -n "$duration" ]; then
        echo "  Duration: ${duration}ms"
        echo "$duration"
    else
        echo "  Could not extract timing information"
        echo "0"
    fi
}

# Function to compare performance
compare_performance() {
    local myco_time="$1"
    local c_time="$2"
    local test_name="$3"
    
    if [ "$c_time" = "0" ] || [ "$myco_time" = "0" ]; then
        echo -e "  ${YELLOW}⚠${NC} Could not compare $test_name (missing timing data)"
        return
    fi
    
    # Calculate speedup/slowdown
    ratio=$(echo "scale=2; $myco_time / $c_time" | bc -l)
    
    if (( $(echo "$ratio < 2.0" | bc -l) )); then
        echo -e "  ${GREEN}✓${NC} $test_name: ${ratio}x slower than C (excellent!)"
    elif (( $(echo "$ratio < 5.0" | bc -l) )); then
        echo -e "  ${YELLOW}⚠${NC} $test_name: ${ratio}x slower than C (good)"
    else
        echo -e "  ${RED}✗${NC} $test_name: ${ratio}x slower than C (needs improvement)"
    fi
}

# Run Myco benchmarks
echo "=== Running Myco Benchmarks ==="
echo ""

myco_arithmetic=$(run_benchmark "Myco Arithmetic" "$MYCO_BIN benchmark/ultimate_performance.myco")
myco_float=$(run_benchmark "Myco Float Arithmetic" "$MYCO_BIN benchmark/ultimate_performance.myco")
myco_arrays=$(run_benchmark "Myco Array Operations" "$MYCO_BIN benchmark/ultimate_performance.myco")
myco_strings=$(run_benchmark "Myco String Operations" "$MYCO_BIN benchmark/ultimate_performance.myco")
myco_functions=$(run_benchmark "Myco Function Calls" "$MYCO_BIN benchmark/ultimate_performance.myco")

echo ""

# Run C benchmarks
echo "=== Running C Reference Benchmarks ==="
echo ""

c_arithmetic=$(run_benchmark "C Arithmetic" "./$C_REFERENCE")
c_float=$(run_benchmark "C Float Arithmetic" "./$C_REFERENCE")
c_arrays=$(run_benchmark "C Array Operations" "./$C_REFERENCE")
c_strings=$(run_benchmark "C String Operations" "./$C_REFERENCE")
c_functions=$(run_benchmark "C Function Calls" "./$C_REFERENCE")

echo ""

# Compare performance
echo "=== Performance Comparison ==="
echo ""

compare_performance "$myco_arithmetic" "$c_arithmetic" "Arithmetic Operations"
compare_performance "$myco_float" "$c_float" "Float Arithmetic"
compare_performance "$myco_arrays" "$c_arrays" "Array Operations"
compare_performance "$myco_strings" "$c_strings" "String Operations"
compare_performance "$myco_functions" "$c_functions" "Function Calls"

echo ""

# Summary
echo "=== Summary ==="
echo "Myco optimization system performance comparison complete."
echo "Target: 1.5-2x slower than C (beating LuaJIT's 1.5-3x)"
echo ""

# Cleanup
rm -f "$C_REFERENCE"

echo "Performance comparison complete!"
