#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main() {
    
    double a, b;
    printf("Podaj przedział całkowania (a b): ");
    scanf("%lf %lf", &a, &b);

    if (access("/tmp/przedzial", F_OK) == -1){
        if(mkfifo("/tmp/przedzial", 0666) == -1) {
            perror("mkfifo");
            exit(-1);
        }
    }
    if (access("/tmp/wynik", F_OK) == -1){
        if(mkfifo("/tmp/wynik", 0666) == -1) {
            perror("mkfifo");
            exit(-1);
        }
    }

    int to_fd = open("/tmp/przedzial", O_WRONLY);
    write(to_fd, &a, sizeof(a));
    write(to_fd, &b, sizeof(b));
    close(to_fd);

    double res;
    int from_fd = open("/tmp/wynik", O_RDONLY);
    read(from_fd, &res, sizeof(res));
    close(from_fd);

    printf("Wynik całki na przedziale [%.2lf, %.2lf] to %.15f\n", a, b, res);

    return 0;
}