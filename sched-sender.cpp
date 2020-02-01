#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "smarty_types.h"

using namespace std;

int main (void)
{
	mymsgbuf buf;

	char pathname[] = "/var/tmp";
	int msqid, len; /* IPC дескриптор для очереди сообщений */
	key_t key; /* IPC ключ */

	if ( (key = ftok(pathname,0)) < 0 ) {
		printf("Can\'t generate key\n");
		exit(1);
	}

	if ( (msqid = msgget(key, 0666 | IPC_CREAT)) < 0 ) {
		printf("Can\'t get msqid\n");
		exit(1);
	}

	buf.mtype = 1000;
	buf.mtext.node = 0;
//	buf.mtext.time = time(NULL)+14400;
	len = sizeof(buf.mtext);

	if ( msgsnd(msqid, (struct msgbuf *) &buf, len, 0) < 0 )
	{
		printf("Can\'t send message to queue\n");
		msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
		exit(1);
	}
	exit(0);
}
