#include "main.h"
#include <pthread.h>

/*  TODO 과제 1 공통 사항
 *  session 관리는 Socket정보를 보관하기 위해 사용합니다.
 *  다량의 smtp session 정보를 보관하는 보관소가 존재하여야 합니다..
 *  smtp session 관리 시 충돌을 방지하기 위한 적절한 매커니즘을 적용하여 개발하여야 합니다.
 */


HashMap* session_map = NULL;
pthread_mutex_t session_map_mutex = PTHREAD_MUTEX_INITIALIZER;

void delSmtpSession(char *session_id) {
    /*  TODO 과제 1-1
     *   smtp 세션을 종료
     *   그동안 사용되었던 session을 session_id를 이용하여 session 보관소에서 제거하는 로직을 개발하여야 합니다.
     */
    pthread_mutex_lock(&session_map_mutex);
    delete(session_map, session_id);
    pthread_mutex_unlock(&session_map_mutex);
    return;
}

smtp_session_t *addSmtpSession(smtp_session_t *session) {
    /*  TODO 과제 1-2
     *   smtp 세션을 추가
     *   전달받은 smtp session정보를 활용하여 현재 관리하고 있는 smtp session 보관소에 추가하는 로직을 개발하여야 합니다.
     */
    pthread_mutex_lock(&session_map_mutex);
    insert(session_map, session->session_id, session);
    pthread_mutex_unlock(&session_map_mutex);
    return session;
}

smtp_session_t *getSmtpSession(char *session_id) {
    /*  TODO 과제 1-3 :: 해당 로직은 비동기 로직 흐름상 과제 진행을 해도되고 안해도 무방합니다. (PASS 가능)
     *   smtp 세션을 session_id를 활용하여 가져오려고 합니다.
     *   전달받은 smtp session_id 정보를 활용하여 현재 관리하고 있는 smtp session 보관소에서 session을 가져오는 로직을 개발하여야 합니다.
     */
    pthread_mutex_lock(&session_map_mutex);
    smtp_session_t *session = search(session_map, session_id);
    pthread_mutex_unlock(&session_map_mutex);
    return session;
}

smtp_session_t * popSessionByThreadNum(int thread_num)
{
    smtp_session_t *session = NULL;

    pthread_mutex_lock(&session_map_mutex);
    session = getSmtpSessionByThreadNum(session_map, thread_num);
    pthread_mutex_unlock(&session_map_mutex);

    return session;
}


void initSessionHashmap()
{
    session_map = createHashmap();
}

void cleanSessionHashmap()
{
    freeHashmap(session_map);
}
