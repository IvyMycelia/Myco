#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// C benchmark for comparison
int main() {
    clock_t start = clock();
    
    // Fibonacci calculation (recursive)
    long long fib(int n) {
        if (n <= 1) return n;
        return fib(n-1) + fib(n-2);
    }
    
    // Prime number calculation
    int is_prime(int n) {
        if (n < 2) return 0;
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0) return 0;
        }
        return 1;
    }
    
    // Array operations
    int arr[1000];
    for (int i = 0; i < 1000; i++) {
        arr[i] = i * i;
    }
    
    // Mathematical operations
    double sum = 0;
    for (int i = 0; i < 1000000; i++) {
        sum += sqrt(i) + sin(i) + cos(i);
    }
    
    // String operations
    char str[1000];
    for (int i = 0; i < 1000; i++) {
        str[i] = 'A' + (i % 26);
    }
    str[999] = '\0';
    
    clock_t end = clock();
    double time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("C benchmark completed in %.6f seconds\n", time_spent);
    printf("Fibonacci(30) = %lld\n", fib(30));
    printf("Sum of math operations = %.2f\n", sum);
    printf("String length = %d\n", 999);
    
    return 0;
}
