#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * The number of commands to be stored in the history.
 * Value is one higher to ease the use of the circular array.
 */
#define HISTORY_LENGTH 21

/**
 * Represents an entry in the history.
 * Holds an entry number and its associated command.
 */
typedef struct HISTORY_ENTRY {
    int entry_num;
    char command[512];
} History_Entry;

/**
 * Represents the history of shell commands.
 */
typedef struct HISTORY {
    int front;
    int rear;
    int entry_count;
    History_Entry entries[HISTORY_LENGTH];
} History;

/**
 * Loads saved history of last 20 commands from file.
 * If no saved history is present, a new history is created.
 */
History* load_history();

/**
 * Saves given command history to file.
 * This will overwrite and previous saved history.
 * 
 * @param history The command history to be saved.
 */
void save_history(History* history);

/**
 * Adds a command as a new entry in History.
 * 
 * @param history The struct containing the history
 * @param command The command to be inserted as an entry in the history
 */
void add_entry(History* history, char* command, char* alias_cmd);

/**
 * Reads a command to identify one of three commands to search the History,
 * then replaces the command with the value of the found History entry.
 * If no entry is identified, the command will be an empty string.
 * 
 * Commands:
 * !! - Looks for the most recent command
 * !-x - Looks for xth most recent command
 * !x - Looks for the command with entry number x
 * 
 * @param history The struct containing the history
 * @param command The command which will be replaced with the history entry
 * @return The error message
 */
char* get_entry(History* history, char* command);

bool set_error_msg(History* history, char* command, char* error_msg, char* invoke_type);

bool has_letter(char* input, char* invoke_type, char* trim_zeros);

/**
 * Prints out each entry in the history,
 * with the format entry_num: command.
 * 
 * @param history The struct containing the history
 */
void print_history(History* history);
