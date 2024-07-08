#ifndef CAREER_TEST3_SMTPHASHMAP_H
#define CAREER_TEST3_SMTPHASHMAP_H

#include "main.h"


#define TABLE_SIZE 1200


typedef struct Entry {
    char key[L_SESSION_ID];
    smtp_session_t *session;
    struct Entry *next;
} Entry;


typedef struct {
    Entry **table;
} HashMap;


unsigned int hash(const char *key);
HashMap* createHashmap();
void insert(HashMap *hashmap, const char *key, smtp_session_t *session);
void delete(HashMap *hashmap, const char *key);
smtp_session_t* search(HashMap *hashmap, const char *key);
smtp_session_t* getSmtpSessionByThreadNum(HashMap *hashmap, int thread_num);
void freeHashmap(HashMap *hashmap);
    
#endif //CAREER_TEST3_SMTPHASHMAP_H
