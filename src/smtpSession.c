#include "main.h"

/*  TODO 과제 1 공통 사항
 *  session 관리는 Socket정보를 보관하기 위해 사용합니다.
 *  다량의 smtp session 정보를 보관하는 보관소가 존재하여야 합니다..
 *  smtp session 관리 시 충돌을 방지하기 위한 적절한 매커니즘을 적용하여 개발하여야 합니다.
 */

smtp_session_t session_pool[SESSION_POOL_SIZE] ;
int session_count = 0 ;
pthread_mutex_t session_mutex = PTHREAD_MUTEX_INITIALIZER ;

void delSmtpSession(char *session_id) {
    /*  TODO 과제 1-1
     *   smtp 세션을 종료
     *   그동안 사용되었던 session을 session_id를 이용하여 session 보관소에서 제거하는 로직을 개발하여야 합니다.
     */
    bool found = false ;

    pthread_mutex_lock(&session_mutex) ;

    for(int i = 0; i < session_count; i++)
    {
        if(strcmp(session_pool[i].session_id, session_id) == 0)
        {
            for(int j = i; j < (session_count - 1); j++)
                session_pool[j] = session_pool[j + 1] ;
            
            found = true ;
            break ;
        }
    }

    if(found)
        session_count-- ;

    pthread_mutex_unlock(&session_mutex) ;

    return;
}

smtp_session_t *addSmtpSession(smtp_session_t *session) {
    /*  TODO 과제 1-2
     *   smtp 세션을 추가
     *   전달받은 smtp session정보를 활용하여 현재 관리하고 있는 smtp session 보관소에 추가하는 로직을 개발하여야 합니다.
     */


    if(session_count >= SESSION_POOL_SIZE)
    {
        LOG(LOG_CRT, "Session Count Full : %d", session_count) ;
        return NULL ;
    }

    if(session == NULL)
    {
        LOG(LOG_CRT, "Session NULL") ;
        return NULL ;
    }

    pthread_mutex_lock(&session_mutex) ;

    smtp_session_t* new_session = &session_pool[session_count] ;
    snprintf(new_session->session_id, sizeof(new_session->session_id), "%s", session->session_id) ;

    session_count++ ;

    pthread_mutex_unlock(&session_mutex) ;

    return new_session;
}

smtp_session_t *getSmtpSession(char *session_id) {
    /*  TODO 과제 1-3 :: 해당 로직은 비동기 로직 흐름상 과제 진행을 해도되고 안해도 무방합니다. (PASS 가능)
     *   smtp 세션을 session_id를 활용하여 가져오려고 합니다.
     *   전달받은 smtp session_id 정보를 활용하여 현재 관리하고 있는 smtp session 보관소에서 session을 가져오는 로직을 개발하여야 합니다.
     */

    smtp_session_t *session = NULL ;
    
    pthread_mutex_lock(&session_mutex) ;

    for(int i = 0; i < session_count; i++)
    {
        if(strcmp(session_pool[i].session_id, session_id) == 0)
        {
            session = &session_pool[i] ;
            break ;
        }
    }

    pthread_mutex_unlock(&session_mutex) ;


    return session;
}
