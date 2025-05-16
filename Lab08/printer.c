#include "common.h"

int main() {
    key_t key = get_ipc_key();
    
    int shmid = (SharedQueue*)shmget(key, sizeof(SharedQueue), 0666);
    SharedQueue* queue = (SharedQueue*)shmat(shmid, NULL, 0);

    int semid = semget(key, 3, 0666);

    while(1){
        sem_op(semid, SEM_FULL, -1);
        sem_op(semid, SEM_MUTEX, -1);

        PrintTask task = queue -> queue[queue->front];
        queue -> front = (queue -> front + 1) % QUEUE_SIZE;

        printf("%s", task.data);

        sem_op(semid, SEM_MUTEX, 1);
        sem_op(semid, SEM_EMPTY, 1);

        printf("[Printer %d] Drukuję: ", getpid());
        fflush(stdout);
        for (int i = 0; i < TASK_LEN; i++) {
            printf("%c", task.data[i]);
            fflush(stdout);
            sleep(1);
        }
        printf("\n");
    }

    return 0;
}