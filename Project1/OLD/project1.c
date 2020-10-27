#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

/* key number */
#define SHMKEY ((key_t)1497)

typedef struct
{
    int value;
} shared_mem;

shared_mem *total;

void process1()
{
    int i;
    for (i = 0; i < 110000; i++)
        total->value++;
    printf("Form Process1: Counter = %d\n", total->value);
}

void process2()
{
    int i;
    for (i = 0; i < 100000; i++)
        total->value++;
    printf("Form Process2: Counter = %d\n", total->value);
}

void process3()
{
    int i;
    for (i = 0; i < 100000; i++)
        total->value++;
    printf("Form Process3: Counter = %d\n", total->value);
}

void process4()
{
    int i;
    for (int i = 0; i < 200000; i++)
        total->value++;
    printf("Form Process4: Counter = %d\n", total->value);
}

int main()
{
    int shmid, pid1, pid2, pid3, pid4, ID, status;
    char *shmadd;
    shmadd = (char *)0;

    if ((shmid = shmget(SHMKEY, sizeof(int), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget");
        exit(1);
    }
    if ((total = (shared_mem *)shmat(shmid, shmadd, 0)) == (shared_mem *)-1)
    {
        perror("shmat");
        exit(0);
    }
    total->value = 0;

    if ((pid1 = fork()) == 0)
    {
        process1();
        exit(0);
    }
    if ((pid2 = fork()) == 0)
    {
        process2();
        exit(0);
    }
    if ((pid3 = fork()) == 0)
    {
        process3();
        exit(0);
    }
    if ((pid4 = fork()) == 0)
    {
        process4();
        exit(0);
    }

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    waitpid(pid3, NULL, 0);
    waitpid(pid4, NULL, 0);
    printf("Child 1 %d pid has just exited.\n", pid1);
    printf("Child 2 %d pid has just exited.\n", pid2);
    printf("Child 3 %d pid has just exited.\n", pid3);
    printf("Child 4 %d pid has just exited.\n", pid4);
    if (shmdt(total) == -1)
    {
        perror("shmdt");
        exit(-1);
    }

    shmctl(shmid, IPC_RMID, NULL);
    printf("End of program\n");
    // wait(NULL);
    // if (pid1)
    //     printf("Child %d pid has just exited.\n", pid1);

    // if (pid2)
    //     printf("Child %d pid has just exited.\n", pid2);

    // if (pid2 = fork() == 0)
    //     process2();

    // if (pid3 = fork() == 0)
    //     process3();

    // if (pid4 = fork() == 0)
    //     process4();
    // if (pid1)
    //     printf("Child %d pid has just exited.\n", pid1);
    // if (pid2)
    //     printf("Child %d pid has just exited.\n", pid2);
    // printf("Child %d pid has just exited.\n", pid2);
    // printf("Child %d pid has just exited.\n", pid3);
    // printf("Child %d pid has just exited.\n", pid4);
    // pid1 = fork();
    // if (pid1 == 0) {
    //     process1();
    //     exit(0);
    // } else {
    //     printf("here1\n");
    //     pid2 = fork();
    //     if (pid2 == 0) {
    //         process2();
    //     } else {
    //         pid_t pid = wait(NULL);
    //         printf("CT: child has terminated %ld\n", pid);
    //         shmdt(total);

    //         shmctl(shmid, IPC_RMID, NULL);

    //         printf("End of program\n");
    //     }
    // }
}
