#include "server.h"

using namespace std;

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
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)))
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

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
}

void Server::listening()
{
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	else
	{
		cout << "\033[;34m \nServer started..Listening   \033[0m\n";
	}
}

int Server::accepting()
{
	int connectionStatus = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
	if (connectionStatus <= 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	else
	{
		cout << "\033[;34m \nServer started..Accepting..   \033[0m\n";
	}
	return connectionStatus;
}

void Server::startServer()
{
	system("clear");
	signal(SIGPIPE, SIG_IGN);

	cout << "\033[1;35m WELCOME TO THE CHATROOM   \033[0m\n";

	creatingSocket();
	settingSocket();
	binding();
	listening();

	while (1)
	{
		int connfd = accepting();
		serverOp.adding_client(connfd);
	}
}
