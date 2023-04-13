#include<stdio.h>
#include<omp.h>

#define ARRAY_SIZE 100
#define THREAD_COUNT 4

int main(void)
{
    int noOfThreads, threadId;
    int arr[ARRAY_SIZE];
    int A[THREAD_COUNT];
    for(int i = 0; i < ARRAY_SIZE; i++)
    {
        arr[i] = i;
    }
    int sum = 0;
#pragma omp parallel for num_threads (THREAD_COUNT)
    for(int i = 0; i < THREAD_COUNT; i++)
    {
        int curSum = 0;
        int st = i * (ARRAY_SIZE / THREAD_COUNT);
        int end = st + (ARRAY_SIZE / THREAD_COUNT);
        for(int j = st; j < end; j++)
        {
            curSum += arr[j];
        }
        A[i] = curSum;
    }
#pragma omp parallel for reduction (+: sum)
    for(int i = 0; i < THREAD_COUNT; i++)
    {
        sum += A[i];
    }
    printf("The sum is: %d\n", sum);
    return 0;
}