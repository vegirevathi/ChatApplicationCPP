#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "clientsData.h"

#define BUFFER_SZ 2048
#define LENGTH_MSG 101

class ServerOperations
{
    public:

    char name[32];
	char password[20];
	char buff_out[BUFFER_SZ];
	char buffer[LENGTH_MSG + 32] = {};
	int leave_flag = 0;
	client_t *cli2 = nullptr;
    int i = 0;

    void send_message_to_all(char *, int);
    void send_message_to_one(char *, client_t *);
    bool check_exist(client_t *);
    bool register_client(int);
    client_t *client_for_single_chat(char *);
    bool login_client(client_t *);
   // bool authentication(char *, char *, client_t *);
    void message_handling(client_t *, int);
    void select_receivers(client_t *, char *);
    void get_online_clients(client_t *);
    void chat_mode_selection(client_t *);
};