#include "main.h"

/*  TODO 과제 1 공통 사항
 *  session 관리는 Socket정보를 보관하기 위해 사용합니다.
 *  다량의 smtp session 정보를 보관하는 보관소가 존재하여야 합니다..
 *  smtp session 관리 시 충돌을 방지하기 위한 적절한 매커니즘을 적용하여 개발하여야 합니다.
 */

typedef struct _Node{

	smtp_session_t 	*pSession;
	struct 	_Node	*pNext;

}Node;

#define HTBL_BUCKET_SIZE 2048
Node	g_htbl[HTBL_BUCKET_SIZE];
pthread_mutex_t g_mtx = PTHREAD_MUTEX_INITIALIZER;

unsigned long hash_djb2(unsigned char* _p) {

	unsigned long hash = 5381;
	int c, i = 0;
	const unsigned char* p = _p;
	int len = strlen(p);

	while( i < len )
	{
		c = *p++;
		hash = ((hash << 5) + hash) + c;
		i++;
	}

	return hash % HTBL_BUCKET_SIZE;
}


void delSmtpSession(char *session_id) {
    /*  TODO 과제 1-1
     *   smtp 세션을 종료
     *   그동안 사용되었던 session을 session_id를 이용하여 session 보관소에서 제거하는 로직을 개발하여야 합니다.
     */
	unsigned long id = hash_djb2(session_id);
	if( -1 != id && id < HTBL_BUCKET_SIZE )
	{
		pthread_mutex_lock( &g_mtx );

		Node node = g_htbl[id];
		do{	
			if( NULL != node.pSession )
			{
				if( 0 == strcmp( session_id, node.pSession->session_id) )
				{
					close(node.pSession->sock_fd);
					free(node.pSession);
					node.pSession = NULL;
				}
			}

		}while(node.pNext);
		

		pthread_mutex_unlock( &g_mtx);
	}

    return;
}

smtp_session_t *addSmtpSession(smtp_session_t *session) {
    /*  TODO 과제 1-2
     *   smtp 세션을 추가
     *   전달받은 smtp session정보를 활용하여 현재 관리하고 있는 smtp session 보관소에 추가하는 로직을 개발하여야 합니다.
     */
	if( NULL == session )
		return NULL;
	
	unsigned long id = hash_djb2(session->session_id);
	if( -1 != id && id < HTBL_BUCKET_SIZE )
	{
		pthread_mutex_lock( &g_mtx );

		Node node = g_htbl[id];
		if( NULL == node.pSession )
		{
			node.pSession = session;
			pthread_mutex_unlock( &g_mtx );
			return session;
		}
		else
		{// hash collision.
			Node* p = malloc(sizeof(Node));
			p->pSession = session;
			p->pNext = NULL;
			node.pNext = p;
			pthread_mutex_unlock( &g_mtx );
			return session;
		}

		pthread_mutex_unlock( &g_mtx);
	}

    return NULL;
}

smtp_session_t *getSmtpSession(char *session_id) {
    /*  TODO 과제 1-3 :: 해당 로직은 비동기 로직 흐름상 과제 진행을 해도되고 안해도 무방합니다. (PASS 가능)
     *   smtp 세션을 session_id를 활용하여 가져오려고 합니다.
     *   전달받은 smtp session_id 정보를 활용하여 현재 관리하고 있는 smtp session 보관소에서 session을 가져오는 로직을 개발하여야 합니다.
     */
    return NULL;
}
