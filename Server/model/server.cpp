#include "server.h"

void Server::creatingSocket()
{
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	}
}

void Server::settingSocket()
{
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)))
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT , &opt, sizeof(opt)))
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	}
}

void Server::binding()
{
    address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons(8000); 
	
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
}

void Server::listening()
{
    if (listen(server_fd, 3) < 0) { 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} else {
        std::cout << "Server started. Listening...." << std::endl;
    } 
}

int Server::accepting()
{
    int connectionStatus = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (connectionStatus <= 0) 
	{ 
		perror("accept"); 
		exit(EXIT_FAILURE); 
	} else {
        std::cout << "Server started. Accepting...." << std::endl;
    } 
    return connectionStatus;
}

void Server::sendingMsgs(int sock)
{
		memset(buffer, 0, sizeof buffer);
        std::cin.getline(buffer, 1024);
		send(sock ,buffer ,strlen(buffer) ,0); 
}

void Server::receivingMsgs(int sock)
{
		memset(buffer, 0, sizeof buffer);
		int valread = read(sock, buffer, 1024); 
		printf("Client: %s\n", buffer ); 
}