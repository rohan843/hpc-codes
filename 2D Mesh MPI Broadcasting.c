#include <stdio.h>
#include <mpi.h>

// We have an N*M mesh.
#define N 4
#define M 3

int getRank(int i, int j)
{
    return i * M + j;
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int rnk, sz;
    MPI_Comm_rank(MPI_COMM_WORLD, &rnk);
    MPI_Comm_size(MPI_COMM_WORLD, &sz);
    int data = 0;
    if(rnk == 0)
    {
        data = 42;
    }
    int i, j;
    i = rnk / M;
    j = rnk % M;
    printf("Node %d (%d, %d) ready for broadcast.\n", rnk, i, j);
    //Not needed, just placed for aesthetics in logging.
    MPI_Barrier(MPI_COMM_WORLD);
    if(j == 0)
    {
        if(i != 0)
        {
            // Receive data from upper row.
            MPI_Recv(&data, 1, MPI_INT, getRank(i-1, j), MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
            printf("Node %d (%d, %d) received data %d from node %d (%d, %d).\n", rnk, i, j, data, getRank(i-1, j), i-1, j);
        }
        // Send to data lower rows, and also to the other columns of the current row.
        if(i+1 < N)
        {
            printf("Node %d (%d, %d) sending data %d to node %d (%d, %d).\n", rnk, i, j, data, getRank(i+1, j), i+1, j);
            MPI_Send(&data, 1, MPI_INT, getRank(i+1, j), 0, MPI_COMM_WORLD);
        }
        if(j+1 < M)
        {
            printf("Node %d (%d, %d) sending data %d to node %d (%d, %d).\n", rnk, i, j, data, getRank(i, j+1), i, j+1);
            MPI_Send(&data, 1, MPI_INT, getRank(i, j+1), 0, MPI_COMM_WORLD);
        }
    }
    else
    {
        // Receive data from previous column in the same row.
        MPI_Recv(&data, 1, MPI_INT, getRank(i, j-1), MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
        printf("Node %d (%d, %d) received data %d from node %d (%d, %d).\n", rnk, i, j, data, getRank(i, j-1), i, j-1);
        // Send to the other columns of the current row.
        if(j+1 < M)
        {
            printf("Node %d (%d, %d) sending data %d to node %d (%d, %d).\n", rnk, i, j, data, getRank(i, j+1), i, j+1);
            MPI_Send(&data, 1, MPI_INT, getRank(i, j+1), 0, MPI_COMM_WORLD);
        }
    }
    //Not needed, just placed for aesthetics in logging.
    MPI_Barrier(MPI_COMM_WORLD);
    printf("Node %d (%d, %d) received data %d.\n", rnk, i, j, data);
    MPI_Finalize();
}
