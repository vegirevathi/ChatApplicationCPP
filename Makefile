CC=c++
CFLAGS=-O3 -Wall
LIBS=-pthread
SRC=src

all: server.out client.out
server.out: server.o
	$(CC) --std=c++17 $(CFLAGS) $(LIBS) -o server.out server.o $(pkg-config --cflags --libs libmongocxx)
client.out: client.o string.o
	$(CC) --std=c++17 $(CFLAGS) $(LIBS) -o client.out client.o string.o
server.o: $(SRC)/server.cpp
	$(CC) --std=c++17 $(CFLAGS) -c $(SRC)/server.cpp $(pkg-config --cflags --libs libmongocxx)
string.o: $(SRC)/string.cpp
	$(CC) --std=c++17 $(CFLAGS) -c $(SRC)/string.cpp
client.o: $(SRC)/client.cpp
	$(CC) --std=c++17 $(CFLAGS) -c $(SRC)/client.cpp

.PHONY: clean
clean:
	rm -f *.o *.out