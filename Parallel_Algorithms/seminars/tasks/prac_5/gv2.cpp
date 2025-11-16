#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include <mpi.h>
#include <fstream>
#include <vector>
#include <cstdlib>

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

    srandom(rank + 1);
    int kol = random() % 7 + 1;

    std::vector<double> loc_vec(kol, sqrt(rank));

    std::vector<int> recv_count(size);
    std::vector<int> displ(size);
    std::vector<double> recv_data;

    st = MPI_Gather(&kol, 1, MPI_INT, recv_count.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
    CheckSuccess(st);

    if (rank == 0){        
        for (int i = 0; i < size; i++) {
            if (i == 0) {
                displ[i] = 0;
            } else {
                displ[i] = displ[i-1] + recv_count[i-1];
            }
        }
        recv_data.resize(displ[size-1] + recv_count[size-1]);
    }
    
    st = MPI_Gatherv(loc_vec.data(), kol, MPI_DOUBLE, 
                    recv_data.data(), recv_count.data(), displ.data(), MPI_DOUBLE, 
                    0, MPI_COMM_WORLD);
    CheckSuccess(st);

    if (rank == 0) {
        std::ofstream file("results_gv2.txt");
        
        if (file.is_open()) {
            int cur_pos = 0;
            for (int i = 0; i < size; i++) {                
                for (int j = 0; j < recv_count[i]; j++) {
                    file << recv_data[cur_pos + j] << " ";                    
                }
                file << std::endl;
                cur_pos += recv_count[i];
            }
            file.close();
            std::cout << "Result has been written to results_gv2.txt" << std::endl;
        }        
    }
    MPI_Finalize();    
}