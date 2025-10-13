#!/bin/bash

# Myco Optimization Benchmark Runner
# Runs all benchmarks and compares with C baseline

set -e

BENCHMARK_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MYCO_BIN="$BENCHMARK_DIR/../bin/myco"
C_BINARY="$BENCHMARK_DIR/compare_c"
RESULTS_DIR="$BENCHMARK_DIR/results"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

# Create results directory
mkdir -p "$RESULTS_DIR"

echo "Myco Optimization Benchmark Suite"
echo "================================="
echo "Timestamp: $TIMESTAMP"
echo "Results directory: $RESULTS_DIR"
echo ""

# Check if Myco binary exists
if [ ! -f "$MYCO_BIN" ]; then
    echo "Error: Myco binary not found at $MYCO_BIN"
    echo "Please build Myco first with 'make'"
    exit 1
fi

# Compile C benchmark
echo "Compiling C benchmark..."
gcc -O2 -o "$C_BINARY" "$BENCHMARK_DIR/compare_c.c" -lm
if [ $? -ne 0 ]; then
    echo "Error: Failed to compile C benchmark"
    exit 1
fi

# Function to run a benchmark and capture output
run_benchmark() {
    local name="$1"
    local file="$2"
    local output_file="$RESULTS_DIR/${name}_${TIMESTAMP}.txt"
    
    echo "Running $name benchmark..."
    echo "Output: $output_file"
    
    if [ -f "$file" ]; then
        "$MYCO_BIN" "$file" > "$output_file" 2>&1
        local exit_code=$?
        
        if [ $exit_code -eq 0 ]; then
            echo "✓ $name completed successfully"
        else
            echo "✗ $name failed with exit code $exit_code"
            echo "Check $output_file for details"
        fi
    else
        echo "✗ $name: File not found: $file"
    fi
    echo ""
}

# Function to extract timing information
extract_timing() {
    local file="$1"
    local pattern="$2"
    
    if [ -f "$file" ]; then
        grep "$pattern" "$file" | tail -1 | sed 's/.*: *//'
    else
        echo "N/A"
    fi
}

# Run all Myco benchmarks
echo "Running Myco benchmarks..."
echo "========================="

run_benchmark "arithmetic" "$BENCHMARK_DIR/arithmetic.myco"
run_benchmark "loops" "$BENCHMARK_DIR/loops.myco"
run_benchmark "functions" "$BENCHMARK_DIR/functions.myco"
run_benchmark "arrays" "$BENCHMARK_DIR/arrays.myco"
run_benchmark "objects" "$BENCHMARK_DIR/objects.myco"
run_benchmark "mixed" "$BENCHMARK_DIR/mixed.myco"

# Run C benchmark
echo "Running C benchmark..."
echo "===================="
C_OUTPUT="$RESULTS_DIR/c_baseline_${TIMESTAMP}.txt"
"$C_BINARY" > "$C_OUTPUT" 2>&1
if [ $? -eq 0 ]; then
    echo "✓ C benchmark completed successfully"
else
    echo "✗ C benchmark failed"
fi
echo ""

# Generate comparison report
REPORT_FILE="$RESULTS_DIR/performance_report_${TIMESTAMP}.txt"
echo "Generating performance report..."
echo "Report: $REPORT_FILE"

cat > "$REPORT_FILE" << EOF
Myco Optimization Performance Report
===================================
Generated: $(date)
Timestamp: $TIMESTAMP

BENCHMARK RESULTS SUMMARY
========================

EOF

# Extract and compare timing data
echo "Extracting timing data..."

# Arithmetic benchmark comparison
ARITH_DURATION=$(extract_timing "$RESULTS_DIR/arithmetic_${TIMESTAMP}.txt" "Duration")
ARITH_OPS=$(extract_timing "$RESULTS_DIR/arithmetic_${TIMESTAMP}.txt" "Operations per second")
C_ARITH_DURATION=$(extract_timing "$C_OUTPUT" "Duration")
C_ARITH_OPS=$(extract_timing "$C_OUTPUT" "Operations per second")

cat >> "$REPORT_FILE" << EOF
Arithmetic Benchmark:
- Myco Duration: $ARITH_DURATION seconds
- Myco Ops/sec: $ARITH_OPS
- C Duration: $C_ARITH_DURATION seconds  
- C Ops/sec: $C_ARITH_OPS
- Performance Ratio: $(echo "scale=2; $C_ARITH_OPS / $ARITH_OPS" | bc 2>/dev/null || echo "N/A")

EOF

# Loop benchmark comparison
LOOP_DURATION=$(extract_timing "$RESULTS_DIR/loops_${TIMESTAMP}.txt" "Duration")
LOOP_OPS=$(extract_timing "$RESULTS_DIR/loops_${TIMESTAMP}.txt" "Loop iterations per second")
C_LOOP_DURATION=$(extract_timing "$C_OUTPUT" "Duration")
C_LOOP_OPS=$(extract_timing "$C_OUTPUT" "Loop iterations per second")

