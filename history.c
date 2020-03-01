#include "history.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    history->rear = ++history->rear % 21;
    if(history->rear == history->front) 
        history->front = ++history->front % 21;
}

/**
 * Looks for an entry in history with the given entry num,
 * and returns it if found. Otherwise returns an empty string.
 */
char* get_at(History* history, int entry_num) {
    for(int i = history->front; i != history->rear; i = ++i % 21) 
        if(history->entries[i].entry_num == entry_num) 
            return history->entries[i].command;       

    return "";     
}

void get_entry(History* history, char* command) {
    int target = -1;
    /**
     * Assign the target entry number based on the provided command.
     * Then override the command with the one from the target entry.
     * If the entry is not found the command will be an empty string.
     */
    if(!strncmp(command, "!!", 2))
        target = history->entry_count;          
    else if(!strncmp(command, "!-", 2))
        target = history->entry_count + 1 - atoi(command+2);      
    else if(!strncmp(command, "!", 1))
        target = atoi(command+1);             
    strcpy(command, get_at(history, target));   
}

void print_history(History* history) {
    /** Start at the front and print each entry until we reach the rear. */
    for(int i = history->front; i != history->rear; i = ++i % 21) 
        printf("%d: %s", history->entries[i].entry_num, history->entries[i].command);    
}