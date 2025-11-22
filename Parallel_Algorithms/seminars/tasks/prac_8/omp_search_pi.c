#include <omp.h>
#include <stdio.h>
#include <math.h>

double circle_equation(double x) {
    if (x < 0.0) x = 0.0;
    if (x > 2.0) x = 2.0;
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

int main() {
    double pi = 0;
    int N = 0;

    FILE *file = fopen("N.dat", "r");       
    if (file != NULL) {
        fscanf(file, "%d", &N);        
        printf("Number from file: %d\n", N);
        fclose(file);
    } else {
        printf("File opening error\n");
        return 1;
    }

    int num_procs = omp_get_num_procs();
    omp_set_num_threads(num_procs);
    
    #pragma omp parallel shared(N) reduction(+ : pi)
    {
        int threads_amount = omp_get_num_threads();
        int curr_thread = omp_get_thread_num();
        
        int part_size = N / threads_amount;
        int remains = N % threads_amount;

        int start, end;
        if (curr_thread != threads_amount - 1) {
            start = curr_thread * part_size;
            end = start + part_size;
        } else {
            start = curr_thread * part_size;
            end = start + part_size + remains;
        }
        
        double part_of_pi = calculate_pi_part(N, start, end);
        pi = part_of_pi;
        
        printf("Thread %d: calculated from %d to %d = %f\n", 
               curr_thread, start, end, part_of_pi);
    }

    printf("Pi = %f\n", pi);

    return 0;
}