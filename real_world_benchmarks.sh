#!/bin/bash

# Real-World Performance Benchmarks: Myco vs Python vs C
# Tests realistic workloads that developers actually use

set -e

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

# Benchmark configuration
ITERATIONS=5
WARMUP=2

echo -e "${BLUE}=== REAL-WORLD PERFORMANCE BENCHMARKS ===${NC}"
echo -e "Myco vs Python vs C - Realistic Workloads"
echo -e "Iterations: $ITERATIONS (after $WARMUP warmup runs)"
echo ""

# Create benchmark directory
mkdir -p benchmarks
cd benchmarks

# Test 1: Fibonacci (CPU-intensive recursive algorithm)
echo -e "${GREEN}=== Test 1: Fibonacci (n=35) ===${NC}"
echo "Testing recursive algorithm performance..."

# C version
cat > fibonacci.c << 'EOF'
#include <stdio.h>
#include <time.h>

long long fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n-1) + fibonacci(n-2);
}

int main() {
    clock_t start = clock();
    long long result = fibonacci(35);
    clock_t end = clock();
    double time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("C Result: %lld, Time: %.6f seconds\n", result, time_spent);
    return 0;
}
EOF

# Python version
cat > fibonacci.py << 'EOF'
import time

def fibonacci(n):
    if n <= 1:
        return n
    return fibonacci(n-1) + fibonacci(n-2)

start = time.time()
result = fibonacci(35)
end = time.time()
print(f"Python Result: {result}, Time: {end - start:.6f} seconds")
EOF

# Myco version
cat > fibonacci.myco << 'EOF'
function fibonacci(n):
    if n <= 1:
        return n;
    else:
        return fibonacci(n-1) + fibonacci(n-2);
    end
end

let start_time = time.now();
let result = fibonacci(35);
let end_time = time.now();
let duration = time.difference(end_time, start_time);
print("Myco Result: " + result.toString() + ", Time: " + duration.toString() + " seconds");
EOF

# Compile and run C
echo "Compiling C version..."
gcc -O3 -o fibonacci_c fibonacci.c
echo "Running C benchmark..."
for i in $(seq 1 $((WARMUP + ITERATIONS))); do
    if [ $i -gt $WARMUP ]; then
        echo -n "C Run $((i-WARMUP)): "
    fi
    ./fibonacci_c 2>/dev/null | grep "Time:" | cut -d' ' -f4
done

# Run Python
echo "Running Python benchmark..."
for i in $(seq 1 $((WARMUP + ITERATIONS))); do
    if [ $i -gt $WARMUP ]; then
        echo -n "Python Run $((i-WARMUP)): "
    fi
    python3 fibonacci.py 2>/dev/null | grep "Time:" | cut -d' ' -f4
done

# Run Myco
echo "Running Myco benchmark..."
for i in $(seq 1 $((WARMUP + ITERATIONS))); do
    if [ $i -gt $WARMUP ]; then
        echo -n "Myco Run $((i-WARMUP)): "
    fi
    ../bin/myco fibonacci.myco 2>/dev/null | grep "Time:" | cut -d' ' -f4
done

echo ""

# Test 2: String Processing (Real-world text manipulation)
echo -e "${GREEN}=== Test 2: String Processing (10,000 operations) ===${NC}"
echo "Testing string manipulation performance..."

# C version
cat > string_processing.c << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char* process_string(const char* input) {
    size_t len = strlen(input);
    char* result = malloc(len * 2 + 1);
    int j = 0;
    
    for (size_t i = 0; i < len; i++) {
        if (input[i] >= 'a' && input[i] <= 'z') {
            result[j++] = input[i] - 32; // to uppercase
        } else if (input[i] >= 'A' && input[i] <= 'Z') {
            result[j++] = input[i] + 32; // to lowercase
        } else {
            result[j++] = input[i];
        }
        result[j++] = input[i]; // duplicate
    }
    result[j] = '\0';
    return result;
}

int main() {
    const char* test_string = "Hello World 123 ABC xyz";
    clock_t start = clock();
    
    for (int i = 0; i < 10000; i++) {
        char* result = process_string(test_string);
        free(result);
    }
    
    clock_t end = clock();
    double time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("C Time: %.6f seconds\n", time_spent);
    return 0;
}
EOF

# Python version
cat > string_processing.py << 'EOF'
import time

