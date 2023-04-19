#include <stdio.h>
#include <omp.h>

#define M 2
#define P 1
#define N 2

int main()
{
    int A[M][P], B[P][N], C[M][N];
    for(int i = 0; i < M; i++)
    {
        for(int j = 0; j < P; j++)
        {
            A[i][j] = i + j;
        }
    }
    for(int i = 0; i < P; i++)
    {
        for(int j = 0; j < N; j++)
        {
            B[i][j] = i + j;
        }
    }
#pragma omp parallel for shared(A, B, C)
    for(int i = 0; i < M*N; i++)
    {
        int r = i / N;
        int c = i % N;
        C[r][c] = 0;
        for(int k = 0; k < P; k++)
        {
            C[r][c] += A[r][k] * B[k][c];
        }
    }
    
    for(int i = 0; i < M; i++)
    {
        for(int j = 0; j < N; j++)
        {
            printf("%d ", C[i][j]);
        }
        putchar('\n');
    }
    return 0;
}
