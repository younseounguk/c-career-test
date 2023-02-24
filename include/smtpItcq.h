//
// Created by srkim on 23. 2. 24.
//

#ifndef CAREER_TEST3_SMTPITCQ_H
#define CAREER_TEST3_SMTPITCQ_H
extern SmtpSession_t * itcqGetSession();
extern int itcqPutSession(SmtpSession_t * session);

typedef struct itcqNodes {
    SmtpSession_t * session;
    struct itcqNodes* next;
} itcqNodes_t;
#endif //CAREER_TEST3_SMTPITCQ_H
