#pragma once
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdint>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../DBOperations/DBOperations.h"
//#include "clientsData.h"
#include "../Utils/string.h"
#include "../Utils/proto.h"

#define MAX_CLIENTS 100
#define BUFFER_SZ 2048

// #include <vector>
// #include <bsoncxx/json.hpp>
// #include <mongocxx/client.hpp>
// #include <mongocxx/stdx.hpp>
// #include <mongocxx/uri.hpp>
// #include <mongocxx/instance.hpp>
// #include <bsoncxx/builder/stream/document.hpp>
// #include <bsoncxx/stdx/optional.hpp>

// using bsoncxx::builder::stream::close_array;
// using bsoncxx::builder::stream::close_document;
// using bsoncxx::builder::stream::document;
// using bsoncxx::builder::stream::finalize;
// using bsoncxx::builder::stream::open_array;
// using bsoncxx::builder::stream::open_document;

// mongocxx::instance instance{}; // This should be done only once.
// mongocxx::client client{mongocxx::uri{}};

class Server
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    char buffer[1024];
    int addrlen = sizeof(address);

    DBOperations db;

public:
    void creatingSocket();
    void settingSocket();
    void binding();
    void listening();
    int accepting();

    void send_message_to_all(char *, int);
    void send_message_to_one(char *, client_t *);
    bool check_exist(client_t *);
    void register_client(int);
    client_t *client_for_single_chat(char *);
    void login_client(client_t *);
};