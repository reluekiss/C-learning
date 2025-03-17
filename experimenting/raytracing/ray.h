#ifndef RAY_H
#define RAY_H

#include "vec3.h"

typedef struct {
    vec3 orig;
    vec3 dir;
    double tm;
} ray;

static inline ray ray_create(vec3 orig, vec3 dir, double tm) {
    ray r = { orig, dir, tm };
    return r;
}

static inline ray ray_create_simple(vec3 orig, vec3 dir) {
    return ray_create(orig, dir, 0.0);
}

static inline vec3 ray_at(const ray *r, double t) {
    vec3 scaled = vec3_scale(&r->dir, t);
    return vec3_add(&r->orig, &scaled);
}

#endif /* RAY_H */
