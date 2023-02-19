//
// Created by srkim on 23. 2. 17.
//

#ifndef CAREER_TEST2_LOGGER_H
#define CAREER_TEST2_LOGGER_H

#endif //CAREER_TEST2_LOGGER_H

typedef enum {
    LOG_NONE = 0,
    LOG_CRT,
    LOG_MAJ,
    LOG_MIN,
    LOG_INF,
    LOG_DBG,
    LOG_TRC
} LOG_LEVEL;


#define MAX_LOG_SIZE 1024

void smtpLog (int log_lv, const char * fmttxt, ... );
#define LOG(l,f,a...) smtpLog(l,f,## a )