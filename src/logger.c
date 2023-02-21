//
// Created by srkim on 23. 2. 17.
//

#include "main.h"

static char* getLocalTime(char * buf,size_t sz_buf)
{
    struct tm t;
    time_t timer = time(NULL);
    localtime_r(&timer, &t);
    snprintf(buf, sz_buf, "[%d-%02d-%02d %02d:%02d:%02d]", t.tm_year + 1900, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);

    return buf;
}
int gLogLevel = LOG_INF;
void setLogLevel(int log_lv) {
    if (log_lv >= LOG_NONE && log_lv <= LOG_TRC) {
        gLogLevel = log_lv;
        return;
    }
}

char * getFileName(const char * fullPath) {
    int wpIdx = 0;
    for (int i=0;fullPath[i];i++) {
        if (fullPath[i] == '/') {
            wpIdx = i + 1;
        }
    }
    return (char *)fullPath + wpIdx;
}

void smtpLog (int log_lv, int line, const char * file_name, const char * fmttxt, ... )
{
    if (log_lv > gLogLevel) {
        return;
    }
    va_list ap;
    char buf[MAX_LOG_SIZE] = {0,};
    char localTime[64];
    va_start(ap, fmttxt);
    vsprintf(buf, fmttxt, ap);
    va_end(ap);
    for (int i =0;buf[i];i++) {
        char c = buf[i];
        if (c == '\r') {
            buf[i] = ' ';
        }
    }
    if (buf[strlen(buf)-1] != '\n') {
        strcat(buf, "\n");
    }

    printf("%s[%s:%d]: %s", getLocalTime(localTime, sizeof(localTime)), getFileName(file_name), line, buf);
}
