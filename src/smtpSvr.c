//
// Created by srkim on 23. 2. 17.
//


#include "main.h"

int    gSysClose    =  0 ;

void * smtpWait ()
{
    int nErr            ;
    int clientFd        ;
    int serverFd = -1   ;
    int isHave   =  0   ;
    int sessIdx  =  0   ;

    unsigned int connId = 0 ;
    SmtpSession_t   * session = NULL ;

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

        session = (SmtpSession_t *) malloc(sizeof(SmtpSession_t));
        if (NULL == session) {
            close(clientFd);
            continue;
        }

        session->SockFd = clientFd ;
        session->PortNum = smtpGetPeerPortNum(clientFd);
        smtpGetPeerIP4Addr ( clientFd , session->StrIP4 );
        smtpSetSessionId(session);

        if (addSmtpSession(session) == NULL) {
            LOG ( LOG_MAJ , "Err. Fail to make smtp session\n") ;
            close ( clientFd ) ;
            continue;
        }

        if ( (nErr=pthread_create(&clientTh , &clientThAttr , H_SERVER_RECV_FDSET_TH , (void *)(session))) < 0 ) {
            LOG ( LOG_MAJ , "Err. New Client Thread Create Failed. Err.= '%s'\n" , strerror(nErr) ) ;
            close ( clientFd ) ;
            msleep ( 100 ) ;
            continue ;
        }

    }

    return NULL;
}