#ifndef CAREER_TEST3_SMTPSESSION_H
#define CAREER_TEST3_SMTPSESSION_H

#define SESSION_POOL_SIZE 2048

typedef struct smtp_session {
    /* TCP Information */
    int             sock_fd  ;
    int             port_num ;
    char            str_ipv4   [ L_IP4_ADDR ];
    char            session_id[ L_SESSION_ID ];

    /* SMTP Information */
    smtp_status_t        smtp_status;
    smtp_message_type_t   smtp_last_message;
    char                smtp_data[MAX_DATA ];
    char                smtp_rcpt_to[ MAX_RCPT_TO ];
    char                smtp_mail_from[ MAX_MAIL_FROM ];
} smtp_session_t;

#if 0
smtp_session_t session_pool[1024] ;
int session_count = 0;
#endif

extern void delSmtpSession(char * session_id);
extern smtp_session_t * addSmtpSession(smtp_session_t * session);

#endif //CAREER_TEST3_SMTPSESSION_H

