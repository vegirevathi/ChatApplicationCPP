#include "../controller/serverController.h"

void ServerController::sendMessage(int socket)
{
    while(1)
    {
        server.sendingMsgs(socket);
    }
}

void ServerController::receiveMessage(int socket)
{
    while(1)
    {
        server.receivingMsgs(socket);
    }
}

void ServerController::executingServer()
{
    server.creatingSocket();
    server.settingSocket();
    server.binding();
    server.listening();
    int new_socket = server.accepting();
    std::thread send_thread (&ServerController::sendMessage, this, new_socket);
    std::thread recv_thread (&ServerController::receiveMessage, this, new_socket);

    send_thread.join();
    recv_thread.join();
}


int main()
{
    ServerController controller;
    controller.executingServer();    

    return 0;
}