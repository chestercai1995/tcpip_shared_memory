all: newclient newserver 

server:
	gcc server.c -o server -pthread

client:
	gcc client.c -o client -pthread

tcpiplib.o: tcpiplib.c tcpiplib.h
	gcc -c tcpiplib.c -o tcpiplib.o -pthread

newclient.o: newclient.c
	gcc -c newclient.c -o newclient.o -pthread

newserver.o: newserver.c
	gcc -c newserver.c -o newserver.o -pthread

newclient: newclient.o tcpiplib.o
	gcc newclient.o tcpiplib.o -o newclient -pthread

newserver: newserver.o tcpiplib.o
	gcc newserver.o tcpiplib.o -o newserver -pthread

clean:
	rm -f server client output* *.o newclient newserver
