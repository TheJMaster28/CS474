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
#include <time.h>
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

void* producer(void* arg) {
    printf("Starting Thread Producer\n");
    char newChar;
    FILE* fp;
    fp = fopen("mytest.dat", "r");

    int index = 0;
    // critical section
    while (fscanf(fp, "%c", &newChar) != EOF) {
        // wait on empty
        sem_wait(&empty);

        // wait mutex
        sem_wait(&mutex);

        // printf("%c\n", newChar);
        BufferPointer->buffer[index] = newChar;

        // signal mutex
        sem_post(&mutex);

        // signal full
        sem_post(&full);

        index++;
        if (index >= 15) {
            index = 0;
        }
    }

    // wait on empty
    sem_wait(&empty);

    // wait mutex
    sem_wait(&mutex);

    // printf("%c\n", newChar);
    BufferPointer->buffer[index] = '\0';

    // signal mutex
    sem_post(&mutex);

    // signal full
    sem_post(&full);

    fclose(fp);
}
void* consumer(void* arg) {
    printf("Starting Thread Consumer\n");

    char readChar;

    int index = 0;
    int endOfFile = 0;
    printf("From Buffer: ");
    while (1) {
        if (index >= 15) {
            index = 0;
        }

        sleep(1);
        // wait full
        sem_wait(&full);

        // wait mutex
        sem_wait(&mutex);

        // critical Section
        readChar = BufferPointer->buffer[index];
        if (readChar != '\0') {
            printf("%c", readChar);
            fflush(stdout);
        } else {
            endOfFile = 1;
        }

        // signal mutex
        sem_post(&mutex);

        // signal empty
        sem_post(&empty);

        index++;

        if (endOfFile) {
            printf("\n");
            break;
        }
    }
}

int main() {
    time_t timeStart = time(NULL);
    printf("Creating Shared Memory\n");
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

    printf("Creating Semphoares\n");
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, SIZE);
    sem_init(&full, 0, 0);

    printf("Creating Threads\n");
    pthread_t pro[1], con[1];

    pthread_attr_t attr[1];

    pthread_attr_init(&attr[0]);
    pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM);

    pthread_create(&pro[0], &attr[0], producer, NULL);
    pthread_create(&con[0], &attr[0], consumer, NULL);

    printf("Waiting for Threads\n");
    pthread_join(pro[0], NULL);
    pthread_join(con[0], NULL);

    printf("Done with Threads\n");

    // release shared memory
    if (shmdt(BufferPointer) == -1) {
        perror("shmdt");
        exit(-1);
    }
    shmctl(shmid, IPC_RMID, NULL);

    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    time_t timeEnd = time(NULL);
    double time_taken = difftime(timeEnd, timeStart);
    printf("End of program, Time Taken : %f\n", time_taken);

    return 0;
}