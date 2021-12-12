#ifndef HEADER_CLIENT_H
#define HEADER_CLIENT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// socket headers
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// close()
#include <unistd.h>

// thread
#include <pthread.h>

#include "pecel_utils.h"
#include "cmd.h"
#include "database.h"

struct client {
    unsigned int sock_fd;
    struct sockaddr* sock_client;
};

struct client* init_client(unsigned int sock_fd, struct sockaddr* sock_client);
void destroy_client(struct client* c);

int write_text(int sock_fd, const char* msg);

void* client_handler(void* args);

#endif