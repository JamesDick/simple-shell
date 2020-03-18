#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include "history.h"
#include "alias.h"

#define TOO_FEW_ARGS "Too few arguments: "
#define TOO_MANY_ARGS "Too many arguments: "
#define BUFFER_SIZE 512
#define ARG_LIMIT 50

void display_prompt();
void invoke_history(History* history, char* user_input);
void exec_cmd(char** args, bool stop);
void get_new_path(char* user_input);
void set_dir(char* user_input);
void set_new_path(char* new_path);
void replacetilde(char** path);
char* reconstruct_args(char** args, int start, int end);
char* get_input(char* user_input);
char* get_last_word(char* user_input);
int split_str(char** array, char* user_input, char* seperators);
bool handle_cmd(char** args, History* history, Alias_List alias_list);
