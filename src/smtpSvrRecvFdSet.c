//
// Created by srkim on 23. 2. 19.
//

#include "main.h"







void smtpWaitSync(int serverFd) {
    int nErr;

    SmtpSession_t *session = NULL;

    pthread_t clientTh;
    pthread_attr_t clientThAttr;
    pthread_attr_init(&clientThAttr);

    nErr = pthread_attr_setstacksize(&clientThAttr, (10 * 1024 * 1024));

    while (!gSysClose) {

        if ((session = smtpHandleInboundConnection(serverFd)) == NULL) {
            break;
        }

        if ((nErr = pthread_create(&clientTh, &clientThAttr, H_SERVER_RECV_FDSET_TH, (void *) (session))) < 0) {
            LOG (LOG_MAJ, "Err. New Client Thread Create Failed. Err.= '%s'\n", strerror(nErr));
            delSmtpSession(session->SessionId);
            msleep(100);
            continue;
        }
    }
    close(serverFd);
}

void * H_SERVER_RECV_FDSET_TH ( void * args )
{
    struct timeval  timeValue ;
    fd_set readFds   ;
    size_t nLine;
    char buf[MAX_BUF_SIZE] = {0,};
    int sockFd, maxFd, nErr;
    SmtpSession_t  * session     = (SmtpSession_t *)args ;
    pthread_detach ( pthread_self() ) ;

    sockFd = session->SockFd ;
    LOG ( LOG_INF , "%s : SMTP Connection created : fd = %d, SessionId=%s\n", __func__, sockFd, session->SessionId) ;
    sendGreetingMessage(session);
    while ( !gSysClose ) {
        FD_ZERO ( &readFds ) ;
        FD_SET  ( sockFd , &readFds);
        maxFd = sockFd ;
        timeValue.tv_sec  = 1 ;
        timeValue.tv_usec = 0 ;
        nErr  = select ( maxFd+1 , &readFds , NULL , NULL , &timeValue ) ;

        if ( nErr < 0 ) {
            if ( errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK ) continue ;
            LOG ( LOG_MAJ , "%s : Err. select failed. error = %s\n", __func__, strerror(errno) ) ;
            break ;
        }
        if ((nLine = smtpReadLine(sockFd, buf, sizeof(buf))) <= 0) {
            break;
        }

        if ((nErr = doSmtpDispatch(session, buf)) != SMTP_DISPATCH_OK) {
            if (nErr == SMTP_DISPATCH_FAIL) {
                LOG(LOG_INF, "Smtp connection close by error!");
            }
            break;
        }
    }

    LOG ( LOG_INF , "%s : SMTP Connection closed : fd = %d, SessionId=%s\n", __func__, sockFd, session->SessionId) ;
    delSmtpSession(session->SessionId);
    return NULL;
}