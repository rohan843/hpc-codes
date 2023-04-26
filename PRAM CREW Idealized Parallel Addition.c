#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// ARR_SIZE must be a power of 2.
#define ARR_SIZE 8
#define THREAD_COUNT (ARR_SIZE/2)
#define bool int
#define false 0
#define true 1

int p = ARR_SIZE / 2;
int arr[ARR_SIZE];

typedef struct {
    int wait;
    int count;
    pthread_mutex_t mutCount;
} barrier_t;

// Creates and initializes a barrier.
barrier_t create_barrier()
{
    barrier_t res;
    res.wait = 1;
    res.count = 0;
    pthread_mutex_init(&res.mutCount, NULL);
    return res;
}

// A wait on barrier function for a while loop, within which multiple threads access and manipulate data in such a manner that some sequential algorithm would have control or data dependencies.
// !!!IMPORTANT!!!: There must be either 0 or ATLEAST 2 barrier()'s inside a while loop.
// !!!IMPORTANT!!!: All threads MUST call this for EACH barrier.
void waitOnBarrier(barrier_t* barrier, int rnk)
{    
    if(rnk == 0)
    {
        barrier -> wait = 0;
        while(barrier -> count < THREAD_COUNT - 1);
        pthread_mutex_lock(&barrier -> mutCount);
        barrier -> count = 0;
        pthread_mutex_unlock(&barrier -> mutCount);
        barrier -> wait = 1;
    }
    else
    {
        while(barrier -> wait == 1);
        pthread_mutex_lock(&barrier -> mutCount);
        barrier -> count++;
        pthread_mutex_unlock(&barrier -> mutCount);
    }
}

barrier_t b1, b2, b3;

void* thread_fn(void* dataPtr)
{
    int rnk = *((int*)dataPtr);
    printf("Thread %d ready.\n", rnk);
    while(p > 0)
    {
        // READ
        int t1 = 0, t2 = 0;
        if(rnk < p)
        {
            t1 = arr[2*rnk];
            t2 = arr[2*rnk + 1];
            printf("Thread %d, t1 = %d, t2 = %d.\n", rnk, t1, t2);
        }
        waitOnBarrier(&b1, rnk);
        // WRITE
        if(rnk < p)
        {
            arr[rnk] = t1 + t2;
        }
        waitOnBarrier(&b2, rnk);
        // GO TO NEXT PASS
        if(rnk == 0)
        {
            p /= 2;
        }
        waitOnBarrier(&b3, rnk);
    }
    printf("Thread %d exiting.\n", rnk);
    return NULL;
}

int main()
{
    b1 = create_barrier(), b2 = create_barrier(), b3 = create_barrier();
    for(int i = 0; i < ARR_SIZE; i++)
    {
        arr[i] = 1;
    }
    pthread_t threads[THREAD_COUNT];
    for(int i = 0; i < THREAD_COUNT; i++)
    {
        int* arg = (int*)malloc(sizeof(int));
        *arg = i;
        pthread_create(&threads[i], NULL, thread_fn, arg);
    }
    for(int i = 0; i < THREAD_COUNT; i++)
    {
        pthread_join(threads[i], NULL);
    }
    printf("The result is: %d.\n", arr[0]);
    return 0;
}
