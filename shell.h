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

#define BUFFER_SIZE 512
#define ARG_LIMIT 50

char* get_input(char* user_input);
void display_prompt();
void invoke_history(History* history, char* user_input);
void get_args(char** args, char* user_input);
void exec_cmd(char** args, bool stop);
void get_new_path(char* user_input);
void display_path();
int set_new_path(char* new_path);
bool handle_cmd(char** args, History* history);