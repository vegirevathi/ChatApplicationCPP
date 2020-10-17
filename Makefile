CC=c++
CFLAGS=-O3 -Wall
LIBS=-pthread
SRC=src

all: server.out client.out
server.out: server.o
	$(CC) $(CFLAGS) $(LIBS) -o server.out server.o
client.out: client.o string.o
	$(CC) $(CFLAGS) $(LIBS) -o client.out client.o string.o
server.o: $(SRC)/server.cpp
	$(CC) $(CFLAGS) -c $(SRC)/server.cpp
string.o: $(SRC)/string.cpp
	$(CC) $(CFLAGS) -c $(SRC)/string.cpp
client.o: $(SRC)/client.cpp
	$(CC) $(CFLAGS) -c $(SRC)/client.cpp

.PHONY: clean
clean:
	rm -f *.o *.out