#include "main.h"

#include <stdlib.h>
#include <string.h>


/**
 * djb2 hash 알고리즘 
 */
unsigned int hash(const char *key) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash % TABLE_SIZE;
}


HashMap* createHashmap() {
    HashMap *hashmap = malloc(sizeof(HashMap));
    hashmap->table = malloc(sizeof(Entry*) * TABLE_SIZE);
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashmap->table[i] = NULL;
    }
    return hashmap;
}


void insert(HashMap *hashmap, const char *key, smtp_session_t *session) {
    unsigned int index = hash(key);
    Entry *new_entry = malloc(sizeof(Entry));
    strncpy(new_entry->key, key, L_SESSION_ID);
    new_entry->session = session;
    new_entry->next = hashmap->table[index];
    hashmap->table[index] = new_entry;
}


void delete(HashMap *hashmap, const char *key)
{
    unsigned int index = hash(key);
    Entry *entry = hashmap->table[index];
    Entry *prev = NULL;

    while (entry != NULL && strncmp(entry->key, key, L_SESSION_ID) != 0) {
        prev = entry;
        entry = entry->next;
    }

    if (entry == NULL) {
        return;
    }

    if (prev == NULL) {
        hashmap->table[index] = entry->next;
    } else {
        prev->next = entry->next;
    }

    g_tcp_close += 1;

    close(entry->session->sock_fd);
    if (entry->session) free(entry->session);
    free(entry);
}


smtp_session_t* search(HashMap *hashmap, const char *key) {
    unsigned int index = hash(key);
    Entry *entry = hashmap->table[index];
    while (entry != NULL) {
        if (strncmp(entry->key, key, L_SESSION_ID) == 0) {
            return entry->session;
        }
        entry = entry->next;
    }
    return NULL;
}

smtp_session_t* getSmtpSessionByThreadNum(HashMap *hashmap, int thread_num)
{
    for (int i = 0; i < TABLE_SIZE; i++) {
        Entry *entry = hashmap->table[i];
        if (entry) {
            if (i % MAX_ASYNC_WORKS_TH == thread_num) return entry->session;
        }

    }
    return NULL;
}


void freeHashmap(HashMap *hashmap) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Entry *entry = hashmap->table[i];
        while (entry != NULL) {
            Entry *temp = entry;
            entry = entry->next;
            free(temp);
        }
    }
    free(hashmap->table);
    free(hashmap);
}

