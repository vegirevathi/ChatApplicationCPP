#include "server.h"
#include "clientsData.h"

/* Send message to all clients except sender */
void Server::send_message_to_all(char *s, int uid)
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

void Server::send_message_to_one(char *s, client_t *cli)
{
	pthread_mutex_lock(&clients_mutex);

	write(cli->sockfd, s, strlen(s));

	pthread_mutex_unlock(&clients_mutex);
}

bool Server::check_exist(client_t *cli)
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

void Server::register_client(int sockfd)
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
	}
	else
	{
        db.registerClient(name, password);
		write(sockfd, "1", strlen("1"));
		cout << "Client added successfully:" << name << endl;
	}
}

client_t* Server::client_for_single_chat(char *name)
{
	for (int i = 0; i < MAX_CLIENTS; i++)
		if (clients[i])
			if (!clients[i]->chatroom_status)
				if (strcmp(clients[i]->name, name) == 0)
					return clients[i];
	return nullptr;
}

void Server::login_client(client_t *cli)
{
	char name[32];
	char password[20];
	char buff_out[BUFFER_SZ];
	char buffer[LENGTH_MSG + 32] = {};
	int leave_flag = 0;
	client_t *cli2 = nullptr;

	cout << "\033[;33mLogin is in process...   \033[0m\n";

	if ((recv(cli->sockfd, name, 32, 0) <= 0 || strlen(name) < 2 || strlen(name) >= 32 - 1) ||
		(recv(cli->sockfd, password, 32, 0) <= 0 || strlen(password) < 2 || strlen(password) >= 32 - 1))
	{
		cout << "\033[;31mDidnt enter the name or password \033[0m\n";
		leave_flag = 1;
	}
	else
	{
		if (db.authenticate(name, password))
		{
			cout << "\033[;32mAuthentication Successful  \033[0m\n";

			strcpy(cli->name, name);
			if (check_exist(cli))
			{
				write(cli->sockfd, "0", strlen("0"));
				leave_flag = 1;
			}
            else
			{
				write(cli->sockfd, "1", strlen("1"));
				recv(cli->sockfd, buff_out, 1, 0);
				if (strcmp("1", buff_out) == 0)
				{
					recv(cli->sockfd, name, 32, 0);
					cli->cli2 = client_for_single_chat(name);
					if (cli->cli2 == nullptr)
					{
						write(cli->sockfd, "0", 1);
						leave_flag = 1;
					}
					else
					{
						write(cli->sockfd, "1", 1);
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
		}
        else
		{
			cout << "\033[;31mNo Client Exists!!!   \033[0m\n";

			write(cli->sockfd, "2", strlen("2"));
			leave_flag = 1;
		}
	}

	bzero(buff_out, BUFFER_SZ);

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
		else if (receive == 0 || strcmp(buff_out, "exit") == 0)
		{
			sprintf(buff_out, "\n\x1B[36m%s has left\033[0m\n", cli->name);
			cout << "\x1B[36m" << buff_out << "\033[0m" << endl;
			if (cli->chatroom_status)
				send_message_to_all(buff_out, cli->uid);
			else
				send_message_to_one(buff_out, cli->cli2);
			leave_flag = 1;
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