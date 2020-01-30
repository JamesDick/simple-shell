#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Declare a buffer to store the user input
    char user_input[512];
    // Print to console and prompt user to enter a value
    printf("$ ");
    fgets(user_input, sizeof(user_input), stdin);
    // Keep asking the user to enter a value (except when they enter 'exit')
    while (strncmp(user_input, "exit", 4) && !feof(stdin)) {
        // Split the user input into words
        char* token = strtok(user_input, " \n\t|><&;");
        while (token != NULL) {
            printf("'%s'\n", token);
            token = strtok(NULL, " \n\t|><&;");
        }
        // Prompt for the next user value
        printf("$ ");
        fgets(user_input, sizeof(user_input), stdin);
    }
    // Create a new line
    printf("\n");
}
