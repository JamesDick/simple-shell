#include "shell.h"

int main() {
    char user_input[BUFFER_SIZE];
    char* args[ARG_LIMIT];
    char* orig_path = getenv("PATH");
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
    set_new_path(orig_path);
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

int split_path(char** args, char* user_input) {
    char* token = strtok(user_input, "/");
    int arg_count = 0;
    while(token) {
        args[arg_count++] = token;
        token = strtok(NULL, "/");
    }
    args[arg_count] = NULL;
    return arg_count;
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

    if(strcmp(args[0], "getpath") == 0) {
        if(args[1] == NULL) {
            display_path();
        }
        else {
            printf(TOO_MANY_ARGS "please enter the command without any arguments\n");
        }

        return true;
    }

    if(strcmp(args[0], "setpath") == 0) {
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

    if(strcmp(args[0], "history") == 0) {
        if(args[1] == NULL) {
            print_history(history);
        }
        else {
            printf(TOO_MANY_ARGS "please enter the command without any arguments\n");
        }

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
            printf("New PATH: %s\n", getenv("PATH"));
            return 0;
        }
        else {
            perror("Error");
            return -1;
        }
    }
}

void display_path() {
    char* path = getenv("PATH");
    printf("PATH: %s\n", path);
}

void set_dir(char* user_input) {
    char path_buffer[BUFFER_SIZE] = "";
    strcat(path_buffer, user_input);

    char* ready_path_ptr = path_buffer;

    if(strncmp(path_buffer, "~", 1) == 0) {
        ready_path_ptr = replacetilde(user_input);
    }

    if(chdir(ready_path_ptr) == -1) {
        char* prefix = "cd:";
        int errnum = errno;
        char* error_msg = strerror(errnum);

        if(errnum == 20) {
            char* file_name = get_last_word(user_input);
            printf("%s %s: %s\n", prefix, file_name, error_msg);
            return;
        }

        printf("%s %s\n", prefix, error_msg);
    }
}

char* get_last_word(char* user_input) {
    char word_buffer[BUFFER_SIZE] = "";
    char* path_split[ARG_LIMIT];
    int args_length = split_path(path_split, user_input);
    strcat(word_buffer, path_split[args_length - 1]);

    char* word_buffer_ptr = word_buffer;
    return word_buffer_ptr;

}

char* replacetilde(char* path) {
    char* path_split[ARG_LIMIT];
    int args_length = split_path(path_split, path);

    char ready_path[BUFFER_SIZE] = "";
    path_split[0] = getenv("HOME");

    for(int i = 0; i < args_length; i++) {
        strcat(ready_path, path_split[i]);
        if(i == args_length - 1) break;
        strcat(ready_path, "/");
    }

    char* ready_path_ptr = ready_path;
    return ready_path_ptr;
}
