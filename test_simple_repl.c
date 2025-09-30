#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

int main() {
    printf("Simple REPL test\n");
    
    char* input;
    while (1) {
        input = readline("test> ");
        if (!input) {
            printf("No input, breaking...\n");
            break;
        }
        
        if (strlen(input) == 0) {
            free(input);
            continue;
        }
        
        printf("You entered: %s\n", input);
        
        if (strcmp(input, "exit") == 0) {
            free(input);
            break;
        }
        
        free(input);
    }
    
    printf("Goodbye!\n");
    return 0;
}
