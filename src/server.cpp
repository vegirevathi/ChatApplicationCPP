#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "string.h"
#include "clientsData.h"
#include "server.h"

#include <list>

#define MAX_CLIENTS 100
#define BUFFER_SZ 2048

static unsigned int cli_count = 0;
static int uid = 10;
static list<char *> clientNames;

mongocxx::database db = client["Chat_data"];
auto clients_db = db["clients"];

/* Send message to all clients except sender */
void send_message(char *s, int uid)
{
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients[i])
        {
            if (clients[i]->uid != uid)
            {
                if (write(clients[i]->sockfd, s, strlen(s)) < 0)
                {
                    perror("ERROR: write to descriptor failed");
                    break;
                }
            }
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

bool check_name(char *name)
{
    bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result = clients_db.find_one(document{} << "name" << name << finalize);
    if (maybe_result)
    {
        std::cout << bsoncxx::to_json(*maybe_result) << "\n";
        return true;
    }
    return false;
}

bool authenticate(char *name, char *password)
{
    bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result = clients_db.find_one(document{}
                                                                                         << "name" << name
                                                                                         << "password" << password
                                                                                         << finalize);
    cout << "In Auth" << endl;
    cout << name << "..." << password << endl;
    if (maybe_result)
    {
        cout << "ret tr";
        std::cout << bsoncxx::to_json(*maybe_result) << "\n";
        return true;
    }
    return false;
}

bool check_exist(client_t *cli)
{
    cout << "Exi check";
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients[i])
        {
            if (clients[i]->uid != cli->uid)
            {
                if (strcmp(cli->name, clients[i]->name) == 0)
                {
                    cout << "Exi check";
                    return true;
                }
            }
        }
    }
    return false;
}

void register_client(int sockfd)
{
    char name[32];
    char password[20];

    cout << "Registration" << endl;

    if ((recv(sockfd, name, 32, 0) <= 0 || strlen(name) < 2 || strlen(name) >= 32 - 1) ||
        (recv(sockfd, password, 32, 0) <= 0 || strlen(password) < 2 || strlen(password) >= 32 - 1))
    {
        printf("Didn't enter the name or password.\n");
    }
    
    else if (check_name(name))
    {
        cout << "Cant Register Existing Client";
        write(sockfd, "Client Already Exists", strlen("Client Already Exists"));
    }
    else
    {
        cout << strlen(password) << ", " << strlen(name) << endl;
        auto builder = bsoncxx::builder::stream::document{};
        bsoncxx::document::value doc_value = builder
                                             << "name" << name
                                             << "password" << password
                                             << "status"
                                             << "ofline"
                                             << finalize;
        clients_db.insert_one(doc_value.view());
        write(sockfd, "Client Registered Successfully!!!", strlen("Client Registered Successfully!!!"));
        cout << "Client added:" << name;
    }
}

void login_client(client_t *cli)
{
    char name[32];
    char password[20];
    char buff_out[BUFFER_SZ];
    int leave_flag = 0;

    cout << "Registration" << endl;

    if ((recv(cli->sockfd, name, 32, 0) <= 0 || strlen(name) < 2 || strlen(name) >= 32 - 1) ||
        (recv(cli->sockfd, password, 32, 0) <= 0 || strlen(password) < 2 || strlen(password) >= 32 - 1))
    {
        printf("Didn't enter the name.\n");
        leave_flag = 1;
    }
    else
    {
        cout << name << ", " << password << endl;
        if (authenticate(name, password))
        {
            cout << "Auth Succ" << endl;

            strcpy(cli->name, name);
            if (check_exist(cli))
            {
                write(cli->sockfd, "Client Exists", strlen("Client Exists"));
                leave_flag = 1;
            }
            else
            {
                sprintf(buff_out, "%s has joined\n", cli->name);
                printf("%s", buff_out);
                send_message(buff_out, cli->uid);
            }
        }
        else
        {
            cout << "No cli" << endl;

            write(cli->sockfd, "No Such Client", strlen("No Such Client"));
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
                send_message(buff_out, cli->uid);

                str_trim_lf(buff_out, strlen(buff_out));
                printf("%s -> %s\n", buff_out, cli->name);
            }
        }
        else if (receive == 0 || strcmp(buff_out, "exit") == 0)
        {
            sprintf(buff_out, "%s has left\n", cli->name);
            printf("%s", buff_out);
            send_message(buff_out, cli->uid);
            leave_flag = 1;
        }
        else
        {
            printf("ERROR: -1\n");
            leave_flag = 1;
        }

        bzero(buff_out, BUFFER_SZ);
    }

    cout << "end" << endl;
}

/* Handle all communication with the client */
void *handle_client(void *arg)
{
    char buff_out[BUFFER_SZ];
    char name[32];
    int leave_flag = 0;
    char option[1];

    cli_count++;
    client_t *cli = (client_t *)arg;
    if (recv(cli->sockfd, option, 1, 0) <= 0)
    {
        printf("Invalid.\n");
    }

    cout << option;

    if (strcmp(option, "1") == 0)
    {
        register_client(cli->sockfd);
    }

    else if (strcmp(option, "2") == 0)
    {
        login_client(cli);
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

int main(int argc, char **argv)
{
    signal(SIGPIPE, SIG_IGN);

    Server server;

    server.creatingSocket();
    server.settingSocket();
    server.binding();
    server.listening();

    printf("=== WELCOME TO THE CHATROOM ===\n");

    while (1)
    {

        int connfd = server.accepting();

        /* Client settings */
        client_t *cli = (client_t *)malloc(sizeof(client_t));
        cli->sockfd = connfd;
        cli->uid = uid++;

        /* Add client to the array */
        array_add(cli);

        pthread_t tid;
        pthread_create(&tid, NULL, &handle_client, (void *)cli);

        /* Reduce CPU usage */
        sleep(1);
    }

    return EXIT_SUCCESS;
}
