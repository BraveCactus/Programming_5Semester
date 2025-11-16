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
    int st;
    st = MPI_Init(&argc, &argv);
    CheckSuccess(st);

    int rank, size;
    MPI_Status status;

    st = MPI_Comm_size(MPI_COMM_WORLD, &size);
    CheckSuccess(st);
    st = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    CheckSuccess(st);

    std::vector<int> loc_vec(rank+1, rank+1);

    std::vector<int> recv_count(size);
    std::vector<int>displ(size);
    std::vector<int>recv_data;

    if (rank==0){        
        for (int i = 0; i < size; i++)
        {
            recv_count[i] = i + 1;       
            displ[i] = i*(i+1);
                        
        }
        recv_data.resize(displ[size-1] + size, 0);
    }

    st = MPI_Gatherv(loc_vec.data(), rank+1, MPI_INT,recv_data.data(), recv_count.data(), displ.data(), MPI_INT, 0, MPI_COMM_WORLD);
    CheckSuccess(st);

    if (rank == 0) {
        std::ofstream file("results_gv1.txt");
        
        if (file.is_open()) {
            for (int i = 0; i < recv_data.size(); i++) {
                file << recv_data[i] << " ";                
            }
            file.close();
            std::cout << "Result has been written to results_gv1.txt" << std::endl;
        }        
    }
    MPI_Finalize();
    return 0;
}