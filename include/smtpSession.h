#ifndef CAREER_TEST3_SMTPSESSION_H
#define CAREER_TEST3_SMTPSESSION_H

#define MAX_SMTP_SESSION	2048

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

	/* Node Information */
	struct smtp_session* next;
	uint8_t is_used;
} smtp_session_t;

extern void delSmtpSession(char * session_id);
extern smtp_session_t * addSmtpSession(smtp_session_t * session);

typedef struct smtp_session_list {
	int count; //smtp session count
	pthread_mutex_t mutex; //for protect critical section
	smtp_session_t* head; //list start point
} smtp_session_list_t;

extern int initSmtpSessionList();
extern smtp_session_t* getSmtpSession();

#endif //CAREER_TEST3_SMTPSESSION_H

