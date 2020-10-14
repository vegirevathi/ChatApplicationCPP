#pragma once
#include<iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

using namespace std;

class Client
{
    int client_socket;
    char buffer[1024];
    struct sockaddr_in serverAddress;

    public:

    void creatingSocket();
    int getClientSocket();
    void connectingToServer();
    void sendingMsgs(int);
    void receivingMsgs(int);
};