#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#define BUFFER_SIZE 512
#define ARG_LIMIT 50

void get_input(char* user_input);
void get_args(char** args, char* user_input);
void exec_cmd(char** args, bool stop);
void get_new_path(char* user_input);
void display_path();
int set_new_path(char* new_path);
bool handle_cmd(char** args);

int main() {
    char user_input[BUFFER_SIZE];
    char* args[ARG_LIMIT];
    get_input(user_input);

    chdir(getenv("HOME"));

    // Loop until the user enters "exit" or presses CTRL+D
    while(strncmp(user_input, "exit", 4) && !feof(stdin)) {
        get_args(args, user_input);
        exec_cmd(args, handle_cmd(args));
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
    while(token) {
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
void exec_cmd(char** args, bool stop) {
    if(stop) return;

    pid_t c_pid, pid;
    int status;
    c_pid = fork();
    if(c_pid == -1) { // fork failed
        perror("Error");
        _exit(1);
    }
    else if(c_pid == 0) { // child
        execvp(args[0], args);
        perror("Error");
        _exit(1);
    }
    else if(c_pid > 0){ // parent
        if((pid = wait(&status)) < 0) {
            perror("Error");
            _exit(1);
        }
    }
}

bool handle_cmd(char** args) {
    if(args[0] == NULL) return true;

    if(strncmp(args[0], "getpath", 7) == 0) {
        if(args[1] == NULL) {
            display_path();
        }
        else {
            printf("To display the path environment variable please enter the command without any arguments\n");
        }

        return true;
    }

    if(strncmp(args[0], "setpath", 7) == 0) {
        if(args[1] == NULL) {
            printf("Please enter a path to add the value to the environment variable\n");
        }
        else if(args[2] == NULL) {
            get_new_path(args[1]);
        }
        else {
            printf("Please only enter one path\n");
        }

        return true;
    }

    return false;
}

void get_new_path(char* user_input) {
    char new_path[BUFFER_SIZE] = "";
    char* orig_path = getenv("PATH");

    strcat(new_path, orig_path);
    strcat(new_path, ":");
    strcat(new_path, user_input);

    DIR* dir = opendir(user_input);
    if(dir) {
        set_new_path(new_path);
        closedir(dir);
    }
    else {
        perror("Error");
    }
}

int set_new_path(char* new_path) {
    if(new_path != NULL) {
        int output = setenv("PATH", new_path, 1);
        if(output == 0) {
            printf("Path has been set succesfully!\n");
            return 0;
        }
        else {
            printf("Failed to set path!\n");
            return -1;
        }
    }
}

void display_path() {
    char* path = getenv("PATH");
    printf("Path: %s\n", path);
}
