#ifndef HEADER_CONFIG_H
#define HEADER_CONFIG_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "type.h"
#include "config.h"
#include "pecel_utils.h"

struct config {
    char host[16];
    unsigned short port;
};

int load_conf(const char* conf_file, struct config* conf);
    
#endif