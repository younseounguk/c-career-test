#include "main.h"

/*  TODO 과제 1 공통 사항
 *  session 관리는 Socket정보를 보관하기 위해 사용합니다.
 *  다량의 smtp session 정보를 보관하는 보관소가 존재하여야 합니다..
 *  smtp session 관리 시 충돌을 방지하기 위한 적절한 매커니즘을 적용하여 개발하여야 합니다.
 */
smtp_session_t g_session[MAX_SESSION];
pthread_mutex_t session_mutex = PTHREAD_MUTEX_INITIALIZER;

void delSmtpSession(char *session_id) {

	pthread_mutex_lock(&session_mutex);

	/*  TODO 과제 1-1
	 *   smtp 세션을 종료
	 *   그동안 사용되었던 session을 session_id를 이용하여 session 보관소에서 제거하는 로직을 개발하여야 합니다.
	 */
	for(int i=0;i<MAX_SESSION;i++)
	{
		size_t session_id_len = strlen(session_id);
		if(strncmp(session_id, g_session[i].session_id, session_id_len) == 0)
		{	
			close(g_session[i].sock_fd);
			g_session[i].sock_fd = NOT_CONNECTED;
			break;
		}
	}

	pthread_mutex_unlock(&session_mutex);

	return;
}

void copy_session(smtp_session_t *session, int index)
{
	/* TCP Information */
	g_session[index].sock_fd = session->sock_fd;
	g_session[index].port_num = session->port_num;
	snprintf(g_session[index].str_ipv4, 		sizeof(g_session[index].str_ipv4), "%s", session->str_ipv4);
	snprintf(g_session[index].session_id, 		sizeof(g_session[index].session_id), "%s", session->session_id);

    	/* SMTP Information */
	g_session[index].smtp_status = session->smtp_status;
	g_session[index].smtp_last_message = session->smtp_last_message;
	snprintf(g_session[index].smtp_data, 		sizeof(g_session[index].smtp_data), "%s", session->smtp_data);
	snprintf(g_session[index].smtp_rcpt_to, 	sizeof(g_session[index].smtp_rcpt_to), "%s", session->smtp_rcpt_to);
	snprintf(g_session[index].smtp_mail_from, 	sizeof(g_session[index].smtp_mail_from), "%s", session->smtp_mail_from);
}

smtp_session_t *addSmtpSession(smtp_session_t *session) {
	/*  TODO 과제 1-2
	 *   smtp 세션을 추가
	 *   전달받은 smtp session정보를 활용하여 현재 관리하고 있는 smtp session 보관소에 추가하는 로직을 개발하여야 합니다.
	 */
	pthread_mutex_lock(&session_mutex);

	for(int i=0;i<MAX_SESSION;i++)
	{
		if(g_session[i].sock_fd != NOT_CONNECTED) continue;
		copy_session(session, i);
		pthread_mutex_unlock(&session_mutex);
		return session;
	}

	pthread_mutex_unlock(&session_mutex);

	return NULL;
}

smtp_session_t *getSmtpSession(char *session_id) {
	/*  TODO 과제 1-3 :: 해당 로직은 비동기 로직 흐름상 과제 진행을 해도되고 안해도 무방합니다. (PASS 가능)
	 *   smtp 세션을 session_id를 활용하여 가져오려고 합니다.
	 *   전달받은 smtp session_id 정보를 활용하여 현재 관리하고 있는 smtp session 보관소에서 session을 가져오는 로직을 개발하여야 합니다.
	 */

	pthread_mutex_lock(&session_mutex);

	for(int i=0;i<MAX_SESSION;i++)
	{
		size_t session_id_len = strlen(session_id);
		if(!strncmp(session_id, g_session[i].session_id, session_id_len))
		{	
			pthread_mutex_unlock(&session_mutex);
			return &g_session[i];
		}
	}

	pthread_mutex_unlock(&session_mutex);

	return NULL;
}
