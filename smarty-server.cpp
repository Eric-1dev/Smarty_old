#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <mysql.h>
#include <time.h>
#include "smarty-server.h"

cSmarty::cSmarty()
{
	maxlen = sizeof(mymsgbuf);
}

cSmarty::~cSmarty()
{
	DBclose();
}

void cSmarty::IPCinit(void)
{
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
	{}
}

int cSmarty::DBconnect(void)
{
	uint8_t i;
	mysql_init(&conn);
	for( i=0; i<100; i++ )
	{
		if ( i )
			printf("Попытка № %u...\n", i+1);
		if ( mysql_real_connect(&conn, MYSQL_SERVER, MYSQL_DATABASE, MYSQL_PASSWORD, MYSQL_LOGIN, 3306, 0, 0) )
			return 1;
		delay(2000);
	}
	puts("Не могу соединиться с базой данных");
	return 0;
}

void cSmarty::DBclose(void)
{
	mysql_close(&conn);
}

void cSmarty::hello(void)
{
	MYSQL_ROW row;
	MYSQL_RES *result;
	uint8_t i;
	uint8_t num_nodes;
	int16_t value;

	delay(1000); // Ждем запуска драйверов
	DBconnect();
	puts("Запрашиваю список узлов");

	snprintf(query, sizeof(query), "SELECT `node_id` FROM `nodes` WHERE `node_id`");
	if ( DBquery() )
	{
		result = mysql_store_result(&conn);
		num_nodes = mysql_num_rows(result);
		printf("Узлов найдено: %d\n", num_nodes);
	}

	puts("Актуализирую статус системы");
	snprintf(query, sizeof(query), "SELECT `node_id`, `param` FROM `status` WHERE `update_command` > 0");
	if ( DBquery() )
	{
		result = mysql_store_result(&conn);
		while ( (row = mysql_fetch_row(result)) != NULL  )
		{
			if ( atoi(row[0]) )
			{
				getValue(&value, atoi(row[0]), atoi(row[1]), 2001, true);
				delay(30);
			}
		}
	}
}

long cSmarty::getMsgIPC(void)
{
	int len; /* Реальная длина и максимальная
	длина информативной части сообщения */

	/* В бесконечном цикле принимаем сообщения
	типа 1-1000 в порядке FIFO с максимальной длиной информативной
	части maxlen */
	len = msgrcv(msqid, (struct msgbuf *) &mybuf, maxlen, -1000, 0);
	if ( len < 0 ) {
		puts("Не могу получить сообщение из очереди\n");
		exit(1);
	}
	return mybuf.mtype;
}

