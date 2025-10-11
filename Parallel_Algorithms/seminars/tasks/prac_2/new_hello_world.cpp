#include <mpi.h>
#include <iostream>

void check_success(int st){
    if(st != MPI_SUCCESS) MPI_Abort(MPI_COMM_WORLD, st);
}

int main(int argc, char *argv[]) {
  int st;
  st = MPI_Init(&argc, &argv);
  check_success(st);  

  int size;
  st = MPI_Comm_size(MPI_COMM_WORLD, &size);
  check_success(st);

  int rank;
  st = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  check_success(st);

  std::cout << "Hello, I'm " << rank << "process. There is " << size << "processes." << std::endl;
  MPI_Finalize();
}