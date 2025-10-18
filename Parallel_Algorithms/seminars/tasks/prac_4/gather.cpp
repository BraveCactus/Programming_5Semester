#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include <mpi.h>
#include <fstream>
#include <vector>

void CheckSuccess(int state) {
    if (state != MPI_SUCCESS) MPI_Abort(MPI_COMM_WORLD, state);
}

int main(int argc, char* argv[]){
    int rank, size;
    MPI_Status status;

    int st;
    st = MPI_Init(&argc, &argv);
    CheckSuccess(st);

    st = MPI_Comm_size(MPI_COMM_WORLD, &size);
    CheckSuccess(st);
    st = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    CheckSuccess(st);


    int N = 120;

    if(N % size != 0 and rank == 0){
        std::cout << "N is not divisible by processes amount!" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    int batch_size = N / size;
    std::vector<int> local_vector(batch_size);

    for (int i = 0; i < batch_size; i++)
    {
        local_vector[i] = rank;
    }

    std::vector<int> global_vector;
    if (rank == 0){
        global_vector.resize(N);        
    }

    st = MPI_Gather(local_vector.data(), batch_size, MPI_INT, 
               global_vector.data(), batch_size, MPI_INT, 
               0, MPI_COMM_WORLD);

    CheckSuccess(st);

    if (rank == 0){
        std::ofstream file("global_vector.txt");
        
        if(file.is_open()){
            for (int i = 0; i < N; i++)
            {
                file << global_vector[i] << std::endl;                
            }            
        }
        file.close();
    }
    MPI_Finalize();
    return 0;
}