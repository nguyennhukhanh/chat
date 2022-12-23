CC=gcc
CFLAGS=-I. -lpthread
DEPS1 = server.h
OBJ1 = server.o

DEPS2 = client.h
OBJ2 = client.o

all: server client

%.o: %.c $(DEPS1)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJ1)
	$(CC) -o $@ $^ $(CFLAGS)
	rm server.o

%.o: %.c $(DEPS2)
	$(CC) -c -o $@ $< $(CFLAGS)

client: $(OBJ2)
	$(CC) -o $@ $^ $(CFLAGS)
	rm client.o

clean:
	rm client
	rm server