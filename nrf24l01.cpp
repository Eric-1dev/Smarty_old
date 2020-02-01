#include "RF24Mesh/RF24Mesh.h"
#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include "ipclib.h"

#define RADIO_CHANNEL 101
#define RADIO_SPEED RF24_1MBPS

#define RETRY_NUM 3
#define WDOG_TIMEOUT 1000 // *2 ms

uint8_t rfSend(uint8_t, uint8_t, uint8_t, int16_t, uint16_t);
//uint8_t sendStatus(uint8_t, bool);
uint8_t crc8(const uint8_t *, uint8_t);

smarty_msg packet;

RF24 radio(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);
RF24Network network(radio);
RF24Mesh mesh(radio,network);
IPC ipc;

// --------------- Массив для быстрого расчета CRC --------------- //
uint8_t dscrc_table[] = {
    0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
  157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
   35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
  190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
   70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
  219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
  101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
  248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
  140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
   17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
  175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
   50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
  202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
   87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
  233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
  116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};
// ---------------------------------------------------------------- //

int main(int argc, char** argv)
{
// ------------- Инициализируем радио ----------------------------- //
	mesh.setNodeID(0);
	mesh.begin(RADIO_CHANNEL, RADIO_SPEED);
	radio.setPALevel(RF24_PA_MAX);
	radio.printDetails();
// ---------------------------------------------------------------- //
	uint16_t wdog=0; // Счетчик сторожевого таймера
// --------------- Инициализируем очередь сообщений --------------- //
	mymsgbuf mybuf;

	uint8_t i=0; /* Счетчик цикла и максимальная длина информативной части сообщения */
	int16_t len_out; /* Длина информативной части сообщения */

// ---------------------------------------------------------------- //
	while(1)
	{
		mesh.update();

		mesh.DHCP();

		while( network.available() )
		{
			RF24NetworkHeader header;
			network.peek(header);

			network.read( header, &packet.data, sizeof(data_t) );
			if ( crc8((uint8_t*)&packet.data, sizeof(data_t)-1)+1 == packet.data.crc )
			{
				printf("Rcv c=%u p=%u v=%d scrId=%u from %u\n", header.type, packet.data.param, packet.data.value, packet.data.scriptId, mesh.getNodeID(header.from_node));
				ipc.send(mesh.getNodeID(header.from_node), header.type, packet.data.param, packet.data.value, packet.data.scriptId);

				ipc.sendStatus(mesh.getNodeID(header.from_node), true);	// Обновляем статус узла
			}
			else
				puts("CRC is incorrect!");
			// -------------------------------------
		}
// ------------------- Получаем сообщения из очереди ( тип сообщения: 1001 ) ---------------- //
// ------------------- и формируем сообщение исполнителю                    ---------------- //
		if ( ipc.recv(&mybuf, 1001) > 0 )
			rfSend( mybuf.mtext.node, mybuf.mtext.packet.command, mybuf.mtext.packet.data.param, mybuf.mtext.packet.data.value, mybuf.mtext.packet.data.scriptId );
// ----------------------------------------------------------------------------------------- //
		wdog++;
		if ( wdog > WDOG_TIMEOUT )
		{
			wdog = 0;
			if ( radio.getChannel() != RADIO_CHANNEL )
//			if ( radio.failureDetected ) 
			{
				printf("NRF is down. Restarting radio... \n");
				mesh.begin(RADIO_CHANNEL, RADIO_SPEED);
				printf("Ok\n");
			}
		}

		delay(2);
	}
	return 0;
}

uint8_t rfSend( uint8_t to_node, uint8_t command, uint8_t param, int16_t value, uint16_t scriptId )
{
	uint8_t i;

	packet.command = command;
	packet.data.param = param;
	packet.data.value = value;
	packet.data.scriptId = scriptId;
	printf("Sending message type=%u param=%u value=%d scriptId=%u to node = %u ... \n", packet.command, packet.data.param, packet.data.value, packet.data.scriptId, to_node);
	packet.data.crc = crc8((uint8_t*) &packet.data, sizeof(data_t)-1)+1;

	for (i=0; i<RETRY_NUM; i++)
	{
		if ( mesh.write(&packet.data, packet.command, sizeof(data_t), to_node) )
		{
			if ( packet.data.scriptId ) // Если пакет принадлежит сценарию - дублируем для сервера
			{
				printf( "  ---> Sending ok. Script id=%u\n", (packet.data.scriptId-2000) );
				ipc.sendByScript(packet.data.scriptId);
			}
			ipc.sendStatus(to_node, true);
			printf("Ok!\n\n");
			return 1;
		}
		delay(2);
	}
	ipc.sendStatus(to_node, false);
	printf("failed!\n\n");
	return 0;
}

// uint8_t sendStatus(uint8_t node, bool status)
// {
	// ipc.send(node, STATUS, 0, status, 0);
// }

uint8_t crc8(const uint8_t *addr, uint8_t length)
{
	uint8_t crc = 0;
	while (length--)
		crc = dscrc_table[crc ^ *addr++];
	return crc;
}
