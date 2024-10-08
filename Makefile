CC=gcc
CFLAGS+=-Wall -I.
LDFLAGS+=-L. 

.PHONY : all clean

all : client server

bin:
	mkdir -p bin

bin/client.o : client.c | bin
	$(CC) $(CFLAGS) -c $< -o $@

bin/server.o : server.c | bin
	$(CC) $(CFLAGS) -c $< -o $@

client : bin/client.o 
	$(CC) $^ -o $@ $(LDFLAGS)

server : bin/server.o
	$(CC) $^ -o $@ $(LDFLAGS)

clean : 
	rm -rf bin client server
