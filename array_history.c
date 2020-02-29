#include "array_history.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

History* create_history() {
    History* history = malloc(sizeof(History));
    history->front = 0;
    history->rear = 0;
    history->entry_count = 0;
    return history;
}

int is_empty(History* history) {
    return history->entry_count = 0;
}

void add_entry(History* history, char* command) {
    History_Entry new_entry = {++(history->entry_count), command};
    history->entries[history->rear] = new_entry;
    history->rear = ++(history->rear) % 20;
    if(history->rear == history->front) 
        history->front = ++(history->front) % 20;
}

char* get_at(History* history, int entry_num) {
    if((history->entry_count) - 20 > entry_num || entry_num > history->entry_count) 
        return NULL;    

    for(int i = history->front; i != history->rear; i = ++i % 20) 
        if(history->entries[i].entry_num == entry_num) 
            return history->entries[i].command;       

    return NULL;     
}

char* get_entry(History* history, char* command) {
    if(!strncmp(command, "!!", 2)) 
        strcpy(command, get_at(history, history->entry_count));
    
    else if(!strncmp(command, "!-", 2)) 
        strcpy(command, get_at(history, (history->entry_count - atoi(command+2))));
    
    else if(!strncmp(command, "!", 1)) 
        strcpy(command, get_at(history, atoi(command+1)));              
}

void print_history(History* history) {
    if(is_empty(history)) {
        printf("No entries in history\n");
        return;
    }

    for(int i = history->front; i != history->rear; i = ++i % 20) 
        printf("%d: %s\n", history->entries[i].entry_num, history->entries[i].command);    
}