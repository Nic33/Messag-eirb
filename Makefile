CC=gcc
CFLAGS+=-Wall -I.
LDFLAGS+=-L. 

.PHONY : all clean

all : client server

bin:
	mkdir -p bin

bin/function.o : function.c function.h | bin
	$(CC) $(CFLAGS) -c $< -o $@

bin/client.o : client.c function.h | bin
	$(CC) $(CFLAGS) -c $< -o $@

bin/server.o : server.c function.h | bin
	$(CC) $(CFLAGS) -c $< -o $@

client : bin/client.o bin/function.o
	$(CC) $^ -o $@ $(LDFLAGS)

server : bin/server.o bin/function.o
	$(CC) $^ -o $@ $(LDFLAGS)

clean : 
	rm -rf bin client server
