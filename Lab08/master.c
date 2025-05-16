#include <signal.h>
#include "common.h"

int shmid = -1;
int semid = -1;
SharedQueue *queue = NULL;

void cleanup() {
    printf("Czyszczenie zasobów ...\n");

    if (queue != NULL) {
        shmdt(queue);
    }

    if (shmid != -1) {
        shmctl(shmid, IPC_RMID, NULL);
        printf("[MASTER] Usunięto pamięć współdzieloną (shmid=%d)\n", shmid);
    }

    if (semid != -1) {
        semctl(semid, 0, IPC_RMID);
        printf("[MASTER] Usunięto semafory (semid=%d)\n", semid);
    }

    exit(0);
}

int main() {
    signal(SIGINT, cleanup);

    key_t key = get_ipc_key();

    shmid = shmget(key, sizeof(SharedQueue), 0666 | IPC_CREAT);
    if (shmid == -1) { perror("shmget"); exit(-1); }

    queue = (SharedQueue*)shmat(shmid, NULL, 0);
    queue->front = queue->rear = 0;

    semid = semget(key, 3, 0666 | IPC_CREAT);
    if (shmid == -1) { perror("semget"); exit(-1); }

    if (semctl(semid, SEM_MUTEX, SETVAL, 1) == -1) { perror("semctl MUTEX"); exit(1); }
    if (semctl(semid, SEM_EMPTY, SETVAL, QUEUE_SIZE) == -1) { perror("semctl EMPTY"); exit(1); }
    if (semctl(semid, SEM_FULL, SETVAL, 0) == -1) { perror("semctl FULL"); exit(1); }


    printf("System printowania gotowy!\n");
    printf("Crtl+C aby wyłączyć\n");

    while(1) pause();

    return 0;
}