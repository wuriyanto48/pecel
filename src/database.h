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

static struct database* db;

int init_database() 
{
    db = (struct database*) malloc(sizeof(*db));
    if (db == NULL)
        return -1;
    return 0;
}

void destroy_database()
{
    if (db != NULL)
        free((void*) db);
}

void destroy_element(struct element* d)
{
    if (d != NULL)
    {
        free((void*) d->key);
        free((void*) d->val);
    }
}

unsigned int element_hash(const char* key) 
{
    unsigned int hashval;
    for (hashval = 0; *key != '\0'; key++)
        hashval = *key + 31 * hashval;
    return (hashval % HASH_SIZE);
}

struct element* element_get(const char* key)
{
    struct element* data;
    for (data = db->element_table[element_hash(key)]; data != NULL; data = data->next) {
        printf("iter\n");
        if (strcmp(key, data->key) == 0)
            return data;
    }
    return NULL;
}

struct element* element_insert(const char* key, const char* val)
{
    struct element* res;
    unsigned int hashval;

    if ((res = element_get(key)) == NULL) { // not found
        res = (struct element*) malloc(sizeof(*res));
        if (res == NULL || (res->key = element_dup(key)) ==  NULL)
            return NULL;
        hashval = element_hash(key);
        res->next = db->element_table[hashval];
        db->element_table[hashval] = res;
    } else // key found
        free((void*) res->val);
    
    if ((res->val = element_dup(val)) == NULL)
        return NULL;
    db->size++;
    return res;
}

void element_delete(const char* key) 
{
    struct element* res;
    unsigned int hashval;

    if ((res = element_get(key)) != NULL) {
        hashval = element_hash(key);
        db->element_table[hashval] = NULL;
        destroy_element(res);
        db->size--;
    }
}

char* element_dup(const char* s)
{
    char* res;
    res = (char*) malloc(strlen(s) + 1); // + 1 for NULL terminator '\0'
    if (res != NULL)
        strcpy(res, s);
    return res;
}

size_t element_size() {
    return db->size;
}
    
#endif