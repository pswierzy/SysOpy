#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main() {
    
    void* handle = dlopen("libcollatz.so", RTLD_LAZY);
    if(handle == NULL) {
        printf("Błąd z ładowaniem biblioteki!\n");
        return 0;
    }
    int (*lib_fun)(int, int, int**);
    lib_fun = (int (*)())dlsym(handle,"test_collatz_convergence");
    if(dlerror() != NULL){
        printf("Błąd z ładowaniem funkcji!\n");
        return 0;
    }

    int input;
    printf("Podaj liczbę początkową!\n");
    scanf("%d", &input);
    int max_iter;
    printf("Podaj maksymalną liczbę iteracji!\n");
    scanf("%d", &max_iter);

    int* steps = malloc(sizeof(int));
    int amount = (*lib_fun)(input, max_iter, &steps);
    if(amount == 0) {
        printf("Nie udało się dostać do 1 w %d krokach!\n", max_iter);
    } else {
        printf("%d\n", amount);
        for(int i=0; i<amount-1 ;i++){
            printf("%d, ", steps[i]);
        }printf("%d\n", steps[amount-1]);
    }

    dlclose(handle);
    free(steps);
    return 0;
}