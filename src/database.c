#include "database.h"

static struct database_map* DATABASE_MAP;

struct database* init_database(void) 
{
    struct database* database = (struct database*) malloc(sizeof(*database));
    if (database == NULL)
        return NULL;
    database->db_set = element_insert;
    database->db_get = element_get;
    database->db_del = element_delete;
    database->db_size = element_size;
    return database;
}

void destroy_database(struct database* database)
{
    if (database != NULL)
        free((void*) database);
}

int init_database_map(void) 
{
    DATABASE_MAP = (struct database_map*) malloc(sizeof(*DATABASE_MAP));
    if (DATABASE_MAP == NULL)
        return -1;
    return 0;
}

void destroy_database_map(void)
{
    if (DATABASE_MAP != NULL)
        free((void*) DATABASE_MAP);
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
    for (data = DATABASE_MAP->element_table[element_hash(key)]; data != NULL; data = data->next) {
        if (strcmp(key, data->key) == 0)
            return data;
    }
    return NULL;
}

struct element* element_insert(const char* key, const char* val)
{
    if (DATABASE_MAP->size >= HASH_SIZE)
        return NULL;
        
    struct element* res;
    unsigned int hashval;

    if ((res = element_get(key)) == NULL) { // not found
        res = (struct element*) malloc(sizeof(*res));
        if (res == NULL || (res->key = element_dup(key)) ==  NULL)
            return NULL;
        hashval = element_hash(key);
        res->next = DATABASE_MAP->element_table[hashval];
        DATABASE_MAP->element_table[hashval] = res;
    } else // key found
        free((void*) res->val);
    
    if ((res->val = element_dup(val)) == NULL)
        return NULL;
    DATABASE_MAP->size++;
    return res;
}

void element_delete(const char* key) 
{
    struct element* res;
    unsigned int hashval;

    if ((res = element_get(key)) != NULL) {
        hashval = element_hash(key);
        DATABASE_MAP->element_table[hashval] = NULL;
        destroy_element(res);
        DATABASE_MAP->size--;
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

size_t element_size(void) {
    return DATABASE_MAP->size;
}