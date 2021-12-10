#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "type.h"
#include "config.h"
#include "pecel_utils.h"

int load_conf(const char* conf_file, struct config* conf) {
    FILE* f = fopen(conf_file, "r");
    if (f == NULL)
        return -1;

    char* line = NULL;
    size_t len = 0;

    int index;
    char port_str[5];

    while(getline(&line, &len, f) != -1) {
        
        char** out = (char**) malloc(2 * sizeof(char*));
        extract_line_val(line, "= ", out);

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