CC=gcc
CFLAGS+=-Wall -I.
LDFLAGS+=-L. 

.PHONY : all clean

all : client server

bin:
	mkdir -p bin

bin/aux.o : aux.c aux.h | bin
	$(CC) $(CFLAGS) -c $< -o $@

bin/client.o : client.c aux.h | bin
	$(CC) $(CFLAGS) -c $< -o $@

bin/server.o : server.c aux.h | bin
	$(CC) $(CFLAGS) -c $< -o $@

client : bin/client.o bin/aux.o
	$(CC) $^ -o $@ $(LDFLAGS)

server : bin/server.o bin/aux.o
	$(CC) $^ -o $@ $(LDFLAGS)

clean : 
	rm -rf bin client server