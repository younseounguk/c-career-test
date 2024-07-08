#include "main.h"

#define SMTP_PORT 2500
#define MAX_SEESSION 2048
int g_sys_close;
extern smtp_session_t g_session[MAX_SESSION];
/*
 * [어플리케이션 설명]
 * 다음 코드는 간단한 SMTP 서버의 구현입니다.
 * TCP Port 2500 으로 클라이언트의 요청을 받고 있으며 accept, date received, close등의 이벤트 감지 및 적절한 처리를 하고 있습니다.
 * 이벤트 감지 방법에는 다음 메커니즘이 제공 됩니다
 *  - 비동기식, N개의 Work thread (기본 설정)
 */

void initSession()
{
	for(int i=0;i<MAX_SESSION;i++)
	{
		g_session[i].sock_fd = NOT_CONNECTED;
	}
	g_sys_close = 0;
}

int main() {
	int server_fd;

	/* 1. Initialize log */
	setLogLevel(LOG_INF);
 	initSession();

	/* 2. Initialize Server socket */
	if ((server_fd = smtpServerOpen(SMTP_PORT)) <= 0) {
		LOG(LOG_CRT, "Error: Server socket initialization failed");
	}

	/* 3. Start smtp service based on socket type */
	smtpStartWorkThreads(MAX_ASYNC_WORKS_TH);
	smtpWaitAsync(server_fd);
	return 0;
}
