#include "main.h"

/* TODO 과제 2 공통
 *  비동기 통신방식으로 데이터가 수신 처리되어야 합니다.
 *  기존 소스코드 변경 및 파일이 추가하는데에는 전혀 제약사향이 없습니다.
 *  socket 정보 관리는 session을 통해서 관리되어야 합니다.
 *  smtpHandleInboundConnection로직을 통해 Connection을 맺는걸 권유 드립니다.(해당 로직을 사용안하셔도 무방합니다.)
 *
 */

extern smtp_session_t g_session[MAX_SESSION];

void *H_SERVER_SESSION_TH(void *args)
{
	int server_fd = *((int *)args);
	int max_fd = server_fd;
	fd_set sockets, copy_sockets;
	FD_ZERO(&sockets);
	FD_SET(server_fd, &sockets);
	while(1)
	{
		copy_sockets = sockets;
		select(max_fd+1, &copy_sockets, NULL, NULL, NULL);
		if(FD_ISSET(server_fd, &sockets))
		{
			smtp_session_t *session = smtpHandleInboundConnection(server_fd);
			if(session == NULL) continue;
		}
	}	
}

void smtpWaitAsync(int server_fd) {
	/* TODO 과제 2-1
	 *  smtpSvrRecvAsync.c 파일은 비동기 처리를 이용하여 데이터를 제어 하는 로직이 작성 되어 있습니다.
	 *  N개의 H_SERVER_WORK_TH 스레드와 하나의 smtpWaitAsync 스레드가 동시에 동작하고 있습니다.
	 *  smtpWaitAsync Function에서 Connection을 맺고(smtpHandleInboundConnection로직 사용 권유)
	 *  비동기 통신이 가능하게 개발한후 session정보를 WorkThread로 전달하는 로직을 개발하여야 합니다.
	 */
	int nErr;
	pthread_t serverTh;
	pthread_attr_t serverThAttr;
	pthread_attr_init(&serverThAttr);

	nErr = pthread_attr_setstacksize(&serverThAttr, (10 * 1024 * 1024));

	if ((nErr = pthread_create(&serverTh, &serverThAttr, H_SERVER_SESSION_TH, (void *)&server_fd)) < 0) {
		LOG (LOG_MAJ, "Err. Worker Thread Create Failed. Err.= '%s'\n", strerror(nErr));
	}

	pthread_join(serverTh, (void **)NULL);
}

void *H_SERVER_WORK_TH(void *args) 
{
	int nLine, nErr;
	char buf[MAX_BUF_SIZE];
	smtp_session_t *session = NULL;

	while (!g_sys_close) {

		/* TODO 과제 2-2
		 *  session정보를 해당 위치에 받아 올수있게 개발하여야 합니다.
		 */
		/*	
		if (session == NULL) {
			msleep(25);
			continue;
		}
		*/
		for(int i=0;i<MAX_SESSION;i++)
		{
			if(g_session[i].sock_fd == NOT_CONNECTED) continue;			
			fd_set sockset;
			struct timeval tv;

			FD_ZERO(&sockset);
			FD_SET(g_session[i].sock_fd, &sockset);
			tv.tv_sec = 120;
			tv.tv_usec = 0;

			select(g_session[i].sock_fd+1, &sockset, NULL, NULL, &tv);

			if (FD_ISSET(g_session[i].sock_fd, &sockset)) 
			{
				if ((nLine = smtpReadLine(g_session[i].sock_fd, buf, sizeof(buf))) <= 0) {
					LOG (LOG_INF, "%s : %sSMTP Connection closed%s : fd = %d, session_id=%s\n", __func__, C_YLLW, C_NRML,
							g_session[i].sock_fd,
							g_session[i].session_id);
					delSmtpSession(g_session[i].session_id);
					continue;
				}

				if ((nErr = doSmtpDispatch(&g_session[i], buf)) != SMTP_DISPATCH_OK) {
					if (nErr == SMTP_DISPATCH_FAIL) {
						LOG(LOG_INF, "Smtp connection close by error!");
					}
					continue;
				}
			}
		}
	}
	return NULL;
}


int smtpStartWorkThreads(int n_work_threads) {
	int nErr;
	pthread_t clientTh[MAX_ASYNC_WORKS_TH];
	pthread_attr_t clientThAttr[MAX_ASYNC_WORKS_TH];


	for (int i = 0; i < n_work_threads; i++) {
		nErr = pthread_attr_setstacksize(&clientThAttr[i], (10 * 1024 * 1024));
		pthread_attr_init(&clientThAttr[i]);
		if ((nErr = pthread_create(&clientTh[i], &clientThAttr[i], H_SERVER_WORK_TH, NULL)) < 0) {
			LOG (LOG_MAJ, "Err. Worker Thread Create Failed. Err.= '%s', idx=%d\n", strerror(nErr), i);
			return -1;
		}
		
		pthread_detach(clientTh[i]);
	}

	return 0;
}
