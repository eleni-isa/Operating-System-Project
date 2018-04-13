all: server client clean

server: server.o
	gcc -o server  server.o -pthread

client: client.o
	gcc -o client client.o

server.o: server.c
	gcc -c server.c

client.o:client.c
	gcc -c client.c

clean:
	rm -f client.o server.o
#gcc -Wall -o "server" "server.c" -pthread
#gcc -Wall -o "client" "client.c"
