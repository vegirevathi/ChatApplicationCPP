
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdint>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "serverOperations.h"

using namespace std;

class Server
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    char buffer[1024];
    int addrlen = sizeof(address);

public:

	ServerOperations serverOp;

    void creatingSocket();
    void settingSocket();
    void binding();
    void listening();
    int accepting();
    void startServer();
};