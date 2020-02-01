#include "ipclib.h"

IPC::IPC()
{
	char pathname[] = "/var/tmp";
	key_t key; /* IPC ключ */

	if ( (key = ftok(pathname,0)) < 0 )
	{
		printf("Can\'t generate key\n");
		exit(1);
	}

	if ( (msqid = msgget(key, 0666 | IPC_CREAT)) < 0 )
	{
		printf("Can\'t get msqid\n");
		exit(1);
	}
}

void IPC::send(uint8_t node_id, uint8_t command, uint8_t param, int16_t value, uint16_t scriptId)
{
	mybuf.mtype = 1;
	mybuf.mtext.node = node_id;
	mybuf.mtext.packet.command = command;
	mybuf.mtext.packet.data.param = param;
	mybuf.mtext.packet.data.value = value;
	mybuf.mtext.packet.data.scriptId = scriptId;
	_sendbuf();
	if ( scriptId ) // Если пакет принадлежит сценарию - дублируем для сервера
	{
		printf( "  ---> Answer for script id=%u received\n", (scriptId-2000) );
		sendByScript(scriptId);
	}
}

void IPC::sendByScript(uint16_t scriptId)
{
	mybuf.mtype = scriptId;
	mybuf.mtext.node = 0;
	mybuf.mtext.packet.command = 0;
	mybuf.mtext.packet.data.param = 0;
	mybuf.mtext.packet.data.value = 0;
	mybuf.mtext.packet.data.scriptId = 0;
	_sendbuf();
}

int IPC::getMsqid(void)
{
	return msqid;
}

void IPC::_sendbuf(void)
{
	int16_t len = sizeof(mybuf.mtext);

	if ( msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0 )	// Отправляем серверу
	{
		printf("Can\'t send message to queue\n");
		msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
	}
}

// ------------------- Получаем сообщения из очереди ( тип сообщения: type ) ---------------- //
int16_t IPC::recv(mymsgbuf * buf, long type)
{
	uint8_t maxlen = sizeof(mymsgbuf);
	return msgrcv(msqid, (struct msgbuf *) buf, maxlen, type, IPC_NOWAIT);
}

void IPC::sendStatus(uint8_t node, bool status)
{
	send(node, STATUS, 0, status, 0);
}