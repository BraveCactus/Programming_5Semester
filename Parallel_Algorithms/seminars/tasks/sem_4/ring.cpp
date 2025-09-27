#include <mpi.h>

#include <iostream>

void check_success(int st){
    if(st != MPI_SUCCESS) MPI_Abort(MPI_COMM_WORLD, st);
}

int main(int argc, char *argv[]) {
    int tag = 1;
    MPI_Status status;

    int st;
    st = MPI_Init(&argc, &argv);
    check_success(st);

    int size;
    st = MPI_Comm_size(MPI_COMM_WORLD, &size);
    check_success(st);

    int rank;
    st = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    check_success(st);

    int i;
    int target_rank = 0;
    if (rank < size && rank != size - 1) {
        target_rank += (rank + 1);
    }

    if (rank == 0) {
        i = 0;
        st = MPI_Send(&i, 1, MPI_INT, target_rank, tag, MPI_COMM_WORLD);
        check_success(st);

        std::cout << "Process " << rank << ": I send " << i << " to process " << rank + 1 << std::endl; 

        st = MPI_Recv(&i, 1, MPI_INT, size - 1, tag, MPI_COMM_WORLD, &status);

        std::cout << "Process " << rank << ": I receive " << i << " from " << status.MPI_SOURCE << std::endl;

        check_success(st);

    } else {
        st = MPI_Recv(&i, 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
        check_success(st);

        std::cout << "Process " << rank << ": I receive " << i << " from " << status.MPI_SOURCE << std::endl;
        i += rank;
        st = MPI_Send(&i, 1, MPI_INT, target_rank, tag, MPI_COMM_WORLD);
        check_success(st);

        std::cout << "Process " << rank << ": I send " << i << " to process " << rank + 1 << std::endl;

    }

    MPI_Finalize();
}