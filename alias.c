#include "alias.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

Alias_List new_alias_list() {
    Alias_List list;
  	list = (Alias_List) malloc(sizeof(Alias_List));
  	*list = NULL;
  	return list;
}

int is_empty(Alias_List list) {
    return *list == NULL;
}

Alias* new_alias(char* alias, char* replacement) {
    Alias* new_alias;
    new_alias = (Alias*) malloc(sizeof(Alias));
    strcpy(new_alias->alias, alias);
    strcpy(new_alias->replacement, replacement);
    new_alias->next = NULL;
    return new_alias;
}

int alias_exists(Alias_List list, char* alias) {
    if(is_empty(list)) {
        return 0;
    }

    Alias* current = *list;
    while(current) {
        if(!strcmp(current->alias, alias))
            return 1;
    }

    return 0;
}

void add_alias(Alias_List list, char* alias, char* replacement) {
    if(alias_exists(list, alias)) {
        printf("%s: alias already exists, to replace this, first try \"unalias %s\"", alias, alias);
        return;
    }

    Alias* new = new_alias(alias, replacement);
    if(is_empty(list)) {
        *list = new;
        return;
    }

    new->next = *list;
    *list = new;
}

void delete_alias(Alias* alias) {
    free(alias);
    alias = NULL;
}

void remove_alias(Alias_List list, char* alias) {
    if(is_empty(list)) {
        return;
    }

    Alias* current = *list;
    if(!strcmp(current->alias, alias)) {
        *list = current->next;
        delete_alias(current);
        return;
    }

    while(current->next) {
        if(!strcmp(current->next->alias, alias)) {
            current->next = current->next->next;
            delete_alias(current->next);
            return;
        }

        current = current->next;
    }

    printf("unalias: alias not found\n");
}

char* insert_alias(Alias* alias, char* command) {
    static char buffer[4096];
    char *p;

    if(!(p = strstr(command, alias->alias)))
        return command;

    strncpy(buffer, command, p-command);
    buffer[p-command] = '\0';

    sprintf(buffer+(p-command), "%s%s", alias->replacement, p+strlen(alias->alias));
    return buffer;
}

void insert_aliases(Alias_List list, char* command) {
    if(is_empty(list) || !strncmp(command, "unalias", 7))
        return;
        
    Alias* current = *list;    
    while(current) {
        strcpy(command, insert_alias(current, command));
        current = current->next;
    }
}

void print_aliases(Alias_List list) {
    if(is_empty(list)) {
        printf("No aliases exist\n");
        return;
    }

    Alias* current = *list;
    while(current) {
        printf("%s: %s\n", current->alias, current->replacement);
        current = current->next;
    }
}

Alias_List load_aliases() {
    
    Alias_List list = new_alias_list();
    char path[255];
    strcpy(path, getenv("HOME"));
    strcat(path, "/.aliases");
    FILE* alias_file = fopen(path, "r");
    
    if(!alias_file) 
        return list;    

    
    char line[1025];
    char alias[512];
    char replacement[512];

    
    while(fgets(line, 1025, alias_file)) {
        sscanf(line, "%s %[^\n]s", alias, replacement);
        add_alias(list, alias, replacement);
    }
    
    fclose(alias_file);
    return list;
}

void save_aliases(Alias_List list) {
    char path[255];
    strcpy(path, getenv("HOME"));
    strcat(path, "/.aliases");
    FILE* alias_file = fopen(path, "w");
    if(!alias_file) 
        return;

    Alias* current = *list;
    while(current) {
        fprintf(alias_file, "%s %s", current->alias, current->replacement); 
        current = current->next;
    }

    fclose(alias_file);
}