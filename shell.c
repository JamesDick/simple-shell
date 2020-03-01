#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h>
#include "history.h"

#define BUFFER_SIZE 512
#define ARG_LIMIT 50

char* get_input(char* user_input);
void get_args(char** args, char* user_input);
void exec_cmd(char** args);
void invoke_history(History* history, char* user_input);

int main() {
    char user_input[BUFFER_SIZE];
    char* args[ARG_LIMIT];
    History* history = create_history();
    
    while (get_input(user_input)) { /* loop until the user enters "exit" or presses CTRL+D */
        invoke_history(history, user_input); 
        add_entry(history, user_input);
        get_args(args, user_input);
        if(!strncmp(user_input, "history", 7)) /* history is an internal command and should be handled separately */
            print_history(history);
        else
            exec_cmd(args);
    }
}

/**
* Displays a prompt and gets the user input
* 
* @param user_input Buffer in which to place the user input
*/
char* get_input(char* user_input) {
    printf("$ ");
    char* result = fgets(user_input, BUFFER_SIZE, stdin);
    if(strncmp(user_input, "exit", 4) == 0)
        return 0;
    return result;
}


/**
* Populates an array of arguments by tokenizing the user input
* with each of " \n\t|><&;" as separators
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
 * Invokes the appropriate command from history 
 * based off of the command entered by the user.
 * 
 * @param history The data structure in which the history is stored
 * @param user_input The most recent command from the user
 */
void invoke_history(History* history, char* user_input) {
    if(user_input[0] == '!') {
        get_entry(history, user_input);

        if(!strcmp(user_input, "")) {
            printf("Command not found in history\n");
        }   
    }    
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