//
// Created by srkim on 23. 2. 19.
//



#include "main.h"

int smtpAcceptSock(int serverFd) {
    struct sockaddr_in sockAddrin;

    int clientFd = -1;
    int addrLen = sizeof(sockAddrin);

    clientFd = accept(serverFd, (struct sockaddr *) &sockAddrin, (socklen_t *) &addrLen);

    LOG (LOG_TRC, "TCP Socket Accepted < new client : %d > \n", clientFd);

    if (clientFd < 0) {
        LOG (LOG_MAJ, "%s : Err. accept() Failed. serverFd = %d , Err is ( %d , %s )\n", __func__, serverFd, errno,
             strerror(errno));
        return -1;
    }

    return clientFd;
}


int smtpServerOpen(uint16_t nPort) {
    struct sockaddr_in sockInAddr;
    struct linger optLinger;

    int nRet = 0;
    int nOpt = 0;
    int nReuse = 1;
    int sockFd = -1;

    /* 1. Create Server Socket				*/
    if ((sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        LOG(LOG_INF, "Err.  socket() Failed. \n");
        return (-1);
    }

    /* 2. Set socket option : Reuse Address */
    if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, (char *) &nReuse, sizeof(int)) < 0) {
        LOG(LOG_INF, "Err.  setsockopt() : Reuse Err. nRet = %d\n", nRet);
        close(sockFd);
        return (-1);
    }

    /* 3. Set Socket Option : Linger : When Socket Closed, Prevent Time_Wait Status of Socket   */
    optLinger.l_onoff = 1;
    optLinger.l_linger = 0;

    if (setsockopt(sockFd, SOL_SOCKET, SO_LINGER, (char *) &optLinger, sizeof(optLinger)) < 0) {
        LOG(LOG_INF, "Err.  setsockopt() : Linger Err. nRet = %d\n", nRet);
        close(sockFd);
        return (-1);
    }

    /* 4. Set Socket Option : Re-Use Port	*/
    nOpt = 1;
    if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEPORT, &nOpt, sizeof(nOpt)) < 0) {
        close(sockFd);
        return (-1);
    }

    /* 5. Set Socket Option : Recv. Socket Buffer	*/
    nOpt = 8 * 1024 * 1024;
    if (setsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, &nOpt, sizeof(nOpt)) < 0) {
        LOG(LOG_INF, "Err. Setsockopt() : Set Receive Socket Buffer Failed. \n");
        close(sockFd);
        return (-1);
    }

    /* 6. Set Socket Option : Send. Socket Buffer	*/
    if (setsockopt(sockFd, SOL_SOCKET, SO_SNDBUF, &nOpt, sizeof(nOpt)) < 0) {
        LOG(LOG_INF, "Err. Setsockopt() : Set Send Socket Buffer Failed. \n");
        close(sockFd);
        return (-1);
    }

    /* 7. Binding Socket    */
    sockInAddr.sin_family = AF_INET;
    sockInAddr.sin_port = htons(nPort);
    sockInAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockFd, (struct sockaddr *) &sockInAddr, sizeof(sockInAddr))) {
        LOG(LOG_INF, "Err.  bind() Err. nRet = %d\n", nRet);
        close(sockFd);
        return (-1);
    }

    /* 8. Listen Socket		*/
    if (listen(sockFd, MAX_TCP_LISTEN) < 0) {
        LOG(LOG_INF, "Err.  listen() Err. nRet = %d\n", nRet);
        close(sockFd);
        return (-1);
    }

    return sockFd;
}

size_t smtpReadLine(int sockFd, char *pData, size_t szBuf) {
    char *wp = NULL;
    size_t nByte;
    size_t nByteRead = 0;
    memset(pData, 0x00, szBuf);
    wp = (char *) pData;

    while (nByteRead < szBuf - 1) {
        nByte = read(sockFd, wp, 1);
        nByteRead = wp - (char *) pData;
        //LOG(LOG_INF, "'%c'", *wp);
        if (nByte <= 0) {
            if (!nByte) break; /* EOF */

            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
                continue;
            } else {
                LOG (LOG_MAJ, "%s : Err. read error. errno=%d, sErr=%s\n", __func__, errno, strerror(errno));
                return (-1);
            }
        }

        if (*wp == '\n') {
            *(wp+1) = 0;
            break;
        }
        wp += nByte;
    }

    return wp - (char *) pData;
}

int __smtpSendData ( int sockFd , void *pData , size_t nLength ) {
    char *          wp          = NULL ;

    size_t          nLeft       = 0 ;
    ssize_t         nWritten    = 0 ;

    int             sendCount   = 0 ;
    int             retryCount  = 0 ;

    wp      = (char *) pData    ;
    nLeft   = nLength ;

    while ( nLeft > 0 )
    {
        nWritten = send ( sockFd , wp , nLeft , MSG_NOSIGNAL ) ;

        if ( nWritten <= 0 )
        {
            if ( errno == EWOULDBLOCK || errno == EAGAIN || errno == ENOBUFS || errno == EINTR ) {
                if ( retryCount++ > 10 ) {
                    LOG ( LOG_MAJ , "%s : Err. Retry Error. error=%d, sErr=%s\n" , __func__ , errno , strerror(errno) ) ;
                    return (-1) ;
                }
                continue ;
            }

            LOG ( LOG_MAJ , "%s : Err. send error. errno=%d, sErr=%s\n" , __func__ , errno , strerror(errno) ) ;
            return (-1) ;
        }

        nLeft     -= nWritten ;
        wp        += nWritten ;
        sendCount += nWritten ;
    }

    return sendCount ;
}

int smtpSendData ( int sockFd , void *pData , size_t nLength ) {
    LOG(LOG_DBG, "send -> %s", (char *)pData);
    return __smtpSendData(sockFd, pData, nLength);
}


int smtpGetPeerIP4Addr ( int sockFd , char * strIP4 )
{
    struct sockaddr     peer ;
    struct sockaddr_in* peer_in = (struct sockaddr_in *) &peer ;
    int                 peerlen = sizeof(peer) ;

    getpeername ( sockFd , &peer , (socklen_t *)&peerlen ) ;
    sprintf ( strIP4 , "%s" , inet_ntoa(peer_in->sin_addr) ) ;
    return 0 ;
}

int smtpGetPeerPortNum ( int sockFd )
{
    struct sockaddr     peer ;
    struct sockaddr_in* peer_in = (struct sockaddr_in *)&peer ;
    int                 peerlen = sizeof(peer) ;
    getpeername( sockFd, &peer, (socklen_t *)&peerlen );
    return (int)( (unsigned int) peer_in->sin_port ) ;
}

void smtpSetSessionId(SmtpSession_t * session) {
    sprintf(session->SessionId, "sid:%s:%d:%d", session->StrIP4, session->PortNum, session->SockFd);
}