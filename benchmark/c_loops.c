#include <stdio.h>
#include <time.h>

void c_loops_benchmark() {
    printf("=== C Loops Benchmark ===\n");
    
    clock_t start = clock();
    
    // Basic loop
    long long sum = 0;
    for (int i = 0; i < 100000; i++) {
        sum += i;
    }
    
    // Nested loops
    long long result = 0;
    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < 1000; j++) {
            result += (i * j);
        }
    }
    
    // Array operations
    int arr[16] = {0};
    for (int i = 0; i < 16; i++) {
        arr[i] = i;
    }
    
    for (int i = 0; i < 16; i++) {
        arr[i] *= 2;
    }
    
    int array_sum = 0;
    for (int i = 0; i < 16; i++) {
        array_sum += arr[i];
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Loops benchmark completed\n");
    printf("Sum: %lld\n", sum);
    printf("Nested loops benchmark completed\n");
    printf("Result: %lld\n", result);
    printf("Array operations benchmark completed\n");
    printf("Sum: %d\n", array_sum);
    printf("=== Benchmark completed ===\n");
    printf("Time: %.6f seconds\n", time_taken);
}

int main() {
    c_loops_benchmark();
    return 0;
}
