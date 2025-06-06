#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


double f(double x) {
    return 4.0/(x*x + 1.0);
}

double rect_area(double start, double end, double width) {
    double sum = 0.0;
    double x = start;
    for(; x+width<end; x+=width) {
        sum += f(x)*width;
    }
    sum += f(x+width)*(end - x);
    return sum;
}

int main() {
    double a, b;

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
    int to_fd = open("/tmp/przedzial", O_RDONLY);
    
    read(to_fd, &a, sizeof(a));
    read(to_fd, &b, sizeof(b));
    close(to_fd);

    double h = 0.000000001;
    double res = rect_area(a, b, h);

    int from_fd = open("/tmp/wynik", O_WRONLY);
    write(from_fd, &res, sizeof(res));
    close(from_fd);
    
    return 0;
}