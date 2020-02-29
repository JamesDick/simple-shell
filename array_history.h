#define HISTORY_LENGTH = 20;

typedef struct HISTORY_ENTRY {
    int entry_num;
    char command[512];
} History_Entry;

typedef struct HISTORY {
    int front;
    int rear;
    History_Entry entries[20];
} History;

History create_history();

void add_entry(History history, char* command);

char* get_entry(History history, char* command);

void print_history(History history);