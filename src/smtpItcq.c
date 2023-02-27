#include "main.h"
pthread_mutex_t gQueueLock = PTHREAD_MUTEX_INITIALIZER ;


typedef struct itcqNode {
    smtp_session_t* data;
    struct itcqNode* next;
} itcqNode_t;

itcqNode_t g_first_node;

void enqueue(smtp_session_t * data) {
    itcqNode_t* new_node = (itcqNode_t*)malloc(sizeof(itcqNode_t));
    new_node->data = data;
    new_node->next = NULL;

    itcqNode_t * node_at = &g_first_node;
    while (TRUE) {
        if (node_at->next == NULL) {
            node_at->next = new_node;
            break;
        }
        node_at = node_at->next;
    }
}

smtp_session_t * dequeue() {
    smtp_session_t * session = NULL;

    itcqNode_t * node_at = &g_first_node;
    itcqNode_t * node_prev = NULL;
    while (TRUE) {
        if (node_at->next == NULL) {
            session = node_at->data;

            if (node_prev) node_prev->next = NULL;
            if (&g_first_node != node_at) free(node_at);
            break;
        }
        node_prev = node_at;
        node_at = node_at->next;
    }

    return session;
}


smtp_session_t * itcqGetSession() {
    pthread_mutex_lock ( &gQueueLock );
    smtp_session_t * session = dequeue();
    pthread_mutex_unlock ( &gQueueLock );
    return session;
}

int itcqPutSession(smtp_session_t * session) {
    pthread_mutex_lock ( &gQueueLock ) ;
    enqueue(session);
    pthread_mutex_unlock ( &gQueueLock );
    return 0;
}