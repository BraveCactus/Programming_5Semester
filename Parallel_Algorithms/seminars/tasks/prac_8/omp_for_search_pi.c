#include <stdio.h>
#include <math.h>
#include <omp.h>

double circle_equation(double x) {
    if (x < 0.0) x = 0.0;
    if (x > 2.0) x = 2.0;
    return sqrt(4 - x * x);
}

double calculate_area(double x, double h) {
    return (circle_equation(x) + circle_equation(x + h)) / 2.0 * h; 
}

double calculate_pi(int n) {
    double pi = 0;
    double h = 2.0 / n;
    
    #pragma omp parallel
    {
        #pragma omp for
        for (int i = 0; i < n; i++) {
            double curr_pos = i * h;
            double area = calculate_area(curr_pos, h);            
            pi += area;
        }
    }
    
    return pi;
}

int main() {
    int n = 0;
    double global_pi = 0.0;
    FILE *file = fopen("N.dat", "r");       
    
    if (file != NULL) {
        fscanf(file, "%d", &n);
        printf("Number from file: %d\n", n);
        fclose(file);
    } else {
        printf("File opening error\n");
        return 1;
    }

    global_pi = calculate_pi(n);
    
    printf("Pi = %f\n", global_pi);        

    return 0;
}