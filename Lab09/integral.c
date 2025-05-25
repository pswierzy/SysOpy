#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

double f(double x) {
    return 4.0 / (x * x + 1.0);
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

typedef struct {
    int id;
    double start;
    double end;
    double dx;
    double* results;
    int* ready;
} ThreadData;

void* thread_func(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    double sum = rect_area(data->start, data->end, data->dx);

    data->results[data->id] = sum;
    data->ready[data->id] = 1;
    return NULL;
}

int main(int argc, char* argv[]) {

    double dx = atof(argv[1]);
    int num_threads = atoi(argv[2]);

    double start = 0.0;
    double end = 1.0;
    double range = end - start;
    double chunk = range / num_threads;

    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    ThreadData* thread_data = malloc(num_threads * sizeof(ThreadData));
    double* results = calloc(num_threads, sizeof(double));
    int* ready = calloc(num_threads, sizeof(int));

    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].id = i;
        thread_data[i].start = start + i * chunk;
        thread_data[i].end = thread_data[i].start + chunk;
        thread_data[i].dx = dx;
        thread_data[i].results = results;
        thread_data[i].ready = ready;

        pthread_create(&threads[i], NULL, thread_func, &thread_data[i]);
    }

    int flag = 1;
    while(flag) {
        flag = 0;
        for(int i=0; i<num_threads; i++) {
            if(ready[i] == 0) 
                flag = 1;
        }
    }

    double total = 0.0;
    for (int i = 0; i < num_threads; ++i) {
        if (ready[i]) {
            total += results[i];
        }
    }

    printf("Wynik całki: %.10f\n", total);

    free(threads);
    free(thread_data);
    free(results);
    free(ready);

    return 0;
}
