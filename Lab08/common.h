#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define QUEUE_SIZE 10
#define TASK_LEN 10

#define SEM_MUTEX 0
#define SEM_EMPTY 1
#define SEM_FULL  2

typedef struct {
    char data[TASK_LEN + 1];
} PrintTask;

typedef struct {
    PrintTask queue[QUEUE_SIZE];
    int front;
    int rear;
} SharedQueue;

key_t get_ipc_key() {
    return ftok("common.h", 'P');
}

void sem_op(int semid, int semnum, int op) {
    struct sembuf sb = { semnum, op, 0 };
    semop(semid, &sb, 1);
}

#endif