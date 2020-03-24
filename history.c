#include "history.h"

History* create_history() {
    /**
     * Allocate memory for the history and
     * initialize all counters to 0. 
     */
    History* history = malloc(sizeof(History));
    history->front = 0;
    history->rear = 0;
    history->entry_count = 0;
    return history;
}

void add_entry(History* history, char* command, char* alias_cmd) {
    char cmd[512];
    /**
     * Don't add nothing to the History.
     * Non-Empty commands that are garbage are allowed. 
     */
    if(!strcmp("", command) || !strcmp(" ", command) || !strcmp("\n", command))
        return;

    if(strcmp(alias_cmd, ""))
        strcpy(cmd, alias_cmd);
    else
        strcpy(cmd, command);

    /**
     * Increment the entry counter,
     * update the rear of the history with the new entry num
     * and the given command.
     */
    history->entry_count++;
    history->entries[history->rear].entry_num = history->entry_count;
    strcpy(history->entries[history->rear].command, cmd);

    /**
     * Move the rear forward, and if it hits the end of the array, reset the rear to zero.
     * If rear and front are equal, do the same with front.
     */
    history->rear = ++history->rear % HISTORY_LENGTH;
    if(history->rear == history->front) 
        history->front = ++history->front % HISTORY_LENGTH;
}

/**
 * Adds a history entry with an existing entry num.
 * This is intended to be used when loading history from file.
 */
void add_existing_entry(History* history, int entry_num, char* command) {
    /**
     * Set the entry count of the history to be that of the given entry.
     * If working as intended entries will be entered in ascending numerical order so this should be accurate.
     */    
    history->entry_count = entry_num;
    history->entries[history->rear].entry_num = entry_num;
    strncat(command, "\n", 2);
    strcpy(history->entries[history->rear].command, command);

    /**
     * Move the rear forward, and if it hits the end of the array, reset the rear to zero.
     * If rear and front are equal, do the same with front.
     */
    history->rear = ++history->rear % HISTORY_LENGTH;
    if(history->rear == history->front) 
        history->front = ++history->front % HISTORY_LENGTH;
}

/**
 * Looks for an entry in history with the given entry num.
 * If no entry was found, returns an empty string.
 */
char* get_at(History* history, int entry_num) {
    for(int i = history->front; i != history->rear; i = ++i % HISTORY_LENGTH) 
        if(history->entries[i].entry_num == entry_num) 
            return history->entries[i].command;       

    return "";     
}

char* get_entry(History* history, char* command) {
    char* error_msg = malloc(sizeof(char) * 512);
    strcpy(error_msg, "Unknown error\n");
    /**
     * Assign the target entry number based on the provided command.
     * Then override the command with the one from the target entry.
     * If the entry is not found the command will be an empty string.
     */
    int target = -1;
    if(!strcmp(command, "!\n")) {
        strcpy(error_msg, "Please enter a number after '!'\n");
    }
    else if(!strncmp(command, "!!", 2)) {
        target = history->entry_count;
    }     
    else if(!strncmp(command, "!-", 2)) {
        if(set_error_msg(command, error_msg, "negative"))
            return error_msg;
        target = history->entry_count + 1 - atoi(command+2);
    }
    else if(!strncmp(command, "!", 1)) {
        if(set_error_msg(command, error_msg, "positive"))
            return error_msg;
        target = atoi(command+1);
    }

    if(history->entries[0].entry_num == 0)
    {
        strcpy(error_msg, "History is empty\n");
    }

    strcpy(command, get_at(history, target));
    return error_msg;
}

