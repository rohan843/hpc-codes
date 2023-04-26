#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// ARR_SIZE_ORIG must be 16.
#define ARR_SIZE_ORIG 16
#define ARR_SIZE 4
#define THREAD_COUNT (ARR_SIZE/2)

int p = ARR_SIZE / 2;
int arr[ARR_SIZE_ORIG];

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
void waitOnBarrier(barrier_t* barrier, int rnk, int threadCount)
{    
    if(rnk == 0)
    {
        barrier -> wait = 0;
        while(barrier -> count < threadCount - 1);
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

barrier_t b0, b1, b2, b3;

void* thread_fn(void* dataPtr)
{
    int rnk = *((int*)dataPtr);
    free(dataPtr);
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
        waitOnBarrier(&b1, rnk, THREAD_COUNT);
        // WRITE
        if(rnk < p)
        {
            arr[rnk] = t1 + t2;
        }
        waitOnBarrier(&b2, rnk, THREAD_COUNT);
        // GO TO NEXT PASS
        if(rnk == 0)
        {
            p /= 2;
        }
        waitOnBarrier(&b3, rnk, THREAD_COUNT);
    }
    printf("Thread %d exiting.\n", rnk);
    return NULL;
}

int log2Custom(int x)
{
    int cnt = 0;
    while(x > 1)
    {
        x = x >> 1;
        cnt++;
    }
    return cnt;
}

typedef struct {
    int rnk;
    int* arr;
    int si;
    int ei;
    int ri;
    int numThreads;
} args_for_array_opt;

void* thread_fn_for_array_optimization(void* args)
{
    args_for_array_opt* data = (args_for_array_opt*)args;
    // READ
    int tmp = 0;
    for(int i = data -> si; i <= data -> ei; i++)
    {
        tmp += data -> arr[i];
    }
    waitOnBarrier(&b0, data -> rnk, data -> numThreads);
    //WRITE
    arr[data -> ri] = tmp;
    free(data);
    return NULL;
}

void optimizeArray(int arr[], int sz)
{
    int noOfGrps = sz / log2Custom(sz);
    int grpSize = log2Custom(sz);
    pthread_t *threads = (pthread_t*) malloc(sizeof(pthread_t) * noOfGrps);
    for(int i = 0; i < noOfGrps; i++)
    {
        args_for_array_opt* arg = (args_for_array_opt*)malloc(sizeof(args_for_array_opt));
        arg -> rnk = i;
        arg -> arr = arr;
        arg -> si = i * grpSize;
        arg -> ei = i * grpSize + grpSize - 1;
        arg -> ri = i;
        arg -> numThreads = noOfGrps;
        pthread_create(&threads[i], NULL, thread_fn_for_array_optimization, arg);
    }
    for(int i = 0; i < noOfGrps; i++)
    {
        pthread_join(threads[i], NULL);
    }
    free(threads);
}

int main()
{
    b0 = create_barrier(), b1 = create_barrier(), b2 = create_barrier(), b3 = create_barrier();
    for(int i = 0; i < ARR_SIZE_ORIG; i++)
    {
        arr[i] = 1;
    }
    optimizeArray(arr, ARR_SIZE_ORIG);
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
