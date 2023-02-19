//
// Created by srkim on 23. 2. 19.
//

#include <stddef.h>
#include <pthread.h>
#include <malloc.h>
#include <sys/select.h>
#include <errno.h>
#include <string.h>
#include <stdint-gcc.h>
#include "../include/smtpSvrRecvFdSet.h"
#include "../include/smtpData.h"
#include "../include/logger.h"
#include "../include/smtpSock.h"

extern int gSysClose;


void * H_SERVER_RECV_FDSET_TH ( void * args )
{
    struct timeval  timeValue ;
    fd_set readFds   ;
    size_t nLine;
    char * buf[MAX_BUF_SIZE] = {0,};
    int SockFd, maxFd, nErr;
    SmtpSvrArgs_t  * pSvrArgs     = (SmtpSvrArgs_t *)args ;
    pthread_detach ( pthread_self() ) ;

    SockFd = pSvrArgs->SockFd ;
    while ( !gSysClose ) {
        FD_ZERO ( &readFds ) ;
        FD_SET  ( SockFd , &readFds);
        maxFd = SockFd ;
        timeValue.tv_sec  = 1 ;
        timeValue.tv_usec = 0 ;
        nErr  = select ( maxFd+1 , &readFds , NULL , NULL , &timeValue ) ;

        if ( nErr < 0 ) {
            if ( errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK ) continue ;
            LOG ( LOG_MAJ , "%s : Err. select failed. error = %s\n", __func__, strerror(errno) ) ;
            break ;
        }
        nLine = smtpReadLine(SockFd, buf, sizeof(buf));
        if (nLine == 0) {
            break;
        }


        LOG(LOG_DBG, "%s", buf);

    }

    LOG ( LOG_INF , "%s : SMTP Connection close : fd = %d\n", __func__, SockFd) ;
    free(pSvrArgs);
    return NULL;
}