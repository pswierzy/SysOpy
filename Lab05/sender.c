#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

volatile int confirmed = 0;

void confirm_handler(int sig) {
    confirmed = 1;
}

int main(int argc, char *argv[]) {
    pid_t catcher_pid = atoi(argv[1]);
    int mode = atoi(argv[2]);

    if (mode < 1 || mode > 5) {
        fprintf(stderr, "Nieprawidłowy tryb: %d\n", mode);
        return 1;
    }

    struct sigaction act;
    act.sa_handler = confirm_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGUSR1, &act, NULL);

    union sigval value;
    value.sival_int = mode;

    sigqueue(catcher_pid, SIGUSR1, value);
    
    sigset_t mask;
    sigemptyset(&mask);
    while (confirmed == 0) {
        sigsuspend(&mask);
    }

    printf("Otrzymano potwierdzenie od catchera.\n");


    return 0;
}
