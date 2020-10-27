/*
    Jeffrey Lansford
    Program 1 
    10/4/2020
    Program to create 4 child process and increment a shared memory space to show the harmful effects of not protecting shared memory
*/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* key number */
#define SHMKEY ((key_t)1497)

// Creates the shared memory
typedef struct
{
    int value;
} shared_mem;

shared_mem *total;

// Process Functions to incremently loop the shared memory varible until desireable number
void process1() {
    while (total->value < 100000) {
        total->value++;
    }
    printf("Form Process1: Counter = %d\n", total->value);
}

void process2() {
    while (total->value < 200000) {
        total->value++;
    }
    printf("Form Process2: Counter = %d\n", total->value);
}

void process3() {
    while (total->value < 300000) {
        total->value++;
    }
    printf("Form Process3: Counter = %d\n", total->value);
}

void process4() {
    while (total->value < 500000) {
        total->value++;
    }
    printf("Form Process4: Counter = %d\n", total->value);
}

int main() {
    int shmid, pid1, pid2, pid3, pid4, ID, status;
    char *shmadd;
    shmadd = (char *)0;

    if ((shmid = shmget(SHMKEY, sizeof(int), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    if ((total = (shared_mem *)shmat(shmid, shmadd, 0)) == (shared_mem *)-1) {
        perror("shmat");
        exit(0);
    }

    // set value to Zero
    total->value = 0;

    // creates Child 1
    if ((pid1 = fork()) == 0) {
        // Child 1 execute process 1
        process1();
    } else {
        // Create Child 2
        if ((pid2 = fork()) == 0) {
            // Child 2 executes process 2
            process2();
        } else {
            // creates Child 3
            if ((pid3 = fork()) == 0) {
                // Child 3 executes process 3
                process3();
            } else {
                // creates Child 4
                if ((pid4 = fork()) == 0) {
                    // Child 4 executes process 4
                    process4();
                } else {
                    // Parent execution
                    // wait for child processes to be finsh
                    waitpid(pid1, NULL, 0);
                    waitpid(pid2, NULL, 0);
                    waitpid(pid3, NULL, 0);
                    waitpid(pid4, NULL, 0);
                    // print child pid
                    printf("Child 1 %d pid has just exited.\n", pid1);
                    printf("Child 2 %d pid has just exited.\n", pid2);
                    printf("Child 3 %d pid has just exited.\n", pid3);
                    printf("Child 4 %d pid has just exited.\n", pid4);

                    // release shared memory
                    if (shmdt(total) == -1) {
                        perror("shmdt");
                        exit(-1);
                    }
                    shmctl(shmid, IPC_RMID, NULL);
                    printf("End of program\n");
                }
            }
        }
    }
}
