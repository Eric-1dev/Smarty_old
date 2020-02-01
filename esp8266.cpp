#include "tcpserver.h"

int main(int argc , char *argv[])
{
	begin(3000); // 3000-порт
	puts("Ждем входящих соединений...");

	while(1)
	{
		handleClient();
	}
	return 0;
}