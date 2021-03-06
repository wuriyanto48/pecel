
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "type.h"
#include "server.h"
#include "config.h"
#include "database.h"
#include "cmd.h"

void graceful_handler(int sig);
void on_listen_server(char* host, unsigned short port);
void on_error_accept_server(int err);

struct database* database;

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
    srv = init_server(&conf, AF_INET, SOCK_STREAM, 5);
    if (srv == NULL)
        EXIT_ERR(-1, "init server error");
    
    // set server callback
    srv->on_success_listen = on_listen_server;
    srv->on_error_accept = on_error_accept_server;

    // init database
    int init_db_r = init_database_map();
    if (init_db_r < 0) {
        destroy_server(srv);
        EXIT_ERR(-1, "init database map error");
    }

    database = init_database();
    if (database == NULL) {
        destroy_server(srv);
        destroy_database_map();
        EXIT_ERR(-1, "init database error");
    }

    // set database to server
    srv->database = database;

    // init command
    int init_cmd_r = init_cmd();
    if (init_cmd_r < 0) {
        destroy_server(srv);
        destroy_database_map();
        EXIT_ERR(-1, "init cmd error");
    }

    // start to listen
    int listen_r = listen_server(srv);
    if (listen_r == -1) {
        destroy_server(srv);
        destroy_database_map();
        destroy_database(database);
        destroy_cmd();
        EXIT_ERR(-1, "start to listen error");
    }

    // start to accepting client
    (void) accept_server(srv);

    // accept_server() stopped or returned errors
    // handle resources
    destroy_server(srv);
    destroy_database_map();
    destroy_database(database);
    destroy_cmd();

    return 0;
}

void graceful_handler(int sig) 
{
    printf("graceful_handler | received signal: %d\n", sig);
    server_stop = TRUE;
    destroy_server(srv);
    destroy_database_map();
    destroy_database(database);
    destroy_cmd();

    printf("shutdown server...\n");
    exit(1);
}

void on_listen_server(char* host, unsigned short port)
{
    char port_str[5];
    sprintf(port_str, "%d", port);
    printf("server running on %s:%s\n", host, port_str);
}

void on_error_accept_server(int err)
{
    printf("error accept connection, returned fd: %d\n", err);
}