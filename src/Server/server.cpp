#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "string.h"
#include "../Utils/string.h"
#include "clientsData.h"
#include "server.h"
#include "../Utils/proto.h"
#include "../DBOperations/DBOperations.h"

#define MAX_CLIENTS 100
#define BUFFER_SZ 2048

using namespace std;

static unsigned int cli_count = 0;
static int uid = 10;

DBOperations db;

/* Send message to all clients except sender */
void send_message_to_all(char *s, int uid)
{
	pthread_mutex_lock(&clients_mutex);

	for (int i = 0; i < MAX_CLIENTS; ++i)
		if (clients[i])
			if (clients[i]->uid != uid)
				if (clients[i]->chatroom_status)
					if (write(clients[i]->sockfd, s, strlen(s)) < 0)
					{
						perror("ERROR: write to descriptor failed");
						break;
					}

	pthread_mutex_unlock(&clients_mutex);
}

void send_message_to_one(char *s, client_t *cli)
{
	pthread_mutex_lock(&clients_mutex);

	write(cli->sockfd, s, strlen(s));

	pthread_mutex_unlock(&clients_mutex);
}

bool check_exist(client_t *cli)
{
	for (int i = 0; i < MAX_CLIENTS; ++i)
		if (clients[i])
			if (clients[i]->uid != cli->uid)
				if (strcmp(cli->name, clients[i]->name) == 0)
				{
					cout << "\033[;32m Client is present  \033[0m\n\n";
					return true;
				}

	return false;
}

bool register_client(int sockfd)
{
	char name[32];
	char password[20];

	cout << "\033[;33m \nRegistration is in process...  \033[0m\n";

	if ((recv(sockfd, name, 32, 0) <= 0 || strlen(name) < 2 || strlen(name) >= 32 - 1) ||
		(recv(sockfd, password, 32, 0) <= 0 || strlen(password) < 2 || strlen(password) >= 32 - 1))
		cout << "\033[;31m \nDidnt enter the name or password   \033[0m\n";

	else if (db.check_name(name))
	{
		cout << "\033[;31m \nClient is already registered!!!   \033[0m\n";
		write(sockfd, "0", strlen("0"));
		return false;
	}
	else
	{
		db.registerClient(name, password);
		write(sockfd, "1", strlen("1"));
		cout << "Client added successfully:" << name << endl;
		return true;
	}
}

client_t *client_for_single_chat(char *name)
{
	for (int i = 0; i < MAX_CLIENTS; i++)
		if (clients[i])
			if (!clients[i]->chatroom_status)
				if (strcmp(clients[i]->name, name) == 0)
					return clients[i];
	return nullptr;
}

void message_handler(client_t *cli, char *buffer, char *buff_out, int leave_flag)
{
	while (1)
	{
		if (leave_flag)
		{
			break;
		}

		int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
		if (receive > 0)
		{
			if (strlen(buff_out) > 0)
			{
				sprintf(buffer, "\x1B[33m%s : \033[0m%s", cli->name, buff_out);

				if (cli->chatroom_status)
				{
					db.storeGroupMessages(cli->name, buff_out);
					send_message_to_all(buffer, cli->uid);
				}
				else
				{
					db.storePrivateMessages(cli->name, cli->cli2->name, buff_out);
					send_message_to_one(buffer, cli->cli2);
				}
				str_trim_lf(buffer, strlen(buffer));
			}
		}
		else if (receive == 0)
		{
			sprintf(buff_out, "\n\x1B[36m%s has left\033[0m\n", cli->name);
			cout << "\x1B[36m" << buff_out << "\033[0m" << endl;
			leave_flag = 1;
		}
		else if (strcmp(buff_out, "$$exit") == 0)
		{
			sprintf(buff_out, "\n\x1B[36m%s has left\033[0m\n", cli->name);
			cout << "\x1B[36m" << buff_out << "\033[0m" << endl;
			if (cli->chatroom_status)
			{
				send_message_to_all(buff_out, cli->uid);
				cli->chatroom_status = false;
			}
			else
			{
				send_message_to_one(buff_out, cli->cli2);
				cli->cli2 = nullptr;
			} // leave_flag = 1;
		}
		else
		{
			cout << "ERROR: -1\n";
			leave_flag = 1;
		}
		bzero(buffer, LENGTH_MSG + 32);
		bzero(buff_out, BUFFER_SZ);
	}
}

