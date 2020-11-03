#define _REENTRANT
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>  // for close

#define SIZE 15
// const int SIZE = 15;

/* key number */
#define SHMKEY ((key_t)1497)

// Creates the shared memory
typedef struct
{
    char buffer[SIZE];
} shared_mem;

shared_mem* BufferPointer;

sem_t mutex, empty, full;

void* producer() {
    printf("Starting THreead Producer");
    char newChar;
    FILE* fp;
    fp = fopen("mytest.dat", "r");
    while (fscanf(fp, "%c", &newChar) != EOF) {
        // wait on empy
        sem_wait(&empty);

        // wait mutex
        sem_wait(&mutex);

        // critical section
        printf("%c", newChar);

        // signal mutex
        sem_post(&mutex);

        // signal full
        sem_post(&full);
    }

    fclose(fp);
}
void* consumer() {
    printf("Starting THreead Consumer");
    // wait full
    sem_wait(&full);

    // wait mutex
    sem_wait(&mutex);

    // Critcial section
    sleep(4);

    // signal mutex
    sem_post(&mutex);

    // signal empty
    sem_post(&empty);
}

int main() {
    printf("Creating Shared Memory");
    int shmid;
    char* shmadd;
    shmadd = (char*)0;
    if ((shmid = shmget(SHMKEY, sizeof(int), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    if ((BufferPointer = (shared_mem*)shmat(shmid, shmadd, 0)) == (shared_mem*)-1) {
        perror("shmat");
        exit(0);
    }

    printf("Creating Semphoares");
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, SIZE);
    sem_init(&full, 0, 0);

    printf("Creating threads");
    pthread_t pro, con;
    pthread_create(&pro, NULL, producer, NULL);
    pthread_create(&con, NULL, consumer, NULL);

    printf("Waiting for Threads");
    pthread_join(pro, NULL);
    pthread_join(con, NULL);

    printf("Done with Threads");

    // release shared memory
    if (shmdt(BufferPointer) == -1) {
        perror("shmdt");
        exit(-1);
    }
    shmctl(shmid, IPC_RMID, NULL);

    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    printf("End of program\n");
}