#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include "string.h"
#include "clientsData.h"
#include "../DBOperations/DBOperations.h"

#define MAX_CLIENTS 100
#define BUFFER_SZ 2048
#define LENGTH_NAME 31
#define LENGTH_MSG 101
#define LENGTH_SEND 201

static unsigned int cli_count = 0;
static int uid = 10;

class ServerOperations
{
    public:

    static DBOperations db;
    static client_t *cli;

    static void send_message_to_all(char *, int);
    static char *get_online_clients();
    static void send_message_to_one(char *, client_t *);
    static bool check_exist(client_t *);
    static bool register_client(int);
    static client_t *client_for_single_chat(char *);
    static void chatmode_selection(client_t *, char *);
    static void message_handler(client_t *, char *, char *, int);
    static bool login_client(client_t *);
    static void *handle_client(void *);
    static void adding_client(int);
};