#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char buffer[513];
    printf("$ ");
    fgets(buffer, 512, stdin);
    while (strncmp(buffer, "exit", 4) && !feof(stdin)) {
        
        char* token = strtok(buffer, " ");
        while (token != NULL) {
            printf("arg: %s\n", token);
            token = strtok(NULL, " ");
        }
        printf("$ ");
        fgets(buffer, 512, stdin);
    }
    printf("\n");
}
