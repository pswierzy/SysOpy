#include <stdio.h>
#include <stdlib.h>
#include "collatz.h"

int main() {
    
    int input;
    printf("Podaj liczbę początkową!\n");
    scanf("%d", &input);
    int max_iter;
    printf("Podaj maksymalną liczbę iteracji!\n");
    scanf("%d", &max_iter);
    
    int* steps = malloc(sizeof(int));
    int amount = test_collatz_convergence(input, max_iter, &steps);

    if(amount == 0) {
        printf("Nie udało się dostać do 1 w %d krokach!\n", max_iter);
    } else {
        printf("%d\n", amount);
        for(int i=0; i<amount-1 ;i++){
            printf("%d, ", steps[i]);
        }printf("%d\n", steps[amount-1]);
    }

    free(steps);
    return 0;
}