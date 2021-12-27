#ifndef HEADER_DATABASE_H
#define HEADER_DATABASE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define HASH_SIZE 1000

struct element {
    struct element* next;
    char* key;
    char* val;
};

// db interface
typedef struct element* (*database_set) (const char* key, const char* val);
typedef struct element* (*database_get) (const char* key);
typedef void (*database_delete) (const char* key);
typedef size_t (*database_size) (void);

struct database {
    database_set db_set;
    database_get db_get;
    database_delete db_del;
    database_size db_size;
};

struct database* init_database(void);
void destroy_database(struct database* database);

int init_database_map(void);
void destroy_database_map(void);

unsigned int element_hash(const char* key);
char* element_dup(const char* s);
struct element* element_insert(const char* key, const char* val);
struct element* element_get(const char* key);
void element_delete(const char* key);
char* element_dup(const char* s);
size_t element_size(void);
void destroy_element(struct element* d);

struct database_map {
    size_t size;
    struct element* element_table[HASH_SIZE];
};

#endif