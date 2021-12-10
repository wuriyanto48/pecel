#ifndef HEADER_SERVER_H
#define HEADER_SERVER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

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

struct server {
    struct addrinfo *serviceinfo;
    unsigned int server_fd;
    unsigned int s_family;
    unsigned int s_type;
    char* host;
    unsigned short port;
    unsigned int max_conn_queue;
};

struct client {
    unsigned int sock_fd;
    struct sockaddr* sock_client;
};

struct server* init_server(char* host, unsigned short port, 
    unsigned int s_family, unsigned int s_type, unsigned int max_con_queue);

int listen_server(struct server* s);
void accept_server(struct server* s);
void destroy_server(struct server* s);

struct client* init_client(unsigned int sock_fd, struct sockaddr* sock_client);
void destroy_client(struct client* c);

int write_text(int sock_fd, const char* msg);

void* handle_client(void* args);

// global var
extern struct server* srv;

extern volatile sig_atomic_t server_stop;

#endif