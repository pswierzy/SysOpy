#include "zad2.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void mask()
{
    /*  Zamaskuj sygnał SIGUSR2, tak aby nie docierał on do procesu */

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR2);

    if(sigprocmask(SIG_SETMASK, &mask, NULL)) {
        perror("Błąd ustawienia maski!");
        exit(-1);
    }

    check_mask();
}

void process()
{
    /*  Stworz nowy process potomny i uruchom w nim program ./check_fork
        W procesie macierzystym:
            1. poczekaj 1 sekundę
            2. wyślij SIGUSR1 do procesu potomnego
            3. poczekaj na zakończenie procesu potomnego */

    pid_t pid = fork();

    if(pid == 0) {
        execl("./check_fork", NULL);

        perror("Błąd execl w procesie dziecko!");
        exit(-1);
    } else {
        sleep(1);
        kill(pid, SIGUSR1);

        waitpid(pid, NULL, NULL);
    }
}

int main()
{
    mask();
    process();

    return 0;
}