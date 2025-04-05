#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

void sigusr1_handler(int signum) {
    printf("Odebrano sygnał SIGUSR1 w handlerze!\n");
}

int main(int argc, char *argv[]) {

    char *option = argv[1];

    if (strcmp(option, "ignore") == 0) {
        // IGNORE
        signal(SIGUSR1, SIG_IGN);
        printf("Ustawiono ignorowanie SIGUSR1.\n");
    } else if (strcmp(option, "handler") == 0) {
        // HANDLER
        signal(SIGUSR1, sigusr1_handler);
        printf("Ustawiono handler dla SIGUSR1.\n");
    } else if (strcmp(option, "mask") == 0) {
        // MASK
        sigset_t sigset;
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGUSR1);
        if(sigprocmask(SIG_SETMASK, &sigset, NULL)) {
            perror("Nie udało się utworzyć maski sygnałów!");
            exit(1);
        }

        printf("Zamaskowano sygnał SIGUSR1.\n");

        raise(SIGUSR1);

        // Sprawdzamy, czy sygnał wisi
        sigset_t pending;
        sigpending(&pending);
        if (sigismember(&pending, SIGUSR1)) {
            printf("Sygnał SIGUSR1 jest oczekujący (wiszący).\n");
        } else {
            printf("Sygnał SIGUSR1 NIE jest oczekujący.\n");
        }

        return 0;
    } else if (strcmp(option, "none") != 0) {
        fprintf(stderr, "Nieznana opcja: %s\n", option);
        return 1;
    }

    // Wysyłamy sygnał do samego siebie
    printf("Wysyłam sygnał SIGUSR1 do siebie (raise)...\n");
    raise(SIGUSR1);

    // Chwila na przetworzenie sygnału
    sleep(1);

    printf("Koniec programu.\n");
    return 0;
}
