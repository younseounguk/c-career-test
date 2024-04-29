#include "main.h"

/* TODO 과제 2 공통
 *  비동기 통신방식으로 데이터가 수신 처리되어야 합니다.
 *  기존 소스코드 변경 및 파일이 추가하는데에는 전혀 제약사향이 없습니다.
 *  socket 정보 관리는 session을 통해서 관리되어야 합니다.
 *  smtpHandleInboundConnection로직을 통해 Connection을 맺는걸 권유 드립니다.(해당 로직을 사용안하셔도 무방합니다.)
 *
 */
#include <sys/epoll.h>


pthread_mutex_t gMQmtx = PTHREAD_MUTEX_INITIALIZER;

typedef struct _sQNode{

	smtp_session_t *session;
	struct _sQNode *next;

}sQNode;

sQNode gfirst; 

void enqueue( smtp_session_t *session )
{
	pthread_mutex_lock( &gMQmtx );
	
	sQNode* node = calloc(1, sizeof(sQNode));
	node->session = session;

	sQNode* cur = &gfirst; 

	while( 1 ){
		if( NULL == cur->next )
		{
			cur->next = node;
			break;
		}
		
		cur = cur->next;
	}
	
	pthread_mutex_unlock( &gMQmtx );	
}

smtp_session_t* dequeue()
{
	pthread_mutex_lock( &gMQmtx );

	smtp_session_t *session = NULL;

	sQNode* cur = &gfirst; 
	sQNode* prev = NULL;

	while( 1 )
	{
		if( NULL == cur->next )
		{
			session = cur->session;

			if( prev ) 
				prev->next = NULL;
			if( &gfirst != cur ) 
				free(cur);

			break;
		}
		
		prev = cur;
		cur = cur->next;
	}

	pthread_mutex_unlock( &gMQmtx );	
	
	return session;
}


#define EPOLL_WAIT_TIME		(-1)
#define MAXEVENTS		2048

int epoll_fd;		// file descriptor refererring to the new epoll instance.

void smtpWaitAsync(int server_fd) {
        /* TODO 과제 2-1
        *  smtpSvrRecvAsync.c 파일은 비동기 처리를 이용하여 데이터를 제어 하는 로직이 작성 되어 있습니다.
        *  N개의 H_SERVER_WORK_TH 스레드와 하나의 smtpWaitAsync 스레드가 동시에 동작하고 있습니다.
        *  smtpWaitAsync Function에서 Connection을 맺고(smtpHandleInboundConnection로직 사용 권유)
        *  비동기 통신이 가능하게 개발한후 session정보를 WorkThread로 전달하는 로직을 개발하여야 합니다.
        */

	int event_count;
	struct epoll_event ie;
	struct epoll_event *events = NULL;
	smtp_session_t *session = NULL;

	// create epoll instance
	epoll_fd = epoll_create1(0);

	// Event : server socket's client requests.
	ie.data.fd = server_fd;
	ie.events = EPOLLIN;

	epoll_ctl( epoll_fd, EPOLL_CTL_ADD, server_fd, &ie );

	events = calloc(MAXEVENTS, sizeof(struct epoll_event));

	if( NULL == events )
		return;

	while( !g_sys_close )
	{
		int i = 0;
		event_count = epoll_wait(epoll_fd, events, MAXEVENTS, EPOLL_WAIT_TIME);
		
		for( i; i < event_count; ++i )
		{
			if( events[i].data.fd == server_fd )
			{
				if( NULL == (session = smtpHandleInboundConnection(server_fd)))
					break;

				ie.events = EPOLLIN;
				ie.data.fd = session->sock_fd;
				ie.data.ptr = (void*)session;

				sendGreetingMessage(session);
				epoll_ctl( epoll_fd, EPOLL_CTL_ADD, session->sock_fd, &ie);
				
			}
			else
			{
				session = events[i].data.ptr;
				epoll_ctl( epoll_fd, EPOLL_CTL_DEL, session->sock_fd, NULL );

				// session info to worker thread.
				enqueue(session);
			}
		}				
	}

	free(events);
	close(server_fd);
	close(epoll_fd);

}

void *H_SERVER_WORK_TH(void *args) {
    int nLine, nErr;
    char buf[MAX_BUF_SIZE];
    smtp_session_t *session = NULL;

    while (!g_sys_close) {

        /* TODO 과제 2-2
        *  session정보를 해당 위치에 받아 올수있게 개발하여야 합니다.
        */
	struct epoll_event ie;
	
	session = dequeue();

        if (session == NULL) {
            msleep(25);
            continue;
        }

        if ((nLine = smtpReadLine(session->sock_fd, buf, sizeof(buf))) <= 0) {
            LOG (LOG_INF, "%s : %sSMTP Connection closed%s : fd = %d, session_id=%s\n", __func__, C_YLLW, C_NRML,
                 session->sock_fd,
                 session->session_id);
            delSmtpSession(session->session_id);
            continue;
        }

        if ((nErr = doSmtpDispatch(session, buf)) != SMTP_DISPATCH_OK) {
            if (nErr == SMTP_DISPATCH_FAIL) {
                LOG(LOG_INF, "Smtp connection close by error!");
            }
            continue;
        }

	ie.events = EPOLLIN;
	ie.data.fd = session->sock_fd;
	ie.data.ptr = (void*)session;

	epoll_ctl( epoll_fd, EPOLL_CTL_ADD, session->sock_fd, &ie); 

    }
    return NULL;
}


int smtpStartWorkThreads(int n_work_threads) {
    int nErr;
    pthread_t clientTh;
    pthread_attr_t clientThAttr;
    pthread_attr_init(&clientThAttr);

    nErr = pthread_attr_setstacksize(&clientThAttr, (10 * 1024 * 1024));

    for (int i = 0; i < n_work_threads; i++) {
        if ((nErr = pthread_create(&clientTh, &clientThAttr, H_SERVER_WORK_TH, NULL)) < 0) {
            LOG (LOG_MAJ, "Err. Worker Thread Create Failed. Err.= '%s', idx=%d\n", strerror(nErr), i);
            return -1;
        }
    }

    return 0;
}
