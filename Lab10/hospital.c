#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_WAITING_PATIENTS 10
#define MAX_MEDICINE 20
#define RAND_SEED 11

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_doctor = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_medicine = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_patient_waiting = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_pharma_wait = PTHREAD_COND_INITIALIZER;

int waiting_patients = 0;
int total_patients = 0;
int served_patients = 0;
int medicine_stock = MAX_MEDICINE;
int active_pharmacists = 0;
int pharmacists_waiting = 0;

int patient_ids[MAX_WAITING_PATIENTS];

void* patient_thread(void* arg) {
    int id = *(int*)arg;
    srand(RAND_SEED + id);

    int delay = 2 + rand() % 4;
    printf("[%ld] - Pacjent(%d): Ide do szpitala, bede za %d s\n", time(NULL), id, delay);
    sleep(delay);

    while(1) {
        pthread_mutex_lock(&mutex);

        if (waiting_patients >= MAX_WAITING_PATIENTS) {
            pthread_mutex_unlock(&mutex);
            int retry = 2 + rand() % 4;
            printf("[%ld] - Pacjent(%d): Za duzo pacjentow, wracam pozniej za %d s\n", time(NULL), id, retry);
            sleep(retry);
            continue;
        }

        break;
    }
    
    patient_ids[waiting_patients++] = id;
    printf("[%ld] - Pacjent(%d): Czeka %d pacjentow na lekarza\n", time(NULL), id, waiting_patients);

    if ((waiting_patients == MAX_WAITING_PATIENTS && medicine_stock >= MAX_WAITING_PATIENTS) || (total_patients - served_patients == waiting_patients && medicine_stock >= waiting_patients)) {
        printf("[%ld] - Pacjent(%d): Budzę lekarza\n", time(NULL), id);
        pthread_cond_signal(&cond_doctor);
    }
    
    pthread_cond_wait(&cond_patient_waiting, &mutex);

    printf("[%ld] - Pacjent(%d): Kończę wizytę\n", time(NULL), id);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

void* pharmacist_thread(void* arg) {
    int id = *(int*) arg;
    srand(RAND_SEED + id);

    int delay = 5 + rand() % 11;
    printf("[%ld] - Farmaceuta(%d): Idę do szpitala, będę za %ds\n", time(NULL), id, delay);
    sleep(delay);

    pthread_mutex_lock(&mutex);
    if(medicine_stock >= MAX_WAITING_PATIENTS) {
        printf("[%ld] - Farmaceuta(%d): Czekam na opróżnienie apteczki\n", time(NULL), id);
        pthread_cond_wait(&cond_medicine, &mutex);
    }
    pharmacists_waiting++;

    if(served_patients != total_patients) {
        printf("[%ld] - Farmaceuta(%d): budzę lekarza\n", time(NULL), id);
        pthread_cond_signal(&cond_doctor);
        pthread_cond_wait(&cond_pharma_wait, &mutex);

        printf("[%ld] - Farmaceuta(%d): dostarczam leki\n", time(NULL), id);
        medicine_stock = MAX_MEDICINE;
    }
    printf("[%ld] - Farmaceuta(%d): zakończyłem dostawe\n", time(NULL), id);
    pharmacists_waiting--;

    pthread_mutex_unlock(&mutex);

    return NULL;
}

void* doctor_thread(void* arg) {
    while(1) {
        pthread_mutex_lock(&mutex);
        while(!((waiting_patients == MAX_WAITING_PATIENTS || total_patients - served_patients == waiting_patients) && medicine_stock >=MAX_WAITING_PATIENTS) && !(medicine_stock < MAX_WAITING_PATIENTS)) {
            printf("[%ld] - Lekarz: zasypiam\n", time(NULL));
            pthread_cond_wait(&cond_doctor, &mutex);
            printf("[%ld] - Lekarz: budze sie\n", time(NULL));
        }

        if (served_patients == total_patients) {
            printf("[%ld] - Lekarz: wszyscy pacjenci obsluzeni. Koncze prace na dzisiaj\n", time(NULL));
            pthread_cond_broadcast(&cond_medicine);
            pthread_cond_broadcast(&cond_pharma_wait);
                   
            pthread_mutex_unlock(&mutex);
            break;
        }

        if((waiting_patients == MAX_WAITING_PATIENTS || total_patients - served_patients == waiting_patients) && medicine_stock >= MAX_WAITING_PATIENTS) {
            printf("[%ld] - Lekarz: konsultuje pacjentow ", time(NULL));
            for(int i=0; i<waiting_patients-1; i++) {
                printf("%d, ", patient_ids[i]);
            } printf("%d\n", patient_ids[waiting_patients-1]);

            int duration = 2 + rand() % 3;

            pthread_mutex_unlock(&mutex);
            sleep(duration);
            pthread_mutex_lock(&mutex);
            served_patients += waiting_patients;
            medicine_stock -= waiting_patients;
            for(int i=0; i<waiting_patients; i++) {
                pthread_cond_signal(&cond_patient_waiting);
            }
            if(medicine_stock < MAX_WAITING_PATIENTS) pthread_cond_signal(&cond_medicine);
            
            waiting_patients = 0;
        } else if (pharmacists_waiting > 0 && medicine_stock < MAX_WAITING_PATIENTS) {
            printf("[%ld] - Lekarz: przyjmuje dostawe lekow\n", time(NULL));
            pthread_cond_signal(&cond_pharma_wait);
            int duration = 1 + rand() % 3;

            pthread_mutex_unlock(&mutex);
            sleep(duration);
            pthread_mutex_lock(&mutex);
        }
        
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}


int main(int argc, char* argv[]) {
    
    int num_patients = atoi(argv[1]);
    int num_pharmacists = atoi(argv[2]);
    total_patients = num_patients;
    
    pthread_t doctor;
    pthread_t patients[num_patients];
    pthread_t pharmacists[num_pharmacists];

    int ids[num_patients > num_pharmacists ? num_patients : num_pharmacists];
    
    pthread_create(&doctor, NULL, doctor_thread, NULL);

    for(int i=0; i<num_patients; i++) {
        ids[i] = i + 1;
        pthread_create(&patients[i], NULL, patient_thread, &ids[i]);
    }

    for(int i=0; i<num_pharmacists; i++) {
        ids[i] = i + 1;
        pthread_create(&pharmacists[i], NULL, pharmacist_thread, &ids[i]);
    }

    for(int i=0; i<num_patients; i++) {
        pthread_join(patients[i], NULL);
    }

    pthread_join(doctor, NULL);

    for(int i=0; i<num_pharmacists; i++){
        pthread_join(pharmacists[i], NULL);
    }

    return 0;
}