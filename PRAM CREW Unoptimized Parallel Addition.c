#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int rnk;
    int p;
    int* arr;
} ThreadArgs;

pthread_barrier_t b0, b1;

void* thread_fn(void* args)
{
    ThreadArgs* data = (ThreadArgs*) args;
    while(data -> p > 0)
    {
        // READ
        int tmp = 0;
        if(data -> rnk < data -> p)
        {
            tmp = data -> arr[2 * data -> rnk] + data -> arr[2 * data -> rnk + 1];
        }
        pthread_barrier_wait(&b0);
        // WRITE
        if(data -> rnk < data -> p)
        {
            data -> arr[data -> rnk] = tmp;
        }
        data -> p /= 2;
        pthread_barrier_wait(&b1);
    }
    free(data);
    return NULL;
}

int main()
{
    int ARR_SIZE = 8;
    int *arr = (int*) malloc(sizeof(int) * ARR_SIZE);
    for(int i = 0; i < ARR_SIZE; i++) arr[i] = 2;
    int p = ARR_SIZE / 2;
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * (p));
    pthread_barrier_init(&b0, NULL, p);
    pthread_barrier_init(&b1, NULL, p);
    for(int i = 0; i < p; i++)
    {
        ThreadArgs *args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
        args -> rnk = i;
        args -> p = p;
        args -> arr = arr;
        pthread_create(&threads[i], NULL, thread_fn, args);
    }
    for(int i = 0; i < p; i++)
    {
        pthread_join(threads[i], NULL);
    }
    free(threads);
    printf("Sum is %d.\n", arr[0]);
    return 0;
}
