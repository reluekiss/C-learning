#ifndef COLOR_H
#define COLOR_H

#include "interval.h"
#include "vec3.h"
#include <math.h>
#include <stdio.h>

typedef vec3 color;

static inline double linear_to_gamma(double linear_component) {
    return linear_component > 0 ? sqrt(linear_component) : 0;
}

static inline void write_color(FILE *out, color pixel_color) {
    double r = pixel_color.x;
    double g = pixel_color.y;
    double b = pixel_color.z;

    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    static const interval intensity = { 0.000, 0.999 };
    int rbyte = (int)(256 * interval_clamp(&intensity, r));
    int gbyte = (int)(256 * interval_clamp(&intensity, g));
    int bbyte = (int)(256 * interval_clamp(&intensity, b));

    fprintf(out, "%d %d %d\n", rbyte, gbyte, bbyte);
}

#endif /* COLOR_H */
