#include "chat.h"

int running = 1;
int user_id = -1;
mqd_t server_q;
mqd_t user_q;
char user_queue_name[MAX_QUEUE_NAME_LENGHT];

void cleanup() {
    message stop_msg = { .type = MSG_STOP, .user_id = user_id };
    mq_send(server_q, (char*)&stop_msg, sizeof(stop_msg), 0);
    mq_close(server_q);
    mq_close(user_q);
    mq_unlink(user_queue_name);
}

void handle_stop() {
    running = 0;
    cleanup();
    exit(0);
}

int main() {

    snprintf(user_queue_name, MAX_QUEUE_NAME_LENGHT, "/user_%d", getpid());
    user_id = getpid();

    struct mq_attr attr = { .mq_flags = 0, .mq_maxmsg = 10, .mq_msgsize = sizeof(message), .mq_curmsgs = 0 };
    user_q = mq_open(user_queue_name, O_CREAT | O_RDONLY, 0666, &attr);
    if (user_q == -1) {
        perror("mq_open (user)");
        exit(-1);
    }

    server_q = mq_open(SERVER_QUEUE_NAME, O_WRONLY);
    if (server_q == -1) {
        perror("mq_open (server)");
        exit(-1);
    }

    message reply;
    while(reply.type != MSG_INIT) {
        message init = { .type = MSG_INIT, .user_id = user_id };
        strncpy(init.text, user_queue_name, MAX_QUEUE_NAME_LENGHT);
        mq_send(server_q, (char*)&init, sizeof(init), 0);

        mq_receive(user_q, (char*)&reply, sizeof(reply), NULL);

        if (reply.type == MSG_ERR) {
            printf("Błąd w połączeniu do serwera: %s\n", reply.text);
            sleep(1);
        }
    }

    printf("Połączono z serwerem! Twoje ID to %d\n", reply.user_id);

    pid_t child_pid = fork();

    if (child_pid == 0){
        // odbieracz
        message msg;
        while(running) {
            if (mq_receive(user_q, (char*)&msg, sizeof(msg), NULL) == -1) {
                perror("mq_receive");
                continue;
            }
            
            if (msg.type == MSG_TEXT) {
                if (msg.user_id == -1) {
                    printf("[Serwer]: %s\n", msg.text);
                } else {
                    printf("[User %d]: %s\n", msg.user_id, msg.text);
                }
            } else if (msg.type == MSG_STOP) {
                printf("Rozłączono z serwerem!\n");

                handle_stop();
            }
        }
    } else {
        //wysyłacz
        signal(SIGINT, handle_stop);

        char text[MAX_MSG_LENGTH];
        while(running) {
            if(fgets(text, MAX_MSG_LENGTH, stdin)) {

                text[strcspn(text, "\n")] = '\0';
                if (strlen(text) == 0) continue;

                message msg = { .type = MSG_TEXT, .user_id = user_id };
                strncpy(msg.text, text, MAX_MSG_LENGTH);
                mq_send(server_q, (char*)&msg, sizeof(msg), 0);
            }
        }
    }
    return 0;
}