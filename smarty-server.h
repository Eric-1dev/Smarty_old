#include <mysql.h>
#include "smarty_config.h"
#include "smarty_types.h"

class cSmarty
{
	private:
		mymsgbuf mybuf;
		int msqid; /* IPC дескриптор для очереди сообщений */
		int maxlen;
		MYSQL conn;
		char query[1024];
		bool sendCommand(uint16_t, int16_t, uint16_t);
		bool getValue(int16_t*, int16_t, int16_t, uint16_t, bool);
		bool script_handler(int16_t*, uint8_t, int16_t, int16_t, uint16_t);
		void delay(unsigned int millis);
		void DBclose(void);
		bool DBquery(void);
	public:
		cSmarty();
		~cSmarty();
		void IPCinit(void);
		int DBconnect(void);
		long getMsgIPC(void);
		void hello(void);
		uint8_t handler(void);
		uint8_t scheduler(void);
};