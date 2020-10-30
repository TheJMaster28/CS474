#define _REENTRANT
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>

/* key number */
#define SHMKEY ((key_t)1497)

// Creates the shared memory
typedef struct
{
    char buffer[15];
} shared_mem;

shared_mem *BufferPointer;

void producer() {}
void consumer() {}

int main() {
    // release shared memory
    if (shmdt(total) == -1) {
        perror("shmdt");
        exit(-1);
    }
    shmctl(shmid, IPC_RMID, NULL);
    printf("End of program\n");
}