#include "main.h"


int g_tcp_connect = 0;
int g_tcp_close = 0;
int g_smtp_message_send = 0;
int g_smtp_message_recv = 0;

void smtpServerTrafficMonitor(void *args)
{
    int last_tcp_connect = 0;
    int last_tcp_close = 0;
    int last_smtp_message_send = 0;
    int last_smtp_message_recv = 0;

    while (!g_sys_close) {
        LOG(LOG_MIN,    "SMTP Server Traffic: "
                        "%sn_tcp_connect%s=%s%d%s(+%s%d%s), "
                        "%sn_tcp_close%s=%s%d%s(+%s%d%s), "
                        "%sn_smtp_message_send%s=%s%d%s(+%s%d%s), "
                        "%sn_smtp_message_recv%s=%s%d%s(+%s%d%s)",
                        C_YLLW, C_NRML, C_GREN, g_tcp_connect, C_NRML, C_RED, g_tcp_connect - last_tcp_connect, C_NRML, 
                        C_YLLW, C_NRML, C_GREN, g_tcp_close, C_NRML, C_RED, g_tcp_close - last_tcp_close, C_NRML, 
                        C_YLLW, C_NRML, C_GREN, g_smtp_message_send, C_NRML, C_RED, g_smtp_message_send - last_smtp_message_send, C_NRML, 
                        C_YLLW, C_NRML, C_GREN, g_smtp_message_recv, C_NRML, C_RED, g_smtp_message_recv - last_smtp_message_recv, C_NRML);

        last_tcp_connect = g_tcp_connect;
        last_tcp_close = g_tcp_close;
        last_smtp_message_send = g_smtp_message_send;
        last_smtp_message_recv = g_smtp_message_recv;

        msleep(1000);
    }
}

int smtpStartServerTrafficMonitor() {
    int nErr;
    pthread_t clientTh;
    pthread_attr_t clientThAttr;
    pthread_attr_init(&clientThAttr);

    if ((nErr = pthread_create(&clientTh, &clientThAttr, smtpServerTrafficMonitor, NULL)) < 0) {
            LOG (LOG_MAJ, "Err. SMTP Server Traffic monitor Thread Create Failed. Err.= '%s'\n", strerror(nErr));
            return -1;
    }

    return 0;
}
