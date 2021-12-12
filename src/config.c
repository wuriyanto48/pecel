#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "type.h"
#include "config.h"
#include "pecel_utils.h"

int load_conf(const char* conf_file, struct config* conf) 
{
    FILE* f = fopen(conf_file, "r");
    if (f == NULL)
        return -1;

    char* line = NULL;
    size_t len = 0;

    int index;
    char port_str[5];
    char auth_str[6];

    while(getline(&line, &len, f) != -1) {
        
        char** out = (char**) malloc(2 * sizeof(char*));
        if (out == NULL)
            return -1;

        extract_line_val(line, "= ", out);

        if (strcmp("host", out[0]) == 0)
            strcpy(conf->host, out[1]);

        if (strcmp("port", out[0]) == 0)
            strcpy(port_str, out[1]);
        
        if (strcmp("auth", out[0]) == 0)
            strcpy(auth_str, out[1]);
        
        if (strcmp("pass", out[0]) == 0)
            strcpy(conf->pass, out[1]);

        free((void*) out);
    }

    conf->port = atoi(port_str);

    if (strcmp("TRUE", to_upper(auth_str)) == 0) {
        conf->auth = 1;
    } else 
        conf->auth = 0;

    fclose(f);
    free((void*) line);

    return 0;
}