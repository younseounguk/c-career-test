//
// Created by srkim on 23. 2. 20.
//

#ifndef CAREER_TEST3_SMTPSESSION_H
#define CAREER_TEST3_SMTPSESSION_H



typedef struct SmtpSession {
    /* TCP Information */
    int             SockFd  ;
    int             PortNum ;
    char            StrIP4   [ L_IP4_ADDR ];
    char            SessionId[ L_SESSION_ID ];

    /* SMTP Information */
    smtpStatus_t        smtpStatus;
    smtpMessageType_t   smtpLastMessage;
    char                smtpData[ MAX_DATA ];
    char                smtpRcptTo[ MAX_RCPT_TO ];
    char                smtpMailFrom[ MAX_MAIL_FROM ];
} SmtpSession_t;

extern void delSmtpSession(char * sessionId);
extern SmtpSession_t * getSmtpSession(char * sessionId);
extern SmtpSession_t * addSmtpSession(SmtpSession_t * session);

#endif //CAREER_TEST3_SMTPSESSION_H

