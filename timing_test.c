#include <stdio.h>
#include <time.h>

int main() {
    clock_t start = clock();
    
    long long sum = 0;
    for (int i = 1; i <= 100000; i++) {
        sum += i;
    }
    
    clock_t end = clock();
    double time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("C: %lld in %.6f seconds\n", sum, time_spent);
    return 0;
}
