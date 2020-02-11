typedef struct HISTORY_ENTRY {
    int entry_num;
    char command[512];
    struct HISTORY_ENTRY *next;
} History_Entry;

typedef History_Entry** History;

History create_history();

int is_empty(History history);

int size(History history);

void push(History history, char* command);

char *get_at(History history, int index);

void print_history(History history);
