#include "zad3.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void readwrite(int pd, size_t block_size);

void createpipe(size_t block_size)
{
    /* Utwórz potok nienazwany */
    int fd[2];
    pipe(fd);

    /* Odkomentuj poniższe funkcje zamieniając ... na deskryptory potoku */
    check_pipe(fd);
    check_write(fd, block_size, readwrite);
}

void readwrite(int write_pd, size_t block_size)
{
    /* Otworz plik `unix.txt`, czytaj go po `block_size` bajtów
    i w takich `block_size` bajtowych kawałkach pisz do potoku `write_pd`.*/
    /* Zamknij plik */

    FILE* file = fopen("unix.txt", "r");
    if(!file) {
        perror("Błąd otwierania pliku!");
        exit(-1);
    }

    char str[block_size];

    int bytes_read;
    while((bytes_read = fread(str, sizeof(char), block_size, file)) > 0) {
        write(write_pd, str, sizeof(char)*bytes_read);
    }

    fclose(file);
}

int main()
{
    srand(42);
    size_t block_size = rand() % 128;
    createpipe(block_size);

    return 0;
}