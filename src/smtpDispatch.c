//
// Created by srkim on 23. 2. 21.
//

#include "main.h"

int sendGreetingMessage(SmtpSession_t  * session) {
    const char * message = "220 tmse.daou.co.kr ESMTP TERRACE MAIL Security EE 1.2.2 2022-5\r\n";
    smtpSendData(session->SockFd, (void *)message, strlen(message));
    return 0;
}

int sendQuitMessage(SmtpSession_t  * session) {
    const char * message = "221 Service closing transmission change\r\n";
    smtpSendData(session->SockFd, (void *)message, strlen(message));
    return 0;
}

smtpMessageType_t detectSmtpCommand(char * line) {
    char buf[8] = {0,};
    memcpy(buf, line, 4);
    if (!strcasecmp(buf, "HELO") || !strcasecmp(buf, "EHLO")) {
        return SMTP_HELO;
    }
    else if (!strcasecmp(buf, "MAIL")) {
        return SMTP_MAIL;
    }
    else if (!strcasecmp(buf, "RCPT")) {
        return SMTP_RCPT;
    }
    else if (!strcasecmp(buf, "DATA")) {
        return SMTP_DATA;
    }
    else if (!strcasecmp(buf, "QUIT")) {
        return SMTP_QUIT;
    }
    else {
        return SMTP_UNKNOWN;
    }
}

int onSmtpHelo(SmtpSession_t  * session, char * line) {
    const char * message = "250 Requested mail action okay\r\n";
    smtpSendData(session->SockFd, (void *)message, strlen(message));
    return 0;
}

int onSmtpData(SmtpSession_t  * session, char * line) {
    const char * message = "354 Start mail input\r\n";
    smtpSendData(session->SockFd, (void *)message, strlen(message));
    return 0;
}

int onSmtpDataContinue(SmtpSession_t  * session, char * line) {
    const char * message = "250 Message accept for delivery\r\n";
    if (!strcmp(line, ".\r\n")) {
        session->smtpStatus = SMTP_STATUS_READY;
        if (saveMailAsFile(session) != 0) {
            message = "500 Internal error\r\n";
        }
        smtpSendData(session->SockFd, (void *) message, strlen(message));
        return 0;
    }
    strcat(session->smtpData, line);
    return 0;
}

int onSmtpMailFrom(SmtpSession_t  * session, char * line) {
    int s_idx, e_idx;
    char message[512] = {0,};
    for (int i=0;line[i];i++) {
        char c = line[i];
        if (c == '<') s_idx = i;
        if (c == '>') e_idx = i;
    }
    memcpy(session->smtpMailFrom, line+s_idx, e_idx-s_idx);
    sprintf(message, "250 Sender <%s> OK\r\n", (const char *)session->smtpMailFrom);
    smtpSendData(session->SockFd, (void *)message, strlen(message));
    return 0;
}

int onSmtpRcptTo(SmtpSession_t  * session, char * line) {
    int s_idx, e_idx;
    char message[512] = {0,};
    for (int i=0;line[i];i++) {
        char c = line[i];
        if (c == '<') s_idx = i+1;
        if (c == '>') e_idx = i;
    }
    memcpy(session->smtpRcptTo, line+s_idx, e_idx-s_idx);
    sprintf(message, "250 Recipient <%s> OK\r\n", session->smtpRcptTo);
    smtpSendData(session->SockFd, (void *)message, strlen(message));
    return 0;
}


int doSmtpDispatch(SmtpSession_t  * session, char * line) {
    int nErr = -1;
    LOG(LOG_DBG, "recv <-- %s", line);

    if (session->smtpStatus == SMTP_STATUS_READY) {
        session->smtpLastMessage = detectSmtpCommand(line);
        switch (session->smtpLastMessage) {
            case SMTP_HELO:
                return onSmtpHelo(session, line);
            case SMTP_MAIL:
                return onSmtpMailFrom(session, line);
            case SMTP_RCPT:
                return onSmtpRcptTo(session, line);
            case SMTP_QUIT:
                return sendQuitMessage(session);
            case SMTP_DATA:
                session->smtpStatus = SMTP_STATUS_READ_DATA;
                return onSmtpData(session, line);
            default:
                break;
        }
    }
    else if (session->smtpStatus == SMTP_STATUS_READ_DATA) {
        return onSmtpDataContinue(session, line);
    }

    return nErr;
}