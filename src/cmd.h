#ifndef HEADER_CMD_H
#define HEADER_CMD_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "type.h"

#define CMD_SIZE 4

enum command { 
    // SET: set value with key to the database
    SET = 1, 
    // GET: get value with key from database
    GET = 2,
    // REM: remove value with key from database
    REM = 3,
    // ATH: authenticating to the database
    ATH = 4
};

struct command_s {
    enum command c;
    char* c_s;

    struct command_s* next;
};

int init_cmd();
void destroy_cmd();
struct command_s* cmd_get(const char* c_key);

#endif