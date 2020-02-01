#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "smarty_types.h"

int main ()
{
	mymsgbuf mybuf;
	int msqid; /* IPC дескриптор для очереди сообщений */
	int maxlen;
	char pathname[] = "/var/tmp"; /* Имя файла,
	использующееся для генерации ключа. Файл с таким
	именем должен существовать в текущей директории */
	key_t key; /* IPC ключ */

	/* Генерируем IPC ключ из имени файла msg1.c в текущей
	директории и номера экземпляра очереди сообщений 0 */
	if ( (key = ftok(pathname,0)) < 0 )
	{
		puts("Can\'t generate key");
		exit(1);
	}

	/* Пытаемся получить доступ по ключу к очереди сообщений,
	если она существует, или создать ее, с правами доступа
	read & write для всех пользователей */
	if ( (msqid = msgget(key, 0666 | IPC_CREAT)) < 0 )
	{
		puts("Can\'t get msqid");
		exit(1);
	}

	// Очищаем очередь сообщений
	while( (msgrcv(msqid, (struct msgbuf *) &mybuf, maxlen, 0, IPC_NOWAIT)) > 0 )
	{
		printf("mtype: %d, ", mybuf.mtype);
		printf("Node: %d, ", mybuf.mtext.node);
		printf("Command: %d, ", mybuf.mtext.packet.command);
		printf("Param: %d, ", mybuf.mtext.packet.data.param);
		printf("Value: %d, ", mybuf.mtext.packet.data.value);
		printf("Script: %d\n", mybuf.mtext.packet.data.scriptId);
	}
}