uint8_t cSmarty::handler()
{
	MYSQL_ROW row;
	MYSQL_RES *result;
	uint8_t num_rows;

	switch ( mybuf.mtext.packet.command )
	{
		case STATUS:
			printf("\033[33m(info) Обновлен статус узла № = %d. Статус: %d\033[0m\n", mybuf.mtext.node, mybuf.mtext.packet.data.value);
			snprintf(query, sizeof(query), "UPDATE `nodes` SET `status`=%d WHERE `node_id`=%d", mybuf.mtext.packet.data.value, mybuf.mtext.node);
			if ( DBquery() )
				return 0;
			break;
		case NOTIFY:
			printf("\033[33m(info) Принято сообщение с типом NOTIFY от узла № = %d. Param: %d, value: %d\033[0m\n", mybuf.mtext.node, mybuf.mtext.packet.data.param, mybuf.mtext.packet.data.value);
			snprintf(query, sizeof(query), "UPDATE `status` SET `value`=%d, `last_update`=now() WHERE `node_id`=%d AND `param`=%d", mybuf.mtext.packet.data.value, mybuf.mtext.node, mybuf.mtext.packet.data.param);
			if ( DBquery() )
			{
				// Если сообщение получено в процессе выполнения сценария - выходим
				if ( mybuf.mtext.packet.data.scriptId )
					return 0;
				// Проверяем, есть ли скрипт для данного узла с данными параметрами
				snprintf(query, sizeof(query), "SELECT `id` FROM `scripts` WHERE (`node_id`=%u) AND (`param`=%u) AND (%d BETWEEN `value_min` AND `value_max`) AND (`active`=1)", mybuf.mtext.node, mybuf.mtext.packet.data.param, mybuf.mtext.packet.data.value);
				if ( DBquery() )
				{
					result = mysql_store_result(&conn);

					if ( mysql_num_rows(result) )	// Если есть скрипт...
					{
						uint8_t i=0;
						int16_t value;
						uint16_t scriptId;
						puts("\n\033[34m\033[1m<----------Найден скрипт для этой команды. Выполняю...---------->\033[0m");
						value = mybuf.mtext.packet.data.value;
						row = mysql_fetch_row(result); // ...запрашиваем последовательность действий в порядке, определённом полем order
						scriptId = atoi(row[0])+2000;
						printf("ID сценария %u\n", atoi(row[0]));
						puts("Очищаю очередь сообщений скрипта");
						while ( msgrcv(msqid, (struct msgbuf *) &mybuf, maxlen, scriptId, IPC_NOWAIT) > 0 )
						{}
						snprintf(query, sizeof(query), "SELECT `action_id`, `value1`, `value2` FROM `scripts_table` WHERE `script_id`=%u ORDER BY `order` ASC", atoi(row[0]));
						if ( DBquery() )
						{
							result = mysql_store_result(&conn);
							num_rows = mysql_num_rows(result);
							printf("Всего строк: %u\n", num_rows);
							while ( (row = mysql_fetch_row(result)) != NULL  )
							{
								printf("Строка №%u: ", ++i);
								if ( !script_handler(&value, atoi(row[0]), atoi(row[1]), atoi(row[2]), scriptId) )
									break;
							}
						}
						puts("\033[34m\033[1m<------------------------Скрипт завершен------------------------>\033[0m\n");
					}
					else
						return 0;
				}
			}
			break;
		case HELLO:
			// Сбрасываем done в 0 в заданиях, в которых stoptime не NULL
			snprintf(query, sizeof(query), "UPDATE jobs_table SET done=0 WHERE sched_id in (SELECT s.id FROM scheduler s JOIN commands c ON c.id= s.command_id WHERE c.node_id = %d ) AND stoptime IS NOT NULL", mybuf.mtext.node);
			if ( DBquery() )
				printf("\033[33m(info) Узел № %d снова в сети. Связанные с ним задания планировщика обновлены\033[0m\n", mybuf.mtext.node);
			break;
		default:
			break;
	}
}

// Отправляет предустановленную в SQL команду command_id со значением value
bool cSmarty::sendCommand(uint16_t command_id, int16_t value, uint16_t scriptId)
{
	MYSQL_ROW row;
	MYSQL_RES *result;
	uint8_t node, command, param;
	uint16_t i=0;
	int16_t len;

	len = sizeof(mybuf.mtext);

	snprintf(query, sizeof(query), "SELECT `node_id`, `param`, `get_set`, `value_min`, `value_max` FROM `commands` WHERE `id`=%d", command_id);
	if ( DBquery() )
	{
		result = mysql_store_result(&conn);
		if ( (row = mysql_fetch_row(result)) != NULL  )
		{
			if ( (value < (int16_t) atoi(row[3])) || (value > (int16_t) atoi(row[4])) )
			{
				puts("\033[31m!!!Значение вне допустимого диапазона!!!\033[0m");
				return false;
			}
			if ( atoi(row[2]) == 1 ) command = GET;
			else command = SET;
			node = atoi(row[0]);
			param = atoi(row[1]);
			snprintf(query, sizeof(query), "SELECT `protocol_id` FROM `nodes` WHERE `node_id`=%u", node);	// Запрашиваем протокол и прибавляем к нему 1000. Получим mtype
			if ( DBquery() )
			{
				result = mysql_store_result(&conn);
				if ( (row = mysql_fetch_row(result)) != NULL  )
					mybuf.mtype = atoi(row[0]) + 1000;
				else
				{
					puts("\033[31m!!!Протокол не определён!!!\033[0m");
					return false;
				}
			}
			mybuf.mtext.node = node;
			mybuf.mtext.packet.command = command;
			mybuf.mtext.packet.data.param = param;
			mybuf.mtext.packet.data.value = value;
			mybuf.mtext.packet.data.scriptId = scriptId;
			if ( msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0 )
			{
				printf("Can\'t send message to queue\n");
				msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
				return false;
			}

			for (i=0; i<500; i++)
			{
				len = msgrcv(msqid, (struct msgbuf *) &mybuf, maxlen, scriptId, IPC_NOWAIT);
				if ( len > 0 )
				{
					printf("  \033[32m...Ok. Сообщение доставлено через %u миллисекунд\033[0m\n", i);
					return true;
				}
				delay(1);
			}
			printf("\033[31m!!!Сообщение не доставлено. Таймаут %u миллисекунд!!!\033[0m\n", i);
			return false;
		}
		else
			puts("\033[31m!!!Команда не найдена!!!\033[0m");
	}
	return false;
}

