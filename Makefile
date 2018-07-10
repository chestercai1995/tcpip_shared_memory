all: clean openjpg server client

openjpg:
	gcc openjpg.c -o openjpg

server:
	gcc server.c -o server -pthread

client:
	gcc client.c -o client -pthread

clean:
	rm -f openjpg server client output.jpg
