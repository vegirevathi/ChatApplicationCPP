#include "../model/server.h"
#include <thread>

class ServerController
{
    public:
    Server server;

    void sendMessage(int);
    void receiveMessage(int);
    void executingServer();
};