//
// Created by srkim on 23. 2. 19.
//

#ifndef CAREER_TEST2_SMTPSOCK_H
#define CAREER_TEST2_SMTPSOCK_H

extern int smtpAcceptSock ( int serverFd );
extern int smtpServerOpen ( uint16_t nPort );
extern size_t smtpReadLine (int sockFd , void * pData , size_t nLength );
extern int smtpGetPeerIP4Addr ( int sockFd , char * strIP4 );
extern int smtpGetPeerPortNum ( int sockFd );
#endif //CAREER_TEST2_SMTPSOCK_H
