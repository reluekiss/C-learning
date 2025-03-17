#ifndef COMMON_H
#define COMMON_H

#include <math.h>
#include <stdlib.h>
#include <float.h>

static const double infinity = INFINITY;
static const double pi = 3.1415926535897932385;

static inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

static inline double random_double(void) {
    /* Returns a random real in [0,1). */
    return rand() / (RAND_MAX + 1.0);
}

static inline double random_double_range(double min, double max) {
    /* Returns a random real in [min, max). */
    return min + (max - min) * random_double();
}

static inline int random_int(int min, int max) {
    /* Returns a random integer in [min, max]. */
    return (int)random_double_range(min, max + 1);
}

#endif /* COMMON_H */
