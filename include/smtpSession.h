#ifndef CAREER_TEST3_SMTPSESSION_H
#define CAREER_TEST3_SMTPSESSION_H

#include "list.h"

#define SESSION_HASH_SIZE	512
#define SESSION_ID_LENGTH 	32


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

    /* list */
    list_t		chain;
} smtp_session_t;

extern void delSmtpSession(char * session_id);
extern smtp_session_t * addSmtpSession(smtp_session_t * session);
smtp_session_t *getSmtpSession(char *session_id);
smtp_session_t *getFirstSmtpSession(void);

#endif //CAREER_TEST3_SMTPSESSION_H

