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
