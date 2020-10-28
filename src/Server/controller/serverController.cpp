#include <iostream>
#include <signal.h>
#include "../model/server.h"

using namespace std;

int main(int argc, char **argv)
{
	system("clear");
	signal(SIGPIPE, SIG_IGN);

	cout << "\033[1;35m WELCOME TO THE CHATROOM   \033[0m\n";

	Server server;

	server.creatingSocket();
	server.settingSocket();
	server.binding();
	server.listening();

	while (1)
	{
		int connfd = server.accepting();
		server.addingClient(connfd);
	}

	return EXIT_SUCCESS;
}