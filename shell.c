#include "shell.h"

int main() {
    char* user_input = malloc(sizeof(char) * BUFFER_SIZE);
    char* alias_cmd = malloc(sizeof(char) * BUFFER_SIZE);
    char* args[ARG_LIMIT];
    char* orig_path = getenv("PATH");

    History* history = load_history();    
    Alias_List alias_list = load_aliases();
    chdir(getenv("HOME"));
    
    while(get_input(user_input)) {
        strcpy(alias_cmd, "");

        split_str(args, user_input, " \n\t|><&;");
        insert_aliases(alias_list, args, user_input, alias_cmd, false);
        free(args[0]);

        invoke_history(history, user_input);

        split_str(args, user_input, " \n\t|><&;");
        insert_aliases(alias_list, args, user_input, alias_cmd, true);
        invoke_history(history, user_input);
        add_entry(history, user_input, alias_cmd);
        free(args[0]);

        split_str(args, user_input, " \n\t|><&;");
        exec_cmd(args, handle_cmd(args, history, alias_list));
        free(args[0]);
    }
    
    if(feof(stdin)) printf("\n");
    set_new_path(orig_path);
    save_history(history);
    save_aliases(alias_list);

    free(user_input);
    free(alias_cmd);
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
    strtok(token, separators);

    int size = strlen(token);
    if(token[size - 1] == '\n') token[size - 1] = 0;
    if(!strcmp(token, "")) token = NULL;

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
        char* error_msg = get_entry(history, user_input);

        if(!strcmp(user_input, "")) {
            printf(error_msg);
        }

        free(error_msg);
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
        fprintf(stderr, "Command [%s] failed: %s\n", args[0], strerror(errno));
        _exit(1);
    }
    else if(c_pid > 0){ // parent
        if((pid = wait(&status)) < 0) {
            perror("Error");
            _exit(1);
        }
    }
}

/**
 * Function dedicated to the 'cd' internal command
 * 
 * @param args Array containing the arguments
 */
void changedir_cmd(char** args) {
    if(args[1] == NULL) {
        chdir(getenv("HOME"));
    }
    else if(args[2] == NULL) {
        set_dir(args[1]);
    }
    else {
        printf(TOO_MANY_ARGS "please only enter one path\n");
    }
}

/**
 * Function dedicated to the 'getpath' internal command
 * 
 * @param args Array containing the arguments
 */
void getpath_cmd(char** args) {
    if(args[1] == NULL) {
        printf("PATH: %s\n", getenv("PATH"));
    }
    else {
        printf(TOO_MANY_ARGS "please enter the command without any arguments\n");
    }
}

/**
 * Function dedicated to the 'setpath' internal command
 * 
 * @param args Array containing the arguments
 */
void setpath_cmd(char** args) {
    if(args[1] == NULL) {
        printf(TOO_FEW_ARGS "please enter a path to set\n");
    }
    else if(args[2] == NULL) {
        get_new_path(args[1]);
    }
    else {
        printf(TOO_MANY_ARGS "please only enter one path\n");
    }
}

/**
 * Function dedicated to the 'history' internal command
 * 
 * @param args Array containing the arguments
 * @param history Pointer to the history structure
 */
void history_cmd(char** args, History* history) {
    if(args[1] == NULL) {
        print_history(history);
    }
    else {
        printf(TOO_MANY_ARGS "please enter the command without any arguments\n");
    }
}

/**
 * Function dedicated to the 'alias' internal command
 * 
 * @param args Array containing the arguments to be executed
 * @param alias_list Pointer to the alias structure
 */
void alias_cmd(char** args, Alias_List alias_list) {
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
}

/**
 * Function dedicated to the 'unalias' internal command
 * 
 * @param args Array containing the arguments to be executed
 * @param alias_list Pointer to the alias structure
 */
void unalias_cmd(char** args, Alias_List alias_list) {
    if(args[1] == NULL) {
        printf(TOO_FEW_ARGS "please enter an alias to remove\n");
    }
    else if(args[2] == NULL) {
        remove_alias(alias_list, args[1]);
    }
    else {
        printf(TOO_MANY_ARGS "please enter only one alias to remove\n");
    }
}

/**
* Handle internal commands
*
* @param args Array containing the arguments to be executed
* @param history Pointer to the history structure
* @param alias_list Pointer to the alias structure
* @return Bool value to indicate whether the command should execute externally
*/
bool handle_cmd(char** args, History* history, Alias_List alias_list) {
    if(args[0] == NULL) return true;

    if(strcmp(args[0], "cd") == 0) {
        changedir_cmd(args);
        return true;
    }
    else if(strcmp(args[0], "getpath") == 0) {
        getpath_cmd(args);
        return true;
    }
    else if(strcmp(args[0], "setpath") == 0) {
        setpath_cmd(args);
        return true;
    }
    else if(strcmp(args[0], "history") == 0) {
        history_cmd(args, history);
        return true;
    }
    else if(strcmp(args[0], "alias") == 0) {
        alias_cmd(args, alias_list);
        return true;
    }
    else if(strcmp(args[0], "unalias") == 0) {
        unalias_cmd(args, alias_list);
        return true;
    }

    return false;
}

/**
* Make a copy of the specified arguments
*
* @param args Array containing the arguments to be executed
* @param start Integer which indicates the start point of the array
* @param end Integer which indicates the end point of the array
* @return Buffer containing the arguments split by a space character
*/
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

/**
* Check if the path is a valid directory
*
* @param user_input String containing the user entered path
*/
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

/**
* Sets the specified path to the PATH environment variable
*
* @param new_path String containing the new path
*/
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

/**
* Sets the user entered directory
*
* @param user_input String containing the user entered directory
*/
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

/**
* Gets the last word of the user entered directory
*
* @param user_input String containing the user entered directory
* @return The last word
*/
char* get_last_word(char* user_input) {
    char* word_ptr = malloc(sizeof(char) * BUFFER_SIZE);
    char* path_split[ARG_LIMIT];
    int words_length = split_str(path_split, user_input, "/");

    strcpy(word_ptr, path_split[words_length - 1]);
    return word_ptr;
}

/**
* Replaces the '~' character with the HOME directory of the user
*
* @param path String containing the user entered directory
*/
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
