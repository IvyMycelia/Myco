#!/bin/bash

# Simple Performance Benchmark: Myco vs Python vs C
# Focus on clear, measurable results

set -e

echo "=== SIMPLE PERFORMANCE BENCHMARKS ==="
echo "Myco vs Python vs C - Clear Results"
echo ""

# Test 1: Simple arithmetic loop
echo "=== Test 1: Arithmetic Loop (1,000,000 iterations) ==="

# C version
cat > arithmetic.c << 'EOF'
#include <stdio.h>
#include <time.h>

int main() {
    clock_t start = clock();
    
    long long sum = 0;
    for (int i = 1; i <= 1000000; i++) {
        sum += i;
    }
    
    clock_t end = clock();
    double time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("C: %lld in %.6f seconds\n", sum, time_spent);
    return 0;
}
EOF

# Python version
cat > arithmetic.py << 'EOF'
import time

start = time.time()
sum_val = 0
for i in range(1, 1000001):
    sum_val += i
end = time.time()
print(f"Python: {sum_val} in {end - start:.6f} seconds")
EOF

# Myco version
cat > arithmetic.myco << 'EOF'
let sum = 0;
for i in 1..1000000:
    sum = sum + i;
end
print("Myco: " + sum.toString() + " (interpreter)");
EOF

echo "Compiling C..."
gcc -O3 -o arithmetic_c arithmetic.c

echo "Running C..."
./arithmetic_c

echo "Running Python..."
python3 arithmetic.py

echo "Running Myco (interpreter)..."
./bin/myco arithmetic.myco

echo "Running Myco (compiler)..."
./bin/myco arithmetic.myco --build >/dev/null 2>&1
if [ -f temp_output ]; then
    echo "Myco (compiled):"
    ./temp_output
else
    echo "Myco compilation failed"
fi

echo ""

# Test 2: String concatenation
echo "=== Test 2: String Concatenation (10,000 operations) ==="

# C version
cat > string_test.c << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main() {
    clock_t start = clock();
    
    char* result = malloc(1);
    result[0] = '\0';
    
    for (int i = 0; i < 10000; i++) {
        char temp[50];
        sprintf(temp, "test%d", i);
        size_t new_len = strlen(result) + strlen(temp) + 1;
        result = realloc(result, new_len);
        strcat(result, temp);
    }
    
    clock_t end = clock();
    double time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("C: %zu chars in %.6f seconds\n", strlen(result), time_spent);
    free(result);
    return 0;
}
EOF

# Python version
cat > string_test.py << 'EOF'
import time

start = time.time()
result = ""
for i in range(10000):
    result += f"test{i}"
end = time.time()
print(f"Python: {len(result)} chars in {end - start:.6f} seconds")
EOF

# Myco version
cat > string_test.myco << 'EOF'
let result = "";
for i in 1..10000:
    result = result + "test" + i.toString();
end
print("Myco: " + result.length().toString() + " chars (interpreter)");
EOF

echo "Compiling C..."
gcc -O3 -o string_test_c string_test.c

echo "Running C..."
./string_test_c

echo "Running Python..."
python3 string_test.py

echo "Running Myco (interpreter)..."
./bin/myco string_test.myco

echo "Running Myco (compiler)..."
./bin/myco string_test.myco --build >/dev/null 2>&1
if [ -f temp_output ]; then
    echo "Myco (compiled):"
    ./temp_output
else
    echo "Myco compilation failed"
fi

echo ""

# Test 3: Array operations
echo "=== Test 3: Array Operations (100,000 elements) ==="

# C version
cat > array_test.c << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    clock_t start = clock();
    
    int* arr = malloc(100000 * sizeof(int));
    for (int i = 0; i < 100000; i++) {
        arr[i] = i * 2;
    }
    
    int sum = 0;
    for (int i = 0; i < 100000; i++) {
        sum += arr[i];
    }
    
    clock_t end = clock();
    double time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("C: Sum %d in %.6f seconds\n", sum, time_spent);
    free(arr);
    return 0;
}
EOF

# Python version
cat > array_test.py << 'EOF'
import time

start = time.time()
arr = [i * 2 for i in range(100000)]
sum_val = sum(arr)
end = time.time()
print(f"Python: Sum {sum_val} in {end - start:.6f} seconds")
EOF

# Myco version
cat > array_test.myco << 'EOF'
let arr = [];
for i in 1..100000:
    arr = arr + [i * 2];
end
let sum = 0;
for i in 1..arr.length():
    sum = sum + arr[i-1];
end
print("Myco: Sum " + sum.toString() + " (interpreter)");
EOF

echo "Compiling C..."
gcc -O3 -o array_test_c array_test.c

echo "Running C..."
./array_test_c

echo "Running Python..."
python3 array_test.py

echo "Running Myco (interpreter)..."
./bin/myco array_test.myco

echo "Running Myco (compiler)..."
./bin/myco array_test.myco --build >/dev/null 2>&1
if [ -f temp_output ]; then
    echo "Myco (compiled):"
    ./temp_output
else
    echo "Myco compilation failed"
fi

echo ""

# Cleanup
rm -f arithmetic.c arithmetic.py arithmetic_c arithmetic.myco
rm -f string_test.c string_test.py string_test_c string_test.myco
rm -f array_test.c array_test.py array_test_c array_test.myco
rm -f temp_output temp_output.c

echo "=== BENCHMARK COMPLETE ==="
echo "Results show relative performance between languages."
