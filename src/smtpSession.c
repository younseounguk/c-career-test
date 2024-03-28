#include <semaphore.h>
#include "main.h"
#include "list_func.h"

/*  TODO 과제 1 공통 사항
 *  session 관리는 Socket정보를 보관하기 위해 사용합니다.
 *  다량의 smtp session 정보를 보관하는 보관소가 존재하여야 합니다..
 *  smtp session 관리 시 충돌을 방지하기 위한 적절한 매커니즘을 적용하여 개발하여야 합니다.
*/

list_t smtp_sessions[SESSION_HASH_SIZE];
int smtp_sessions_cnt;
sem_t g_sem;

static inline int getHashIndex(char *session_id)
{
	int i, len, tmp = 0;

	if (10 > (len = strlen(session_id))) {
		return 0;
	}

	/* 앞 4글자 공통 문자열이므로 idx 4부터 시작 */
	for (i = 4; i < len; i++) {
		tmp += session_id[i];
	}

	return tmp % SESSION_HASH_SIZE;
}

void initSessions(void)
{
	int i = 0;

	for (i = 0; i < SESSION_HASH_SIZE; i++) {
		initList(&smtp_sessions[i]);
	}
	smtp_sessions_cnt = 0;
	sem_init(&g_sem, 0, 1);
}

void cleanupSessions(void)
{
	smtp_session_t *entry = NULL;
	list_t *pos, *next = NULL;
	int i = 0;

	sem_wait(&g_sem);
	for (i = 0; i < SESSION_HASH_SIZE; i++) {
		list_for_each(pos, next, &smtp_sessions[i]) {
			entry = list_entry(pos, smtp_session_t, chain);
			delList(&entry->chain);
			free(entry);
			smtp_sessions_cnt--;
		}
	}
	sem_post(&g_sem);
	sem_destroy(&g_sem);
}

void delSmtpSession(char *session_id) {
    /*  TODO 과제 1-1
     *   smtp 세션을 종료
     *   그동안 사용되었던 session을 session_id를 이용하여 session 보관소에서 제거하는 로직을 개발하여야 합니다.
     */
	smtp_session_t *entry = NULL;
	list_t *pos, *next = NULL;
	int idx = 0;

	idx = getHashIndex(session_id);

	sem_wait(&g_sem);
	list_for_each(pos, next, &smtp_sessions[idx]) {
		entry = list_entry(pos, smtp_session_t, chain);
		if (0 != strcmp(session_id, entry->session_id)) {
			continue;
		}
		LOG(LOG_DBG, "session [%s] is deleted.", entry->session_id);
		delList(&entry->chain);
		close(entry->sock_fd);
		free(entry);
		smtp_sessions_cnt--;
		break;
	}
	sem_post(&g_sem);
}

smtp_session_t *addSmtpSession(smtp_session_t *session) {
    /*  TODO 과제 1-2
     *   smtp 세션을 추가
     *   전달받은 smtp session정보를 활용하여 현재 관리하고 있는 smtp session 보관소에 추가하는 로직을 개발하여야 합니다.
     */
	int idx = 0;

	idx = getHashIndex(session->session_id);
	sem_wait(&g_sem);
	addList(&session->chain, &smtp_sessions[idx]);
	LOG(LOG_DBG, "session [%s] is added.", session->session_id);
	smtp_sessions_cnt++;
	sem_post(&g_sem);

	return session;
}

smtp_session_t *getSmtpSession(char *session_id) {
    /*  TODO 과제 1-3 :: 해당 로직은 비동기 로직 흐름상 과제 진행을 해도되고 안해도 무방합니다. (PASS 가능)
     *   smtp 세션을 session_id를 활용하여 가져오려고 합니다.
     *   전달받은 smtp session_id 정보를 활용하여 현재 관리하고 있는 smtp session 보관소에서 session을 가져오는 로직을 개발하여야 합니다.
     */
	smtp_session_t *entry = NULL;
	list_t *pos, *next = NULL;
	int idx = 0;

	idx = getHashIndex(session_id);

	sem_wait(&g_sem);
	list_for_each(pos, next, &smtp_sessions[idx]) {
		entry = list_entry(pos, smtp_session_t, chain);
		if (0 != strcmp(session_id, entry->session_id)) {
			continue;
		}
		LOG(LOG_DBG, "session [%s] is found.", session_id);
		sem_post(&g_sem);
		return entry;
	}
	sem_post(&g_sem);

	return NULL;
}

smtp_session_t *getFirstSmtpSession(void) {
	smtp_session_t *entry = NULL;
	list_t *pos, *next = NULL;
	int i = 0;

	if (0 == smtp_sessions_cnt) {
		return NULL;
	}

	sem_wait(&g_sem);
	for (i = 0; i < SESSION_HASH_SIZE; i++) {
		if (isEmptyList(&smtp_sessions[i])) {
			continue;
		}
		sem_post(&g_sem);
		return list_entry((&smtp_sessions[i])->next, smtp_session_t, chain);
	}
	sem_post(&g_sem);

	return NULL;
}
