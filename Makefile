all: smarty-server nrf24l01 esp8266 smarty-sender sched-sender

nrf24l01: nrf24l01.cpp
	g++ nrf24l01.cpp ipclib.cpp -o nrf24l01 -lrf24-bcm -lrf24network -lrf24mesh -Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s

esp8266: esp8266.cpp
	g++ esp8266.cpp tcpserver.cpp ipclib.cpp AES.cpp -o esp8266 -lpthread

smarty-server: server-main.cpp
	g++ server-main.cpp smarty-server.cpp -o smarty-server -I/usr/include/mysql -lmysqlclient

smarty-sender: smarty-sender.cpp
	g++ smarty-sender.cpp -o smarty-sender

sched-sender: sched-sender.cpp
	g++ sched-sender.cpp -o sched-sender

clean:
	rm -rf RFreceiver smarty-server nrf24l01 esp8266 smarty-sender sched-sender *.o

install:
	install ./smarty-server /usr/local/bin/
#	install ./RFreceiver /usr/local/bin/
	install ./nrf24l01 /usr/local/bin/
	install ./esp8266 /usr/local/bin/
	install ./smarty-sender /usr/local/bin/
	install ./sched-sender /usr/local/bin/
	install ./init.d/smarty /etc/init.d/
	install ./smarty_sched /etc/cron.d/

uninstall:
	rm -f /usr/local/bin/smarty-server /usr/local/bin/nrf24l01 /usr/local/bin/esp8266 /usr/local/bin/smarty-sender /usr/local/bin/RFreceiver /etc/init.d/smarty /usr/local/bin/sched-sender /etc/cron.d/smarty_sched
