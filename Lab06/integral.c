#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

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

int main(int argc, char *argv[]) {

    double h = atof(argv[1]);
    int max_k = atoi(argv[2]);
    
    for(int k = 1; k<= max_k; k++){
        int pipes[k][2];
        pid_t pids[k];

        clock_t start_time = clock();

        for(int i = 0; i < k; i++) {
            pipe(pipes[i]);
            
            pids[i] = fork();

            if(pids[i] == 0) {
                close(pipes[i][0]);
                
                double a = (double) i/k;
                double b = (double) (i+1)/k;
                double result = rect_area(a, b, h);
                write(pipes[i][1], &result, sizeof(result));
                close(pipes[i][1]);
                exit(0);
            } else {
                close(pipes[i][1]);
            }
        }

        double total_res = 0.0;

        for(int i=0; i<k; i++) {
            waitpid(pids[i], NULL, 0);
            double res;
            read(pipes[i][0], &res, sizeof(res));
            total_res += res;

            close(pipes[i][0]);
        }

        clock_t end_time = clock();
        double time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

        printf("Dla k = %d, wynik = %.15f, czas = %0.10f s\n", k, total_res, time);
    }
    
    return 0;
}