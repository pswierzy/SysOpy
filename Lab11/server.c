#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <time.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define MAX_NAME 32
#define BUFFER_SIZE 1024

typedef struct {
    int socket;
    char name[MAX_NAME];
    time_t last_alive;
} Client;

Client clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast(const char* msg, int exclude_sock) {
    pthread_mutex_lock(&clients_mutex);
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i].socket != -1 && clients[i].socket != exclude_sock) {
            write(clients[i].socket, msg, strlen(msg));
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void list_clients(int sock) {
    char msg[BUFFER_SIZE] = "Active clients:\n";
    pthread_mutex_lock(&clients_mutex);
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i].socket != -1) {
            strcat(msg, clients[i].name);
            strcat(msg, "\n");
        }
    }
    write(sock, msg, strlen(msg));
    pthread_mutex_unlock(&clients_mutex);
}

void remove_client(int sock) {
    pthread_mutex_lock(&clients_mutex);
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i].socket == sock) {
            close(sock);
            clients[i].socket = -1;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void* handle_client(void* arg) {
    int sock = *(int*)arg;
    char buffer[BUFFER_SIZE];
    char name[MAX_NAME];
    memset(name, 0, MAX_NAME);
    read(sock, name, MAX_NAME);

    pthread_mutex_lock(&clients_mutex);
    int idx = -1;
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i].socket == -1) {
            clients[i].socket = sock;
            strncpy(clients[i].name, name, MAX_NAME);
            clients[i].last_alive = time(NULL);
            idx = i;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    if (idx == -1) {
        char* msg = "Server full\n";
        write(sock, msg, strlen(msg));
        close(sock);
        return NULL;
    }

    while(1) {
        int bytes = read(sock, buffer, BUFFER_SIZE - 1);
        if(bytes <= 0) {
            break;
        }
        buffer[bytes] = '\0';

        if(strncmp(buffer, "LIST", 4) == 0) {
            list_clients(sock);
        } else if(strncmp(buffer, "2ALL ", 5) == 0) {
            char msg[BUFFER_SIZE];
            time_t now = time(NULL);

            char time_str[26];
            ctime_r(&now, time_str);
            time_str[24] = '\0';

            snprintf(msg, BUFFER_SIZE, "[%s | %s] %s", clients[idx].name, time_str, strtok(buffer + 5, "\n"));
            broadcast(msg, sock);
        } else if(strncmp(buffer, "2ONE ", 5) == 0) {
            char* target = strtok(buffer + 5, " ");
            char* msg_text = strtok(NULL, "\n");

            if(!target || !msg_text) continue;

            pthread_mutex_lock(&clients_mutex);
            for(int i = 0; i < MAX_CLIENTS; i++) {
                if(clients[i].socket != -1 && strcmp(clients[i].name, target) == 0) {
                    char msg[BUFFER_SIZE];
                    time_t now = time(NULL);

                    char time_str[26];
                    ctime_r(&now, time_str);
                    time_str[24] = '\0';

                    snprintf(msg, BUFFER_SIZE, "[%s | %s] %s", clients[idx].name, time_str, msg_text);
                    write(clients[i].socket, msg, strlen(msg));
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);
        } else if(strncmp(buffer, "STOP", 4) == 0) {
            close(clients[idx].socket);
            clients[idx].socket = -1;
            break;
        } else if(strncmp(buffer, "ALIVE", 5) == 0) {
            pthread_mutex_lock(&clients_mutex);
            clients[idx].last_alive = time(NULL);
            pthread_mutex_unlock(&clients_mutex);
        }
    }
    return NULL;
}

void* alive_checker(void* arg) {
    while(1) {
        sleep(10);
        time_t now = time(NULL);
        pthread_mutex_lock(&clients_mutex);
        for(int i = 0; i < MAX_CLIENTS; i++) {
            if(clients[i].socket != -1 && now - clients[i].last_alive > 15) {
                printf("Client %s timed out\n", clients[i].name);
                close(clients[i].socket);
                clients[i].socket = -1;
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    
    int port = atoi(argv[2]);
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, MAX_CLIENTS);

    for(int i = 0; i < MAX_CLIENTS; i++) clients[i].socket = -1;

    pthread_t checker_thread;
    pthread_create(&checker_thread, NULL, alive_checker, NULL);

    printf("Server listening on %s:%s\n", argv[1], argv[2]);

    while(1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, &client_sock);
        pthread_detach(tid);
    }

    return 0;
}