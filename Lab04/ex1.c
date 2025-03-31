#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char*argv[]) {
    int arg = atoi(argv[1]);
    pid_t child_pit;

    for(int i=0; i<arg; i++) {
        child_pit = fork();

        if(child_pit == 0) {
            printf("PID procesu: %d\n", getpid());
            printf("PID procesu macierzystego: %d\n", getppid());
            return 0;
        }
    }
    int status;
    while(waitpid(child_pit, &status, 0) != -1);
    printf("%d\n", arg);
    
    return 0;
}