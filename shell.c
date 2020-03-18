#include "shell.h"

int main() {
    char* user_input = malloc(sizeof(char) * BUFFER_SIZE);
    char* args[ARG_LIMIT];
    char* orig_path = getenv("PATH");
    History* history = load_history();    
    Alias_List alias_list = load_aliases();
    chdir(getenv("HOME"));
    
    while(get_input(user_input)) {
        split_str(args, user_input, " \n\t|><&;");
        insert_aliases(alias_list, args[0], user_input);
        free(args[0]);

        invoke_history(history, user_input);
        add_entry(history, user_input);

        split_str(args, user_input, " \n\t|><&;");
        exec_cmd(args, handle_cmd(args, history, alias_list));
        free(args[0]);
    }
    
    printf("\n");
    set_new_path(orig_path);
    save_history(history);
    save_aliases(alias_list);

    free(user_input);
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
* Populates an array of strings by tokenizing the user input
*
* @param array Array in which to place the tokenized strings
* @param user_input Buffer holding the user input to be tokenized
* @param separators Buffer holding the separators
* @return The number of strings
*/
int split_str(char** array, char* user_input, char* separators) {
    char* token = malloc(sizeof(char) * BUFFER_SIZE);
    strcpy(token, user_input);
    token = strtok(token, separators);

    int count = 0;

    while(token) {
        array[count++] = token;
        token = strtok(NULL, separators);
    }

    array[count] = NULL;
    return count;
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
* @param stop Bool set to true if no command is to be ran or an internal command was ran
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
        fprintf(stderr, "Command %s failed: %s\n", args[0], strerror(errno));
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

    if(strncmp(args[0], "cd", 2) == 0) {
        if(args[1] == NULL) {
            chdir(getenv("HOME"));
        }
        else if(args[2] == NULL) {
            set_dir(args[1]);
        }
        else {
            printf(TOO_MANY_ARGS "please only enter one path\n");
        }

        return true;
    }
    else if(strcmp(args[0], "getpath") == 0) {
        if(args[1] == NULL) {
            printf("PATH: %s\n", getenv("PATH"));
        }
        else {
            printf(TOO_MANY_ARGS "please enter the command without any arguments\n");
        }

        return true;
    }
    else if(strcmp(args[0], "setpath") == 0) {
        if(args[1] == NULL) {
            printf(TOO_FEW_ARGS "please enter a path to set\n");
        }
        else if(args[2] == NULL) {
            get_new_path(args[1]);
        }
        else {
            printf(TOO_MANY_ARGS "please only enter one path\n");
        }

        return true;
    }
    else if(strcmp(args[0], "history") == 0) {
        if(args[1] == NULL) {
            print_history(history);
        }
        else {
            printf(TOO_MANY_ARGS "please enter the command without any arguments\n");
        }

        return true;
    }
    else if(strcmp(args[0], "alias") == 0) {
        if(args[1] == NULL) {
            print_aliases(alias_list);
        }
        else if(args[2] == NULL) {
            printf(TOO_FEW_ARGS "please enter the command and its arguments aswell\n");
        }
        else {
            char* alias_ptr = reconstruct_args(args, 1, 1);
            char* full_command_ptr = reconstruct_args(args, 2, -1);

            add_alias(alias_list, alias_ptr, full_command_ptr);

            free(alias_ptr);
            free(full_command_ptr);
        }

        return true;
    }
    else if(strcmp(args[0], "unalias") == 0) {
        if(args[1] == NULL) {
            printf(TOO_FEW_ARGS "please enter an alias to remove\n");
        }
        else if(args[2] == NULL) {
            remove_alias(alias_list, args[1]);
        }
        else {
            printf(TOO_MANY_ARGS "please enter only one alias to remove\n");
        }

        return true;
    }

    return false;
}

char* reconstruct_args(char** args, int start, int end) {
    char* buffer = malloc(sizeof(char) * BUFFER_SIZE);
    strcpy(buffer, "");

    while((start <= end && args[start] != NULL) || (end == -1 && args[start] != NULL)) {
        strcat(buffer, args[start++]);
        if((end != -1 && start > end) || (end == -1 && args[start] == NULL)) break;
        strcat(buffer, " ");
    }

    return buffer;
}

void get_new_path(char* user_input) {
    char new_path[BUFFER_SIZE];
    strcpy(new_path, user_input);

    DIR* dir = opendir(user_input);
    if(dir) {
        set_new_path(new_path);
        closedir(dir);
    }
    else {
        perror("Error");
    }
}

void set_new_path(char* new_path) {
    if(new_path != NULL) {
        if(setenv("PATH", new_path, 1) == 0) {
            printf("New PATH: %s\n", getenv("PATH"));
        }
        else {
            perror("Error");
        }
    }
    else {
        printf("New PATH cannot be equal to null\n");
    }
}

void set_dir(char* user_input) {
    char* final_path_ptr = malloc(sizeof(char) * BUFFER_SIZE);
    strcpy(final_path_ptr, user_input);

    if(final_path_ptr[0] == '~') {
        replacetilde(&final_path_ptr);
    }

    if(chdir(final_path_ptr) == -1) {
        char* prefix = "cd:";
        char* error_msg = strerror(errno);

        if(errno == ENOTDIR) {
            char* file_name = get_last_word(user_input);
            printf("%s %s: %s\n", prefix, file_name, error_msg);
            free(file_name);
            return;
        }

        printf("%s %s\n", prefix, error_msg);
    }
    
    free(final_path_ptr);
}

char* get_last_word(char* user_input) {
    char* word_ptr = malloc(sizeof(char) * BUFFER_SIZE);
    char* path_split[ARG_LIMIT];
    int words_length = split_str(path_split, user_input, "/");

    strcpy(word_ptr, path_split[words_length - 1]);
    return word_ptr;

}

void replacetilde(char** path) {
    char* ready_path_ptr = malloc(sizeof(char) * BUFFER_SIZE);
    char* path_split[ARG_LIMIT];
    split_str(path_split, *path, "/");

    strcpy(ready_path_ptr, getenv("HOME"));
    strcat(ready_path_ptr, "/");

    int i = 1;
    while(path_split[i] != NULL) {
        strcat(ready_path_ptr, path_split[i++]);
        if(path_split[i] == NULL) break;
        strcat(ready_path_ptr, "/");
    }

    strcpy(*path, ready_path_ptr);
    free(ready_path_ptr);
}