def process_string(input_str):
    result = ""
    for char in input_str:
        if char.islower():
            result += char.upper()
        elif char.isupper():
            result += char.lower()
        else:
            result += char
        result += char  # duplicate
    return result

test_string = "Hello World 123 ABC xyz"
start = time.time()

for i in range(10000):
    result = process_string(test_string)

end = time.time()
print(f"Python Time: {end - start:.6f} seconds")
EOF

# Myco version
cat > string_processing.myco << 'EOF'
function process_string(input):
    let result = "";
    for i in 0..input.length():
        let char = input[i];
        if char >= "a" and char <= "z":
            result = result + char.toUpperCase();
        else if char >= "A" and char <= "Z":
            result = result + char.toLowerCase();
        else:
            result = result + char;
        end
        result = result + char; // duplicate
    end
    return result;
end

let test_string = "Hello World 123 ABC xyz";
let start_time = time.now();

for i in 1..10000:
    let result = process_string(test_string);
end

let end_time = time.now();
let duration = time.difference(end_time, start_time);
print("Myco Time: " + duration.toString() + " seconds");
EOF

# Compile and run C
echo "Compiling C version..."
gcc -O3 -o string_processing_c string_processing.c
echo "Running C benchmark..."
for i in $(seq 1 $((WARMUP + ITERATIONS))); do
    if [ $i -gt $WARMUP ]; then
        echo -n "C Run $((i-WARMUP)): "
    fi
    ./string_processing_c 2>/dev/null | grep "Time:" | cut -d' ' -f3
done

# Run Python
echo "Running Python benchmark..."
for i in $(seq 1 $((WARMUP + ITERATIONS))); do
    if [ $i -gt $WARMUP ]; then
        echo -n "Python Run $((i-WARMUP)): "
    fi
    python3 string_processing.py 2>/dev/null | grep "Time:" | cut -d' ' -f3
done

# Run Myco
echo "Running Myco benchmark..."
for i in $(seq 1 $((WARMUP + ITERATIONS))); do
    if [ $i -gt $WARMUP ]; then
        echo -n "Myco Run $((i-WARMUP)): "
    fi
    ../bin/myco string_processing.myco 2>/dev/null | grep "Time:" | cut -d' ' -f3
done

echo ""

# Test 3: Array Operations (Data structure performance)
echo -e "${GREEN}=== Test 3: Array Operations (50,000 elements) ===${NC}"
echo "Testing array manipulation performance..."

# C version
cat > array_operations.c << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    clock_t start = clock();
    
    // Create and populate array
    int* arr = malloc(50000 * sizeof(int));
    for (int i = 0; i < 50000; i++) {
        arr[i] = i * 2;
    }
    
    // Perform operations
    int sum = 0;
    for (int i = 0; i < 50000; i++) {
        sum += arr[i];
        if (i % 2 == 0) {
            arr[i] = arr[i] * 3;
        }
    }
    
    // Find max
    int max = arr[0];
    for (int i = 1; i < 50000; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    
    free(arr);
    
    clock_t end = clock();
    double time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("C Time: %.6f seconds (Sum: %d, Max: %d)\n", time_spent, sum, max);
    return 0;
}
EOF

# Python version
cat > array_operations.py << 'EOF'
import time

start = time.time()

# Create and populate array
arr = [i * 2 for i in range(50000)]

# Perform operations
sum_val = sum(arr)
for i in range(0, 50000, 2):
    arr[i] = arr[i] * 3

# Find max
max_val = max(arr)

end = time.time()
print(f"Python Time: {end - start:.6f} seconds (Sum: {sum_val}, Max: {max_val})")
EOF

# Myco version
cat > array_operations.myco << 'EOF'
let start_time = time.now();

// Create and populate array
let arr = [];
for i in 1..50000:
    arr = arr + [i * 2];
end

// Perform operations
let sum_val = 0;
for i in 1..arr.length():
    sum_val = sum_val + arr[i-1];
    if i % 2 == 0:
        arr[i-1] = arr[i-1] * 3;
    end
end

// Find max
let max_val = arr[0];
for i in 2..arr.length():
    if arr[i-1] > max_val:
        max_val = arr[i-1];
    end
end

let end_time = time.now();
let duration = time.difference(end_time, start_time);
print("Myco Time: " + duration.toString() + " seconds (Sum: " + sum_val.toString() + ", Max: " + max_val.toString() + ")");
EOF

