#include <stdio.h>
#include <omp.h>

#define ARRAY_SIZE 10

void swap(int* a, int* b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int main()
{
    int arr[ARRAY_SIZE];
    printf("Enter an array of size %d\n", ARRAY_SIZE);
    for(int i = 0; i < ARRAY_SIZE; i++)
    {
        scanf("%d", &arr[i]);
    }
    for(int pass = 0; pass < (ARRAY_SIZE + 1) / 2; pass++)
    {
#pragma omp parallel for shared(arr)
        // Odd pass
        for(int i = 0; i + 1 < ARRAY_SIZE; i += 2)
        {
            if(arr[i] > arr[i+1])
            {
                swap(&arr[i], &arr[i+1]);
            }
        }
        printf("Array after pass %d\n", 2*pass);
        for(int i = 0; i < ARRAY_SIZE; i++)
        {
            printf("%d ", arr[i]);
        }
        putchar('\n');
#pragma omp parallel for shared(arr)
        // Even pass
        for(int i = 1; i + 1 < ARRAY_SIZE; i += 2)
        {
            if(arr[i] > arr[i+1])
            {
                swap(&arr[i], &arr[i+1]);
            }
        }
        printf("Array after pass %d\n", 2*pass+1);
        for(int i = 0; i < ARRAY_SIZE; i++)
        {
            printf("%d ", arr[i]);
        }
        putchar('\n');
    }
    printf("Sorted array\n");
    for(int i = 0; i < ARRAY_SIZE; i++)
    {
        printf("%d ", arr[i]);
    }
    putchar('\n');
    return 0;
}