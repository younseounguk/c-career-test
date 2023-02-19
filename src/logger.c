//
// Created by srkim on 23. 2. 17.
//

#include <stdint-gcc.h>
#include <stdio.h>
#include <stdarg.h>
#include <bits/types/time_t.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "../include/logger.h"

static char* getLocalTime(char * buf,size_t sz_buf)
{
    struct tm t;
    time_t timer = time(NULL);
    localtime_r(&timer, &t);
    snprintf(buf, sz_buf, "[%d-%02d-%02d %02d:%02d:%02d]", t.tm_year + 1900, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);

    return buf;
}

void smtpLog (int log_lv, const char * fmttxt, ... )
{
    va_list ap;

    char buf[MAX_LOG_SIZE] = {0,};
    char localTime[64];
    va_start(ap, fmttxt);
    vsprintf(buf, fmttxt, ap);
    if (buf[strlen(buf)-1] != '\n') {
        strcat(buf, "\n");
    }
    va_end(ap);
    printf("%s: %s", getLocalTime(localTime, sizeof(localTime)), buf);
}