// Запрашивает обновление параметра узла (обновляет таблицу status), затем забирает значение в value
bool cSmarty::getValue(int16_t *value, int16_t node_id, int16_t param, uint16_t scriptId, bool no_wait)
{
	MYSQL_ROW row;
	MYSQL_RES *result;
	uint16_t i;
	int16_t len;

	snprintf(query, sizeof(query), "SELECT `update_command` FROM `status` WHERE `node_id`=%d AND `param`=%d", node_id, param);
	if ( DBquery() )
	{
		result = mysql_store_result(&conn);
		if ( (row = mysql_fetch_row(result)) != NULL  )
		{
			sendCommand(atoi(row[0]), 0, scriptId);
			for (i=0; i<5000; i++)
			{
				len = msgrcv(msqid, (struct msgbuf *) &mybuf, maxlen, scriptId, IPC_NOWAIT);
				if ( len > 0 )
				{
					// Получили ответ от узла
					*value = mybuf.mtext.packet.data.value;
					printf("  \033[32m...Ok. Ответ получен через %u миллисекунд\033[0m\n", i);
					printf("           ---> Значение параметра равно %d\n", *value);
					return true;
				}
				if ( no_wait )
					return true;
				delay(1);
			}
			puts("\033[31m!!!Ответ не получен. Таймаут 5 секунд!!!\033[0m");
			return false;
		}
		else
			puts("\033[31m!!!Параметр узла не найден в базе!!!\033[0m");
	}
	return false;
}

// Задержка в миллисекундах
void cSmarty::delay(unsigned int millis)
{
	struct timespec sleeper;

	sleeper.tv_sec  = (time_t)(millis / 1000);
	sleeper.tv_nsec = (long)(millis % 1000) * 1000000;
	nanosleep(&sleeper, NULL);
}

// Обработчик скриптов. Здесь описываются предустановленные в SQL действия
// Если функция возвращает true, то скрипт продолжает выполнение, иначе - вылетает
bool cSmarty::script_handler(int16_t *value, uint8_t action_id, int16_t value1, int16_t value2, uint16_t scriptId)
{
	switch (action_id)
	{
		case 1:
			printf("---> Отправляю команду %u, со значением %d\n", value1, value2);
			return sendCommand(value1, value2, scriptId);
		case 2:
			printf("---> Запрашиваю значение показателя %u узла %u\n", value2, value1);
			return getValue(value, value1, value2, scriptId, false);
		case 3:
			printf("---> Значение, полученное от узла равно %d\n", value1);
			return *value == value1;
		case 4:
			printf("---> Значение, полученное от узла больше или равно %d\n", value1);
			return *value >= value1;
		case 5:
			printf("---> Значение, полученное от узла меньше или равно %d\n", value1);
			return *value <= value1;
		case 6:
			printf("---> Задержка в %u секунд\n", value1);
			delay(value1*1000);
			return true;
		default:
			puts("\033[31m!!!Неизвестное действие!!!\033[0m");
			return false;
	}
}

