#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "string.cpp"
#include "proto.h"
#include "client.h"

// Global variables
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[32];

void catch_ctrl_c_and_exit(int sig) {
    flag = 1;
}

void* send_msg_handler(void* arg) {
  char message[LENGTH_MSG] = {};
	char buffer[LENGTH_MSG + 32] = {};

  while(1) {
  	str_overwrite_stdout();
    fgets(message, LENGTH_MSG, stdin);
    str_trim_lf(message, LENGTH_MSG);

    if (strcmp(message, "exit") == 0) {
			break;
    } else {
      sprintf(buffer, "%s: %s\n", name , message);
      send(sockfd, buffer, strlen(buffer), 0);
    }
  }
  catch_ctrl_c_and_exit(2);
}

void* recv_msg_handler(void* arg) {
	char message[LENGTH_MSG] = {};
  while (1) {
		int receive = recv(sockfd, message, LENGTH_MSG, 0);
    if (receive > 0) {
      printf("%s", message);
      str_overwrite_stdout();
    } else if (receive == 0) {
			break;
    } else {
			// -1
		}
		memset(message, 0, sizeof(message));
  }
}

int Client::creatingSocket()
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if ( sockfd == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	}
	return sockfd;
}

void Client::connectingToServer(int sock)
{
	char *ip = "127.0.0.1";
	int port = 8000;

	serverAddress.sin_family = AF_INET; 
	serverAddress.sin_port = htons(port); 
    serverAddress.sin_addr.s_addr = inet_addr(ip);

    if (connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) 
    { 
		perror("\nConnection Failed \n");  
        exit(EXIT_FAILURE);
	} else {
		printf("\nConnected with server. Sending data...\n");
	}
}

void Client::clientLogin(int sockfd)
{
	printf("Please enter your name: ");
	fgets(name, 32, stdin);
	str_trim_lf(name, strlen(name));


	if (strlen(name) > 32 || strlen(name) < 2){
		printf("Name must be less than 30 and more than 2 characters.\n");
		exit(EXIT_FAILURE);
	}

	send(sockfd, name, LENGTH_NAME, 0);

	printf("=== WELCOME TO THE CHATROOM ===\n");

	pthread_t send_msg_thread;
  	if(pthread_create(&send_msg_thread, NULL, send_msg_handler, NULL) != 0){
		printf("ERROR: pthread\n");
		exit(EXIT_FAILURE);
	}

	pthread_t recv_msg_thread;
  	if(pthread_create(&recv_msg_thread, NULL, recv_msg_handler, NULL) != 0){
		printf("ERROR: pthread\n");
		exit(EXIT_FAILURE);
	}
}


int main(int argc, char **argv)
{
	signal(SIGINT, catch_ctrl_c_and_exit);

	Client client;
	sockfd = client.creatingSocket();
	client.connectingToServer(sockfd);
	client.clientLogin(sockfd);

	while (1){
		if(flag){
			printf("\nBye\n");
			break;
    	}
	}

	close(sockfd);

	return EXIT_SUCCESS;
}
