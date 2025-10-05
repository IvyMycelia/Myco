#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char* process_string(const char* input) {
    size_t len = strlen(input);
    char* result = malloc(len * 2 + 1);
    int j = 0;
    
    for (size_t i = 0; i < len; i++) {
        if (input[i] >= 'a' && input[i] <= 'z') {
            result[j++] = input[i] - 32; // to uppercase
        } else if (input[i] >= 'A' && input[i] <= 'Z') {
            result[j++] = input[i] + 32; // to lowercase
        } else {
            result[j++] = input[i];
        }
        result[j++] = input[i]; // duplicate
    }
    result[j] = '\0';
    return result;
}

int main() {
    const char* test_string = "Hello World 123 ABC xyz";
    clock_t start = clock();
    
    for (int i = 0; i < 10000; i++) {
        char* result = process_string(test_string);
        free(result);
    }
    
    clock_t end = clock();
    double time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("C Time: %.6f seconds\n", time_spent);
    return 0;
}
