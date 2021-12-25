#include "server.h"


volatile sig_atomic_t server_stop = FALSE;
struct server* srv;

struct server* init_server(struct config* conf, unsigned int s_family, 
    unsigned int s_type, unsigned int max_con_queue) 
{
    struct server* s = (struct server*) malloc(sizeof(*s));
    if (s != NULL) {
        s->s_family = AF_UNSPEC;
        s->s_type = SOCK_STREAM;
        s->conf = conf;
        s->max_conn_queue = max_con_queue;

        struct addrinfo hints;
        struct addrinfo *serviceinfo, *pinfo;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = s_family;
        hints.ai_socktype = s_type;
        // hints.ai_flags = AI_PASSIVE; auto set to local ip

        char port_str[5];
        sprintf(port_str, "%d", s->conf->port);

        printf("server running on %s:%s\n", s->conf->host, port_str);

        if (getaddrinfo(s->conf->host, port_str, &hints, &serviceinfo) != 0) {
            free((void*) s);
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
        // address not available
        if (addr_count < 0) {
            freeaddrinfo(serviceinfo);
            free((void*) s);
            return NULL;
        }
        
        // debug end

        int sock_fd = socket(serviceinfo->ai_family, 
            serviceinfo->ai_socktype, serviceinfo->ai_protocol);
        if (sock_fd == -1) {
            freeaddrinfo(serviceinfo);
            free((void*) s);
            return NULL;
        }

        printf("socket fd %d\n", sock_fd);
        
        // bind to port
        int bind_r = bind(sock_fd, serviceinfo->ai_addr, serviceinfo->ai_addrlen);
        if (bind_r == -1) {
            close(sock_fd);
            freeaddrinfo(serviceinfo);
            free((void*) s);
            return NULL;
        }

        s->server_fd = sock_fd;
        s->serviceinfo = serviceinfo;
    }

    return s;
}

int listen_server(struct server* s) 
{
    int listen_r = listen(s->server_fd, s->max_conn_queue);
    if (listen_r == -1)
        return -1;

    return 0;
}

void accept_server(struct server* s) 
{
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
        if (c == NULL) {
            printf("error initialize new client\n");
            break;
        }

        struct handler_arg* ha = init_handler_arg(c, srv->conf);
        if (ha == NULL) {
            printf("error initialize client handler arg\n");
            break;
        }

        pthread_t conn_thread;
        pthread_create(&conn_thread, NULL, client_handler, (void*) ha);
    }
}

void destroy_server(struct server* s) 
{
    if (s != NULL) {
        close(s->server_fd);
        freeaddrinfo(s->serviceinfo);
        free((void*) s);
    }
}
