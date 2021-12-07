#ifndef HEADER_TYPE_H
#define HEADER_TYPE_H

#define TRUE 1
#define FALSE 0

#define BUFFER_MSG_SIZE 1024

#define EXIT_ERR(code, msg) {\
        printf("exit err: %s\n", msg);\
        exit(code); \
    }
    
#endif