void chatmode_selection(client_t* cli, char* buff_out)
{
	char name[32];

	recv(cli->sockfd, buff_out, 1, 0);
	if (strcmp("1", buff_out) == 0)
	{
		recv(cli->sockfd, name, 32, 0);
		cli->cli2 = client_for_single_chat(name);
		if (cli->cli2 == nullptr)
		{
			write(cli->sockfd, "0", 1);
		}
		else
		{
			sprintf(buff_out, "1");
			write(cli->sockfd, buff_out, strlen(buff_out));
		}
	}
	else if (strcmp("2", buff_out) == 0)
	{
		cli->chatroom_status = true;
		sprintf(buff_out, "\n\x1B[36m%s has joined\033[0m\n", cli->name);
		cout << "\x1B[36m" << buff_out << "\033[0m" << endl;
		send_message_to_all(buff_out, cli->uid);
	}
}

bool login_client(client_t *cli)
{
	char name[32];
	char password[20];
	char buff_out[BUFFER_SZ];
	char buffer[LENGTH_MSG + 32] = {};
	int leave_flag = 0;
	client_t *cli2 = nullptr;

	bzero(name, 32);
	bzero(password, 20);

	cout << "\033[;33mLogin is in process...   \033[0m\n";

	if ((recv(cli->sockfd, name, 32, 0) <= 0 || strlen(name) < 2 || strlen(name) >= 32 - 1) ||
		(recv(cli->sockfd, password, 32, 0) <= 0 || strlen(password) < 2 || strlen(password) >= 32 - 1))
	{
		cout << "\033[;31mDidnt enter the name or password \033[0m\n";
		return false;
		// leave_flag = 1;
	}
	else
	{
		cout << name << ", " << password << endl;
		if (db.authenticate(name, password))
		{
			cout << "\033[;32mAuthentication Successful  \033[0m\n";

			strcpy(cli->name, name);
			if (check_exist(cli))
			{
				write(cli->sockfd, "0", strlen("0"));
				cout << "not" << endl;
				login_client(cli);
				return false;
			}
			else
			{
				write(cli->sockfd, "1", strlen("1"));
				chatmode_selection(cli, buff_out);
				// recv(cli->sockfd, buff_out, 1, 0);
				// if (strcmp("1", buff_out) == 0)
				// {
				// 	recv(cli->sockfd, name, 32, 0);
				// 	cli->cli2 = client_for_single_chat(name);
				// 	if (cli->cli2 == nullptr)
				// 	{
				// 		write(cli->sockfd, "0", 1);
				// 	}
				// 	else
				// 	{
				// 		sprintf(buff_out, "1");
				// 		write(cli->sockfd, buff_out, strlen(buff_out));
				// 	}
				// }
				// else if (strcmp("2", buff_out) == 0)
				// {
				// 	cli->chatroom_status = true;
				// 	sprintf(buff_out, "\n\x1B[36m%s has joined\033[0m\n", cli->name);
				// 	cout << "\x1B[36m" << buff_out << "\033[0m" << endl;
				// 	send_message_to_all(buff_out, cli->uid);
				// }
				return true;
			}
		}
		else
		{
			cout << "\033[;31mNo Client Exists!!!   \033[0m\n";
			write(cli->sockfd, "2", strlen("2"));
			return false;
		}
	}
}

/* Handle all communication with the client */
void *handle_client(void *arg)
{
	char buff_out[BUFFER_SZ];
	char buffer[LENGTH_MSG + 32] = {};
	char name[32];
	int leave_flag = 0;
	char option[1];

	cli_count++;
	client_t *cli = (client_t *)arg;
	if (recv(cli->sockfd, option, 1, 0) <= 0)
	{
		cout << "\033[;31m Client Exited   \033[0m\n";
	}

	if (strcmp(option, "1") == 0)
	{
		bool flag = false;
		while (!flag)
			flag = register_client(cli->sockfd);
		// register_client(cli->sockfd);
	}

	else if (strcmp(option, "2") == 0)
	{
		bool flag = false;
		while (!flag)
			flag = login_client(cli);
		bzero(buff_out, BUFFER_SZ);
		message_handler(cli, buffer, buff_out, leave_flag);
	}

	/* Delete client from array and yield thread */
	close(cli->sockfd);
	array_remove(cli->uid);
	free(cli);
	cli_count--;
	pthread_detach(pthread_self());

	return NULL;
}

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

		/* Client settings */
		client_t *cli = (client_t *)malloc(sizeof(client_t));
		cli->sockfd = connfd;
		cli->uid = uid++;
		cli->chatroom_status = false;
		cli->cli2 = nullptr;
		array_add(cli);

		pthread_t tid;
		pthread_create(&tid, NULL, &handle_client, (void *)cli);
		sleep(1);
	}

	return EXIT_SUCCESS;
}
