#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_NAME 32
#define BUFFER_SIZE 1024

int sock;
char name[MAX_NAME];

void sigint_handle(int sig) {
    write(sock, "STOP", 4);
    exit(0);
}

void* receive_thread(void* arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        int bytes = read(sock, buffer, BUFFER_SIZE-1);
        if (bytes <= 0) break;
        buffer[bytes] = '\0';
        printf("%s\n", buffer);
    }
    return NULL;
}

void* alive_thread(void* arg) {
    while (1) {
        sleep(5);
        write(sock, "ALIVE", 5);
    }
    return NULL;
}

int main(int arc, char* argv[]) {
    strncpy(name, argv[1], MAX_NAME);
    char* ip = argv[2];
    int port = atoi(argv[3]);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        perror("connect");
        return 1;
    }

    write(sock, name, strlen(name));

    signal(SIGINT, sigint_handle);

    pthread_t recv_thread, al_thread;
    pthread_create(&recv_thread, NULL, receive_thread, NULL);
    pthread_create(&al_thread, NULL, alive_thread, NULL);

    char buffer[BUFFER_SIZE];
    while(fgets(buffer, BUFFER_SIZE, stdin)) {
        write(sock, buffer, strlen(buffer));
    }
}