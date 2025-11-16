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

int main(int argc, char *argv[]){
    int st;
    st = MPI_Init(&argc, &argv);
    CheckSuccess(st);

    int rank, size;
    MPI_Status status;

    st = MPI_Comm_size(MPI_COMM_WORLD, &size);
    CheckSuccess(st);
    st = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    CheckSuccess(st);

    double delta, maxdelta;   

    FILE *ff;
    
    if(argc != 2){
        if (rank == 0) std::cout << "Usage: exefile npoints" << std::endl;        
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    int N = atoi(argv[1]);
    if (N == 0) {
        if (rank == 0) std::cout << "Set N to 1000" << std::endl;
        N = 1000;
    } else {
        if (rank == 0) std::cout << "Set N to " << N << std::endl;
    }

    int count = 0;
    double h = 1.0 / N;
    double tau = 0.5 * (h * h);
    double eps = 1e-6;

    int beg_j = 1 + rank * (N - 1) / size;
    int end_j = 1 + (rank + 1) * (N - 1) / size;    
    if (rank == size - 1) {
        end_j = N;
    }

    std::vector<double> u(N + 1, 0.0);
    std::vector<double> unew(N + 1, 0.0);    
    
    unew[0] = u[0] = 1.0;
    unew[N] = u[N] = 0;

    double coef = tau/(h * h);

    while(1){        
        for (int i = beg_j; i < end_j; i++){
            unew[i] = u[i] + coef * (u[i-1] - 2 * u[i] + u[i+1]);
        }  	  
       
        maxdelta = 0;
        for (int i = beg_j; i < end_j; i++){            
            delta = std::fabs(unew[i] - u[i]);
            if(delta > maxdelta) maxdelta = delta;
        }
        
        double global_maxdelta;
        st = MPI_Reduce(&maxdelta, &global_maxdelta, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
        CheckSuccess(st);        
        
        st = MPI_Bcast(&global_maxdelta, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        CheckSuccess(st);
        
        if(global_maxdelta < eps) break;
        count++;        
        
        if (rank > 0) {            
            st = MPI_Send(&unew[beg_j], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
            CheckSuccess(st);            
            st = MPI_Recv(&unew[beg_j - 1], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &status);
            CheckSuccess(st);
        }

        if (rank < size - 1) {            
            st = MPI_Send(&unew[end_j - 1], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
            CheckSuccess(st);            
            st = MPI_Recv(&unew[end_j], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &status);
            CheckSuccess(st);
        }
        
        int start_i = beg_j - 1;
        if (start_i < 1) start_i = 1;
        int end_i = end_j + 1;
        if (end_i > N) end_i = N;
        
        for(int i = start_i; i < end_i; i++){
            u[i] = unew[i];
        }
    }

    std::vector<int> recvcounts(size);
    std::vector<int> displs(size);    
    
    for (int i = 0; i < size; i++) {
        int proc_beg = 1 + i * (N - 1) / size;
        int proc_end = 1 + (i + 1) * (N - 1) / size;
        if (i == size - 1){
            recvcounts[i] = N - proc_beg + 1;
        } else {
            recvcounts[i] = proc_end - proc_beg;
        }
        if (i == 0){
            displs[i] = 1;
        } else {
            displs[i] = displs[i - 1] + recvcounts[i - 1];
        }        
    } 

    st = MPI_Gatherv(&unew[beg_j], recvcounts[rank], MPI_DOUBLE,
                    unew.data(),
                    recvcounts.data(), displs.data(), MPI_DOUBLE,
                    0, MPI_COMM_WORLD);
    CheckSuccess(st);

    if (rank == 0) {
        std::cout << count << " steps" << std::endl;
        if ((ff = fopen("seqres2", "w+")) == NULL){
            std::cout << "Can't open file" << std::endl; 
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        
        for(int i = 0; i < N + 1; i++) {
            fprintf(ff, "%f\n", unew[i]);
        }
            
        fclose(ff); 
    }
    
    MPI_Finalize();
    return 0;  
}