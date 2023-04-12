#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ARRAY_SIZE 100000

int main(int argc, char **argv) {
    int rank, size;
    int i, start, end, sum = 0;
    int array[ARRAY_SIZE];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        // Initialize the array with some random values
        for (i = 0; i < ARRAY_SIZE; i++) {
            array[i] = rand() % 100;
        }
    }

    // Broadcast the array to all processes
    MPI_Bcast(array, ARRAY_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

    // Divide the array into chunks for each process to work on
    start = rank * (ARRAY_SIZE / size);
    end = start + (ARRAY_SIZE / size);

    // Calculate the sum of the chunk
    for (i = start; i < end; i++) {
        sum += array[i];
    }

    // Reduce the sum from each process to the final sum
    MPI_Reduce(&sum, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Print the final sum
        printf("The sum of all elements in the array is %d.\n", sum);
    }

    MPI_Finalize();

    return 0;
}
