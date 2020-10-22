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
#include "../Utils/proto.h"
#include "client.h"
#include "../Utils/string.h"
#include "../Utils/getchMethod.h"

#define PORT 8000

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
            send(sockfd, message, strlen(message), 0);
        }
        bzero(message, LENGTH_MSG);
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
            cout << "\x1B[36m" << message << "\033[0m" << endl;
            str_overwrite_stdout();
        }
        else if (receive == 0)
        {
            break;
        }
        memset(message, 0, sizeof(message));
    }
}

void *passwordPrinting()
{
    int i = 0;
    char a;
    for (i = 0;;)
    {
        a=getch();
        if((a>='a'&&a<='z')||(a>='A'&&a<='Z')||(a>='0'&&a<='9'))
        {
            password[i] = a;
            ++i;
            cout << "*";
        }
        if (i != 0)
        {
            if (a == '\n')
            {
                break;
            }
        }
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

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = inet_addr(ip);

    if (connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("\nConnection Failed \n");
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << "\033[;34m Connected with server. Sending data...  \033[0m\n";
    }
}

int Client::messageHandler(int sockfd)
{
    pthread_t send_msg_thread;
    if (pthread_create(&send_msg_thread, NULL, send_msg_handler, NULL) != 0)
    {
        cout << "ERROR: pthread\n";
    }

    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, recv_msg_handler, NULL) != 0)
    {
        cout << "ERROR: pthread\n";
    }
}

void Client::chatSelection(int sockfd)
{
    system("clear");
    cout << "\033[1;35m  CHAT OPTIONS  \033[0m\n\n";
    cout << "\033[;34mEnter 1 to single chat \033[0m\n";
    cout << "\033[;34mEnter 2 to pool chat \033[0m\n";

    int choice;
    cout << "\033[;34mEnter your choice \033[0m\n";
    cin >> choice;
    cout << endl;
    switch (choice)
    {
    case 1:
        send(sockfd, "1", 1, 0);
        cout << "\033[;34mEnter name of the person you want to chat \033[0m\n";
        cin >> name;
        // send(sockfd, name, strlen(name), 0);
        messageHandler(sockfd);
        break;
    case 2:
        send(sockfd, "2", 1, 0);
        system("clear");
        cout << "\033[1;34m WELCOME TO GROUP CHAT \033[0m\n";
        messageHandler(sockfd);
        break;
    default:
        cout << "\033[;34m Invalid Option \033[0m\n";
        break;
    }
}

void Client::clientLogin(int sockfd)
{
    system("clear");
    cout << "\033[1;35m  LOGIN PAGE  \033[0m\n\n";
    send(sockfd, "2", 1, 0);
    char message[1];

    cout << "\033[;34mPlease Enter Your Name :   \033[0m\n";
    cin >> name;
    str_trim_lf(name, strlen(name));

    if (strlen(name) > 32 || strlen(name) < 2)
    {
        cout << "\033[;34m Name must be more than two characters and less than 32 characters \033[0m\n";
        catch_ctrl_c_and_exit(2);
    }

    cout << "\033[;34mPlease Enter Your Password :   \033[0m\n";
    passwordPrinting();

    if (strlen(password) > 10 || strlen(password) < 5)
    {
        cout << "\033[;34m Password must be less than 10 and more than 5 characters   \033[0m\n";
        catch_ctrl_c_and_exit(2);
    }

    send(sockfd, name, LENGTH_NAME, 0);
    send(sockfd, password, LENGTH_NAME, 0);
    recv(sockfd, message, 1, 0);

    if (strcmp(message, "1") == 0)
    {
        cout << "\033[;33m \n\nLogin Successful   \033[0m\n";
        sleep(1);
        chatSelection(sockfd);
    }
    else
    {
        cout << "\033[;34m Username or password is incorrect.. Enter again!!!   \033[0m\n";
        sleep(5);
        clientLogin(sockfd);
    }
}

void Client::clientRegister(int sockfd)
{
    system("clear");
    cout << "\033[1;35m  REGISTRATION PAGE  \033[0m\n\n";
    char message[1];
    send(sockfd, "1", 1, 0);

    cout << "\033[;34mPlease Enter Your Name :   \033[0m\n";
    cin >> name;
    str_trim_lf(name, strlen(name));

    if (strlen(name) > 32 || strlen(name) < 2)
    {
        cout << "\033[;34m Name must be more than 2 and less than 32 characters   \033[0m\n";
        catch_ctrl_c_and_exit(2);
    }

    cout << "\033[;34mPlease Create your Password :   \033[0m\n";
    passwordPrinting();

    if (strlen(password) > 10 || strlen(password) < 5)
    {
        cout << "\033[;34m Password must be less than 10 and more than 5 characters \033[0m\n";
        catch_ctrl_c_and_exit(2);
    }

    send(sockfd, name, LENGTH_NAME, 0);
    send(sockfd, password, LENGTH_NAME, 0);
    recv(sockfd, message, 50, 0);

    if (strcmp(message, "1") == 0)
    {
        cout << "\033[;33m \n\nRegistration Successful   \033[0m\n";
        sleep(5);
    }
    else
    {
        cout << "\033[;31m \n\nUnsuccessful Registration \033[0m\n";
        sleep(5);
    }

    clientSelection();
}

void Client::clientSelection()
{
    system("clear");
    cout << "\033[1;35m  DASHBOARD  \033[0m\n\n";

    cout << "\033[;34mEnter 1 to register   \033[0m\n\n";
    cout << "\033[;34mEnter 2 to login   \033[0m\n\n";

    int choice;
    cout << "\033[;34mEnter your choice \033[0m\n";
    cin >> choice;
    cout << endl;
    switch (choice)
    {
    case 1:
        clientRegister(sockfd);
        break;
    case 2:
        clientLogin(sockfd);
        break;
    default:
        cout << "\033[;34m Invalid Option \033[0m\n";
        sleep(5);
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
        if (flag)
        {
            cout << "\nBye\n";
            break;
        }

    close(sockfd);

    return EXIT_SUCCESS;
}
