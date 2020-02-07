#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include<sys/wait.h> 
#include <unistd.h> 

void get_input(char* user_input);
void get_args(char** args, char* user_input);
void exec_cmd(char** args);

int main() {
    // Declare a buffer to store the user input and an array to hold the arguments
    char user_input[512];
    char* args[50];
    get_input(user_input);

    while (strncmp(user_input, "exit", 4) && !feof(stdin)) {
        get_args(args, user_input);
        exec_cmd(args);    
        get_input(user_input);
    }
}

void get_input(char* user_input) {
    printf("$ ");
    fgets(user_input, sizeof(user_input), stdin);
}

void get_args(char** args, char* user_input) {
    char* token = strtok(user_input, " \n\t|><&;");
    int arg_count = 0;
    while (token != NULL) {
        args[arg_count++] = token;
        token = strtok(NULL, " \n\t|><&;");
    }
    args[arg_count] = NULL;
}

void exec_cmd(char** args) {
    pid_t c_pid, pid;
    int status;
    c_pid = fork();
    if (c_pid == -1) {
        perror("Error: Fork failed!\n");
        _exit(1);
    }
    if (c_pid == 0) {
        execvp(args[0], args);
        perror("");
    }
    else if (c_pid > 0){
        if((pid = wait(&status)) < 0) {
            _exit(1);
        }
    } 
}