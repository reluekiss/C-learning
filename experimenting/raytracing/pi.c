#include <stdio.h>
#include <stdlib.h>

inline static double random_double(double min, double max) {
    return min + (max-min)*((float)rand() / (RAND_MAX + 1));
}

int main() {
    int inside_circle = 0;
    int inside_circle_stratified = 0;
    int sqrt_N = 1000;

    for (int i = 0; i < sqrt_N; i++) {
        for (int j = 0; j < sqrt_N; j++) {
            double x = random_double(-1, 1); 
            double y = random_double(-1, 1); 
            if (x*x + y*y < 1)
                inside_circle++;

            x = 2*((i + random_double(0, 1)) / sqrt_N) - 1;
            y = 2*((j + random_double(0, 1)) / sqrt_N) - 1;
            if (x*x + y*y < 1)
                inside_circle_stratified++;
        }
    }
    printf("Regular estimate of pi: %f\nStratified estimate: %f\n",
           (4.0 * inside_circle) / (sqrt_N*sqrt_N), (4.0 * inside_circle_stratified) / (sqrt_N*sqrt_N));
}
