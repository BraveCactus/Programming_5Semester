#include <mpi.h>

#include <iostream>

void check_success(int st){
    if(st != MPI_SUCCESS) MPI_Abort(MPI_COMM_WORLD, st);
}

int main(int argc, char *argv[]) {
  int st;
  st = MPI_Init(&argc, &argv);
  check_success(st);
  
  std::cout << "Hello World!" << std::endl;
  MPI_Finalize();
}