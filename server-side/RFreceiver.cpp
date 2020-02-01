/* Программа msg1.c для иллюстрации работы с
очередями сообщений */
/* Эта программа получает доступ к очереди
сообщений, отправляет в нее 5 текстовых сообщений с типом 1 и одно
пустое сообщение с типом 255, которое будет служить для программы
msg2.c сигналом прекращения работы. */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include "RCSwitch.h"

#define RECEIVER_PIN 2

int main()
{
	int msqid; /* IPC дескриптор для очереди сообщений */

	char pathname[] = "/var/tmp"; /* Имя файла,
	использующееся для генерации ключа. Файл с таким
	именем должен существовать в текущей директории */

	key_t key; /* IPC ключ */
	int i,len; /* Счетчик цикла и длина
	информативной части сообщения */
	/* Ниже следует пользовательская структура для
	сообщения */

	int value;
	char lastId=255;

	RCSwitch mySwitch;

	struct mymsgbuf
	{
		long mtype;
		int mtext;
	} mybuf;


	/* Если отсутствует wiringPi, то выходим */
	if(wiringPiSetup() == -1)
		return 0;

	/* Генерируем IPC ключ из имени файла msg1.c в текущей
	директории и номера экземпляра очереди сообщений 0. */
	if ( (key = ftok(pathname,0)) < 0 ) {
		printf("Can\'t generate key\n");
		exit(1);
	}
	/* Пытаемся получить доступ по ключу к очереди сообщений,
	если она существует, или создать ее, с правами доступа
	read & write для всех пользователей */
	if ( (msqid = msgget(key, 0666 | IPC_CREAT)) < 0 ) {
		printf("Can\'t get msqid\n");
		exit(1);
	}

	mySwitch = RCSwitch();
	mySwitch.enableReceive(RECEIVER_PIN);  // Receiver on inerrupt 0 => that is pin #2

	while(1)
	{
		if ( mySwitch.available() )
		{
			/* Получаем данные */
			value = mySwitch.getReceivedValue();
			/* Если данные корректные, то запихиваем в очередь сообщений */
			if ( value )
			{
				if ( ( value >> 5 ) == 309136 )
				{
					printf("Пульт общается с нами :)\n");
					mySwitch.resetAvailable();
					continue;
				}
				if ( ( value & 0b11111111 ) == lastId ) // Фильтруем по id
				{
					mySwitch.resetAvailable();
					continue;
				}
				printf("Получено сообщение %d; id: %d\n", value, value & 0b11111111);
				mybuf.mtype = 2;
				mybuf.mtext = value;
				len = sizeof(mybuf.mtext) + 1;
				if ( msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0 ) {
					printf("Can\'t send message to queue\n");
					msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
				}
				mySwitch.resetAvailable();
				lastId = value & 0b11111111;
			}
		}
		delay(1); // Это чтобы не грузить проц на 100%
	}
	return 0;
}
