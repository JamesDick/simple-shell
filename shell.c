#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char buffer[513];
    printf("$ ");
    fgets(buffer, 512, stdin);
    while (strncmp(buffer, "exit", 4) && !feof(stdin)) {
        
        char* token = strtok(buffer, " \n\t|><&;");
        while (token != NULL) {
            printf("'%s'\n", token);
            token = strtok(NULL, " \n\t|><&;");
        }
        printf("$ ");
        fgets(buffer, 512, stdin);
    }
    printf("\n");
}
