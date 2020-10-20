<<<<<<< HEAD
CC=c++
=======
<<<<<<< HEAD
CC=g++
=======
CC=c++
>>>>>>> d42671ef9f7b4b75f61feca1f53ff333acae4391
>>>>>>> master
CFLAGS=-O3 -Wall
LIBS=-pthread
SRC=src

all: server.out client.out
server.out: server.o
<<<<<<< HEAD
	$(CC) --std=c++17 $(CFLAGS) $(LIBS) -o server.out server.o $(pkg-config --cflags --libs libmongocxx)
=======
<<<<<<< HEAD
	$(CC) $(CFLAGS) $(LIBS) -o server.out server.o
>>>>>>> master
client.out: client.o string.o
	$(CC) --std=c++17 $(CFLAGS) $(LIBS) -o client.out client.o string.o
server.o: $(SRC)/server.cpp
	$(CC) --std=c++17 $(CFLAGS) -c $(SRC)/server.cpp $(pkg-config --cflags --libs libmongocxx)
string.o: $(SRC)/string.cpp
	$(CC) --std=c++17 $(CFLAGS) -c $(SRC)/string.cpp
client.o: $(SRC)/client.cpp
<<<<<<< HEAD
	$(CC) --std=c++17 $(CFLAGS) -c $(SRC)/client.cpp
=======
	$(CC) $(CFLAGS) -c $(SRC)/client.cpp
=======
	$(CC) --std=c++17 $(CFLAGS) $(LIBS) -o server.out server.o $(pkg-config --cflags --libs libmongocxx)
client.out: client.o string.o
	$(CC) --std=c++17 $(CFLAGS) $(LIBS) -o client.out client.o string.o
server.o: $(SRC)/server.cpp
	$(CC) --std=c++17 $(CFLAGS) -c $(SRC)/server.cpp $(pkg-config --cflags --libs libmongocxx)
string.o: $(SRC)/string.cpp
	$(CC) --std=c++17 $(CFLAGS) -c $(SRC)/string.cpp
client.o: $(SRC)/client.cpp
	$(CC) --std=c++17 $(CFLAGS) -c $(SRC)/client.cpp
>>>>>>> d42671ef9f7b4b75f61feca1f53ff333acae4391
>>>>>>> master

.PHONY: clean
clean:
	rm -f *.o *.out