#include <stdio.h>
#include <time.h>

int main() {
    printf("Starting C performance test...\n");
    
    // Test 1: Arithmetic Loop
    long long sum = 0;
    for (int i = 0; i < 5000000; i++) {
        sum += i;
    }
    printf("Arithmetic test: %lld\n", sum);
    
    // Test 2: Array Operations
    int arr[5] = {1, 2, 3, 4, 5};
    long long arr_sum = 0;
    for (int j = 0; j < 1000000; j++) {
        arr_sum += arr[0] + arr[1] + arr[2] + arr[3] + arr[4];
    }
    printf("Array test: %lld\n", arr_sum);
    
    // Test 3: Simple Variables
    long long x = 0, y = 1, z = 0;
    for (int k = 0; k < 2000000; k++) {
        z = x + y;
        x = y;
        y = z;
    }
    printf("Variable test: %lld\n", z);
    
    printf("C benchmark complete!\n");
    return 0;
}
