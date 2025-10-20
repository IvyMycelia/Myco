#include <stdio.h>
#include <time.h>

long long add_numbers(long long a, long long b) {
    return a + b;
}

long long multiply_numbers(long long a, long long b) {
    return a * b;
}

long long complex_calculation(long long x, long long y, long long z) {
    long long temp1 = x * y;
    long long temp2 = y + z;
    long long temp3 = temp1 - temp2;
    return temp3 * 2;
}

void c_functions_benchmark() {
    printf("=== C Functions Benchmark ===\n");
    
    clock_t start = clock();
    
    // Function calls
    long long sum = 0;
    for (int i = 0; i < 100000; i++) {
        sum += add_numbers(i, i + 1);
    }
    
    // Nested functions
    long long result = 0;
    for (int i = 0; i < 50000; i++) {
        long long temp = add_numbers(i, i * 2);
        result += multiply_numbers(temp, 3);
    }
    
    // Complex functions
    long long total = 0;
    for (int i = 0; i < 25000; i++) {
        total += complex_calculation(i, i + 1, i + 2);
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Function calls benchmark completed\n");
    printf("Sum: %lld\n", sum);
    printf("Nested functions benchmark completed\n");
    printf("Result: %lld\n", result);
    printf("Complex functions benchmark completed\n");
    printf("Total: %lld\n", total);
    printf("=== Benchmark completed ===\n");
    printf("Time: %.6f seconds\n", time_taken);
}

int main() {
    c_functions_benchmark();
    return 0;
}
