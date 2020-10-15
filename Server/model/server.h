#pragma once
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <ctime>
#include <signal.h>

// #include <bsoncxx/builder/stream/document.hpp>
// #include <bsoncxx/json.hpp>

// #include <mongocxx/client.hpp>
// #include <mongocxx/instance.hpp>

using namespace std;

class Server
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    char buffer[1024];
    int addrlen = sizeof(address);

public:
    void creatingSocket();
    void settingSocket();
    void binding();
    void listening();
    int accepting();
    void sendingMsgs(int);
    void receivingMsgs(int);
    std::string convertToString(char *a, int size);
};