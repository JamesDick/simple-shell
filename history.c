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

void add_entry(History* history, char* command) {
    /**
     * Don't add nothing to the History.
     * Non-Empty commands that are garbage are allowed. 
     */
    if(!strcmp("", command) || !strcmp(" ", command) || !strcmp("\n", command))
        return;

    /**
     * Increment the entry counter,
     * update the rear of the history with the new entry num
     * and the given command.
     */
    history->entry_count++;
    history->entries[history->rear].entry_num = history->entry_count;
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

void get_entry(History* history, char* command) {
    /**
     * Assign the target entry number based on the provided command.
     * Then override the command with the one from the target entry.
     * If the entry is not found the command will be an empty string.
     */
    int target = -1;
    if(!strncmp(command, "!!", 2))
        target = history->entry_count;          
    else if(!strncmp(command, "!-", 2))
        target = history->entry_count + 1 - atoi(command+2);      
    else if(!strncmp(command, "!", 1))
        target = atoi(command+1);             
    strcpy(command, get_at(history, target));   
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
