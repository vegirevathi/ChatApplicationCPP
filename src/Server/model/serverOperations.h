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

    void send_message_to_all(char *, int);
    void send_message_to_one(char *, client_t *);
    bool check_exist(client_t *);
    void register_client(int);
    client_t *client_for_single_chat(char *);
    void login_client(client_t *);
    void authentication(char *, char *, client_t *);
    void message_handling(client_t *);
    void select_receivers(client_t *, char *);

};