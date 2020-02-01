#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "smarty-server.h"

cSmarty smarty;

int main ()
{
	pid_t pid;
	long msgType;

	puts("\033[33m\033[1m<------------- Инициализирую сервер ------------->\033[0m");
	puts("Инициализирую очередь сообщений");
	smarty.IPCinit();
	puts("Ок");

	puts("Подключаюсь к базе данных");
	smarty.hello();
	puts("\033[32m\033[1m<------------------- Работаю -------------------->\033[0m");

	signal(SIGCLD,SIG_IGN);

	while(1)
	{
		msgType = smarty.getMsgIPC();
		pid = fork();
		if ( pid == -1 )
		{
			puts("Не удалось запустить дочерний процесс");
			exit(1);
		}
		else if ( !pid )
		{
			smarty.DBconnect();

			if ( msgType == 1000 )
				smarty.scheduler();
			else
				smarty.handler();
			exit(0);
		}
	}

	return 0;
}