#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char buffer[512];
    while (strncmp(buffer, "exit", 4)) {
        printf("$ ");
        fgets(buffer, 512, stdin);
        char* token = strtok(buffer, " ");
        while (token != NULL) {
            printf("arg: %s\n", token);
            token = strtok(NULL, " ");
        }
    }
}
