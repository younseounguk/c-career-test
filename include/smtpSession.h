#ifndef CAREER_TEST3_SMTPSESSION_H
#define CAREER_TEST3_SMTPSESSION_H



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

/* added by se00248 */
#define MAX_POOL_NUM 10000

typedef struct smtp_session_pool {
	int occupied;
	int used;
    	char sess_id[ L_SESSION_ID ];
	smtp_session_t *session;
} smtp_session_pool;

extern void initSessionPool();
extern int getSmtpSessionIdxForPool();
extern void unusedSmtpSession(int pool_num);
//extern smtp_session_t *getSmtpSessionForPool();
//extern void unusedSmtpSession(char *session_id);
extern smtp_session_pool sessions[MAX_POOL_NUM];
/* end added by se00248 */

extern void delSmtpSession(char * session_id);
extern smtp_session_t * addSmtpSession(smtp_session_t * session);
#endif //CAREER_TEST3_SMTPSESSION_H

