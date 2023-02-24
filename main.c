#include "main.h"

#define USE_EPOLL_WAIT 1
#define SMTP_PORT 2500
int gSysClose;

int main() {
    int serverFd = -1;

    /* 1. Initialize Processing */
    if ( smtpInitProcess () < 0 ) return (-1) ;

    /* 2. Initialize Server socket */
    if ((serverFd = smtpServerOpen(SMTP_PORT)) <= 0) {
        LOG(LOG_CRT, "Error: Server socket initialization failed");
    }

    /* 3. Start smtp service based on socket type */
#if USE_EPOLL_WAIT
    // 3.1 : Asynchronous implementation using epoll
#define MAX_ASYNC_WORKS_TH 4
    smtpStartWorkThreads(MAX_ASYNC_WORKS_TH);
    smtpWaitAsync(serverFd);
#else
    // 3.2 : Synchronous implementation using select
    smtpWaitSync(serverFd);
#endif
    return 0;
}
