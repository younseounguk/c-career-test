//
// Created by srkim on 23. 2. 22.
//
#include <sys/epoll.h>
#include "main.h"

#define EPOLL_SIZE 1024
int epoll_fd;


void smtpWaitAsync(int serverFd){
    epoll_fd = epoll_create(EPOLL_SIZE);
    struct epoll_event* events = malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

    int event_count, nLine, nErr;
    struct epoll_event init_event;
    memset(&init_event, 0x00, sizeof(struct epoll_event));
    init_event.events = EPOLLIN;
    init_event.data.fd = serverFd;
    SmtpSession_t *session = NULL;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serverFd, &init_event);

    while (!gSysClose) {
        event_count = epoll_wait(epoll_fd, events, EPOLL_SIZE, -1);

        for( int i = 0 ; i < event_count; ++i ) {

            if(events[i].data.fd == serverFd)
            {
                if ((session = smtpHandleInboundConnection(serverFd)) == NULL) {
                    break;
                }
                init_event.events = EPOLLIN;
                init_event.data.fd = session->SockFd;
                init_event.data.ptr = (void *)session;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, session->SockFd, &init_event);
                LOG ( LOG_INF , "%s : SMTP Connection created : fd = %d, SessionId=%s\n", __func__, session->SockFd, session->SessionId) ;
                sendGreetingMessage(session);
            }
            else {
                itcqPutSession(events[i].data.ptr);
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, session->SockFd, NULL);
            }
        }
    }
    close(serverFd);
    close(epoll_fd);
}

void * H_SERVER_EPOLL_WORK_TH ( void * args )
{
    int nLine, nErr;
    struct epoll_event init_event;
    memset(&init_event, 0x00, sizeof(struct epoll_event));
    char buf[MAX_BUF_SIZE] = {0,};
    SmtpSession_t * session = NULL;
    while (!gSysClose) {
        session = itcqGetSession();
        if (session == NULL) {
            msleep(10);
            continue;
        }



        if ((nLine = smtpReadLine(session->SockFd, buf, sizeof(buf))) <= 0) {
            LOG ( LOG_INF , "%s : SMTP Connection closed : fd = %d, SessionId=%s\n", __func__, session->SockFd, session->SessionId) ;
            continue;
        }

        if ((nErr = doSmtpDispatch(session, buf)) != SMTP_DISPATCH_OK) {
            if (nErr == SMTP_DISPATCH_FAIL) {
                LOG(LOG_INF, "Smtp connection close by error!");
            }
            continue;
        }
        init_event.events = EPOLLIN;
        init_event.data.fd = session->SockFd;
        init_event.data.ptr = (void *)session;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, session->SockFd, &init_event);

    }
    return NULL;
}


int smtpStartWorkThreads(int nWorkThreads) {
    int nErr;
    pthread_t clientTh;
    pthread_attr_t clientThAttr;
    pthread_attr_init(&clientThAttr);

    nErr = pthread_attr_setstacksize(&clientThAttr, (10 * 1024 * 1024));

    for (int i=0;i<nWorkThreads;i++) {
        if ((nErr = pthread_create(&clientTh, &clientThAttr, H_SERVER_EPOLL_WORK_TH, NULL)) < 0) {
            LOG (LOG_MAJ, "Err. Worker Thread Create Failed. Err.= '%s', idx=%d\n", strerror(nErr), i);
            return -1;
        }
    }

    return 0;
}