
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "type.h"
#include "server.h"
#include "config.h"

void graceful_handler(int sig);

int main(int argc, char** argv) {
    if (argc < 2)
        EXIT_ERR(1, "require config file");
    
    char* conf_file = argv[1];
    struct config conf;
    int load_conf_r = load_conf(conf_file, &conf);
    if (load_conf < 0)
        EXIT_ERR(1, "error load config file");

    // signal
    signal(SIGTERM, graceful_handler);
    signal(SIGINT, graceful_handler);

    printf("start server\n");
    char* host = conf.host;
    unsigned short port = conf.port;

    srv = init_server(host, port, AF_INET, SOCK_STREAM, 5);
    if (srv == NULL)
        EXIT_ERR(-1, "init server error");

    // start to listen
    int listen_r = listen_server(srv);
    if (listen_r == -1) {
        destroy_server(srv);
        EXIT_ERR(-1, "start to listen error");
    }

    // start to accepting client
    (void) accept_server(srv);

    return 0;
}

void graceful_handler(int sig) {
    printf("graceful_handler | received signal: %d\n", sig);
    server_stop = TRUE;
    destroy_server(srv);

    printf("shutdown server...\n");
    exit(1);
}