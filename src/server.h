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

#include "type.h"
#include "pecel_utils.h"
#include "client.h"
#include "config.h"
#include "database.h"

#define ERROR_LISTEN -1
#define ERROR_ACCEPT -2

// server callback typedef
typedef void (*on_success_cb) (char*, unsigned short);
typedef void (*on_error_cb) (int);

struct server {
    struct addrinfo *serviceinfo;
    unsigned int server_fd;
    unsigned int s_family;
    unsigned int s_type;
    struct config* conf;
    unsigned int max_conn_queue;
    struct database* database;

    // server callback
    on_success_cb on_success_listen;
    on_error_cb on_error_listen;
    on_error_cb on_error_accept;
};

struct server* init_server(struct config* conf, unsigned int s_family, 
    unsigned int s_type, unsigned int max_con_queue);

int listen_server(struct server* s);
void accept_server(struct server* s);
void destroy_server(struct server* s);

// global var
extern struct server* srv;

extern volatile sig_atomic_t server_stop;

#endif