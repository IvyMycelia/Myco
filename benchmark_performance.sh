#!/bin/bash

# Myco Performance Benchmark Script
# Tests interpreter vs compiler performance

set -e

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

# Benchmark functions
benchmark_interpreter() {
    local test_file=$1
    local iterations=$2
    
    echo -e "${YELLOW}Benchmarking interpreter...${NC}"
    
    local total_time=0
    for i in $(seq 1 $iterations); do
        local start_time=$(date +%s.%N)
        ./bin/myco "$test_file" >/dev/null 2>&1
        local end_time=$(date +%s.%N)
        local duration=$(echo "$end_time - $start_time" | bc)
        total_time=$(echo "$total_time + $duration" | bc)
    done
    
    local avg_time=$(echo "scale=4; $total_time / $iterations" | bc)
    echo "Interpreter average: ${avg_time}s"
    echo "$avg_time"
}

benchmark_compiler() {
    local test_file=$1
    local iterations=$2
    
    echo -e "${YELLOW}Benchmarking compiler...${NC}"
    
    # Compile once
    ./bin/myco "$test_file" --build >/dev/null 2>&1
    
    local total_time=0
    for i in $(seq 1 $iterations); do
        local start_time=$(date +%s.%N)
        ./temp_output >/dev/null 2>&1
        local end_time=$(date +%s.%N)
        local duration=$(echo "$end_time - $start_time" | bc)
        total_time=$(echo "$total_time + $duration" | bc)
    done
    
    local avg_time=$(echo "scale=4; $total_time / $iterations" | bc)
    echo "Compiler average: ${avg_time}s"
    echo "$avg_time"
}

benchmark_memory() {
    local test_file=$1
    
    echo -e "${YELLOW}Benchmarking memory usage...${NC}"
    
    # Interpreter memory usage
    local interp_mem=$(./bin/myco "$test_file" 2>&1 | grep -o '[0-9]*' | head -1 || echo "0")
    
    # Compiler memory usage
    ./bin/myco "$test_file" --build >/dev/null 2>&1
    local comp_mem=$(./temp_output 2>&1 | grep -o '[0-9]*' | head -1 || echo "0")
    
    echo "Interpreter memory: ${interp_mem}KB"
    echo "Compiler memory: ${comp_mem}KB"
}

# Create benchmark test files
create_benchmark_tests() {
    echo -e "${BLUE}Creating benchmark test files...${NC}"
    
    # Arithmetic benchmark
    cat > benchmark_arithmetic.myco << 'EOF'
let sum = 0;
for i in 1..100000:
    sum = sum + i;
end
print("Sum: " + sum.toString());
EOF

    # String benchmark
    cat > benchmark_string.myco << 'EOF'
let result = "";
for i in 1..10000:
    result = result + "test" + i.toString();
end
print("String length: " + result.length().toString());
EOF

    # Array benchmark
    cat > benchmark_array.myco << 'EOF'
let arr = [];
for i in 1..50000:
    arr = arr + [i];
end
print("Array length: " + arr.length().toString());
EOF

    # Function call benchmark
    cat > benchmark_functions.myco << 'EOF'
function fibonacci(n):
    if n <= 1:
        return n;
    else:
        return fibonacci(n-1) + fibonacci(n-2);
    end
end

let result = fibonacci(25);
print("Fibonacci(25): " + result.toString());
EOF
}

# Run comprehensive benchmarks
run_benchmarks() {
    local iterations=${1:-5}
    
    echo -e "${BLUE}=== Myco Performance Benchmarks ===${NC}"
    echo -e "Iterations: $iterations"
    echo ""
    
    # Test files
    local tests=("benchmark_arithmetic.myco" "benchmark_string.myco" "benchmark_array.myco" "benchmark_functions.myco")
    
    for test in "${tests[@]}"; do
        echo -e "${GREEN}=== Testing $test ===${NC}"
        
        # Performance benchmarks
        local interp_time=$(benchmark_interpreter "$test" $iterations)
        local comp_time=$(benchmark_compiler "$test" $iterations)
        
        # Calculate speedup
        local speedup=$(echo "scale=2; $interp_time / $comp_time" | bc)
        
        echo -e "${BLUE}Speedup: ${speedup}x${NC}"
        echo ""
        
        # Memory benchmarks
        benchmark_memory "$test"
        echo ""
    done
    
    # Cleanup
    rm -f benchmark_*.myco temp_output temp_output.c
}

# Main execution
case "${1:-all}" in
    "create")
        create_benchmark_tests
        echo -e "${GREEN}Benchmark tests created!${NC}"
        ;;
    "run")
        run_benchmarks ${2:-5}
        ;;
    "clean")
        rm -f benchmark_*.myco temp_output temp_output.c
        echo -e "${GREEN}Benchmark files cleaned!${NC}"
        ;;
    "all"|*)
        create_benchmark_tests
        run_benchmarks ${2:-5}
        rm -f benchmark_*.myco temp_output temp_output.c
        ;;
esac
