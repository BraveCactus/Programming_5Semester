#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: exefile npoints\n");
        return 1;
    }
    
    int N = atoi(argv[1]);
    if (N == 0) {
        printf("Set N to 1000\n");
        N = 1000;
    } else {
        printf("Set N to %d\n", N);
    }

    int count = 0;
    double h = 1.0 / N;
    double tau = 0.5 * (h * h);
    double eps = 1e-6;

    double *u = (double*)malloc((N + 1) * sizeof(double));
    double *unew = (double*)malloc((N + 1) * sizeof(double));
    double *proc_max_array = (double*)malloc(omp_get_num_procs() * sizeof(double));
    
    if (u == NULL || unew == NULL || proc_max_array == NULL) {
        printf("Can't allocate memory\n");
        if (u) free(u);
        if (unew) free(unew);
        if (proc_max_array) free(proc_max_array);
        return 1;
    }

    for (int i = 0; i <= N; i++) {
        u[i] = 0.0;
        unew[i] = 0.0;
    }
    
    unew[0] = u[0] = 1.0;
    unew[N] = u[N] = 0.0;

    double coef = tau / (h * h);
    double maxdelta;
    
    for (int i = 0; i < omp_get_num_procs(); i++) {
        proc_max_array[i] = 0.0;
    }

    for (;;) {
        maxdelta = 0;

        #pragma omp parallel
        {
            #pragma omp for
            for (int i = 1; i < N; i++) {
                unew[i] = u[i] + coef * (u[i-1] - 2 * u[i] + u[i+1]);
                double delta = fabs(unew[i] - u[i]);
                if (delta > proc_max_array[omp_get_thread_num()]) {
                    proc_max_array[omp_get_thread_num()] = delta;
                }
            }

            #pragma omp single
            {
                maxdelta = proc_max_array[0];
                for (int i = 1; i < omp_get_num_procs(); i++) {
                    if (proc_max_array[i] > maxdelta) {
                        maxdelta = proc_max_array[i];
                    }
                }
                
                // Reset proc_max_array for next iteration
                for (int i = 0; i < omp_get_num_procs(); i++) {
                    proc_max_array[i] = 0.0;
                }
            }
        }

        if (maxdelta < eps) break;
        count++;

        // Copy unew to u
        #pragma omp parallel for
        for (int i = 1; i < N; i++) {
            u[i] = unew[i];
        }
    }

    printf("%d steps\n", count);
    
    FILE* file = fopen("seqres2", "w");
    if (file == NULL) {
        printf("Can't open file\n");
        free(u);
        free(unew);
        free(proc_max_array);
        return 1;
    }
    
    for (int i = 0; i <= N; i++) {
        fprintf(file, "%f\n", unew[i]);
    }
        
    fclose(file);
    
    free(u);
    free(unew);
    free(proc_max_array);
    
    return 0;  
}