#include <bits/stdc++.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <stdio.h>
#include <fcntl.h>

class Semaphore
{
private:
    bool counting;
    sem_t mutex;

public:
    Semaphore()
    {
        
    }
    Semaphore(char *SEM_NAME, bool counting, int count = 1)
    {
        this->counting = counting;
        sem_init(&mutex, 1, count);
    }
    void incr()
    {
        sem_post(&mutex);
    }
    void decr()
    {
        sem_wait(&mutex);
    }
    void set()
    {
        if (!(this->counting) && getvalue() == 0)
            incr();
    }
    void unset()
    {
        if (!(this->counting) && getvalue() == 1)
            decr();
    }
    int getvalue()
    {
        int value;
        sem_getvalue(&mutex, &value);
        return value;
    }
};

Semaphore SEM_BEGIN,
        SEM_NEXTPASS,
        SEM_PASSCOUNT,
        SEM_READCOUNT,
        SEM_EXECCOUNT,
        SEM_WRITECOUNT,
        SEM_NOOFWAITINGPROCESSES;
        
int* arr;
int n;

using namespace std;

#define endl '\n'

void log(long int th_id, int th_rnk, int t1, int t2)
{
    printf("Thread %ld | %d --> Swapped %d and %d\n", th_id, th_rnk, t1, t2);
}

void* threadFn(void* ptr)
{
    int thread = *((int*)ptr);
    while (!SEM_BEGIN.getvalue())
        ;
    bool lastProc = (thread == n / 2);
    while (SEM_PASSCOUNT.getvalue() < n)
    {
        int i = 2 * thread - 1, j = 2 * thread;
        // READ
        bool odd = (SEM_PASSCOUNT.getvalue() + 1) & 1;
        if (!odd)
        {
            i++, j++;
        }
        int tmp1, tmp2;
        if(lastProc)
        {
            tmp1 = odd ? arr[i] : 0;
            tmp2 = odd ? arr[j] : 0;
        }
        else
        {
            tmp1 = arr[i];
            tmp2 = arr[j];
        }
        SEM_READCOUNT.decr();
        while (SEM_READCOUNT.getvalue() > 0)
            ;
        // EXECUTE
        if (tmp1 > tmp2)
        {
            swap(tmp1, tmp2);
            log(pthread_self(), thread, tmp1, tmp2);
        }
        SEM_NEXTPASS.unset();
        SEM_EXECCOUNT.decr();
        while (SEM_EXECCOUNT.getvalue() > 0)
            ;
        // WRITE
        if (!lastProc || odd)
        {
            arr[i] = tmp1;
            arr[j] = tmp2;
        }
        SEM_WRITECOUNT.decr();
        while (SEM_WRITECOUNT.getvalue() > 0)
            ;
        SEM_NOOFWAITINGPROCESSES.incr();
        while (!SEM_NEXTPASS.getvalue())
            ;
    }
}

int main()
{    
    arr = (int*)malloc(22 * sizeof(int));
    cout<<"Enter the number of elements (must be <= 20 and even)"<<endl;
    cin>>n;
    while(n > 20 || n < 1 || n % 2 != 0)
    {
        cout<<"Re - enter the number of elements (must be <= 20 and even)"<<endl;
        cin>>n;
    }
    
    cout << "Enter " << n << " elements" << endl;
    for (int i = 1; i <= n; i++)
    {
        cin >> arr[i];
    }
    
    SEM_BEGIN = Semaphore("begin", false, 0),
    SEM_NEXTPASS = Semaphore("nextpass", false, 0),
    SEM_PASSCOUNT = Semaphore("passcount", true, 0),
    SEM_READCOUNT = Semaphore("readcount", true, n / 2),
    SEM_EXECCOUNT = Semaphore("execcount", true, n / 2),
    SEM_WRITECOUNT = Semaphore("writecount", true, n / 2),
    SEM_NOOFWAITINGPROCESSES = Semaphore("noofwaitingprocesses", true, 0);
        
    pthread_t *t_ids = (pthread_t*)malloc((n/2) * sizeof(pthread_t));
    for(int i = 1; i <= n / 2; i++)
    {
        int* arg = (int*)malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&t_ids[i], NULL, threadFn, arg);
    }    
    
    SEM_BEGIN.set();
    
    for (int pass = 0; pass <= n - 1; pass++)
    {
        while (SEM_NOOFWAITINGPROCESSES.getvalue() < n / 2)
            ;
        SEM_PASSCOUNT.incr();
        while (SEM_NOOFWAITINGPROCESSES.getvalue() > 0)
            SEM_NOOFWAITINGPROCESSES.decr();
        while (SEM_READCOUNT.getvalue() < n / 2)
            SEM_READCOUNT.incr();
        while (SEM_WRITECOUNT.getvalue() < n / 2)
            SEM_WRITECOUNT.incr();
        while (SEM_EXECCOUNT.getvalue() < n / 2)
            SEM_EXECCOUNT.incr();
        
        printf("Main Function --> after pass %d:\n", pass);
        for(int i = 1; i <= n; i++)
        {
            printf("%d ", arr[i]);
        }
        printf("\n");
        
        SEM_NEXTPASS.set();
    }
    
    for(int i = 1; i <= n / 2; i++)
    {
        pthread_join(t_ids[i], NULL);
    }
    
    for (int i = 1; i <= n; i++)
    {
        cout << arr[i] << ' ';
    }
    cout << endl;

    return 0;
}
