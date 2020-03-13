#include "shell.h"

int main() {
    char user_input[BUFFER_SIZE];
    char* args[ARG_LIMIT];
    History* history = load_history();    
    Alias_List alias_list = new_alias_list();
    chdir(getenv("HOME"));
    
    while (get_input(user_input)) {
        invoke_history(history, user_input);
        insert_aliases(alias_list, user_input);
        add_entry(history, user_input);
        get_args(args, user_input);
        exec_cmd(args, handle_cmd(args, history, alias_list));
    }
    
    printf("\n");
    save_history(history);
}

/**
* Displays a prompt and gets the user input.
* Returns NULL if the user has indicated they wish to exit.
* @param user_input Buffer in which to place the user input
* @return The result of the input prompt.
*/
char* get_input(char* user_input) {
    display_prompt();
    char* result = fgets(user_input, BUFFER_SIZE, stdin);
    if(strncmp(user_input, "exit", 4) == 0)
        result = NULL;
    return result;
}

/**
 * Displays the prompt for user input,
 * in the format: username@hostname:/path/to/directory$
 */
void display_prompt() {
    char username[32];
    strcpy(username, getenv("USER"));

    char hostname[255];
    gethostname(hostname, 255);

    char directory[4096];
    getcwd(directory, 4096);

    /** 
     * \033 is an escape sequence, 
     * [1;92m and [1;94m are colour codes, 
     * [0m is to reset
     */
    printf("\033[1;92m%s@%s\033[0m:\033[1;94m%s\033[0m$ ", 
        username, hostname, directory);
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
    while(token) {
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

bool handle_cmd(char** args, History* history, Alias_List alias_list) {
    if(args[0] == NULL) return true;

    if(strcmp(args[0], "getpath") == 0) {
        if(args[1] == NULL) {
            display_path();
        }
        else {
            printf("To display the path environment variable please enter the command without any arguments\n");
        }

        return true;
    }

    if(strcmp(args[0], "setpath") == 0) {
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

    if(strcmp(args[0], "history") == 0) {
        print_history(history);
        return true;
    }

    if(strcmp(args[0], "alias") == 0) {
        if(!args[1]) {
            print_aliases(alias_list);
        }
        else if(args[2]) {
            add_alias(alias_list, args[1], reconstruct_args(args, 2));
        }

        return true;
    }

    if(strcmp(args[0], "unalias") == 0) {
        if(!args[1]) {
            printf("unalias: too few arguments\n");
        }
        if(args[1] && !args[2]) {
            remove_alias(alias_list, args[1]);
        }
        else {
            printf("unalias: too many arguments\n");
        }

        return true;
    }

    return false;
}

char* reconstruct_args(char** args, int i) {
    char* buffer;
    buffer = (char*) malloc(sizeof(BUFFER_SIZE));

    while(args[i]) {
        strcat(buffer, args[i++]);
        strcat(buffer, " ");
    }

    return buffer;
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
