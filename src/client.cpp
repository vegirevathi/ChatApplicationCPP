#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "proto.h"
#include "client.h"
#include "string.h"

// Global variables
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[32];
char password[10];

void catch_ctrl_c_and_exit(int sig)
{
    flag = 1;
}

void *send_msg_handler(void *arg)
{
    char message[LENGTH_MSG] = {};
    char buffer[LENGTH_MSG + 32] = {};

    while (1)
    {
        str_overwrite_stdout();
        fgets(message, LENGTH_MSG, stdin);
        str_trim_lf(message, LENGTH_MSG);

        if (strcmp(message, "exit") == 0)
        {
            break;
        }
        else
        {
            sprintf(buffer, "%s: %s\n", name, message);
            send(sockfd, buffer, strlen(buffer), 0);
        }
    }
    catch_ctrl_c_and_exit(2);
}

void *recv_msg_handler(void *arg)
{
    char message[LENGTH_MSG] = {};
    while (1)
    {
        int receive = recv(sockfd, message, LENGTH_MSG, 0);
        if (receive > 0)
        {
            printf("%s", message);
            str_overwrite_stdout();
        }
        else if (receive == 0)
        {
            break;
        }
        else
        {
            // -1
        }
        memset(message, 0, sizeof(message));
    }
}

int Client::creatingSocket()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == 0)
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
    }
    else
    {
        printf("\nConnected with server. Sending data...\n");
    }
}

void Client::clientLogin(int sockfd)
{
    printf("Please enter your name: ");
    cin >> name;
    str_trim_lf(name, strlen(name));

    if (strlen(name) > 32 || strlen(name) < 2)
    {
        printf("Name must be less than 30 and more than 2 characters.\n");
        catch_ctrl_c_and_exit(2);
    }

    printf("Please enter your password: ");
    cin >> password;
    str_trim_lf(password, strlen(password));

    if (strcmp(password, "123456") != 0)
    {
        printf("Wrong password\n");
        catch_ctrl_c_and_exit(2);
    }

    cout << "Logged in successfully" << endl;

    send(sockfd, name, LENGTH_NAME, 0);

    pthread_t send_msg_thread;
    if (pthread_create(&send_msg_thread, NULL, send_msg_handler, NULL) != 0)
    {
        printf("ERROR: pthread\n");
    }

    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, recv_msg_handler, NULL) != 0)
    {
        printf("ERROR: pthread\n");
    }
}

void Client::clientRegister(int sockfd)
{
    cout << "Please enter your name: " << endl;
    cin >> name;
    str_trim_lf(name, strlen(name));

    if (strlen(name) > 32 || strlen(name) < 2)
    {
        printf("Name must be less than 30 and more than 2 characters.\n");
        catch_ctrl_c_and_exit(2);
    }

    cout << "Create your password: " << endl;
    cin >> password;
    str_trim_lf(password, strlen(password));

    if (strlen(password) > 10 || strlen(password) < 5)
    {
        printf("Password must be less than 10 and more than 5 characters.\n");
        catch_ctrl_c_and_exit(2);
    }
    printf("Registered successfully, exit the terminal to start login");
    catch_ctrl_c_and_exit(2);
}

void Client::clientSelection()
{
    printf("=== WELCOME TO THE CHATROOM ===\n");

    cout << "Enter 1 to login" << endl;
    cout << "Enter 2 to register" << endl;

    int choice;
    cout << "Enter your choice " << endl;
    cin >> choice;
    cout << endl;
    switch (choice)
    {
    case 1:
        clientLogin(sockfd);
        break;
    case 2:
        clientRegister(sockfd);
        break;
    default:
        cout << "Invalid option" << endl;
        break;
    }
}

int main(int argc, char **argv)
{
    signal(SIGINT, catch_ctrl_c_and_exit);

    Client client;
    sockfd = client.creatingSocket();
    client.connectingToServer(sockfd);
    client.clientSelection();

    while (1)
    {
        if (flag)
        {
            printf("\nBye\n");
            break;
        }
    }

    close(sockfd);

    return EXIT_SUCCESS;
}
