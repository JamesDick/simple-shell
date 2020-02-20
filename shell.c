#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include<sys/wait.h> 
#include <unistd.h>
#include "history.h"

#define BUFFER_SIZE 512
#define ARG_LIMIT 50

void get_input(char* user_input);
void get_args(char** args, char* user_input);
void exec_cmd(char** args);
void update_history(History history, char* user_input);
void invoke_history(History history, char* user_input);

int main() {
    char user_input[BUFFER_SIZE];
    char* args[ARG_LIMIT];
    History history = create_history();
    
    get_input(user_input); // Loop until the user enters "exit" or presses CTRL+D
    while (strncmp(user_input, "exit", 4) && !feof(stdin)) {   
        update_history(history, user_input);        
        if(user_input[0] == '!') // If we recognise a command is to be used from the history
            invoke_history(history, user_input); // Replace the user input with the appropriate command from history    
        get_args(args, user_input);
        if(!strncmp(user_input, "history", 7)) // history is an internal command and should be handled separately
            print_history(history);
        else
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
 * Adds the user input to the command history,
 * except for when the command was an invocation of history
 * (denoted by the command starting with !)
 * 
 * @param history The data structure in which the history is stored
 * @param user_input The most recent command from the user
 */
void update_history(History history, char* user_input) {
    if(user_input[0] == '!' || !strcmp(user_input, ""))
        return;
    
    push(history, user_input);
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
void invoke_history(History history, char* user_input) {
    if(!strncmp(user_input, "!!", 2)) { 
        // Invoke the most recent command from history
        strcpy(user_input, get_at(history, size(history)));
    }
    else if(!strncmp(user_input, "!-", 2)) { 
        // Invoke the command a specified number back from the most recent
        // eg !-3 looks for the third most recent command
        strcpy(user_input, get_at(history, (size(history) - atoi(user_input+2))));
    }
    else if(user_input[0] == '!') { 
        // Invoke the command with the specified number
        // eg !4 invokes the fourth command in the history
        strcpy(user_input, get_at(history, atoi(user_input+1)));           
    }
    if(!strcmp(user_input, "")) 
        // If no command was found by this point, print an error
        printf("Command not found in history\n");
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