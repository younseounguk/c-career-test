//
// Created by srkim on 23. 2. 17.
//

#ifndef CAREER_TEST2_SMTPSVR_H
#define CAREER_TEST2_SMTPSVR_H
#define MAX_TCP_LISTEN 40
#define SMTP_PORT 2500

extern int smtpServerOpen ( uint16_t nPort );
extern void * smtpWait ();
#endif //CAREER_TEST2_SMTPSVR_H
