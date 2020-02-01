using namespace std;

/* Коды команд */
#define HELLO 121
#define PING 123
#define STATUS 124
#define GET 125
#define SET 126
#define NOTIFY 127

struct data_t
{
	int16_t value;
	uint16_t scriptId;
	uint8_t param;
	uint8_t crc;
};

typedef struct
{
	data_t data;
	uint8_t command;
} smarty_msg;

struct Message
{
	uint8_t node;
	smarty_msg packet;
};

struct mymsgbuf
{
	long mtype;
	Message mtext;
};