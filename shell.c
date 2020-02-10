#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h>

#define BUFFER_SIZE 512
#define ARG_LIMIT 50

void get_input(char* user_input);
void get_args(char** args, char* user_input);
void exec_cmd(char** args);
void handle_path_cmds(char** args);

int main() {
    char user_input[BUFFER_SIZE];
    char* args[ARG_LIMIT];
    get_input(user_input);

    // Loop until the user enters "exit" or presses CTRL+D
    while (strncmp(user_input, "exit", 4) && !feof(stdin)) {        
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
        handle_path_cmds(args);
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

void handle_path_cmds(char** args) {
    if(strncmp(args[0], "getpath", 7) == 0) {
        char* path = getenv("PATH");
        printf("\n%s\n\n", path);
    }
    else if(strncmp(args[0], "setpath", 7) == 0 && args[1] != NULL) {
        char new_path[BUFFER_SIZE] = "";
        char* orig_path = getenv("PATH");

        strcat(new_path, orig_path);
        strcat(new_path, ":");
        strcat(new_path, args[1]);

        DIR* dir = opendir(args[1]);
        if (dir) {
            setenv("PATH", new_path, 1);
            printf("Original path:\n%s\n\n", orig_path);
            printf("Changed path:\n%s\n\n", getenv("PATH"));

            closedir(dir);
        }
        else if (ENOENT == errno) {
        perror("Directory does not exist!");
        }
    }

    _exit(1);
}
