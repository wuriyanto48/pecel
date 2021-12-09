#ifndef HEADER_CONFIG_H
#define HEADER_CONFIG_H

struct config {
    char host[16];
    unsigned short port;
};

int load_conf(const char* conf_file, struct config* conf);
    
#endif