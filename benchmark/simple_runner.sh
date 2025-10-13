#!/bin/bash

# Simple Myco Benchmark Runner
# Runs basic benchmarks to establish baseline

set -e

BENCHMARK_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MYCO_BIN="$BENCHMARK_DIR/../bin/myco"
RESULTS_DIR="$BENCHMARK_DIR/results"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

# Create results directory
mkdir -p "$RESULTS_DIR"

echo "Simple Myco Benchmark Suite"
echo "=========================="
echo "Timestamp: $TIMESTAMP"
echo "Results directory: $RESULTS_DIR"
echo ""

# Check if Myco binary exists
if [ ! -f "$MYCO_BIN" ]; then
    echo "Error: Myco binary not found at $MYCO_BIN"
    echo "Please build Myco first with 'make'"
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

# Run basic benchmarks
echo "Running basic Myco benchmarks..."
echo "==============================="

run_benchmark "basic_arithmetic" "$BENCHMARK_DIR/basic_arithmetic.myco"

# Generate simple report
REPORT_FILE="$RESULTS_DIR/simple_report_${TIMESTAMP}.txt"
echo "Generating simple report..."
echo "Report: $REPORT_FILE"

cat > "$REPORT_FILE" << EOF
Simple Myco Benchmark Report
===========================
Generated: $(date)
Timestamp: $TIMESTAMP

BENCHMARK RESULTS SUMMARY
========================

EOF

# Check if basic arithmetic passed
if [ -f "$RESULTS_DIR/basic_arithmetic_${TIMESTAMP}.txt" ]; then
    if grep -q "Benchmark completed successfully" "$RESULTS_DIR/basic_arithmetic_${TIMESTAMP}.txt"; then
        echo "✓ Basic Arithmetic: PASSED" >> "$REPORT_FILE"
    else
        echo "✗ Basic Arithmetic: FAILED" >> "$REPORT_FILE"
    fi
else
    echo "✗ Basic Arithmetic: NO OUTPUT" >> "$REPORT_FILE"
fi

cat >> "$REPORT_FILE" << EOF

DETAILED BENCHMARK OUTPUTS
==========================

All benchmark outputs are saved in: $RESULTS_DIR/

Files:
- basic_arithmetic_${TIMESTAMP}.txt

EOF

echo "Simple benchmark report generated: $REPORT_FILE"
echo ""

# Display summary
echo "QUICK SUMMARY"
echo "============="
if [ -f "$RESULTS_DIR/basic_arithmetic_${TIMESTAMP}.txt" ]; then
    if grep -q "Benchmark completed successfully" "$RESULTS_DIR/basic_arithmetic_${TIMESTAMP}.txt"; then
        echo "✓ Basic Arithmetic: PASSED"
    else
        echo "✗ Basic Arithmetic: FAILED"
    fi
else
    echo "✗ Basic Arithmetic: NO OUTPUT"
fi
echo ""

# Save baseline for future comparison
BASELINE_FILE="$BENCHMARK_DIR/simple_baseline.txt"
cp "$REPORT_FILE" "$BASELINE_FILE"
echo "Baseline saved to: $BASELINE_FILE"

echo "Simple benchmark suite completed!"
echo "Check $REPORT_FILE for detailed results."
