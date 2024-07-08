#ifndef CAREER_TEST3_SMTPTPS_H
#define CAREER_TEST3_SMTPTPS_H

extern int g_tcp_connect;
extern int g_tcp_close;
extern int g_smtp_message_send;
extern int g_smtp_message_recv;

void smtpServerTrafficMonitor(void *args);
int smtpStartServerTrafficMonitor();

#endif //CAREER_TEST3_SMTPTPS_H

