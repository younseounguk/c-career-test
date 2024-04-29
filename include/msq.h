#ifndef _MSQ_H
#define _MSQ_H
#include <sys/msg.h>

#define SMTP_MSQID 	0x1234

extern int g_msqid;

int openMsq(int key);

#endif
