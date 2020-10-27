#define _REENTRANT
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>


/* key number */
#define SHMKEY ((key_t)1497)

// Creates the shared memory
typedef struct
{
    char buffer[15];
} shared_mem;

shared_mem *BufferPointer;






int main() {


    // release shared memory
                    if (shmdt(total) == -1) {
                        perror("shmdt");
                        exit(-1);
                    }
                    shmctl(shmid, IPC_RMID, NULL);
                    printf("End of program\n");
}