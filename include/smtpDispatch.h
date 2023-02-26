//
// Created by srkim on 23. 2. 21.
//

#ifndef CAREER_TEST3_SMTPDISPATCH_H
#define CAREER_TEST3_SMTPDISPATCH_H




extern int sendGreetingMessage(smtp_session_t  * session);
extern int doSmtpDispatch(smtp_session_t  * session, char * line);

#endif //CAREER_TEST3_SMTPDISPATCH_H
