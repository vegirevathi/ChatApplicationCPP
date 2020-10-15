#include "client.h"

void Client::creatingSocket()
{
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	}
}

int Client::getClientSocket()
{
    return this->client_socket;
}

void Client::connectingToServer()
{
    serverAddress.sin_family = AF_INET; 
	serverAddress.sin_port = htons(8000); 
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (connect(client_socket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) 
    { 
		perror("\nConnection Failed \n");  
        exit(EXIT_FAILURE);
	} else {
		printf("\nConnected with server. Sending data...\n");
	}
}

void Client::sendingMsgs(int socket)
{
    memset(buffer, 0, sizeof buffer);
	std::cin.getline(buffer, 1024);
	send(socket ,buffer ,strlen(buffer) ,0); 
}

void Client::receivingMsgs(int socket)
{
    memset(buffer, 0, sizeof buffer);
	int valread = read(socket, buffer, 1024); 
	printf("Server: %s\n", buffer ); 
}