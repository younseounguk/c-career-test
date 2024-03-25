#include "main.h"


int openMsq(int key)
{
	int msqid = -1;

	if (0 > (msqid = msgget(key, IPC_CREAT | 0666))) {
		return -1;
	}
	return msqid;
}

