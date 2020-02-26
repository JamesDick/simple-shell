#include "history.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

History create_history() {
    History history;
    history = (History) malloc(sizeof(History));
    *history = NULL;
    return history;
}

int is_empty(History history) {
    return *history == NULL;
}

int size(History history) {
    if (is_empty(history)) {
        return 0;
    }

    History_Entry *current = *history;
    int size = 1;
    while(current->next) {
        current = current->next;
        size++;
    }

    return size;
}

int entry_exists(History history, int index) {
    if(is_empty(history)) {
        return 0;
    }

    History_Entry* current = *history;
    if(index < current->entry_num || index >= current->entry_num + size(history)) {
        return 0;
    }

    return 1;
}

History_Entry* new_entry(char* command) {
    History_Entry* new_entry;
    new_entry = (History_Entry*) malloc(sizeof(History_Entry));
    strcpy(new_entry->command, command);
    new_entry->next = NULL;
    return new_entry;
}

void delete_entry(History_Entry* entry) {
    free(entry);
    entry = NULL;
}

void remove_head(History history) {
    if(is_empty(history)) {
        return;
    }

    History_Entry* current = *history;
    *history = current->next;
    delete_entry(current);
}

void add_entry(History history, char* command) {
    History_Entry* entry = new_entry(command);
    if(is_empty(history)) {
        entry->entry_num = 1;
        *history = entry;        
        return;
    }

    History_Entry* current = *history;
    while(current->next) {
        current = current->next;
    }

    entry->entry_num = (current->entry_num) + 1;
    current->next = entry;  

    if(size(history) > 20) {
        remove_head(history);
    }  
}

char* get_at(History history, int index) {
    if(!entry_exists(history, index)) {
        return NULL;
    }

    History_Entry* current = *history;
    for(int i = current->entry_num; i < index; i++) {
        current=current->next;
    }
    
    return current->command;
}



char* get_entry(History history, char* command) {
    if(!strncmp(command, "!!", 2)) { 
        strcpy(command, get_at(history, size(history)));
    }
    else if(!strncmp(command, "!-", 2)) { 
        strcpy(command, get_at(history, (size(history) - atoi(command+2))));
    }
    else if(!strncmp(command, "!", 1)) { 
        strcpy(command, get_at(history, atoi(command+1)));           
    }
}

void print_history(History history) {
    if(is_empty(history)) {
        return;
    }

    History_Entry* current = *history;
    while(current) {
        printf("%d: %s", current->entry_num, current->command);
        current = current->next;
    }
}