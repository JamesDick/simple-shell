typedef struct HISTORY_ENTRY {
    int entry_num;
    char command[512];
} History_Entry;

typedef struct HISTORY {
    int front;
    int rear;
    int entry_count;
    History_Entry entries[21];
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
void add_entry(History* history, char* command);

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
 */
void get_entry(History* history, char* command);

/**
 * Prints out each entry in the history,
 * with the format entry_num: command.
 * 
 * @param history The struct containing the history
 */
void print_history(History* history);