all: client.out server.out

client.out:
	c++ --std=c++17 src/Utils/string.cpp src/Client/client.cpp -o client.out -pthread

server.out:
	c++ --std=c++17 src/Utils/string.cpp src/DBOperations/DBOperations.cpp src/Server/server.cpp -o server.out -pthread $(pkg-config --cflags --libs libmongocxx)

.PHONY: clean
clean:
	rm -f *.o *.out