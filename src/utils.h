#ifndef HEADER_UTILS_H
#define HEADER_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "type.h"

int extract_line_size(FILE* f) {
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

void remove_linefeed(char* line) {
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == 0xA)
        line[len-1] = 0;
}

char** extract_line_val(char* line, char* delim) {
    remove_linefeed(line);

    char** out = (char**) malloc(2 * sizeof(char*));
    char* conf_token = strtok(line, delim);
    int index;
    while (conf_token != NULL) {
        out[index] = conf_token;
        
        conf_token = strtok(NULL, delim);
        index++;
    }

    return out;
}

#endif