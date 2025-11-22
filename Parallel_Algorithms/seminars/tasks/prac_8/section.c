#include <omp.h>
#include <stdio.h>

int main() {
    int curr_thread;

    #pragma omp parallel private(curr_thread)
    {
        #pragma omp sections
        {
            #pragma omp section
            {
                curr_thread = omp_get_thread_num();
                printf("Section 1. Current thread: %d", curr_thread);
            }

            #pragma omp section
            {
                curr_thread = omp_get_thread_num();
                printf("Section 2. Current thread: %d", curr_thread);
            }

            #pragma omp section
            {
                curr_thread = omp_get_thread_num();
                printf("Section 3. Current thread: %d", curr_thread);
            }

            #pragma omp section
            {
                curr_thread = omp_get_thread_num();
                printf("Section 4. Current thread: %d", curr_thread);
            }

            #pragma omp section
            {
                curr_thread = omp_get_thread_num();
                printf("Section 5. Current thread: %d", curr_thread);
            }

            #pragma omp section
            {
                curr_thread = omp_get_thread_num();
                printf("Section 6. Current thread: %d", curr_thread);
            }
        }
        curr_thread = omp_get_num_thread();
        printf("After sections. Current thread: %d", curr_thread);
        
    }    

    return 0;
}