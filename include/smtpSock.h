#ifndef CAREER_TEST2_SMTPSOCK_H
#define CAREER_TEST2_SMTPSOCK_H

#define MAX_TCP_LISTEN 128
#define SENDING_RETURN_CODE 1

extern int smtpAcceptSock ( int server_fd );
extern int smtpServerOpen ( uint16_t n_port );
extern size_t smtpReadLine (int sock_fd , char * p_data , size_t sz_buf );
extern int smtpSendData (int sock_fd , void *p_data , size_t n_length );
extern int smtpGetPeerIP4Addr (int sock_fd , char * str_ipv4 );
extern int smtpGetPeerPortNum ( int sock_fd );
extern void smtpSetSessionId(smtp_session_t * session);
extern smtp_session_t * smtpHandleInboundConnection(int server_fd);
extern char * smtpMakeSessionId(char * session_id, char * ls_ipv4, int ln_port, int sock_fd);
#endif //CAREER_TEST2_SMTPSOCK_H
