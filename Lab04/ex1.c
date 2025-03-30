#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    int argv;
    scanf("%d", &argv);
    pid_t child_pit;

    for(int i=0; i<argv; i++) {
        child_pit = fork();

        if(child_pit == 0) {
            printf("PID procesu: %d\n", getpid());
            printf("PID procesu macierzystego: %d\n", getppid());
            return 0;
        }
    }
    int status;
    waitpid(child_pit, &status, 0);
    printf("%d\n", argv);
    
    return 0;
}