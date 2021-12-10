#ifndef HEADER_DATABASE_H
#define HEADER_DATABASE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define HASH_SIZE 1000

int init_database();
void destroy_database();
unsigned int element_hash(const char* key);
char* element_dup(const char* s);
struct element* element_insert(const char* key, const char* val);
void element_delete(const char* key);
char* element_dup(const char* s);
size_t element_size();
void destroy_element(struct element* d);

struct database {
    size_t size;
    struct element* element_table[HASH_SIZE];
};

struct element {
    struct element* next;
    char* key;
    char* val;
};
    
#endif