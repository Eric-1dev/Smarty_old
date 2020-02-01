#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <unistd.h>
#include <map>
#include <time.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <algorithm>
#include <pthread.h>
#include "ipclib.h"
#include "AES.h"

using namespace std;

/* Коды команд */
#define INIT 120
#define HELLO 121
#define PING 123
#define STATUS 124
#define GET 125
#define SET 126
#define NOTIFY 127

struct mes_t {
	uint8_t command;
	uint8_t param;
	int16_t value;
	uint16_t scriptId;
};

struct thread_arg_t {
	mes_t message;
	int sock_desc;
};

typedef map<int, uint8_t> mymap;

void begin(unsigned int);
void handleClient(void);
void enable_keepalive(int);
void msgHandler(mes_t, int);
int getSockByNode(uint8_t);
int getNodeBySock(int);
void delay(unsigned int millis);
void * thread_func(void * args);
void mes_enc(mes_t *);
void mes_dec(void);
void _encrypt(void *, unsigned int);
void _decrypt(void *, unsigned int);