#define _REENTRANT
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
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

// semaphores
sem_t mutex, empty, full;

// function for producer thread
void* producer(void* arg) {
    // open file
    FILE* fp;
    fp = fopen("mytest.dat", "r");

    // set up index and character varible
    char newChar;
    int index = 0;
    // read in characters from file
    while (fscanf(fp, "%c", &newChar) != EOF) {
        // wait on empty
        sem_wait(&empty);

        // wait mutex
        sem_wait(&mutex);

        // critical section
        // store read character into shared memeory buffer
        BufferPointer->buffer[index] = newChar;

        // signal mutex
        sem_post(&mutex);

        // signal full
        sem_post(&full);

        // incremate index and reset index if greater than size of Buffer
        index++;
        if (index >= SIZE) {
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

// function for consumer thread
void* consumer(void* arg) {
    // set up flags, index, and character varibles
    char readChar;
    int index = 0;
    int endOfFile = 0;
    printf("From Buffer: ");
    while (1) {
        // wait one second
        sleep(1);

        // wait full
        sem_wait(&full);

        // wait mutex
        sem_wait(&mutex);

        // critical Section
        // read in Charater from buffer
        readChar = BufferPointer->buffer[index];

        // if character is not null charcter, then print character out
        if (readChar != '\0') {
            printf("%c", readChar);
            fflush(stdout);
        }
        // set flag to leave loop
        else {
            endOfFile = 1;
        }

        // signal mutex
        sem_post(&mutex);

        // signal empty
        sem_post(&empty);

        // incremate index and reset index if greater than size of Buffer
        index++;
        if (index >= SIZE) {
            index = 0;
        }

        // print new line and break out of loop
        if (endOfFile) {
            printf("\n");
            break;
        }
    }
}

// main thread for creating and deleting threads and shared memory
int main() {
    // take beginnig time
    struct timeval timeStart, timeEnd, timeTaken;
    gettimeofday(&timeStart, NULL);

    // create share memory
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

    // initialized semaphores
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, SIZE);
    sem_init(&full, 0, 0);

    // create threads
    pthread_t pro[1], con[1];
    pthread_attr_t attr[1];
    pthread_attr_init(&attr[0]);
    pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM);
    pthread_create(&pro[0], &attr[0], producer, NULL);
    pthread_create(&con[0], &attr[0], consumer, NULL);

    // wait on threads to be finshed
    pthread_join(pro[0], NULL);
    pthread_join(con[0], NULL);

    // release shared memory
    if (shmdt(BufferPointer) == -1) {
        perror("shmdt");
        exit(-1);
    }
    shmctl(shmid, IPC_RMID, NULL);

    // destory semaphores
    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    // evaulate time evaluation
    gettimeofday(&timeEnd, NULL);
    timersub(&timeEnd, &timeStart, &timeTaken);
    printf("End of program, Time Taken : %f seconds\n", (double)timeTaken.tv_sec + (timeTaken.tv_usec / 1000000.0));

    return 0;
}