#pragma once
#include<iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

using namespace std;

class Client
{
    int sockfd = 0;
    char buffer[1024];
    struct sockaddr_in serverAddress;

    public:

    int creatingSocket();
    void connectingToServer(int);
    void clientLogin(int);
};