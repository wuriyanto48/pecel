#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "type.h"
#include "cmd.h"
#include "pecel_utils.h"

static struct command_s* COMMANDS[CMD_SIZE];

static char* cmd_dup(const char* s)
{
    char* res;
    res = (char*) malloc(strlen(s) + 1); // + 1 for NULL terminator '\0'
    if (res != NULL)
        strcpy(res, s);
    return res;
}

unsigned int command_hash(const char* cmd_key) 
{
    unsigned int hashval;
    for (hashval = 0; *cmd_key != '\0'; cmd_key++)
        hashval = *cmd_key + 31 * hashval;
    return (hashval % CMD_SIZE);
}

static struct command_s* cmd_insert(char* c_key, enum command val)
{
        
    struct command_s* res;
    unsigned int hashval;

    if ((res = cmd_get(c_key)) == NULL) { // not found
        res = (struct command_s*) malloc(sizeof(*res));
        if (res == NULL || (res->c_s = cmd_dup(c_key)) ==  NULL)
            return NULL;
        hashval = command_hash(c_key);
        res->next = COMMANDS[hashval];
        COMMANDS[hashval] = res;
    } else // key found
        res->c = 0;
    res->c = val;
    return res;
}

struct command_s* cmd_get(char* c_key)
{
    char* key = (char*) malloc(strlen(c_key) + 1);
    if (key == NULL)
        return NULL;
    strcpy(key, c_key);
    key = to_upper(key);
    
    struct command_s* data;
    for (data = COMMANDS[command_hash(key)]; data != NULL; data = data->next) {
        if (strcmp(key, data->c_s) == 0) {
            free((void*) key);
            return data;
        }
    }

    free((void*) key);
    return NULL;
}

int init_cmd(void) 
{
    if(cmd_insert("SET", SET) == NULL)
        return -1;
    if(cmd_insert("GET", GET) == NULL)
        return -1;
    if(cmd_insert("REM", REM) == NULL)
        return -1;
    if(cmd_insert("ATH", ATH) == NULL)
        return -1;
    return 0;
}

void destroy_cmd(void) 
{
    for (int i = 0; i < CMD_SIZE; i++) {
        struct command_s* c = COMMANDS[i];
        if (c != NULL)
            free((void*) c);
    }
}