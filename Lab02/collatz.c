#include <stdio.h>
#include <stdlib.h>

int collatz_conjecture(int input) {
    if (input%2 == 0) return input / 2;
    return 3 * input + 1;
}

int test_collatz_convergence(int input, int max_iter, int **steps){
    int size = 0;
    int capacity = 1;
    for(int i = 0; i < max_iter; i++) {
        if (size == capacity) {
            capacity *= 2;
            int* temp = realloc(*steps, capacity * sizeof(int));
            if (temp == NULL) {
                printf("Błąd alokacji pamięci!\n");
                free(*steps);
                return 0;
            }
            *steps = temp;
        }
        (*steps)[size++] = input;
        
        if(input == 1) return size;
        
        input = collatz_conjecture(input);
    }
    return 0;
}