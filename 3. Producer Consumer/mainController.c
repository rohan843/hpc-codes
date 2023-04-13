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

sem_t* Semaphore(char *SEM_NAME, int count)
{
    sem_unlink(SEM_NAME);
    return sem_open(SEM_NAME,
                        O_CREAT, S_IRUSR | S_IWUSR, count);
}

int main()
{
    //Shared Memory
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT);
    int *arr = (int*)shmat(shmid, (void*)0, 0);
    sem_t* produced = Semaphore("producer", 0);
    if(!fork()) {system("xterm -e ./producer"); exit(0);}
    if(!fork()) {system("xterm -e ./consumer"); exit(0);}
    int status = 0;
    while(wait(&status) > 0);
    return 0;
}