#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Minimal test to isolate the segfault
int main() {
    printf("=== Myco Minimal Debug ===\n");
    
    // Test 1: Basic printf
    printf("✓ Basic printf works\n");
    
    // Test 2: Memory allocation
    char* test = malloc(100);
    if (test) {
        printf("✓ Memory allocation works\n");
        free(test);
    } else {
        printf("✗ Memory allocation failed\n");
        return 1;
    }
    
    // Test 3: String operations
    char buffer[100];
    strcpy(buffer, "test");
    if (strlen(buffer) == 4) {
        printf("✓ String operations work\n");
    } else {
        printf("✗ String operations failed\n");
        return 1;
    }
    
    printf("=== All basic tests passed ===\n");
    return 0;
}
