#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

volatile int change_count = 0;
volatile int mode;
pid_t sender_pid;

void ctrlc(int signum) {
    printf("Wciśnięto ctrl+c!\n");
}

void handle_signal(int signum, siginfo_t* info) {
    sender_pid = info -> si_pid;
    mode = info -> si_value.sival_int;
    change_count++;

    kill(sender_pid, SIGUSR1);
}


int main(int argc, char *argv[]) {
    
    printf("PID catchera to: %d\n", getpid());

    struct sigaction act;
    act.sa_handler = handle_signal;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);

        
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGINT);

    while(1) {
        mode = 0;

        sigsuspend(&mask);
        
        label:
        switch(mode){
            case 1:
                printf("Otrzymano %d żądań zmiany trybu pracy.\n", change_count);
                break;
            case 2:
                printf("Tryb 2: Liczenie co sekundę...\n");
                for (int i = 1;; ++i) {
                    printf("%d\n", i);
                    sleep(1);
                    if (mode != 2) break;
                }
                goto label;
            case 3:
                printf("Ustawiono ignorowanie CTRL+C\n");
                signal(SIGINT, SIG_IGN);
                break;
            case 4:
                printf("Ustawiono własną obsługę CTRL+C\n");
                signal(SIGINT, ctrlc);
                break;
            case 5:
                printf("Zakończono działanie programu catcher.\n");
                exit(0);
        }

    }

    return 0;
}
