#include "chat.h"

user users[MAX_USERS];
int user_count = 0;
int running = 1;


int main() {

    mq_unlink(SERVER_QUEUE_NAME);
    struct mq_attr attr = { .mq_flags = 0, .mq_maxmsg = 10, .mq_msgsize = sizeof(message), .mq_curmsgs = 0 };
    
    mqd_t server_queue = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_RDONLY, 0666, &attr);
    if (server_queue == -1) {
        perror("mq_open (server)");
        exit(-1);
    }
    printf("Serwer rozpoczęty!\n");

    message msg;
    while(running) {
        if (mq_receive(server_queue, (char*)&msg, sizeof(msg), NULL) == -1) {
            perror("mq_receive");
            continue;
        }

        if (msg.type == MSG_INIT) {
            mqd_t user_q = mq_open(msg.text, O_WRONLY);
            if (user_q == -1) {
                perror("mq_open (user)");
                continue;
            }

            int user_exists = 0;
            for (int i = 0; i < user_count; i++) {
                if (users[i].user_id == msg.user_id) {
                    user_exists = 1;
                    break;
                }
            }
        
            if (user_exists) {
                fprintf(stderr, "User %d już istnieje!\n", msg.user_id);
                message reply = { .type = MSG_ERR, .text = "User już połączony!" };
                mq_send(user_q, (char*)&reply, sizeof(reply), 0);
                mq_close(user_q);
                continue;
            }

            if (user_count >= MAX_USERS) {
                fprintf(stderr, "Za dużo userów!\n");

                message reply = { .type = MSG_ERR, .text = "Za dużo userów!" };
                mq_send(user_q, (char*)&reply, sizeof(reply), 0);

                mq_close(user_q);

                continue;
            }
            
            users[user_count].user_id = msg.user_id;
            users[user_count].queue = user_q;
            strncpy(users[user_count].queue_name, msg.text, MAX_QUEUE_NAME_LENGHT);

            message reply = { .type = MSG_INIT, .user_id = msg.user_id };
            mq_send(user_q, (char*)&reply, sizeof(reply), 0);

            printf("User %d połączony: %s\n", reply.user_id, users[user_count].queue_name);

            message serwer_msg = { .type = MSG_TEXT, .user_id = -1 };
            snprintf(serwer_msg.text, MAX_MSG_LENGTH, "User %d przybył na czat", msg.user_id);


            for (int i = 0; i<user_count; i++) {
                mq_send(users[i].queue, (char*)&serwer_msg, sizeof(serwer_msg), 0);
            }

            user_count++;
        }

        else if (msg.type == MSG_TEXT) {
            printf("Otrzymano wiadomość od %d: %s\n", msg.user_id, msg.text);
            for(int i = 0; i<user_count; i++) {
                if(users[i].user_id != msg.user_id) {
                    mq_send(users[i].queue, (char*)&msg, sizeof(msg), 0);
                }
            }
        }

        else if (msg.type == MSG_STOP) {
            int j = -1;
            message serwer_msg = { .type = MSG_TEXT, .user_id = -1 };
            snprintf(serwer_msg.text, MAX_MSG_LENGTH, "User %d opuścił czat", msg.user_id);

            for(int i = 0; i<user_count; i++) {
                mq_send(users[i].queue, (char*)&serwer_msg, sizeof(serwer_msg), 0);

                if(users[i].user_id == msg.user_id) {

                    mq_close(users[i].queue);
                    printf("Rozłączono usera %d\n", msg.user_id);
                    j = i;
                }
            }

            if (j != -1){
                for(; j<user_count-1; j++) {
                    users[j] = users[j+1];
                }
                user_count--;
            }
        }
    }

    mq_close(server_queue);
    mq_unlink(SERVER_QUEUE_NAME);
    return 0;
}