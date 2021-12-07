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
#include "server.h"

volatile sig_atomic_t server_stop = FALSE;
struct server* srv;

// basic reply
static const char* REPLY_CONNECTED = "CONNECTED*";
static const char* REPLY_OK = "OK*";
static const char* REPLY_EXIT = "BYE*";

struct server* init_server(char* host, unsigned short port, 
    unsigned int s_family, unsigned int s_type, unsigned int max_con_queue) {
    struct server* s = (struct server*) malloc(sizeof(*s));
    if (s != NULL) {
        s->s_family = AF_UNSPEC;
        s->s_type = SOCK_STREAM;
        s->host = host;
        s->port = port;
        s->max_conn_queue = max_con_queue;

        struct addrinfo hints;
        struct addrinfo *serviceinfo, *pinfo;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = s_family;
        hints.ai_socktype = s_type;
        // hints.ai_flags = AI_PASSIVE; auto set to local ip

        char port_str[5];
        sprintf(port_str, "%d", s->port);

        printf("server running on %s:%s\n", s->host, port_str);

        if (getaddrinfo(s->host, port_str, &hints, &serviceinfo) != 0) {
            free(s);
            return NULL;
        }

        // debug
        // pinfo = serviceinfo;
        char ipstr[INET6_ADDRSTRLEN];
        int addr_count = 0;
        
        for(pinfo = serviceinfo; pinfo != NULL; pinfo = pinfo->ai_next) {
            void* addr;
            char* ipversion;
            
            if (pinfo->ai_family == AF_INET) {
                struct sockaddr_in* ipv4 = (struct sockaddr_in*) pinfo->ai_addr;
                addr = &(ipv4->sin_addr);
                ipversion = "IPV4";
                addr_count++;
            } else {
                struct sockaddr_in6* ipv6 = (struct sockaddr_in6*) pinfo->ai_addr;
                addr = &(ipv6->sin6_addr);
                ipversion = "IPV6";
                addr_count++;
            }

            inet_ntop(pinfo->ai_family, addr, ipstr, sizeof(ipstr));
            printf("protocol: %d\nip version: %s\nip : %s\naddr len: %d\n", 
                pinfo->ai_protocol, ipversion, ipstr, pinfo->ai_addrlen);
        }

        printf("addr_count: %d\n", addr_count);
        
        // debug end

        int sock_fd = socket(serviceinfo->ai_family, 
            serviceinfo->ai_socktype, serviceinfo->ai_protocol);
        if (sock_fd == -1) {
            freeaddrinfo(serviceinfo);
            free(s);
            return NULL;
        }

        printf("socket fd %d\n", sock_fd);
        
        // bind to port
        int bind_r = bind(sock_fd, serviceinfo->ai_addr, serviceinfo->ai_addrlen);
        if (bind_r == -1) {
            close(sock_fd);
            freeaddrinfo(serviceinfo);
            free(s);
            return NULL;
        }

        s->server_fd = sock_fd;
        s->serviceinfo = serviceinfo;
    }

    return s;
}

int listen_server(struct server* s) {
    int listen_r = listen(s->server_fd, s->max_conn_queue);
    if (listen_r == -1)
        return -1;

    return 0;
}

void accept_server(struct server* s) {
    while (TRUE) {
        if (server_stop)
            break;
        
        printf("waiting for connection\n");

        struct sockaddr_storage client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        int new_client_fd = accept(s->server_fd, 
            (struct sockaddr*) &client_addr, &client_addr_len);

        if (new_client_fd == -1) {
            printf("error accept connection, returned fd: %d\n", new_client_fd);
            break;
        }

        //handle client
        struct client* c = init_client(new_client_fd, (struct sockaddr*) &client_addr);

        pthread_t conn_thread;
        pthread_create(&conn_thread, NULL, handle_client, (void*) c);
    }
}

void destroy_server(struct server* s) {
    if (s != NULL) {
        free(s);
        close(s->server_fd);
        freeaddrinfo(s->serviceinfo);
    }
}


struct client* init_client(unsigned int sock_fd, struct sockaddr* sock_client) {
    struct client* c = (struct client*) malloc(sizeof(*c));
    if (c != NULL) {
        c->sock_fd = sock_fd;
        c->sock_client = sock_client;
    }

    return c;
}

void destroy_client(struct client* c) {
    if (c != NULL)
        free(c);
}

void* handle_client(void* args) {
    // cast to client*
    struct client* c = (struct client*) args;

    // detach self
    pthread_detach(pthread_self());

    printf("--------------------\n");
    printf("accepted new connection, fd: %d\nfamily: %d\nthread id: %d\n", 
        c->sock_fd, c->sock_client->sa_family, (int) pthread_self());
    
    printf("--------------------\n");

    // send connected notification
    write_text(c->sock_fd, REPLY_CONNECTED);

    char buffer[BUFFER_MSG_SIZE];
    ssize_t bytes_recv_len;
    while (TRUE) {
        bytes_recv_len = recv(c->sock_fd, buffer, BUFFER_MSG_SIZE, 0);
        buffer[bytes_recv_len] = '\0';
        if (bytes_recv_len == 0)
            break;

        if (strcmp("exit\n", buffer) == 0) {
            write_text(c->sock_fd, REPLY_EXIT);
            break;
        }
        
        if (bytes_recv_len > 0) {
            
            printf("bytes_recv_len : %zd\nbuffer: %s\n", bytes_recv_len, buffer);
            write_text(c->sock_fd, REPLY_OK);
        }
    }

    printf("client %d exit its connection\n", c->sock_fd);
    close(c->sock_fd);
    destroy_client(c);

    // exit its thread
    pthread_exit(NULL);
    
    return 0;
}

int write_text(int sock_fd, const char* msg) {
    int text_len = strlen(msg);
    send(sock_fd, msg, text_len, 0);
    send(sock_fd, "\n", 1, 0);
    return 0;
}