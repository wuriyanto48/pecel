#ifndef HEADER_TYPE_H
#define HEADER_TYPE_H

#define TRUE 1
#define FALSE 0

#define BUFFER_MSG_SIZE 1024

#define CARRIAGE_RETURN 0xD
#define LINE_FEED 0xA

#define EXIT_ERR(code, msg) {\
        printf("exit err: %s\n", msg);\
        exit(code); \
    }

// reply
static const char* REPLY_CONNECTED = "CONNECTED*";
static const char* REPLY_OK = "OK*";
static const char* REPLY_EXIT = "BYE*";
static const char* REPLY_ERROR = "ERROR*";
static const char* REPLY_INVALID = "INVALID*";
static const char* REPLY_NOT_FOUND = "NOT_FOUND*";
static const char* REPLY_UNAUTHORIZED = "UNAUTHORIZED*";
   
#endif