bool set_error_msg(char* command, char* error_msg, char* invoke_type) {
    char convert_to_number[512];
    char* trim_zeros = malloc(sizeof(char) * 512);
    strcpy(trim_zeros, "");

    bool not_numbers = has_letter(command, invoke_type, trim_zeros);

    if(!strcmp(invoke_type, "positive"))
        snprintf(convert_to_number, sizeof(convert_to_number), "%d", atoi(command+1));
    else
        snprintf(convert_to_number, sizeof(convert_to_number), "%d", atoi(command+2));
    
    strcat(convert_to_number, "\n");

    if(not_numbers == true) {
        strcpy(error_msg, "Invocations can only be numbers\n");
        strcpy(command, "");
        free(trim_zeros);
        return true;
    }
    else if(not_numbers == false && strcmp(convert_to_number, trim_zeros)) {
        strcpy(error_msg, "Number too large please enter a smaller number\n");
    }
    else if(not_numbers == false) {
        if(!strcmp(invoke_type, "positive"))
            strcpy(error_msg, "Positive invocation failed: out of range\n");
        else
            strcpy(error_msg, "Negative invocation failed: out of range\n");
    }
    if(!strcmp(convert_to_number, "0\n")) {
        strcpy(error_msg, "Please enter a number bigger than zero\n");
    }

    free(trim_zeros);
    return false;
}

bool has_letter(char* input, char* invoke_type, char* trim_zeros) {
    char* control = malloc(sizeof(char) * 512);
    bool not_numbers = false;
    bool stop = false;
    int trim = 0;

    if(!strcmp(invoke_type, "negative"))
        strcpy(control, input+2);
    else if(!strcmp(invoke_type, "positive"))
        strcpy(control, input+1);
    else
        return not_numbers;

    for(int i = 0; i < strlen(control) - 1; i++) {
        char first_char[512];
        char* control_ptr = control+i;
        snprintf(first_char, sizeof(first_char), "%c", control_ptr[0]);
        int target = atoi(first_char);

        if(strcmp(first_char, "0")) {
            stop = true;
        }

        if(stop == false) {
            trim++;
        }

        if(target == 0 && strcmp(first_char, "0"))
            not_numbers = true;
    }

    if(!strcmp(invoke_type, "negative"))
        strcpy(trim_zeros, input+trim+2);
    else if(!strcmp(invoke_type, "positive"))
        strcpy(trim_zeros, input+trim+1);

    free(control);
    return not_numbers;
}

void print_history(History* history) {
    /** 
     * Start at the front and print each entry until we reach the rear. 
     */
    for(int i = history->front; i != history->rear; i = ++i % HISTORY_LENGTH) 
        printf("%d: %s", history->entries[i].entry_num, history->entries[i].command);    
}

History* load_history() {    
    /**
     * Create a new history structure.
     * Open a connection to read from the history file.
     */
    History* history = create_history();
    char path[255];
    strcpy(path, getenv("HOME"));
    strcat(path, "/.hist_list");
    FILE* hist_file = fopen(path, "r");

    /**
     * If the history file wasn't able to be opened,
     * just return the fresh history we made.
     */
    if(!hist_file) 
        return history;    

    /**
     * Create buffers to store each line of the file,
     * and the number and command of each entry.
     */
    char line[517];
    char entry_num[4];
    char command[514];

    /**
     * Scan each line of the file and add 
     * the command to history.
     */
    while(fgets(line, 515, hist_file)) {
        sscanf(line, "%s %[^\n]s", entry_num, command);
        add_existing_entry(history, atoi(entry_num), command);
    }

    /**
     * Close the connection to the file and return the history.
     */
    fclose(hist_file);
    return history;
}

void save_history(History* history) {
    /**
     * Open a connection to write to the history file.
     * If no connection was established, return.
     */
    char path[255];
    strcpy(path, getenv("HOME"));
    strcat(path, "/.hist_list");
    FILE* hist_file = fopen(path, "w");
    if(!hist_file) 
        return;

    /**
     * Write the entry num and command 
     * from each history entry to the history file.
     */
    for(int i = history->front; i != history->rear; i = ++i % HISTORY_LENGTH) 
        fprintf(hist_file, "%d %s", history->entries[i].entry_num, history->entries[i].command); 

    /**
     * Close the connection to the file.
     */
    fclose(hist_file);
}
