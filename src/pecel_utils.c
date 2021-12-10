#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "type.h"
#include "pecel_utils.h"

int extract_line_size(FILE* f) 
{
    int line_size = 0;
    char* line = NULL;
    size_t len = 0;
     while(getline(&line, &len, f) != -1) {
         line_size = line_size + 1;
     }

    // move pointer to the beginning
    fseek(f, 0, SEEK_SET);
    return line_size;
}

void remove_linefeed(char* line) 
{
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == 0xA)
        line[len-1] = 0;
}

void remove_carriage_ret(char* line)
{
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == 0xD)
        line[len-1] = 0;
}

int extract_line_val(char* line, char* delim, char** out) 
{
    remove_linefeed(line);
    remove_carriage_ret(line);

    char* conf_token = strtok(line, delim);
    int index;
    while (conf_token != NULL) {
        out[index] = conf_token;
        conf_token = strtok(NULL, delim);
        index++;
    }

    return 0;
}

char* to_lower(char* s)
{
    for(char* p=s; *p; p++) *p=tolower(*p);
    return s;
}

char* to_upper(char* s)
{
    for(char* p=s; *p; p++) *p=toupper(*p);
    return s;
}