# Compile and run C
echo "Compiling C version..."
gcc -O3 -o array_operations_c array_operations.c
echo "Running C benchmark..."
for i in $(seq 1 $((WARMUP + ITERATIONS))); do
    if [ $i -gt $WARMUP ]; then
        echo -n "C Run $((i-WARMUP)): "
    fi
    ./array_operations_c 2>/dev/null | grep "Time:" | cut -d' ' -f3
done

# Run Python
echo "Running Python benchmark..."
for i in $(seq 1 $((WARMUP + ITERATIONS))); do
    if [ $i -gt $WARMUP ]; then
        echo -n "Python Run $((i-WARMUP)): "
    fi
    python3 array_operations.py 2>/dev/null | grep "Time:" | cut -d' ' -f3
done

# Run Myco
echo "Running Myco benchmark..."
for i in $(seq 1 $((WARMUP + ITERATIONS))); do
    if [ $i -gt $WARMUP ]; then
        echo -n "Myco Run $((i-WARMUP)): "
    fi
    ../bin/myco array_operations.myco 2>/dev/null | grep "Time:" | cut -d' ' -f3
done

echo ""

# Test 4: File I/O (Real-world file processing)
echo -e "${GREEN}=== Test 4: File I/O (10,000 lines) ===${NC}"
echo "Testing file processing performance..."

# Create test data
echo "Creating test data..."
python3 -c "
with open('test_data.txt', 'w') as f:
    for i in range(10000):
        f.write(f'Line {i}: This is test data with some content to process\n')
"

# C version
cat > file_io.c << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main() {
    clock_t start = clock();
    
    FILE* file = fopen("test_data.txt", "r");
    if (!file) {
        printf("Error opening file\n");
        return 1;
    }
    
    char line[256];
    int line_count = 0;
    int total_chars = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line_count++;
        total_chars += strlen(line);
    }
    
    fclose(file);
    
    clock_t end = clock();
    double time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("C Time: %.6f seconds (Lines: %d, Chars: %d)\n", time_spent, line_count, total_chars);
    return 0;
}
EOF

# Python version
cat > file_io.py << 'EOF'
import time

start = time.time()

with open('test_data.txt', 'r') as file:
    lines = file.readlines()
    line_count = len(lines)
    total_chars = sum(len(line) for line in lines)

end = time.time()
print(f"Python Time: {end - start:.6f} seconds (Lines: {line_count}, Chars: {total_chars})")
EOF

# Myco version
cat > file_io.myco << 'EOF'
use file as file;

let start_time = time.now();

let content = file.read("test_data.txt");
let lines = content.split("\n");
let line_count = lines.length();
let total_chars = 0;
for i in 1..lines.length():
    total_chars = total_chars + lines[i-1].length();
end

let end_time = time.now();
let duration = time.difference(end_time, start_time);
print("Myco Time: " + duration.toString() + " seconds (Lines: " + line_count.toString() + ", Chars: " + total_chars.toString() + ")");
EOF

# Compile and run C
echo "Compiling C version..."
gcc -O3 -o file_io_c file_io.c
echo "Running C benchmark..."
for i in $(seq 1 $((WARMUP + ITERATIONS))); do
    if [ $i -gt $WARMUP ]; then
        echo -n "C Run $((i-WARMUP)): "
    fi
    ./file_io_c 2>/dev/null | grep "Time:" | cut -d' ' -f3
done

# Run Python
echo "Running Python benchmark..."
for i in $(seq 1 $((WARMUP + ITERATIONS))); do
    if [ $i -gt $WARMUP ]; then
        echo -n "Python Run $((i-WARMUP)): "
    fi
    python3 file_io.py 2>/dev/null | grep "Time:" | cut -d' ' -f3
done

# Run Myco
echo "Running Myco benchmark..."
for i in $(seq 1 $((WARMUP + ITERATIONS))); do
    if [ $i -gt $WARMUP ]; then
        echo -n "Myco Run $((i-WARMUP)): "
    fi
    ../bin/myco file_io.myco 2>/dev/null | grep "Time:" | cut -d' ' -f3
done

echo ""

# Cleanup
echo -e "${BLUE}=== CLEANUP ===${NC}"
cd ..
rm -rf benchmarks

echo -e "${GREEN}Real-world benchmarks completed!${NC}"
echo -e "These tests represent actual workloads developers encounter:"
echo -e "- Recursive algorithms (Fibonacci)"
echo -e "- String processing and manipulation"
echo -e "- Array operations and data structures"
echo -e "- File I/O and data processing"
