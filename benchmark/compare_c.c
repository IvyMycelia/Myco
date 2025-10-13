#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Forward declarations
long long fibonacci(int n);
long long simple_add(long long a, long long b);
long long factorial(int n);
long long complex_calc(long long a, long long b, long long c, long long d, long long e);

// Arithmetic benchmark equivalent
void arithmetic_benchmark() {
    int iterations = 10000;
    clock_t start = clock();
    
    // Integer arithmetic
    long long sum = 0;
    for (int i = 1; i <= iterations; i++) {
        sum += i;
    }
    
    // Float arithmetic
    double float_sum = 0.0;
    for (int i = 1; i <= iterations; i++) {
        float_sum += i * 1.5;
    }
    
    // Mixed arithmetic
    double mixed_sum = 0.0;
    for (int i = 1; i <= iterations; i++) {
        mixed_sum += (i * 2.5) - (i / 3.0);
    }
    
    // Multiplication
    long long product = 1;
    for (int i = 1; i < 100; i++) {
        product *= i;
    }
    
    // Division
    double quotient = 1000000.0;
    for (int i = 1; i < 100; i++) {
        quotient /= 2.0;
    }
    
    clock_t end = clock();
    double duration = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("C Arithmetic Benchmark Results:\n");
    printf("Iterations: %d\n", iterations);
    printf("Integer sum: %lld\n", sum);
    printf("Float sum: %f\n", float_sum);
    printf("Mixed sum: %f\n", mixed_sum);
    printf("Product: %lld\n", product);
    printf("Final quotient: %f\n", quotient);
    printf("Duration (seconds): %f\n", duration);
    printf("Operations per second: %f\n", (iterations * 5) / duration);
}

// Loop benchmark equivalent
void loop_benchmark() {
    int iterations = 1000;
    clock_t start = clock();
    
    // Nested loops
    long long nested_sum = 0;
    for (int i = 1; i <= 100; i++) {
        for (int j = 1; j <= 100; j++) {
            nested_sum += i * j;
        }
    }
    
    // While loops
    long long while_sum = 0;
    int counter = 0;
    while (counter < iterations) {
        while_sum += counter;
        counter++;
    }
    
    // Complex loop with conditions
    long long complex_sum = 0;
    for (int i = 1; i <= iterations; i++) {
        if (i % 2 == 0) {
            complex_sum += i * 2;
        } else {
            complex_sum += i * 3;
        }
    }
    
    // Fibonacci function
    long long fib_sum = 0;
    for (int i = 1; i <= 25; i++) {
        fib_sum += fibonacci(i);
    }
    
    // Array iteration
    int test_array[1000];
    for (int i = 0; i < 1000; i++) {
        test_array[i] = i + 1;
    }
    
    long long array_sum = 0;
    for (int i = 0; i < 1000; i++) {
        array_sum += test_array[i];
    }
    
    clock_t end = clock();
    double duration = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("\nC Loop Benchmark Results:\n");
    printf("Nested loops sum: %lld\n", nested_sum);
    printf("While loop sum: %lld\n", while_sum);
    printf("Complex loop sum: %lld\n", complex_sum);
    printf("Fibonacci sum: %lld\n", fib_sum);
    printf("Array iteration sum: %lld\n", array_sum);
    printf("Duration (seconds): %f\n", duration);
    printf("Loop iterations per second: %f\n", (iterations * 2 + 10000 + 25 + 1000) / duration);
}

// Fibonacci function
long long fibonacci(int n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

// Function call benchmark equivalent
long long simple_add(long long a, long long b) {
    return a + b;
}

long long factorial(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

long long complex_calc(long long a, long long b, long long c, long long d, long long e) {
    return (a + b) * (c - d) + e;
}

void function_benchmark() {
    int iterations = 10000;
    clock_t start = clock();
    
    // Simple function calls
    long long simple_sum = 0;
    for (int i = 1; i <= iterations; i++) {
        simple_sum += simple_add(i, i * 2);
    }
    
    // Recursive function calls
    long long factorial_sum = 0;
    for (int i = 1; i <= 20; i++) {
        factorial_sum += factorial(i);
    }
    
    // Complex function calls
    long long complex_sum = 0;
    for (int i = 1; i <= iterations; i++) {
        complex_sum += complex_calc(i, i * 2, i * 3, i * 4, i * 5);
    }
    
    clock_t end = clock();
    double duration = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("\nC Function Call Benchmark Results:\n");
    printf("Simple function calls: %lld\n", simple_sum);
    printf("Recursive factorial sum: %lld\n", factorial_sum);
    printf("Complex function calls: %lld\n", complex_sum);
    printf("Duration (seconds): %f\n", duration);
    printf("Function calls per second: %f\n", (iterations + 20 + iterations) / duration);
}

// Array benchmark equivalent
void array_benchmark() {
    int iterations = 10000;
    clock_t start = clock();
    
    // Array creation and initialization
    int *test_array = malloc(iterations * sizeof(int));
    for (int i = 0; i < iterations; i++) {
        test_array[i] = i + 1;
    }
    
    // Array access patterns
    long long access_sum = 0;
    for (int i = 0; i < iterations; i++) {
        access_sum += test_array[i];
    }
    
    // Array modification
    for (int i = 0; i < iterations; i++) {
        test_array[i] *= 2;
    }
    
    // Test array operations
    long long doubled_sum = 0;
    for (int i = 0; i < iterations; i++) {
        doubled_sum += test_array[i];
    }
    
    // Bubble sort
    int sort_array[1000];
    for (int i = 0; i < 1000; i++) {
        sort_array[i] = 1000 - i;
    }
    
    for (int i = 0; i < 999; i++) {
        for (int j = 0; j < 999 - i; j++) {
            if (sort_array[j] > sort_array[j + 1]) {
                int temp = sort_array[j];
                sort_array[j] = sort_array[j + 1];
                sort_array[j + 1] = temp;
            }
        }
    }
    
    // Multidimensional arrays
    long long matrix_sum = 0;
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            matrix_sum += i * j;
        }
    }
    
    free(test_array);
    
    clock_t end = clock();
    double duration = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("\nC Array Benchmark Results:\n");
    printf("Array length: %d\n", iterations);
    printf("Access sum: %lld\n", access_sum);
    printf("Doubled sum: %lld\n", doubled_sum);
    printf("Matrix sum: %lld\n", matrix_sum);
    printf("Duration (seconds): %f\n", duration);
    printf("Array operations per second: %f\n", (iterations * 3 + 1000000) / duration);
}

int main() {
    printf("C Performance Baseline for Myco Optimization Comparison\n");
    printf("======================================================\n");
    
    arithmetic_benchmark();
    loop_benchmark();
    function_benchmark();
    array_benchmark();
    
    printf("\nC baseline complete.\n");
    return 0;
}
