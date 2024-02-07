#include "main.h"

/*  TODO 과제 1 공통 사항
 *  session 관리는 Socket정보를 보관하기 위해 사용합니다.
 *  다량의 smtp session 정보를 보관하는 보관소가 존재하여야 합니다..
 *  smtp session 관리 시 충돌을 방지하기 위한 적절한 매커니즘을 적용하여 개발하여야 합니다.
 */
static smtp_session_list_t ss_list;

int initSmtpSessionList() {
	ss_list.count = 0;
	ss_list.head = NULL;
	pthread_mutex_init(&ss_list.mutex, NULL);
}

void delSmtpSession(char *session_id) {
    /*  TODO 과제 1-1
     *   smtp 세션을 종료
     *   그동안 사용되었던 session을 session_id를 이용하여 session 보관소에서 제거하는 로직을 개발하여야 합니다.
     */
	//매개변수 검사
	if (session_id == NULL || strlen(session_id) <= 0) return;
	//리스트 개수 검사
	if (ss_list.count == 0) return;

	if (strcmp(ss_list.head->session_id, session_id) == 0) {
		pthread_mutex_lock(&ss_list.mutex);
		smtp_session_t* temp = ss_list.head;
		ss_list.head = ss_list.head->next;
		ss_list.count--;
		close(temp->sock_fd); //socket close
		free(temp); //memory release
		pthread_mutex_unlock(&ss_list.mutex);
	} else {
		pthread_mutex_lock(&ss_list.mutex);
		smtp_session_t* node = ss_list.head;
		while (node != NULL && strcmp(node->session_id, session_id)) {
			node = node->next;
		}
		if (node != NULL) {
			smtp_session_t* temp = node;
			node->next = node->next->next;
			ss_list.count--;
			close(temp->sock_fd); //socket close
			free(temp); //memory release
		}
		pthread_mutex_unlock(&ss_list.mutex);
	}

	LOG(LOG_DBG, "SMTP session del. (%d)", ss_list.count);
    return;
}

smtp_session_t *addSmtpSession(smtp_session_t *session) {
    /*  TODO 과제 1-2
     *   smtp 세션을 추가
     *   전달받은 smtp session정보를 활용하여 현재 관리하고 있는 smtp session 보관소에 추가하는 로직을 개발하여야 합니다.
     */
	if (session == NULL) {
		LOG (LOG_MAJ, "Err. SMTP session is null.\n");
		return NULL;
	}
	if (ss_list.count >= MAX_SMTP_SESSION) {
		LOG (LOG_MAJ, "Err. SMTP session list capacity exceeded.\n");
		return NULL;
	}

	pthread_mutex_lock(&ss_list.mutex);
	if (ss_list.head == NULL) {
		ss_list.head = session;
	} else {
		smtp_session_t* tail = ss_list.head;
		while (tail->next != NULL) {
			tail = tail->next;
		}
		tail->next = session;
	}
	ss_list.count++;
	pthread_mutex_unlock(&ss_list.mutex);

	LOG(LOG_DBG, "SMTP session [%s] add. (%d)", session->session_id, ss_list.count);
    return session;
}

smtp_session_t *getSmtpSessionById(char *session_id) {
    /*  TODO 과제 1-3 :: 해당 로직은 비동기 로직 흐름상 과제 진행을 해도되고 안해도 무방합니다. (PASS 가능)
     *   smtp 세션을 session_id를 활용하여 가져오려고 합니다.
     *   전달받은 smtp session_id 정보를 활용하여 현재 관리하고 있는 smtp session 보관소에서 session을 가져오는 로직을 개발하여야 합니다.
     */
	//매개변수 검사
	if (session_id == NULL || strlen(session_id) <= 0) return NULL;
	//리스트 개수 검사
	if (ss_list.count == 0) return NULL;

	if (strcmp(ss_list.head->session_id, session_id) == 0) {
		LOG(LOG_DBG, "SMTP session [%s] get. (%d)", session_id, ss_list.count);
		return ss_list.head;
	} else {
		pthread_mutex_lock(&ss_list.mutex);
		smtp_session_t* node = ss_list.head;
		while (node != NULL && strcmp(node->session_id, session_id)) {
			node = node->next;
		}
		pthread_mutex_unlock(&ss_list.mutex);
		if (node != NULL) {
			LOG(LOG_DBG, "SMTP session [%s] get. (%d)", node->session_id, ss_list.count);
			return node;
		}
	}
    return NULL;
}

/*
 * @brief SMTP 세션 리스트에서 SMTP 세션 하나를 가져오는 함수
 * @note SMTP 세션을 반환하기 전에 is_used flag를 세트시켜 중복 반환을 금지시킴!!!
 *
 * @return smtp_session_t*
 */
smtp_session_t *getSmtpSession() {
	//리스트 개수 검사
	if (ss_list.count == 0) return NULL;

	smtp_session_t* node = NULL;

	pthread_mutex_lock(&ss_list.mutex);
	node = ss_list.head;
	while (node != NULL && node->is_used) {
		node = node->next;
	}
	if (node != NULL) {
		node->is_used = 1;
		LOG(LOG_DBG, "SMTP session [%s] get. (%d)", node->session_id, ss_list.count);
	}
	pthread_mutex_unlock(&ss_list.mutex);

    return node;
}
