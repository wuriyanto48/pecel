
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "type.h"
#include "server.h"
#include "config.h"
#include "database.h"
#include "cmd.h"

void graceful_handler(int sig);

int main(int argc, char** argv) 
{
    if (argc < 2)
        EXIT_ERR(1, "require config file");
    
    char* conf_file = argv[1];
    struct config conf;
    int load_conf_r = load_conf(conf_file, &conf);
    if (load_conf < 0)
        EXIT_ERR(1, "error load config file");
    
    printf("host: %lu | %s\n", strlen(conf.host), conf.host);
    printf("port: %d\n", conf.port);
    printf("auth: %d\n", conf.auth);
    printf("pass: %lu | %s\n", strlen(conf.pass), conf.pass);

    // signal
    signal(SIGTERM, graceful_handler);
    signal(SIGINT, graceful_handler);

    printf("start server\n");
    char* host = conf.host;
    unsigned short port = conf.port;

    // init server
    srv = init_server(host, port, AF_INET, SOCK_STREAM, 5);
    if (srv == NULL)
        EXIT_ERR(-1, "init server error");

    // init database
    int init_db_r = init_database();
    if (init_db_r < 0) {
        destroy_server(srv);
        EXIT_ERR(-1, "init database error");
    }

    // init command
    int init_cmd_r = init_cmd();
    if (init_cmd_r < 0) {
        destroy_server(srv);
        destroy_database();
        EXIT_ERR(-1, "init cmd error");
    }

    // start to listen
    int listen_r = listen_server(srv);
    if (listen_r == -1) {
        destroy_server(srv);
        destroy_database();
        destroy_cmd();
        EXIT_ERR(-1, "start to listen error");
    }

    // start to accepting client
    (void) accept_server(srv);

    // accept_server() stopped or returned errors
    // handle resources
    destroy_server(srv);
    destroy_database();
    destroy_cmd();

    return 0;
}

void graceful_handler(int sig) 
{
    printf("graceful_handler | received signal: %d\n", sig);
    server_stop = TRUE;
    destroy_server(srv);
    destroy_database();
    destroy_cmd();

    printf("shutdown server...\n");
    exit(1);
}