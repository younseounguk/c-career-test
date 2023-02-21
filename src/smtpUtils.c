//
// Created by srkim on 23. 2. 19.
//

#include "main.h"

int msleep(long msec)
{
    struct timespec ts;
    int res;

    int errno;
    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}