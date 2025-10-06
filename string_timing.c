#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main() {
    clock_t start = clock();
    
    char* result = malloc(1);
    result[0] = '\0';
    
    for (int i = 0; i < 1000; i++) {
        char temp[20];
        sprintf(temp, "test%d", i);
        size_t new_len = strlen(result) + strlen(temp) + 1;
        result = realloc(result, new_len);
        strcat(result, temp);
    }
    
    clock_t end = clock();
    double time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("C: %zu chars in %.6f seconds\n", strlen(result), time_spent);
    free(result);
    return 0;
}
