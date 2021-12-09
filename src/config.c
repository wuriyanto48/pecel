#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "type.h"
#include "config.h"

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

char** extract_conf_val(char* line) {
    remove_linefeed(line);

    char** out = (char**) malloc(2 * sizeof(char*));
    char* conf_token = strtok(line, "= ");
    int index;
    while (conf_token != NULL) {
        out[index] = conf_token;
        
        conf_token = strtok(NULL, "= ");
        index++;
    }

    return out;
}

int load_conf(const char* conf_file, struct config* conf) {
    FILE* f = fopen(conf_file, "r");
    if (f == NULL)
        return -1;

    char* line = NULL;
    size_t len = 0;

    int index;
    char port_str[5];

    while(getline(&line, &len, f) != -1) {
        
        char** out = extract_conf_val(line);

        if (strcmp("host", out[0]) == 0)
            strcpy(conf->host, out[1]);

        if (strcmp("port", out[0]) == 0)
            strcpy(port_str, out[1]);

        free(out);
    }

    conf->port = atoi(port_str);

    fclose(f);
    free(line);

    return 0;
}