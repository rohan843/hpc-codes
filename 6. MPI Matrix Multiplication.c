#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 2
#define M 3
#define P 2

void set(int* arr, int R, int C, int r, int c, int val)
{
    arr[C*r + c] = val;
}

int get(int* arr, int R, int C, int r, int c)
{
    return arr[C*r + c];
}

int min(int a, int b)
{
    if(a < b) return a;
    return b;
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    int rnk, np;
    MPI_Comm_rank(MPI_COMM_WORLD, &rnk);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    int A[N*P], B[P*M];
    if(rnk == 0)
    {
        for(int r = 0; r < N; r++)
        {
            for(int c = 0; c < P; c++)
            {
                set(A, N, P, r, c, 1);
            }
        }
        for(int r = 0; r < P; r++)
        {
            for(int c = 0; c < M; c++)
            {
                set(B, P, M, r, c, 1);
            }
        }
    }
    MPI_Bcast(A, N*P, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(B, P*M, MPI_INT, 0, MPI_COMM_WORLD);
    int sz = N*M / (np - 1);
    int st = rnk * sz;
    int end = min(N*M - 1, (rnk + 1) * sz - 1);
    int* C_tmp = (int*)malloc(sizeof(int) * (sz));
    for(int i = st; i <= end; i++)
    {
        C_tmp[i - st] = 0;
        int r = i / M;
        int c = i % M;
        for(int k = 0; k < P; k++)
        {
            C_tmp[i-st] += get(A, N, P, r, k) * get(B, P, M, k, c);
        }
    }
    int* C = (int*)malloc(sizeof(int)*(sz * np));
    MPI_Gather(C_tmp, sz, MPI_INT, C, sz, MPI_INT, 0, MPI_COMM_WORLD);
    free(C_tmp);
    if(rnk == 0)
    {
        for(int r = 0; r < N; r++)
        {
            for(int c = 0; c < M; c++)
            {
                printf("%d ", get(C, N, M, r, c));
            }
            putchar('\n');
        }
    }
    free(C);
    MPI_Finalize();
    return 0;
}