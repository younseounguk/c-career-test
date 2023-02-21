//
// Created by srkim on 23. 2. 20.
//
#include "main.h"

#define BUCKET_SIZE 4096
SmtpSession_t *smtpSessions[BUCKET_SIZE] = {NULL,};
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

void delSmtpSession(char *sessionId) {
    int hash_id = hash_func(sessionId, strlen(sessionId));
    int start_index = hash_id % BUCKET_SIZE;
    int i = 0;
    pthread_mutex_lock ( &gSessionLock ) ;
    do {
        if(smtpSessions[i] != NULL) {
            SmtpSession_t * session = smtpSessions[i];
            if (strcmp(session->SessionId, sessionId) == 0) {
                close(session->SockFd);
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

SmtpSession_t *getSmtpSession(char *sessionId) {
    int hash_id = hash_func(sessionId, strlen(sessionId));
    int start_index = hash_id % BUCKET_SIZE;
    int i = 0;
    pthread_mutex_lock ( &gSessionLock ) ;
    do {
        if(smtpSessions[i] != NULL) {
            if (strcmp(smtpSessions[i]->SessionId, sessionId) == 0) {
                pthread_mutex_unlock(&gSessionLock);
                return smtpSessions[i];
            }
        }
        i = (i + 1) % BUCKET_SIZE;
    } while (i != start_index);

    pthread_mutex_unlock ( &gSessionLock ) ;
    return NULL;
}

SmtpSession_t *addSmtpSession(SmtpSession_t *session) {
    int i;
    int start_index;
    int hash_id;

    hash_id = hash_func(session->SessionId, strlen(session->SessionId));
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