#include "alias.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Creates a new alias list.
 * 
 * @return An empty list of aliases
 */
Alias_List new_alias_list() {
    Alias_List list;
  	list = (Alias_List) malloc(sizeof(Alias_List));
  	*list = NULL;
  	return list;
}

/**
 * Checks whether or not the alias list is empty.
 * 
 * @param list The list to be checked
 */
int is_empty(Alias_List list) {
    return *list == NULL;
}

/**
 * Creates and returns a new instance of an alias.
 * 
 * @param alias The aliased command
 * @param replacement The command to be used in place of the alias
 */
Alias* new_alias(char* alias, char* replacement) {
    Alias* new_alias;
    new_alias = (Alias*) malloc(sizeof(Alias));
    strcpy(new_alias->alias, alias);
    strcpy(new_alias->replacement, replacement);
    new_alias->next = NULL;
    return new_alias;
}

/**
 * Checks whether an alias is already present in the list of aliases.
 * 
 * @param list The list of aliases to be searched
 * @param alias The new alias to be searched for
 * @return 1 if the alias is found, otherwise 0
 */
int alias_exists(Alias_List list, char* alias) {
    /* If the list is empty, the alias does not exist */
    if(is_empty(list)) {
        return 0;
    }

    /* Loop through the list, if we find the alias, return 1 */
    Alias* current = *list;
    while(current) {
        if(!strcmp(current->alias, alias))
            return 1;
        current=current->next;
    }

    /* If we didn't find the alias, return 0 */
    return 0;
}

/**
 * Takes in a new alias and command and adds them to the list of aliases.
 * 
 * @param list The list of aliases in which to insert the new one.
 * @param alias The aliased command
 * @param replacement The command to be used in place of the alias
 */
void add_alias(Alias_List list, char* alias, char* replacement) {
    /* If  the alias is already present in the list, give an error */
    if(alias_exists(list, alias)) {
        printf("alias: %s: alias already exists\n(to replace this, first try \"unalias %s\")\n", alias, alias);
        return;
    }

    /* Create a new alias and insert it at the front of the list */
    Alias* new = new_alias(alias, replacement);
    new->next = *list;
    *list = new;
}

/**
 * Deletes an alias from memory
 * 
 * @param alias The alias to be deleted
 */
void delete_alias(Alias* alias) {
    free(alias);
    alias = NULL;
}

/**
 * Searches for an alias in the list and removes it if found.
 * 
 * @param list The list of aliases to be searched
 * @param alias The alias to search for
 */
void remove_alias(Alias_List list, char* alias) {
    /* If the list is empty, there is nothing to remove */
    if(is_empty(list)) {
        return;
    }

    /* If the alias is at the start of the list,
     * set the next alias to be the start of the list 
     * and delete the alias */
    Alias* current = *list;
    if(!strcmp(current->alias, alias)) {
        *list = current->next;
        delete_alias(current);
        return;
    }

    while(current->next) {
        /* If we find the alias anywhere else, remove it from the list
         * by making the current node point to the node after it 
         * and deleting the alias */
        if(!strcmp(current->next->alias, alias)) {
            current->next = current->next->next;
            delete_alias(current->next);
            return;
        }

        current = current->next;
    }

    /* If we didn't find the alias, print an error */
    printf("unalias: %s: alias not found\n", alias);
}

/**
 * Searches a command for instances of a given alias,
 * and swaps out the alias with its replacement if found.
 * 
 * @param alias The alias to search for instances of
 * @param command The command to be searched
 * @return The command, having been altered if the alias was found
 */
char* insert_alias(Alias* alias, char* command) {
    /* Buffer in which the command with the inserted alias will be stored */
    static char buffer[4096];

    /* A pointer to the start of the alias within the command */
    char* alias_start;

    /* If the alias was not found within the command, return it as is */
    if(!(alias_start = strstr(command, alias->alias)))
        return command;

    /* Copy the command up to the start of the alias into the buffer */
    strncpy(buffer, command, alias_start-command);
    buffer[alias_start-command] = '\0';

    /* Copy the replacement for the alias onto the end of the buffer, 
     * followed by the remainder of the original command */
    sprintf(buffer+(alias_start-command), "%s%s", alias->replacement, alias_start+strlen(alias->alias));
    return buffer;
}

/**
 * Searches a command for instances of each known alias, in turn.
 * 
 * @param list The list of aliases to be applied
 * @param command The command to be modified
 */
void insert_aliases(Alias_List list, char* command) {
    /* If the list if empty, there's nothing to alias.
     * If this is an unalias command, it would never work if we applied the alias */
    if(is_empty(list) || !strncmp(command, "unalias", 7))
        return;
        
    Alias* current = *list;    
    while(current) {
        /* Replace the command with the one that has any potential aliases inserted */
        strcpy(command, insert_alias(current, command));
        current = current->next;
    }
}

/**
 * Prints out each alias in a list
 * 
 * @param list The list of aliases to be printed
 */
void print_aliases(Alias_List list) {
    /* If the list is empty, there's nothing to print */
    if(is_empty(list)) {
        printf("No aliases exist\n");
        return;
    }

    Alias* current = *list;
    while(current) {
        /* Print every alias in the format alias: replacement */
        printf("%s: %s\n", current->alias, current->replacement);
        current = current->next;
    }
}

/**
 * Loads a list of aliases from a file
 * called .aliases in the user's home directory
 * 
 * @return The loaded aliases, or a new list of aliases if there was an issue reading from file
 */
Alias_List load_aliases() {
    /* Get a new alias list in which to add the loaded items */
    Alias_List list = new_alias_list();

    /* Get the path to the .aliases file in the user's home directory */
    char path[255];
    strcpy(path, getenv("HOME"));
    strcat(path, "/.aliases");

    /* Open the aliases file in reading mode */
    FILE* alias_file = fopen(path, "r");    

    /* If we couldn't open the file, just return the empty list */
    if(!alias_file) 
        return list;    
    
    /* Buffer in which each line from the file is held */
    char line[1025];

    /* Buffer in which the alias from each line is held */
    char alias[512];

    /* Buffer in which the replacement command from each line is held */
    char replacement[512];

    /* Read each line of the file, 
     * then store the alias and replacement commands 
     * and add them to the list */
    while(fgets(line, 1025, alias_file)) {
        sscanf(line, "%s %[^\n]s", alias, replacement);
        add_alias(list, alias, replacement);
    }
    
    /* Once we're done, close the file and return the list */
    fclose(alias_file);
    return list;
}

/**
 * Saves the list of aliases to a file
 * called .aliases in the user's home directory
 * 
 * @param list The list of aliases to be saved
 */
void save_aliases(Alias_List list) {

    /* Get the path to the .aliases file in the user's home directory */
    char path[255];
    strcpy(path, getenv("HOME"));
    strcat(path, "/.aliases");

    /* Open the aliases file in writing mode */
    FILE* alias_file = fopen(path, "w");

    /* If we couldn't open the file, just return */
    if(!alias_file) 
        return;

    /* Add each alias and replacement command
     * as a new line in the file */
    Alias* current = *list;
    while(current) {        
        fprintf(alias_file, "%s %s", current->alias, current->replacement); 
        current = current->next;
    }

    /* Once we're done, close the file */
    fclose(alias_file);
}