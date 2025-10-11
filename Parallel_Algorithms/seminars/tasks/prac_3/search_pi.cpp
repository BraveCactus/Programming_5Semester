#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include <mpi.h>
#include <fstream>

void CheckSuccess(int state) {
    if (state != MPI_SUCCESS) MPI_Abort(MPI_COMM_WORLD, state);
}

void Send_to_all(int* data, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
    int rank, size;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == root) {        
        for (int i = 0; i < size; i++) {
            if (i != root) {
                MPI_Send(data, count, datatype, i, 0, comm);
                std::cout << "Process " << root << ": send " << *data << " to process " << i << std::endl;
            }
        }
    }
}

double circle_equation(double x) {
    x = std::max(0.0, std::min(2.0, x));
    return sqrt(4 - x * x);
}

double calculate_area(double x, double h) {
    return (circle_equation(x) + circle_equation(x + h)) / 2.0 * h; 
}

double calculate_pi_part(int n, int start, int end) {
    double part_of_pi = 0;
    double h = 2.0 / n;
    double curr_pos = start * h;

    for (int i = start; i < end; i++) {
        part_of_pi += calculate_area(curr_pos, h);
        curr_pos += h;
    }

    return part_of_pi;
}

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Status status;

    int st;
    st = MPI_Init(&argc, &argv);
    CheckSuccess(st);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int n = 0;
    double local_pi = 0.0;

    if (rank == 0) {
        std::ifstream file("data.txt");       
        
        if (file.is_open()) {
            file >> n;
            std::cout << "Number from file: " << n << std::endl;
            file.close();
        } else {
            std::cout << "File opening error" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }        

        Send_to_all(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);        
        
    } else {        
        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }
    
    int part_size = n / size;
    int remains = n % size;

    int start, end;
    if (rank != size-1){
        start = rank * part_size;
        end = start + part_size;
    } else {
        start = rank * part_size;
        end = start + part_size + remains;
    }   
      
  
    local_pi = calculate_pi_part(n, start, end);
    std::cout << "Process " << rank << ": calculated from " << start << " to " << end 
              << " = " << local_pi << std::endl;
    
    if (rank == 0) {
        double global_pi = local_pi;        
        
        for (int i = 1; i < size; i++) {
            double received_pi;
            MPI_Recv(&received_pi, 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &status);
            std::cout << "Process 0 received: " << received_pi << " from process " << i << std::endl;
            global_pi += received_pi;
        }
        
        std::cout << "Pi = " << global_pi << std::endl;        
        
    } else {        
        MPI_Send(&local_pi, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);        
    }

    MPI_Finalize();
    return 0;
}