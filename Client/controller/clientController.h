#include "../model/client.h"
#include <thread>

class ClientController
{
    public:
    Client client;

    void sendMessage(int);
    void receiveMessage(int);
    void executingClient();
};