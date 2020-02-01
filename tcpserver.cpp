#include "tcpserver.h"

IPC ipc;
int listener;
byte cipher[128];
byte key[16] = { 0x2B, 0x7E, 0x15, 0x17, 0x28, 0xF3, 0xD2, 0xA6, 0x1B, 0xF7, 0x12, 0x88, 0x09, 0xCF, 0x4F, 0x22 };
AES aes;
mes_t message;
mymap clients;
struct sockaddr_in peer;
int addr_size = sizeof(sockaddr_in);
int mes_size = sizeof(mes_t);

void begin(unsigned int port)
{
	struct sockaddr_in server;

	//Create socket
	listener = socket(AF_INET , SOCK_STREAM , 0);
	int reuse = 1;
	setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	
	if (listener == -1)
	{
		perror("socket");
		exit(1);
	}
	fcntl(listener, F_SETFL, O_NONBLOCK);

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( port );

	//Bind
	if( bind(listener, (struct sockaddr *)&server , addr_size) < 0)
	{
		perror("bind");
		exit(2);
	}

	listen(listener, 3);
	clients.clear();		// Очищаем список клиентов
	pthread_t thread;
	pthread_create(&thread, NULL, thread_func, NULL);
}

void handleClient(void) {
	// Заполняем множество сокетов
	fd_set readset;
	FD_ZERO(&readset);
	FD_SET(listener, &readset);			// Слушающий сокет

	for(mymap::iterator it = clients.begin(); it != clients.end(); it++)
		FD_SET(it->first, &readset);	// Клиентские сокеты

	int mx = max(listener, (*max_element( clients.begin(), clients.end() )).first);
	if(select(mx+1, &readset, NULL, NULL, NULL) <= 0)
	{
		perror("select");
		exit(3);
	}

	// Определяем тип события и выполняем соответствующие действия
	if(FD_ISSET(listener, &readset))
	{
		// Поступил новый запрос на соединение, используем accept
		int sock_desc = accept(listener, (struct sockaddr*)&peer, (socklen_t*)&addr_size);
		if( sock_desc < 0 )
		{
			perror("accept");
			exit(3);
		}
		printf("Соединение установлено. IP: %s\n", inet_ntoa(peer.sin_addr));
		
		enable_keepalive(sock_desc);
		fcntl(sock_desc, F_SETFL, O_NONBLOCK);

		clients.insert(std::pair<int, uint8_t>(sock_desc, 0));
	}

	// Пробегаем по клиентам, узнавая не поступило ли сообщение от кого-то из них
	for(mymap::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if(FD_ISSET(it->first, &readset))
		{
			// Поступили данные от клиента, читаем их
			if ( recv(it->first, cipher, sizeof(cipher), 0) <= 0 )
			{
				// Узел ушёл в отвал
				uint8_t node_id = it->second;
				puts("Соединение разорвано");
				printf("Узел %d отключился\n", it->second);
				close(it->first);			// Рвем соединение
				clients.erase(it->first);	// Удаляем сокет из множества
				ipc.sendStatus(node_id, false);
				continue;
			}
			mes_dec();
			msgHandler(message, it->first);
		}
	}
}

void msgHandler(mes_t message, int sock_desc) {
	if (message.command == INIT)
	{
		int sock_old = getSockByNode(message.value);
		if ( sock_old != -1 )
		{
			clients.erase(sock_old);
			printf("Узел %d переподключился\n", message.value);
		}
		clients[sock_desc] = message.value;
		printf("Узел %d передает привет\n", message.value);
		ipc.sendStatus(message.value, true);	// Обновляем статус
		//ipc.send(message.value, HELLO, 0, 0, 0);
		return;
	}
	ipc.send(clients[sock_desc], message.command, message.param, message.value, message.scriptId);
	printf("Получено сообщение: type=%d, param=%d, value=%d, scriptId=%d\n", message.command, message.param, message.value, message.scriptId);
}

void enable_keepalive(int sock) {
    int yes = 1;
    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int));

    int idle = 8;
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int));

    int interval = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int));

    int maxpkt = 2;
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int));
}

// Задержка в миллисекундах
void delay(unsigned int millis)
{
	struct timespec sleeper;

	sleeper.tv_sec  = (time_t)(millis / 1000);
	sleeper.tv_nsec = (long)(millis % 1000) * 1000000;
	nanosleep(&sleeper, NULL);
}

int getSockByNode(uint8_t node_id) {
	for(mymap::iterator it = clients.begin(); it != clients.end(); it++)
		if ( it->second == node_id )
			return it->first;
	return -1;
}

int getNodeBySock(int sock_desc) {
	mymap::iterator it = clients.find(sock_desc);
	if ( it != clients.end() )
		return it->second;
	return -1;
}

// Перегружаем операторы для корректного сравнения коллекции
bool operator <(mymap::value_type a, mymap::value_type b)
{
    return a.first < b.first;
}

bool operator ==(mymap::value_type a, mymap::value_type b)
{
    return a.first == b.first;
}

void * thread_func(void * args) {
	IPC ipc;
	mymsgbuf mybuf;
	mes_t mes;
	while(1)
	{
		if ( ipc.recv(&mybuf, 1002) > 0 )
		{
			// Отправляем данные клиенту
			mes.command = mybuf.mtext.packet.command;
			mes.param = mybuf.mtext.packet.data.param;
			mes.value = mybuf.mtext.packet.data.value;
			mes.scriptId = mybuf.mtext.packet.data.scriptId;
			mes_enc(&mes);
			if ( send(getSockByNode(mybuf.mtext.node), cipher, sizeof(cipher), 0) != -1 )
			{
				printf("Сообщение отправлено узлу %d, com = %d, param = %d, value = %d, scrId = %d\n", mybuf.mtext.node, mes.command, mes.param, mes.value, mes.scriptId);
				if ( mes.scriptId ) // Если пакет принадлежит сценарию - дублируем для сервера
				{
					printf( "  ---> Sending ok. Script id=%u\n", (mes.scriptId-2000) );
					ipc.sendByScript(mes.scriptId);
				}
			}
		}
		delay(1);
	}
}

void mes_enc(mes_t * message)
{
	_encrypt(message, sizeof(mes_t));
}

void mes_dec(void)
{
	_decrypt(&message, sizeof(mes_t));
}

void _encrypt(void * data, unsigned int size)
{
	byte iv[16];
	aes.gen_iv(iv);
	aes.do_aes_encrypt((byte *)data, size, cipher, key, 128, iv);
	for ( uint8_t i=0; i<16; i++ )
		cipher[112+i] = iv[i];
}

void _decrypt(void * data, unsigned int size)
{
	byte _tmp[size + (N_BLOCK - (size % 16))];
	byte iv[16];
	for ( uint8_t i=0; i<16; i++ )
		iv[i] = cipher[112+i];
	aes.do_aes_decrypt(cipher, size + (N_BLOCK - (size % 16)), _tmp, key, 128, iv);
	memcpy((byte *)data, _tmp, size);
}