#include <mpi.h>
#include <stdio.h>

// Gets the log to the base 2 of some POWER OF 2 ONLY
int log2Custom(int sz)
{
    int cnt = 0;
    while(sz != 1)
    {
        cnt++;
        sz = sz >> 1;
    }
    return cnt;
}

int main(int argc, char* argv[])
{
    int sz, rnk;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rnk);
    MPI_Comm_size(MPI_COMM_WORLD, &sz);
    int dims = log2Custom(sz);
    int data = 0;
    if(rnk == 0)
    {
        data = 42;
    }
    printf("Node %d initiating broadcast.\n", rnk);
    // Barrier added just for aesthetic logging, its not actually needed in the code.
    MPI_Barrier(MPI_COMM_WORLD);
    for(int i = 0; i < dims; i++)
    {
        // If for the current dimension, the current node participates.
        if((rnk >> (i+1)) == 0)
        {
            if(rnk & (1 << i))
            {
                // The current node will receive the data.
                int sender = rnk & (~(1 << i));
                MPI_Recv(&data, 1, MPI_INT, sender, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
                printf("Node %d received data %d from node %d.\n", rnk, data, sender);
            }
            else
            {
                // The current node will send the data.
                int receiver = rnk | (1 << i);
                printf("Node %d sending data %d to node %d.\n", rnk, data, receiver);
                MPI_Send(&data, 1, MPI_INT, receiver, 0, MPI_COMM_WORLD);
            }
        }
    }
    // Barrier added just for aesthetic logging, its not actually needed in the code.
    MPI_Barrier(MPI_COMM_WORLD);
    printf("Node %d has data %d.\n", rnk, data);
    return 0;
}
