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
