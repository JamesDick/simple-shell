typedef struct HISTORY_ENTRY {
    int entry_num;
    char command[512];
    struct HISTORY_ENTRY *next;
} History_Entry;

typedef History_Entry** History;

History create_history();

void add_entry(History history, char* command);

char* get_entry(History history, char* command);

void print_history(History history);
