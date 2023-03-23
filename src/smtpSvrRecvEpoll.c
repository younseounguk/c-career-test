#include "main.h"
#include <sys/epoll.h>

#define EPOLL_SIZE 1024
int epoll_fd;

/* TODO 과제 2-1
 *  smtpSvrRecvEpoll.c 파일은 비동기 처리를 위해 epoll을 제어 하는 로직이 작성 되어 있습니다.
 *  N개의 H_SERVER_EPOLL_WORK_TH 스레드와 하나의 smtpWaitAsync 스레드가 동시에 동작하고 있습니다.
 *  그런데 비동기 처리를 위해서 필요한 처리가 누락 되어 있습니다.
 *  적절한 처리를 하는 로직을 본 소스에 추가 하시오
 */
void smtpWaitAsync(int server_fd) {
    int event_count;
    struct epoll_event init_event;
    epoll_fd = epoll_create(EPOLL_SIZE);
    struct epoll_event *events = malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

    memset(&init_event, 0x00, sizeof(struct epoll_event));
    init_event.events = EPOLLIN;
    init_event.data.fd = server_fd;
    smtp_session_t *session = NULL;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &init_event);

    while (!g_sys_close) {
        event_count = epoll_wait(epoll_fd, events, EPOLL_SIZE, -1);

        for (int i = 0; i < event_count; ++i) {

            if (events[i].data.fd == server_fd) {
                if ((session = smtpHandleInboundConnection(server_fd)) == NULL) {
                    break;
                }
                init_event.events = EPOLLIN;
                init_event.data.fd = session->sock_fd;
                init_event.data.ptr = (void *) session;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, session->sock_fd, &init_event);
                LOG (LOG_INF, "%s : %sSMTP Connection created%s : fd = %d, session_id=%s\n", __func__, C_YLLW, C_NRML,
                     session->sock_fd, session->session_id);
                sendGreetingMessage(session);
            } else {
                session = events[i].data.ptr;
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, session->sock_fd, NULL);
                itcqPutSession(session);
            }
        }
    }
    close(server_fd);
    close(epoll_fd);
}

void *H_SERVER_EPOLL_WORK_TH(void *args) {
    int nLine, nErr;
    char buf[MAX_BUF_SIZE];
    smtp_session_t *session = NULL;
    struct epoll_event init_event;

    memset(&init_event, 0x00, sizeof(struct epoll_event));

    while (!g_sys_close) {
            session = itcqGetSession();

        if (session == NULL) {
            msleep(25);
            continue;
        }

        if ((nLine = smtpReadLine(session->sock_fd, buf, sizeof(buf))) <= 0) {
            LOG (LOG_INF, "%s : %sSMTP Connection closed%s : fd = %d, session_id=%s\n", __func__, C_YLLW, C_NRML,
                 session->sock_fd,
                 session->session_id);
            /* TODO 과제 2-2
             *  다음 라인의 delSmtpSession(...) 함수는 소켓 연결이 끊어질 경우 적절한 처리를 하는 함수 입니다.
             *  내부 구현은 제거 된 상태여서 할당 된 자원이 회수 되지 않고 있습니다.
             *  적절한 로직을 넣어 SMTP 연결에 할당된 자원을 회수 하시오 (과제 1-2에 해당 과제 추가)
             */
            delSmtpSession(session->session_id);
            continue;
        }

        if ((nErr = doSmtpDispatch(session, buf)) != SMTP_DISPATCH_OK) {
            if (nErr == SMTP_DISPATCH_FAIL) {
                LOG(LOG_INF, "Smtp connection close by error!");
            }
            continue;
        }
        init_event.events = EPOLLIN;
        init_event.data.fd = session->sock_fd;
        init_event.data.ptr = (void *) session;

        /* TODO 과제 2-3(서술형)
         *  다음 라인에서 epoll_ctl(...) 함수를 주석 등으로 제거 할 경우 첫번째 SMTP EHLO 메시지 처리 이후 다음 메시지를 처리 하지 못 합니다.
         *  해당 현상이 발생하게 되는 이유를 서술 하세요
         *  ----------면접자 답안 작성------------
         *
         *
         *
         */
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, session->sock_fd, &init_event);

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
        if ((nErr = pthread_create(&clientTh, &clientThAttr, H_SERVER_EPOLL_WORK_TH, NULL)) < 0) {
            LOG (LOG_MAJ, "Err. Worker Thread Create Failed. Err.= '%s', idx=%d\n", strerror(nErr), i);
            return -1;
        }
    }

    return 0;
}