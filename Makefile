CC=gcc
CFLAGS+=-Wall -I.
LDFLAGS+=-L. 

.PHONY : all clean

all : docdir doc client1 client2 server
bin:
	mkdir -p bin
client2dir:
	mkdir -p client2
docdir:
	mkdir -p doc
doc: docdir
	doxygen Doxyfile

bin/aux.o : aux.c aux.h | bin
	$(CC) $(CFLAGS) -c $< -o $@

bin/client.o : client.c aux.h | bin
	$(CC) $(CFLAGS) -c $< -o $@

bin/server.o : server.c aux.h | bin
	$(CC) $(CFLAGS) -c $< -o $@

client1 : bin/client.o bin/aux.o
	$(CC) $^ -o $@ $(LDFLAGS)

client2 : bin/client.o bin/aux.o | client2dir
	$(CC) $^ -o client2/client2 $(LDFLAGS)

server : bin/server.o bin/aux.o
	$(CC) $^ -o $@ $(LDFLAGS)

clean : 
	rm -rf bin client1 client2 server doc
