//
// Created by srkim on 23. 2. 20.
//
#include "main.h"

#define BUCKET_SIZE 4096
smtp_session_t *smtpSessions[BUCKET_SIZE] = {NULL,};
pthread_mutex_t gSessionLock = PTHREAD_MUTEX_INITIALIZER ;

int hash_func(const char *string, size_t len) {
    int i;
    int hash;

    hash = 0;
    for (i = 0; i < len; ++i) {
        hash = 65599 * hash + string[i];
    }

    return hash ^ (hash >> 16);
}

void delSmtpSession(char *session_id) {
    int hash_id = hash_func(session_id, strlen(session_id));
    int start_index = hash_id % BUCKET_SIZE;
    int i = 0;
    pthread_mutex_lock ( &gSessionLock ) ;
    do {
        if(smtpSessions[i] != NULL) {
            smtp_session_t * session = smtpSessions[i];
            if (strcmp(session->session_id, session_id) == 0) {
                close(session->sock_fd);
                free(session);
                smtpSessions[i] = NULL;
                pthread_mutex_unlock ( &gSessionLock ) ;
                return;
            }
        }
        i = (i + 1) % BUCKET_SIZE;
    } while (i != start_index);
    pthread_mutex_unlock ( &gSessionLock ) ;
}

smtp_session_t *getSmtpSession(char *session_id) {
    int hash_id = hash_func(session_id, strlen(session_id));
    int start_index = hash_id % BUCKET_SIZE;
    int i = 0;
    pthread_mutex_lock ( &gSessionLock ) ;
    do {
        if(smtpSessions[i] != NULL) {
            if (strcmp(smtpSessions[i]->session_id, session_id) == 0) {
                pthread_mutex_unlock(&gSessionLock);
                return smtpSessions[i];
            }
        }
        i = (i + 1) % BUCKET_SIZE;
    } while (i != start_index);

    pthread_mutex_unlock ( &gSessionLock ) ;
    return NULL;
}

smtp_session_t *addSmtpSession(smtp_session_t *session) {
    int i;
    int start_index;
    int hash_id;

    hash_id = hash_func(session->session_id, strlen(session->session_id));
    start_index = hash_id % BUCKET_SIZE;
    i = start_index;

    pthread_mutex_lock ( &gSessionLock ) ;
    do {
        if (smtpSessions[i] == NULL) {
            smtpSessions[i] = session;
            pthread_mutex_unlock ( &gSessionLock ) ;
            return session;
        }
        i = (i + 1) % BUCKET_SIZE;
    } while (i != start_index);
    pthread_mutex_unlock ( &gSessionLock ) ;
    LOG(LOG_MAJ, "Error. fail to add smtp session : session store is full");
    free(session);
    return NULL;
};