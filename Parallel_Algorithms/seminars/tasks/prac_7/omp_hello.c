#include <stdio.h>
#include <omp.h>

int main() {
    int procs_amount = omp_get_num_procs();
    printf("Processes amount: %d\n", procs_amount);

    #pragma omp parallel num_threads(6)
    {   
        printf("Hello! Current thread: %d. Threads amount: %d\n", 
               omp_get_thread_num(), omp_get_num_threads());
    } 

    printf("Current thread: %d. Threads amount: %d\n", 
           omp_get_thread_num(), omp_get_num_threads());

    return 0;
}