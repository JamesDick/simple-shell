typedef struct ALIAS {
    char alias[512];
    char replacement[512];
    struct ALIAS* next;
} Alias;

typedef Alias** Alias_List;

Alias_List new_alias_list();

void add_alias(Alias_List list, char* alias, char* replacement);

void remove_alias(Alias_List list, char* alias);

/** 
 * Looks through a command and replaces instances of an alias with its replacement 
 * @param alias_list The list of aliases
 * @param command The command to be modified
 */
void insert_aliases(Alias_List list, char* command);

void print_aliases(Alias_List list);