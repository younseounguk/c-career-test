//
// Created by srkim on 23. 2. 24.
//

#include "main.h"
pthread_mutex_t gQueueLock = PTHREAD_MUTEX_INITIALIZER ;


typedef struct itcqNode {
    smtp_session_t* data;
    struct itcqNode* prev;
    struct itcqNode* next;
} itcqNode_t;

typedef struct itcQueue {
    itcqNode_t* front;
    itcqNode_t* rear;
} itcQueue_t;

itcQueue_t gQueue;

int is_empty(itcQueue_t* q) {
    return q->front == NULL;
}

void enqueue(itcQueue_t* q, smtp_session_t * data) {
    itcqNode_t* new_node = (itcqNode_t*)malloc(sizeof(itcqNode_t));
    new_node->data = data;
    new_node->prev = q->rear;
    new_node->next = NULL;
    if (is_empty(q)) {
        q->front = q->rear = new_node;
    }
    else {
        q->rear->next = new_node;
        q->rear = new_node;
    }
}

smtp_session_t * dequeue(itcQueue_t* q) {
    if (is_empty(q)) {
        return NULL;
    }
    smtp_session_t * session = q->front->data;
    itcqNode_t* temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    else {
        q->front->prev = NULL;
    }
    free(temp);
    return session;
}

smtp_session_t * peek(itcQueue_t* q) {
    if (is_empty(q)) {
        return NULL;
    }
    return q->front->data;
}



smtp_session_t * itcqGetSession() {
    pthread_mutex_lock ( &gQueueLock );
    smtp_session_t * session = dequeue(&gQueue);
    pthread_mutex_unlock ( &gQueueLock );
    return session;
}

int itcqPutSession(smtp_session_t * session) {
    pthread_mutex_lock ( &gQueueLock ) ;
    enqueue(&gQueue, session);
    pthread_mutex_unlock ( &gQueueLock );
    return 0;
}