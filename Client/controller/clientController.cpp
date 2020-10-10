#include "../controller/clientController.h"

void ClientController::sendMessage(int socket)
{
    while(1)
    {
        client.sendingMsgs(socket);
    }
}

void ClientController::receiveMessage(int socket)
{
    while(1)
    {
        client.receivingMsgs(socket);
    }
}

void ClientController::executingClient()
{
    client.creatingSocket();
    client.connectingToServer();
    int new_socket = client.getClientSocket();

    std::thread send_thread (&ClientController::sendMessage, this, new_socket);
    std::thread recv_thread (&ClientController::receiveMessage, this, new_socket);

    send_thread.join();
    recv_thread.join();
}

int main()
{
    ClientController controller;
    controller.executingClient();
}