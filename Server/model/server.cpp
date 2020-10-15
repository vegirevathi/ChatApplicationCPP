#include "server.h"

void catch_ctrl_c_and_exit(int sig)
{
	// ClientList *tmp;
	// while (root != NULL) {
	//     printf("\nClose socketfd: %d\n", root->data);
	//     close(root->data); // close all socket include server_sockfd
	//     tmp = root;
	//     root = root->link;
	//     free(tmp);
	// }
	printf("Bye\n");
	exit(EXIT_SUCCESS);
}

void Server::creatingSocket()
{
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		signal(SIGINT, catch_ctrl_c_and_exit);

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
		std::cout << "Server started. Listening...." << std::endl;
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
		std::cout << "Server started. Accepting...." << std::endl;
	}
	return connectionStatus;
}

std::string Server::convertToString(char *a, int size)
{
	int i;
	std::string s = "";
	for (i = 0; i < size; i++)
	{
		s = s + a[i];
	}
	return s;
}

void Server::sendingMsgs(int sock)
{
	memset(buffer, 0, sizeof buffer);
	std::cin.getline(buffer, 1024);
	send(sock, buffer, strlen(buffer), 0);
	// bsoncxx::builder::stream::document document{};
	// std::string newMsg = convertToString(buffer, strlen(buffer));
	// mongocxx::instance inst{};
	// mongocxx::client conn{mongocxx::uri{}};
	// auto collection = conn["chat_app_db"]["chats"];
	// time_t now = time(0);
	// string dt = ctime(&now);
	// document << "sender"
	// 		 << "Server"
	// 		 << "receiver"
	// 		 << "Client"
	// 		 << "message"
	// 		 << newMsg
	// 		 << "time";
}

void Server::receivingMsgs(int sock)
{
	memset(buffer, 0, sizeof buffer);
	int valread = read(sock, buffer, 1024);
	printf("Client: %s\n", buffer);
}