cat >> "$REPORT_FILE" << EOF
Loop Benchmark:
- Myco Duration: $LOOP_DURATION seconds
- Myco Ops/sec: $LOOP_OPS
- C Duration: $C_LOOP_DURATION seconds
- C Ops/sec: $C_LOOP_OPS
- Performance Ratio: $(echo "scale=2; $C_LOOP_OPS / $LOOP_OPS" | bc 2>/dev/null || echo "N/A")

EOF

# Function benchmark comparison
FUNC_DURATION=$(extract_timing "$RESULTS_DIR/functions_${TIMESTAMP}.txt" "Duration")
FUNC_OPS=$(extract_timing "$RESULTS_DIR/functions_${TIMESTAMP}.txt" "Function calls per second")
C_FUNC_DURATION=$(extract_timing "$C_OUTPUT" "Duration")
C_FUNC_OPS=$(extract_timing "$C_OUTPUT" "Function calls per second")

cat >> "$REPORT_FILE" << EOF
Function Call Benchmark:
- Myco Duration: $FUNC_DURATION seconds
- Myco Ops/sec: $FUNC_OPS
- C Duration: $C_FUNC_DURATION seconds
- C Ops/sec: $C_FUNC_OPS
- Performance Ratio: $(echo "scale=2; $C_FUNC_OPS / $FUNC_OPS" | bc 2>/dev/null || echo "N/A")

EOF

# Array benchmark comparison
ARRAY_DURATION=$(extract_timing "$RESULTS_DIR/arrays_${TIMESTAMP}.txt" "Duration")
ARRAY_OPS=$(extract_timing "$RESULTS_DIR/arrays_${TIMESTAMP}.txt" "Array operations per second")
C_ARRAY_DURATION=$(extract_timing "$C_OUTPUT" "Duration")
C_ARRAY_OPS=$(extract_timing "$C_OUTPUT" "Array operations per second")

cat >> "$REPORT_FILE" << EOF
Array Benchmark:
- Myco Duration: $ARRAY_DURATION seconds
- Myco Ops/sec: $ARRAY_OPS
- C Duration: $C_ARRAY_DURATION seconds
- C Ops/sec: $C_ARRAY_OPS
- Performance Ratio: $(echo "scale=2; $C_ARRAY_OPS / $ARRAY_OPS" | bc 2>/dev/null || echo "N/A")

EOF

# Object benchmark
OBJ_DURATION=$(extract_timing "$RESULTS_DIR/objects_${TIMESTAMP}.txt" "Duration")
OBJ_OPS=$(extract_timing "$RESULTS_DIR/objects_${TIMESTAMP}.txt" "Object operations per second")

cat >> "$REPORT_FILE" << EOF
Object Benchmark:
- Myco Duration: $OBJ_DURATION seconds
- Myco Ops/sec: $OBJ_OPS
- C equivalent: N/A (no direct comparison)

EOF

# Mixed workload benchmark
MIXED_DURATION=$(extract_timing "$RESULTS_DIR/mixed_${TIMESTAMP}.txt" "Duration")
MIXED_OPS=$(extract_timing "$RESULTS_DIR/mixed_${TIMESTAMP}.txt" "Operations per second")

cat >> "$REPORT_FILE" << EOF
Mixed Workload Benchmark:
- Myco Duration: $MIXED_DURATION seconds
- Myco Ops/sec: $MIXED_OPS
- C equivalent: N/A (no direct comparison)

EOF

cat >> "$REPORT_FILE" << EOF

DETAILED BENCHMARK OUTPUTS
==========================

All benchmark outputs are saved in: $RESULTS_DIR/

Files:
- arithmetic_${TIMESTAMP}.txt
- loops_${TIMESTAMP}.txt  
- functions_${TIMESTAMP}.txt
- arrays_${TIMESTAMP}.txt
- objects_${TIMESTAMP}.txt
- mixed_${TIMESTAMP}.txt
- c_baseline_${TIMESTAMP}.txt

EOF

echo "Performance report generated: $REPORT_FILE"
echo ""

# Display summary
echo "QUICK SUMMARY"
echo "============="
echo "Arithmetic Performance Ratio: $(echo "scale=2; $C_ARITH_OPS / $ARITH_OPS" | bc 2>/dev/null || echo "N/A")x slower than C"
echo "Loop Performance Ratio: $(echo "scale=2; $C_LOOP_OPS / $LOOP_OPS" | bc 2>/dev/null || echo "N/A")x slower than C"
echo "Function Performance Ratio: $(echo "scale=2; $C_FUNC_OPS / $FUNC_OPS" | bc 2>/dev/null || echo "N/A")x slower than C"
echo "Array Performance Ratio: $(echo "scale=2; $C_ARRAY_OPS / $ARRAY_OPS" | bc 2>/dev/null || echo "N/A")x slower than C"
echo ""

# Save baseline for future comparison
BASELINE_FILE="$BENCHMARK_DIR/baseline.txt"
cp "$REPORT_FILE" "$BASELINE_FILE"
echo "Baseline saved to: $BASELINE_FILE"

# Clean up
rm -f "$C_BINARY"

echo "Benchmark suite completed successfully!"
echo "Check $REPORT_FILE for detailed results."
