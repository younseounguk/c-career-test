//
// Created by srkim on 23. 2. 19.
//

#ifndef CAREER_TEST2_SMTPSOCK_H
#define CAREER_TEST2_SMTPSOCK_H

#define MAX_TCP_LISTEN 128

extern int smtpAcceptSock ( int serverFd );
extern int smtpServerOpen ( uint16_t nPort );
extern size_t smtpReadLine (int sockFd , char * pData , size_t nLength );
extern int smtpSendData ( int sockFd , void *pData , size_t nLength );
extern int smtpGetPeerIP4Addr ( int sockFd , char * strIP4 );
extern int smtpGetPeerPortNum ( int sockFd );
extern void smtpSetSessionId(SmtpSession_t * session);
extern SmtpSession_t * smtpHandleInboundConnection(int serverFd);
extern char * smtpMakeSessionId(char * sessionId, char * strIP4, int portNum, int sockFd);
#endif //CAREER_TEST2_SMTPSOCK_H
