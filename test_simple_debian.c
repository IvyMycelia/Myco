#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Simple test to check if the issue is in Myco's initialization
int main(int argc, char* argv[]) {
    printf("=== Myco Simple Test ===\n");
    printf("Arguments: %d\n", argc);
    
    if (argc > 1) {
        printf("First argument: %s\n", argv[1]);
    }
    
    // Test basic file operations
    FILE* test_file = fopen("test_simple.myco", "w");
    if (test_file) {
        fprintf(test_file, "print(\"Hello from test file\");\n");
        fclose(test_file);
        printf("✓ File creation works\n");
    } else {
        printf("✗ File creation failed\n");
        return 1;
    }
    
    // Test file reading
    test_file = fopen("test_simple.myco", "r");
    if (test_file) {
        char buffer[100];
        if (fgets(buffer, sizeof(buffer), test_file)) {
            printf("✓ File reading works: %s", buffer);
        }
        fclose(test_file);
    } else {
        printf("✗ File reading failed\n");
        return 1;
    }
    
    // Cleanup
    remove("test_simple.myco");
    
    printf("=== All tests passed ===\n");
    return 0;
}
