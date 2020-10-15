#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "string.cpp"
#include "clientsData.h"
#include "server.h"

#define MAX_CLIENTS 100
#define BUFFER_SZ 2048

static unsigned int cli_count = 0;
static int uid = 10;


/* Send message to all clients except sender */
void send_message(char *s, int uid){
	pthread_mutex_lock(&clients_mutex);

	for(int i=0; i<MAX_CLIENTS; ++i){
		if(clients[i]){
			if(clients[i]->uid != uid){
				if(write(clients[i]->sockfd, s, strlen(s)) < 0){
					perror("ERROR: write to descriptor failed");
					break;
				}
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

/* Handle all communication with the client */
void *handle_client(void *arg){
	char buff_out[BUFFER_SZ];
	char name[32];
	int leave_flag = 0;

	cli_count++;
	client_t *cli = (client_t *)arg;

	// Name
	if(recv(cli->sockfd, name, 32, 0) <= 0 || strlen(name) <  2 || strlen(name) >= 32-1){
		printf("Didn't enter the name.\n");
		leave_flag = 1;
	} else{
		strcpy(cli->name, name);
		sprintf(buff_out, "%s has joined\n", cli->name);
		printf("%s", buff_out);
		send_message(buff_out, cli->uid);
	}

	bzero(buff_out, BUFFER_SZ);

	while(1){
		if (leave_flag) {
			break;
		}

		int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
		if (receive > 0){
			if(strlen(buff_out) > 0){
				send_message(buff_out, cli->uid);

				str_trim_lf(buff_out, strlen(buff_out));
				printf("%s -> %s\n", buff_out, cli->name);
			}
		} else if (receive == 0 || strcmp(buff_out, "exit") == 0){
			sprintf(buff_out, "%s has left\n", cli->name);
			printf("%s", buff_out);
			send_message(buff_out, cli->uid);
			leave_flag = 1;
		} else {
			printf("ERROR: -1\n");
			leave_flag = 1;
		}

		bzero(buff_out, BUFFER_SZ);
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


int main(int argc, char **argv)
{
	signal(SIGPIPE, SIG_IGN);

  	Server server;
	
	server.creatingSocket();
	server.settingSocket();
	server.binding();
	server.listening();

	printf("=== WELCOME TO THE CHATROOM ===\n");

	while(1){

		int connfd = server.accepting();

		/* Client settings */
		client_t *cli = (client_t *)malloc(sizeof(client_t));
		cli->sockfd = connfd;
		cli->uid = uid++;

		/* Add client to the array */
		array_add(cli);

		pthread_t tid;
		pthread_create(&tid, NULL, &handle_client, (void*)cli);

		/* Reduce CPU usage */
		sleep(1);
	}

	return EXIT_SUCCESS;
}
