#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

int main() {
    clock_t start = clock();
    
    // Simple mathematical operations
    double sum = 0.0;
    for (int i = 0; i < 10000; i++) {
        sum += i + (i * 2);
    }
    
    // Array operations
    int arr[1000];
    for (int i = 0; i < 1000; i++) {
        arr[i] = i;
    }
    
    // String operations
    char str[101];
    for (int i = 0; i < 100; i++) {
        str[i] = 'A';
    }
    str[100] = '\0';
    
    clock_t end = clock();
    double duration = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("C benchmark completed in %.6f seconds\n", duration);
    printf("Sum = %.0f\n", sum);
    printf("Array length = %d\n", 1000);
    printf("String length = %zu\n", strlen(str));
    
    return 0;
}
