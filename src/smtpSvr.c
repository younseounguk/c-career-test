//
// Created by srkim on 23. 2. 17.
//

#include <fcntl.h>
#include <stdint-gcc.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <malloc.h>
#include <string.h>
#include "../include/smtpSvr.h"
#include "../include/logger.h"
#include "../include/smtpSock.h"
#include "../include/smtpData.h"
#include "../include/smtpSvrRecvFdSet.h"
#include "../include/smtpUtils.h"

int    gSysClose    =  0 ;

void * smtpWait ()
{
    int nErr            ;
    int clientFd        ;
    int serverFd = -1   ;
    int isHave   =  0   ;
    int sessIdx  =  0   ;

    unsigned int connId = 0 ;
    SmtpSvrArgs_t   * pSvrArgs = NULL ;

    pthread_t           clientTh     ;
    pthread_attr_t      clientThAttr ;
    pthread_attr_init (&clientThAttr);

    nErr = pthread_attr_setstacksize ( &clientThAttr, (10 * 1024 * 1024) ) ;

    serverFd = smtpServerOpen( SMTP_PORT ) ;
    LOG (LOG_INF, "< THR > HTTP Wait Thread ( PORT = %4d )  - FD : %-2u Started ..................... [ OK ]\n"
            , SMTP_PORT , serverFd ) ;

    while ( !gSysClose ) {
        clientFd = smtpAcceptSock(serverFd);

        if (clientFd < 0) {
            LOG (LOG_CRT, "Err. H2AcceptSock() Failed. serverFd = %d\n", serverFd);
            clientFd = -1;
            continue;
        }

        pSvrArgs = (SmtpSvrArgs_t *) malloc(sizeof(SmtpSvrArgs_t));
        if (NULL == pSvrArgs) {
            close(clientFd);
            continue;
        }

        pSvrArgs->SockFd = clientFd ;
        pSvrArgs->PortNum = smtpGetPeerPortNum(clientFd);
        smtpGetPeerIP4Addr ( clientFd , pSvrArgs->StrIP4 );

        if ( (nErr=pthread_create(&clientTh , &clientThAttr , H_SERVER_RECV_FDSET_TH , (void *)(pSvrArgs))) < 0 ) {
            LOG ( LOG_MAJ , "Err. New Client Thread Create Failed. Err.= '%s'\n" , strerror(nErr) ) ;
            close ( clientFd ) ;
            msleep ( 100 ) ;
            continue ;
        }

    }

    return NULL;
}