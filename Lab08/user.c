#include "common.h"

int main() {
    srand(getpid());
    key_t key = get_ipc_key();

    int shmid = shmget(key, sizeof(SharedQueue), 0666);
    SharedQueue* queue = (SharedQueue*)shmat(shmid, NULL, 0);

    int semid = semget(key, 3, 0666);   

    while(1) {
        PrintTask task;
        for(int i=0; i<TASK_LEN; i++) {
            task.data[i] = 'a' + rand() % 26;
        }
        task.data[TASK_LEN] = '\0';

        sem_op(semid, SEM_EMPTY, -1);
        sem_op(semid, SEM_MUTEX, -1);

        queue -> queue[queue->rear] = task;
        queue -> rear = (queue -> rear + 1) % QUEUE_SIZE;

        printf("[User %d] Wysłano zadanie: %s\n", getpid(), task.data);

        sem_op(semid, SEM_MUTEX, 1);
        sem_op(semid, SEM_FULL, 1);

        sleep(rand() % 20 + 1);
    }
    return 0;
}