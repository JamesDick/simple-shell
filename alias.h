#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * Entry in a list of aliases.
 * Holds a string for the alias and the command which replaces it.
 * Also holds a pointer to the next alias in the list.
 */
typedef struct Alias {
    char alias[512];
    char replacement[512];
    bool used;
    struct Alias* next;
} Alias;

/**
 * Pointer to the first in a list of aliases.
 * Pointer to null if the list is empty.
 */
typedef Alias** Alias_List;

void add_alias(Alias_List list, char* alias, char* replacement);

void remove_alias(Alias_List list, char* alias);

char* insert_alias(Alias* alias, char** args, char* user_input, char* command, char* alias_cmd);

/**
 * Looks through a command and replaces instances of an alias with its replacement
 * @param list The list of aliases
 * @param args The command and its arguments split into words
 * @param user_input The raw line read from the user
 * @param alias_cmd The alias and its arguments to be stored
 * @param quiet Bool value which determines whether certain messages are to be printed
 */
void insert_aliases(Alias_List list, char** args, char* user_input, char* alias_cmd);

void print_aliases(Alias_List list);

Alias_List load_aliases();

void save_aliases(Alias_List list);

void reset_aliases(Alias_List list);
