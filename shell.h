#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h>
#include "history.h"

#define BUFFER_SIZE 512
#define ARG_LIMIT 50

char* get_input(char* user_input);
void get_args(char** args, char* user_input);
void exec_cmd(char** args);
void invoke_history(History* history, char* user_input);