//
// Created by srkim on 23. 2. 24.
//

#include "main.h"
itcqNodes_t * gNodeFirst = NULL;
pthread_mutex_t gQueueLock = PTHREAD_MUTEX_INITIALIZER ;

itcqNodes_t* createNode(SmtpSession_t * session) {
    itcqNodes_t * node = (itcqNodes_t *)malloc(sizeof(itcqNodes_t));
    node->session = session;
    node->next = NULL;
    return node;
}

void insertNode(itcqNodes_t** head_ref, SmtpSession_t * session) {
    itcqNodes_t * node  = createNode(session);
    if (*head_ref == NULL) {
        *head_ref = node;
        return;
    }
    itcqNodes_t* last = *head_ref;
    while (last->next != NULL) {
        last = last->next;
    }
    last->next = node;
}

void delete_node(itcqNodes_t** head_ref, SmtpSession_t * session) {
    if (*head_ref == NULL) {
        return;
    }
    itcqNodes_t* node = *head_ref;
    if (node->session == session) {
        *head_ref = node->next;
        free(node);
        return;
    }
    itcqNodes_t* prev;
    while (node != NULL && node->session != session) {
        prev = node;
        node = node->next;
    }
    if (node == NULL) {
        return;
    }
    prev->next = node->next;
    free(node);
}

SmtpSession_t * popNode(itcqNodes_t** head_ref) {
    if (*head_ref == NULL) {
        return NULL;
    }
    SmtpSession_t * session = (*head_ref)->session;
    delete_node(head_ref, session);
    return session;
}


SmtpSession_t * itcqGetSession() {
    pthread_mutex_lock ( &gQueueLock );
    SmtpSession_t * session = popNode(&gNodeFirst);
    pthread_mutex_unlock ( &gQueueLock );
    return session;
}

int itcqPutSession(SmtpSession_t * session) {
    pthread_mutex_lock ( &gQueueLock ) ;
    insertNode(&gNodeFirst, session);
    pthread_mutex_unlock ( &gQueueLock );
    return 0;
}