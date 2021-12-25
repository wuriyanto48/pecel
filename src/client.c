#include "client.h"

struct client* init_client(unsigned int sock_fd, struct sockaddr* sock_client) 
{
    struct client* c = (struct client*) malloc(sizeof(*c));
    if (c != NULL) {
        c->sock_fd = sock_fd;
        c->sock_client = sock_client;
        c->is_authenticated = 0;
    }

    return c;
}

void destroy_client(struct client* c) 
{
    if (c != NULL)
        free((void*) c);
}

static int check_is_authenticated(struct handler_arg* h)
{
    if (h->conf->auth)
        if (!h->cl->is_authenticated)
            return -1;
    return 0;
}

static int is_password_valid(struct handler_arg* h, char* pass)
{
    if (strcmp(h->conf->pass, pass) != 0)
        return -1;
    return 0;
}

struct handler_arg* init_handler_arg(struct client* cl, struct config* conf)
{
    struct handler_arg* h = (struct handler_arg*) malloc(sizeof(*h));
    if (h != NULL) {
        h->cl = cl;
        h->conf = conf;
    }

    return h;
}

void destroy_handler_arg(struct handler_arg* h) 
{
    if (h != NULL) {
        destroy_client(h->cl);
        free((void*) h);
    }
}

void* client_handler(void* args) 
{
    // detach self
    pthread_detach(pthread_self());

    // cast to client*
    struct handler_arg* ha = (struct handler_arg*) args;
    char client_ipstr[INET6_ADDRSTRLEN];

    inet_ntop(ha->cl->sock_client->sa_family, ha->cl->sock_client, 
        client_ipstr, sizeof(client_ipstr));
    printf("--------------------\n");
    printf("accepted new connection, client ip: %s\nfamily: %d\nthread id: %d\n", 
        client_ipstr, ha->cl->sock_client->sa_family, (int) pthread_self());
    
    printf("--------------------\n");

    // send connected notification
    write_text(ha->cl->sock_fd, REPLY_CONNECTED);

    char buffer[BUFFER_MSG_SIZE];
    ssize_t bytes_recv_len;
    while (TRUE) {
        bytes_recv_len = recv(ha->cl->sock_fd, buffer, BUFFER_MSG_SIZE, 0);
        
        // add null terminator to buffer
        buffer[bytes_recv_len] = '\0';
        if (bytes_recv_len == 0)
            break;
        
        if (bytes_recv_len > 1) {
            char** buffer_arr = (char**) malloc(3 * sizeof(char*));
            if (buffer_arr == NULL) {
                write_text(ha->cl->sock_fd, REPLY_ERROR);
                break;
            }

            int index_size = 0;
            extract_line_val(buffer, " ", buffer_arr, &index_size);

            if (strcmp("exit", to_lower(buffer_arr[0])) == 0) {
                write_text(ha->cl->sock_fd, REPLY_EXIT);
                break;
            }
            
            printf("bytes_recv_len : %lu\nbuffer: %s\n", bytes_recv_len, buffer);
            printf("-----------------\n");
            printf("buffer_arr len: %d\n", index_size);
            printf("-----------------\n");

            // database operation
            // TODO: 
            // - move to new function
            // - add validation

            struct command_s* cmd_s = cmd_get(buffer_arr[0]);
            
            if (cmd_s == NULL) {
                write_text(ha->cl->sock_fd, REPLY_ERROR);
            } else {

                switch (cmd_s->c) {
                case SET: {
                    if (check_is_authenticated(ha) < 0) {
                        write_text(ha->cl->sock_fd, REPLY_UNAUTHORIZED);
                    } else {
                        if (index_size < 3) {
                            write_text(ha->cl->sock_fd, REPLY_INVALID);
                        } else {
                            char* key = buffer_arr[1];
                            char* val = buffer_arr[2];

                            if (element_insert(key, val) == NULL) {
                                write_text(ha->cl->sock_fd, REPLY_ERROR);
                            } else {
                                write_text(ha->cl->sock_fd, REPLY_OK);
                            }
                        }
                    }
                    
                    break;
                }
                    
                case GET: {
                    if (check_is_authenticated(ha) < 0) {
                        write_text(ha->cl->sock_fd, REPLY_UNAUTHORIZED);
                    } else {
                        if (index_size < 2) {
                            write_text(ha->cl->sock_fd, REPLY_INVALID);
                        } else {
                            char* key = buffer_arr[1];
                            struct element* get_r = element_get(key);
                            if (get_r == NULL) {
                                write_text(ha->cl->sock_fd, REPLY_NOT_FOUND);
                            } else {
                                write_text(ha->cl->sock_fd, get_r->val);
                            }
                        }
                    }
                    
                    break;
                }

                case REM: {
                    if (check_is_authenticated(ha) < 0) {
                        write_text(ha->cl->sock_fd, REPLY_UNAUTHORIZED);
                    } else {
                        if (index_size < 2) {
                            write_text(ha->cl->sock_fd, REPLY_INVALID);
                        } else {
                            char* key = buffer_arr[1];
                            struct element* get_r = element_get(key);
                            if (get_r == NULL) {
                                write_text(ha->cl->sock_fd, REPLY_NOT_FOUND);
                            } else {
                                element_delete(key);
                                write_text(ha->cl->sock_fd, REPLY_OK);
                            }
                        }
                    }
                    
                    break;
                }

                case ATH: {
                    if (index_size < 2) {
                        write_text(ha->cl->sock_fd, REPLY_INVALID);
                    } else {
                        char* pass = buffer_arr[1];
                        if (is_password_valid(ha, pass) < 0) {
                            write_text(ha->cl->sock_fd, REPLY_UNAUTHORIZED);
                        } else {
                            ha->cl->is_authenticated = 1;
                            write_text(ha->cl->sock_fd, REPLY_OK);
                        }
                    }

                    break;
                }
                    

                default: {
                    write_text(ha->cl->sock_fd, REPLY_ERROR);
                    break;
                }
                }

                printf("cmd_s: %s | %d\n", cmd_s->c_s, cmd_s->c);
                // write_text(c->sock_fd, REPLY_OK);
            }

            free((void*) buffer_arr);
        } else {
            write_text(ha->cl->sock_fd, REPLY_INVALID);
        }
    }

    printf("client %s exit its connection\n", client_ipstr);
    close(ha->cl->sock_fd);
    destroy_handler_arg(ha);

    // exit its thread
    pthread_exit(NULL);
    
    return 0;
}

int write_text(int sock_fd, const char* msg) 
{
    int text_len = strlen(msg);
    int msg_len = text_len + 2;
    char message[msg_len];

    for (int i = 0; i < text_len; i++)
        message[i] = msg[i];

    message[text_len] = CARRIAGE_RETURN;
    message[text_len+1] = LINE_FEED;

    send(sock_fd, message, msg_len, 0);
    return 0;
}