#ifndef HEADER_DATABASE_H
#define HEADER_DATABASE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define HASH_SIZE 1000

struct database {
    struct database* next;
    char* key;
    char* val;
};

char* database_dup(const char* s);
void destroy_database(struct database* d);
unsigned int database_hash(const char* key);
struct database* database_get(const char* key);
struct database* database_insert(const char* key, const char* val);
void database_delete(const char* key);

static struct database* database_table[HASH_SIZE];

void destroy_database(struct database* d)
{
    if (d != NULL)
    {
        free((void*) d->key);
        free((void*) d->val);
    }
}

unsigned int database_hash(const char* key) 
{
    unsigned int hashval;
    for (hashval = 0; *key != '\0'; key++)
        hashval = *key + 31 * hashval;
    return (hashval % HASH_SIZE);
}

struct database* database_get(const char* key)
{
    struct database* data;
    for (data = database_table[database_hash(key)]; data != NULL; data = data->next) {
        printf("iter\n");
        if (strcmp(key, data->key) == 0)
            return data;
    }
    return NULL;
}

struct database* database_insert(const char* key, const char* val)
{
    struct database* res;
    unsigned int hashval;

    if ((res = database_get(key)) == NULL) { // not found
        res = (struct database*) malloc(sizeof(*res));
        if (res == NULL || (res->key = database_dup(key)) ==  NULL)
            return NULL;
        hashval = database_hash(key);
        res->next = database_table[hashval];
        database_table[hashval] = res;
    } else // key found
        free((void*) res->val);
    
    if ((res->val = database_dup(val)) == NULL)
        return NULL;
    
    return res;
}

void database_delete(const char* key) 
{
    struct database* res;
    unsigned int hashval;

    if ((res = database_get(key)) != NULL) {
        hashval = database_hash(key);
        database_table[hashval] = NULL;
        destroy_database(res);
    }
}

char* database_dup(const char* s)
{
    char* res;
    res = (char*) malloc(strlen(s) + 1); // + 1 for NULL terminator '\0'
    if (res != NULL)
        strcpy(res, s);
    return res;
}
    
#endif