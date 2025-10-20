#include <stdio.h>
#include <time.h>
#include <math.h>

void c_arithmetic_benchmark() {
    printf("=== C Arithmetic Benchmark ===\n");
    
    clock_t start = clock();
    
    // Basic arithmetic operations
    long long sum = 0;
    double product = 1.0;
    
    for (int i = 0; i < 1000000; i++) {
        sum += i;
        product *= 1.001;  // Prevent overflow
    }
    
    // Array operations
    int arr[16] = {0};
    for (int i = 0; i < 16; i++) {
        arr[i] = i * 2;
    }
    
    int array_sum = 0;
    for (int i = 0; i < 16; i++) {
        array_sum += arr[i];
    }
    
    // Reductions
    int data[] = {42, 17, 89, 23, 56, 91, 8, 34, 67, 12, 45, 78, 3, 29, 61, 95};
    int sum_val = 0;
    int min_val = data[0];
    int max_val = data[0];
    
    for (int i = 0; i < 16; i++) {
        sum_val += data[i];
        if (data[i] < min_val) min_val = data[i];
        if (data[i] > max_val) max_val = data[i];
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Arithmetic benchmark completed\n");
    printf("Sum: %lld\n", sum);
    printf("Product: %f\n", product);
    printf("Array operations benchmark completed\n");
    printf("Array sum: %d\n", array_sum);
    printf("Reductions benchmark completed\n");
    printf("Sum: %d\n", sum_val);
    printf("Min: %d\n", min_val);
    printf("Max: %d\n", max_val);
    printf("=== Benchmark completed ===\n");
    printf("Time: %.6f seconds\n", time_taken);
}

int main() {
    c_arithmetic_benchmark();
    return 0;
}
