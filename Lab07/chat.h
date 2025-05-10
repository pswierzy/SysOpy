#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define SERVER_QUEUE_NAME "/server_queue"
#define MAX_USERS 2
#define MAX_MSG_LENGTH 1024
#define MAX_QUEUE_NAME_LENGHT 64

typedef enum {
    MSG_INIT,
    MSG_TEXT,
    MSG_STOP,
    MSG_ERR
} message_type;

typedef struct {
    message_type type;
    int user_id;
    char text[MAX_MSG_LENGTH];
} message;

typedef struct {
    int user_id;
    mqd_t queue;
    char queue_name[MAX_QUEUE_NAME_LENGHT];
} user;