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

    mkfifo("przedial_a_b", 0666);
    mkfifo("wynik_calki", 0666);

    int to_fd = open("przedial_a_b", O_WRONLY);
    write(to_fd, &a, sizeof(a));
    write(to_fd, &b, sizeof(b));
    close(to_fd);

    double res;
    int from_fd = open("wynik_calki", O_RDONLY);
    read(from_fd, &res, sizeof(res));
    close(from_fd);

    printf("Wynik całki na przedziale [%.2lf, %.2lf] to %.15f\n", a, b, res);

    return 0;
}