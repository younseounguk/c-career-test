#include "main.h"

#define SMTP_PORT 2500
int g_sys_close;

/*
 * [어플리케이션 설명]
 * 다음 코드는 간단한 SMTP 서버의 구현입니다.
 * TCP Port 2500 으로 클라이언트의 요청을 받고 있으며 accept, date received, close등의 이벤트 감지 및 적절한 처리를 하고 있습니다.
 * 이벤트 감지 방법에는 다음 메커니즘이 제공 됩니다
 *  - 비동기식, N개의 Work thread (기본 설정)
 */


int main() {
    int server_fd;
    g_sys_close = 0;

    /* 1. Initialize log */
    setLogLevel(LOG_TRC);

    /* 2. Initialize Server socket */
    if ((server_fd = smtpServerOpen(SMTP_PORT)) <= 0) {
        LOG(LOG_CRT, "Error: Server socket initialization failed");
    }

    /* 3. Initialize Session hashmap */
    initSessionHashmap();


    if (smtpStartServerTrafficMonitor() < 0) {
        LOG(LOG_MAJ, "Err, Start SMTP server traffic monitor failed.");
        return -1;
    }

    /* 4. Start smtp service based on socket type */
    if (smtpStartWorkThreads(MAX_ASYNC_WORKS_TH) < 0) {
        LOG(LOG_MAJ, "Err, Start work thread failed.");
        return -1;
    }

    smtpWaitAsync(server_fd);

    cleanSessionHashmap();
    return 0;
}
