#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

char* flipped(char* buffer) {
    size_t len = strlen(buffer);
    char* flipped = malloc(len + 1);
    if(flipped == NULL) return NULL;

    for(int i=0; i<len; i++) {
        flipped[i] = buffer[len-i-1];
    }
    flipped[len] = '\n';
    return flipped;
}

int main() {
    char dir_name[256];
    printf("Podaj nazwę katalogu: ");
    scanf("%s", dir_name);
    struct dirent *entry;
    DIR *dir = opendir(dir_name);

    if (dir == NULL) {
        perror("Nie można otworzyć katalogu");
        return -1;    
    }
    while((entry=readdir(dir)) != NULL) {
        if(strstr(entry->d_name, ".txt") != NULL) {
            char file_path[512];
            snprintf(file_path, sizeof(file_path), "%s/%s", dir_name, entry->d_name);
            FILE* file = fopen(file_path, "r");
            if(file == NULL) {
                printf("Plik: %s\n", file_path);
                perror("Nie można otworzyć pliku!");
                return -1;
            }
            FILE* new_file = fopen("temp.txt", "w");
            if(new_file == NULL) {
                perror("Nie można utworzyć pliku temp.txt!");
                return -1;
            }
            
            char buffer[256];
            while(fgets(buffer, sizeof(buffer), file) != NULL) {
                buffer[strcspn(buffer, "\n")] = '\0';
                char* flipped_line = flipped(buffer);
                if(flipped_line != NULL) {
                    fwrite(flipped_line, sizeof(char), strlen(buffer) + 1, new_file);
                    free(flipped_line);
                }
            }

            fclose(file);
            fclose(new_file);

            remove(file_path);
            rename("temp.txt", file_path);
        }
    }
    return 0;
}