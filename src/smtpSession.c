#include "main.h"

/*  TODO 과제 1 공통 사항
 *  session 관리는 Socket정보를 보관하기 위해 사용합니다.
 *  다량의 smtp session 정보를 보관하는 보관소가 존재하여야 합니다..
 *  smtp session 관리 시 충돌을 방지하기 위한 적절한 매커니즘을 적용하여 개발하여야 합니다.
 */

//extern smtp_session_pool *sessions[MAX_POOL_NUM];

void delSmtpSession(char *session_id) {
    /*  TODO 과제 1-1
     *   smtp 세션을 종료
     *   그동안 사용되었던 session을 session_id를 이용하여 session 보관소에서 제거하는 로직을 개발하여야 합니다.
     */
	smtp_session_t *session = NULL;
	int pool_num = 0;
	for (pool_num = 0; pool_num < MAX_POOL_NUM; pool_num++) {
		if (sessions[pool_num].occupied == 1) {
			if (!strcmp(session_id, sessions[pool_num].sess_id)) {
				sessions[pool_num].occupied = 0;
				sessions[pool_num].used = 0;
				return;
			}
		}
	}
    return;
}

smtp_session_t *addSmtpSession(smtp_session_t *session) {
    /*  TODO 과제 1-2
     *   smtp 세션을 추가
     *   전달받은 smtp session정보를 활용하여 현재 관리하고 있는 smtp session 보관소에 추가하는 로직을 개발하여야 합니다.
     */
	int pool_num = 0;
	for (pool_num = 0; pool_num < MAX_POOL_NUM; pool_num++) {
		if (sessions[pool_num].occupied == 0) {
			sessions[pool_num].occupied = 1;
			strcpy(sessions[pool_num].sess_id, session->session_id);
			sessions[pool_num].session = (smtp_session_t *)session;
			return session;
		}
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

void initSessionPool()
{
	int pool_num = 0;
	for (pool_num = 0; pool_num < MAX_POOL_NUM; pool_num++) {
		sessions[pool_num].occupied = 0;
		sessions[pool_num].used = 0;
	}	
}

int getSmtpSessionIdxForPool() {
   	int pool_num = 0;
	for (pool_num = 0; pool_num < MAX_POOL_NUM; pool_num++) {
		if (sessions[pool_num].occupied == 1) {
			if (sessions[pool_num].used == 0) {
				sessions[pool_num].used = 1;
				return pool_num;
			}
		}
	}	 
	return -1;
}

void unusedSmtpSession(int pool_num) {
	if (sessions[pool_num].used == 1) {
		sessions[pool_num].used = 0;
	}
	return;
}
/*
void unusedSmtpSession(char) {
	int pool_num = 0;
	for (pool_num = 0; pool_num < MAX_POOL_NUM; pool_num++) {
		if (!strcmp(session_id, sessions[pool_num].sess_id)) {
			sessions[pool_num].used = 0;
		}
	}
} */
