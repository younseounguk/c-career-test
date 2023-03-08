#include "main.h"

#define BUCKET_SIZE 4096
smtp_session_t* g_smtp_sessions[BUCKET_SIZE] = {NULL,};
pthread_mutex_t g_session_lock = PTHREAD_MUTEX_INITIALIZER;

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
    pthread_mutex_lock ( &g_session_lock ) ;
    /* interviewer question 1-1
        smtp 세션을 종료하려고 한다.
        그동안 사용되었던 session은 현재 해시테이블을 이용하여 관리되고 있었는데
        해당 테이블에 일치하는 session정보를 전달받은 session id를 이용하여 제거하는 로직을 개발하시오.
    */
    pthread_mutex_unlock ( &g_session_lock ) ;
}

smtp_session_t *getSmtpSession(char *session_id) {
    int hash_id = hash_func(session_id, strlen(session_id));
    int start_index = hash_id % BUCKET_SIZE;
    int i = 0;
    pthread_mutex_lock ( &g_session_lock ) ;
    do {
        if(g_smtp_sessions[i] != NULL) {
            if (strcmp(g_smtp_sessions[i]->session_id, session_id) == 0) {
                pthread_mutex_unlock(&g_session_lock);
                return  g_smtp_sessions[i];
            }
        }
        i = (i + 1) % BUCKET_SIZE;
    } while (i != start_index);

    pthread_mutex_unlock ( &g_session_lock ) ;
    return NULL;
}

smtp_session_t *addSmtpSession(smtp_session_t *session) {
    int i;
    int start_index;
    int hash_id;

    hash_id = hash_func(session->session_id, strlen(session->session_id));
    start_index = hash_id % BUCKET_SIZE;
    i = start_index;

    pthread_mutex_lock ( &g_session_lock ) ;
    /* interviewer question 1-2
        smtp 세션을 추가하려고 한다.
        전달받은 session정보를 활용하여 현재 관리하고 있는 해시테이블에 추가하는 로직을 개발하시오.
    */
    pthread_mutex_unlock ( &g_session_lock ) ;
    return NULL;
};