uint8_t cSmarty::scheduler()
{
	MYSQL_ROW row;
	MYSQL_RES *result;
	uint8_t i;
	char done_str[9];

//	snprintf(query, sizeof(query), "UPDATE jobs_table SET done=0 WHERE (UNIX_TIMESTAMP(DATE(done)) != UNIX_TIMESTAMP(CURDATE())) AND done != 0 AND (stoptime IS NOT NULL)");
//	DBquery();

	for ( i=0; i<3; i++ )
	{
		switch (i)
		{
			case 0:
				// Разовое задание
				snprintf(query, sizeof(query), "SELECT jt.id, jt.sched_id, s.command_id, jt.startvalue FROM jobs_table jt JOIN scheduler s ON s.id = jt.sched_id WHERE (s.active = 1) \
												AND (((1<<WEEKDAY(NOW())) & jt.days) > 0) \
												AND (UNIX_TIMESTAMP(NOW()) >= (UNIX_TIMESTAMP(CURDATE()) + jt.starttime*60)) \
												AND (UNIX_TIMESTAMP(jt.done) < (UNIX_TIMESTAMP(CURDATE()) + jt.starttime*60)) \
												AND (jt.stoptime IS NULL )");
				snprintf(done_str, sizeof(done_str), "<single>");
				break;
			case 1:
				// Циклическое задание //стоп
				snprintf(query, sizeof(query), "SELECT jt.id, jt.sched_id, s.command_id, jt.stopvalue FROM jobs_table jt JOIN scheduler s ON s.id = jt.sched_id WHERE (s.active = 1) \
												AND (((1<<WEEKDAY(NOW())) & jt.days) > 0) \
												AND (((UNIX_TIMESTAMP(NOW()) >= (UNIX_TIMESTAMP(CURDATE()) + jt.stoptime*60)) AND (UNIX_TIMESTAMP(jt.done) < (UNIX_TIMESTAMP(CURDATE()) + jt.stoptime*60))) OR \
                                                ((UNIX_TIMESTAMP(NOW()) < (UNIX_TIMESTAMP(CURDATE()) + jt.starttime*60)) AND (UNIX_TIMESTAMP(jt.done) < UNIX_TIMESTAMP(CURDATE())))) \
												AND (jt.stoptime IS NOT NULL ) ORDER BY jt.stoptime DESC LIMIT 1");
				snprintf(done_str, sizeof(done_str), "<stop>");
				break;
			case 2:
				// Циклическое задание //старт
				snprintf(query, sizeof(query), "SELECT jt.id, jt.sched_id, s.command_id, jt.startvalue FROM jobs_table jt JOIN scheduler s ON s.id = jt.sched_id WHERE (s.active = 1) \
												AND (((1<<WEEKDAY(NOW())) & jt.days) > 0) \
												AND (UNIX_TIMESTAMP(NOW()) >= (UNIX_TIMESTAMP(CURDATE()) + jt.starttime*60)) \
												AND (UNIX_TIMESTAMP(NOW()) < (UNIX_TIMESTAMP(CURDATE()) + jt.stoptime*60)) \
												AND (UNIX_TIMESTAMP(jt.done) < (UNIX_TIMESTAMP(CURDATE()) + jt.starttime*60)) \
												AND (jt.stoptime IS NOT NULL) ORDER BY jt.starttime DESC LIMIT 1");
				snprintf(done_str, sizeof(done_str), "<start>");
				break;
		}
		if ( DBquery() )
		{
			result = mysql_store_result(&conn);
			while ( (row = mysql_fetch_row(result)) != NULL  )
			{
				if ( sendCommand(atoi(row[2]), atoi(row[3]), 2001) )
				{
					printf("\033[32mВыполнено задание планировщика № %d. ID строки - %d \033[0m", atoi(row[1]), atoi(row[0]));
					puts(done_str);
					snprintf(query, sizeof(query), "UPDATE jobs_table SET done=NOW() WHERE id=%d", atoi(row[0]));
					DBquery();
				}
			}
		}
	}
	return true;
}

bool cSmarty::DBquery(void)
{
	if ( mysql_query(&conn, query) )
	{
		puts( mysql_error(&conn));
		return false;
	}
	return true;
}
