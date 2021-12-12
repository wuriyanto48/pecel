#include "client.h"

// basic reply
static const char* REPLY_CONNECTED = "CONNECTED*";
static const char* REPLY_OK = "OK*";
static const char* REPLY_EXIT = "BYE*";
static const char* REPLY_ERROR = "ERROR*";
static const char* REPLY_INVALID = "INVALID*";
static const char* REPLY_NOT_FOUND = "NOT_FOUND*";

struct client* init_client(unsigned int sock_fd, struct sockaddr* sock_client) 
{
    struct client* c = (struct client*) malloc(sizeof(*c));
    if (c != NULL) {
        c->sock_fd = sock_fd;
        c->sock_client = sock_client;
    }

    return c;
}

void destroy_client(struct client* c) 
{
    if (c != NULL)
        free((void*) c);
}

void* client_handler(void* args) 
{
    // detach self
    pthread_detach(pthread_self());

    // cast to client*
    struct client* c = (struct client*) args;
    char client_ipstr[INET6_ADDRSTRLEN];

    inet_ntop(c->sock_client->sa_family, c->sock_client, 
        client_ipstr, sizeof(client_ipstr));
    printf("--------------------\n");
    printf("accepted new connection, client ip: %s\nfamily: %d\nthread id: %d\n", 
        client_ipstr, c->sock_client->sa_family, (int) pthread_self());
    
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
        
        if (bytes_recv_len > 0) {
            char** buffer_arr = (char**) malloc(3 * sizeof(char*));
            if (buffer_arr == NULL) {
                write_text(c->sock_fd, REPLY_ERROR);
                break;
            }

            extract_line_val(buffer, " ", buffer_arr);

            if (strcmp("exit", to_lower(buffer_arr[0])) == 0) {
                write_text(c->sock_fd, REPLY_EXIT);
                break;
            }
            
            printf("bytes_recv_len : %lu\nbuffer: %s\n", bytes_recv_len, buffer);

            // database operation
            // TODO: 
            // - move to new function
            // - add validation

            struct command_s* cmd_s = cmd_get(buffer_arr[0]);
            
            if (cmd_s == NULL) {
                write_text(c->sock_fd, REPLY_ERROR);
            } else {

                switch (cmd_s->c) {
                case SET: {
                    char* key = buffer_arr[1];
                    char* val = buffer_arr[2];

                    if (element_insert(key, val) == NULL) {
                        write_text(c->sock_fd, REPLY_ERROR);
                    } else {
                        write_text(c->sock_fd, REPLY_OK);
                    }
                    break;
                }
                    
                case GET: {
                    char* key = buffer_arr[1];
                    struct element* get_r = element_get(key);
                    if (get_r == NULL) {
                        write_text(c->sock_fd, REPLY_NOT_FOUND);
                    } else {
                        write_text(c->sock_fd, get_r->val);
                    }
                    break;
                }
                    

                default: {
                    write_text(c->sock_fd, REPLY_ERROR);
                    break;
                }
                }

                printf("cmd_s: %s | %d\n", cmd_s->c_s, cmd_s->c);
                // write_text(c->sock_fd, REPLY_OK);
            }

            free((void*) buffer_arr);
        }
    }

    printf("client %s exit its connection\n", client_ipstr);
    close(c->sock_fd);
    destroy_client(c);

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

    message[text_len] = 0xD;
    message[text_len+1] = 0xA;

    send(sock_fd, message, msg_len, 0);
    return 0;
}