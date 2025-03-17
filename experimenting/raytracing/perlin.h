#ifndef PERLIN_H
#define PERLIN_H

#include "common.h"
#include "vec3.h"
#include <math.h>
#include <stdlib.h>

#define PERLIN_POINT_COUNT 256

typedef struct perlin {
    vec3 randvec[PERLIN_POINT_COUNT];
    int perm_x[PERLIN_POINT_COUNT];
    int perm_y[PERLIN_POINT_COUNT];
    int perm_z[PERLIN_POINT_COUNT];
} perlin;

static inline void perlin_permute(int *p, int n) {
    for (int i = n - 1; i > 0; i--) {
        int target = random_int(0, i);
        int tmp = p[i];
        p[i] = p[target];
        p[target] = tmp;
    }
}

static inline void perlin_generate_perm(int *p) {
    for (int i = 0; i < PERLIN_POINT_COUNT; i++)
        p[i] = i;
    perlin_permute(p, PERLIN_POINT_COUNT);
}

static inline double perlin_interp(const vec3 c[2][2][2], double u, double v, double w) {
    double uu = u * u * (3 - 2 * u);
    double vv = v * v * (3 - 2 * v);
    double ww = w * w * (3 - 2 * w);
    double accum = 0.0;

    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            for (int k = 0; k < 2; k++) {
                vec3 weight_v = (vec3){u - i, v - j, w - k};
                double weight = (i * uu + (1 - i) * (1 - uu)) *
                                (j * vv + (1 - j) * (1 - vv)) *
                                (k * ww + (1 - k) * (1 - ww));
                accum += weight * vec3_dot(&c[i][j][k], &weight_v);
            }
    return accum;
}

static inline void perlin_init(perlin *p) {
    for (int i = 0; i < PERLIN_POINT_COUNT; i++) {
        vec3 temp = vec3_random_range(-1, 1);
        p->randvec[i] = vec3_unit_vector(&temp);
    }
    perlin_generate_perm(p->perm_x);
    perlin_generate_perm(p->perm_y);
    perlin_generate_perm(p->perm_z);
}

static inline double perlin_noise(const perlin *p, const vec3 *pt) {
    double u = pt->x - floor(pt->x);
    double v = pt->y - floor(pt->y);
    double w = pt->z - floor(pt->z);

    int i = (int)floor(pt->x);
    int j = (int)floor(pt->y);
    int k = (int)floor(pt->z);
    vec3 c[2][2][2];

    for (int di = 0; di < 2; di++)
        for (int dj = 0; dj < 2; dj++)
            for (int dk = 0; dk < 2; dk++) {
                int index = p->perm_x[(i + di) & 255] ^
                            p->perm_y[(j + dj) & 255] ^
                            p->perm_z[(k + dk) & 255];
                c[di][dj][dk] = p->randvec[index];
            }

    return perlin_interp(c, u, v, w);
}

static inline double perlin_turb(const perlin *p, const vec3 *pt, int depth) {
    double accum = 0.0;
    vec3 temp = *pt;
    double weight = 1.0;

    for (int i = 0; i < depth; i++) {
        accum += weight * perlin_noise(p, &temp);
        weight *= 0.5;
        temp = vec3_scale(&temp, 2.0);
    }

    return fabs(accum);
}

#endif
