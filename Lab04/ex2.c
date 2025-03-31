#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int global = 0;

int main(int argc, char*argv[]) {
    printf("%s\n", argv[0]);
    int local = 0;

    pid_t child_pit = fork();

    if(child_pit == 0) {
        printf("Child process\n");
        local++;
        global++;
        printf("child pid = %d, parent pid = %d\n", getpid(), getppid());
        printf("child's local = %d, child's global = %d\n", local, global);
        execl("/bin/ls","ls", argv[1]);
        perror("Błąd wywołania execl w child process!\n");
        exit(1);
    } else {        
        int status;
        wait(&status);
        printf("Parent process\n");
        printf("parent pid = %d, child pid = %d\n", getpid(), child_pit);
        printf("Child exit code: %d\n", WEXITSTATUS(status));
        printf("Parent's local = %d, parent's global = %d\n", local, global);
    }

    return 0;    
}