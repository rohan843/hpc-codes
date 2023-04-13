#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#define COUNT 100

int getvalue(sem_t* mutex)
{
    int value;
    sem_getvalue(mutex, &value);
    return value;
}

void set(sem_t* mutex)
{
    if (getvalue(mutex) == 0)
        sem_post(mutex);
}

void unset(sem_t* mutex)
{
    if (getvalue(mutex) == 1)
        sem_wait(mutex);
}

int main()
{
    //Shared Memory
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT);
    int *arr = (int*)shmat(shmid, (void*)0, 0);
    sem_t* produced = sem_open("producer", 0);
    while(1)
    {
        while(!getvalue(produced));
        printf("Consumed %d\n", arr[0]);
        unset(produced);
    }
    return 0;
}