#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include<sys/wait.h> 
#include <unistd.h> 

int main() {
    // Declare a buffer to store the user input
    char user_input[512];
    // Print to console and prompt user to enter a value
    printf("$ ");
    fgets(user_input, sizeof(user_input), stdin);
    // Keep asking the user to enter a value (except when they enter 'exit' or CTRL+D)
    while (strncmp(user_input, "exit", 4) && !feof(stdin)) {
        // Split the user input into words
        char* token = strtok(user_input, " \n\t|><&;");
        char* args[50];
        int argCount = 0;
        pid_t c_pid, pid;
        int status;
        while (token != NULL) {
            args[argCount] = token;
            token = strtok(NULL, " \n\t|><&;");
            argCount++;
        }
        args[argCount] = NULL;
        c_pid = fork();
        if (c_pid == -1) {
            perror("Error: Fork failed!\n");
            _exit(1);
        }
        if (c_pid == 0) {
            execvp(args[0], args);
            perror("Error: execvp failed!\n");
        }
        else if (c_pid > 0){
            if((pid = wait(&status)) < 0) {
                perror("Error: Wait failed!\n");
                _exit(1);
            }
        }
        // Prompt for the next user value
        printf("$ ");
        fgets(user_input, sizeof(user_input), stdin);
    }
    // Create a new line
    printf("\n");
}
