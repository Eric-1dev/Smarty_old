#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "smarty_types.h"

class IPC {
	public:
		IPC();
		void send(uint8_t node_id, uint8_t command, uint8_t param, int16_t value, uint16_t scriptId);
		void sendByScript(uint16_t);
		int getMsqid(void);
		int16_t recv(mymsgbuf *, long);
		void sendStatus(uint8_t, bool);
	private:
		int msqid;	/* IPC дескриптор для очереди сообщений */
		mymsgbuf mybuf;
		void _sendbuf(void);
};