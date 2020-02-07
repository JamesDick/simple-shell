#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include<sys/wait.h> 
#include <unistd.h>

#define BUFFER_SIZE 512
#define ARG_LIMIT 50

void get_input(char* user_input);
void get_args(char** args, char* user_input);
void exec_cmd(char** args);

int main() {
    char user_input[BUFFER_SIZE];
    get_input(user_input);
    while (strncmp(user_input, "exit", 4) && !feof(stdin)) {
        char* args[ARG_LIMIT];
        get_args(args, user_input);
        exec_cmd(args);
        get_input(user_input);
    }
    printf("\n");
}

/**
* Displays a prompt and gets the user input
* 
* @param user_input Buffer in which to place the user input
*/
void get_input(char* user_input) {
    printf("$ ");
    fgets(user_input, BUFFER_SIZE, stdin);
}

/**
* Populates an array of arguments by tokenizing the user input
*
* @param args Array in which to place the tokenized arguments
* @param user_input Buffer holding the user input to be tokenized
*/
void get_args(char** args, char* user_input) {
    char* token = strtok(user_input, " \n\t|><&;");
    int arg_count = 0;
    while (token) {
        args[arg_count++] = token;
        token = strtok(NULL, " \n\t|><&;");
    }
    args[arg_count] = NULL;
}

/** 
* Create a child process and execute the user's arguments
*
* @param args Array containing the arguments to be executed
*/
void exec_cmd(char** args) {
    pid_t c_pid, pid;
    int status;    
    c_pid = fork();
    if (c_pid == -1) { // fork failed
        perror("");
        _exit(1);
    }
    if (c_pid == 0) { // child
        execvp(args[0], args);
        perror("");
        _exit(1);
    }
    else if (c_pid > 0){ // parent
        if((pid = wait(&status)) < 0) {
            perror("");
            _exit(1);
        }
    }
}
