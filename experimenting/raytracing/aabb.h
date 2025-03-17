#ifndef AABB_H
#define AABB_H

#include "interval.h"
#include "vec3.h"
#include "ray.h"
#include <math.h>
#include <stdbool.h>

/* Axis-Aligned Bounding Box */
typedef struct {
    interval x;
    interval y;
    interval z;
} aabb;

/* Helper to access vec3 components by index */
static inline double vec3_get(const vec3 *v, int axis) {
    switch (axis) {
        case 0: return v->x;
        case 1: return v->y;
        case 2: return v->z;
        default: return 0.0;
    }
}

/* Ensure each interval has at least a minimum size */
static inline void aabb_pad_to_minimums(aabb *box) {
    double delta = 0.0001;
    if (interval_size(&box->x) < delta)
        box->x = interval_expand(&box->x, delta);
    if (interval_size(&box->y) < delta)
        box->y = interval_expand(&box->y, delta);
    if (interval_size(&box->z) < delta)
        box->z = interval_expand(&box->z, delta);
}

/* Construct an AABB from three intervals */
static inline aabb aabb_create_from_intervals(interval ix, interval iy, interval iz) {
    aabb box = { ix, iy, iz };
    aabb_pad_to_minimums(&box);
    return box;
}

/* Construct an AABB from two points (order-independent) */
static inline aabb aabb_create_from_points(const vec3 *a, const vec3 *b) {
    interval ix = (a->x <= b->x) ? interval_create(a->x, b->x)
                                 : interval_create(b->x, a->x);
    interval iy = (a->y <= b->y) ? interval_create(a->y, b->y)
                                 : interval_create(b->y, a->y);
    interval iz = (a->z <= b->z) ? interval_create(a->z, b->z)
                                 : interval_create(b->z, a->z);
    return aabb_create_from_intervals(ix, iy, iz);
}

/* Construct an AABB that tightly encloses two AABBs */
static inline aabb aabb_enclose(const aabb *box0, const aabb *box1) {
    interval ix = interval_enclose(box0->x, box1->x);
    interval iy = interval_enclose(box0->y, box1->y);
    interval iz = interval_enclose(box0->z, box1->z);
    return aabb_create_from_intervals(ix, iy, iz);
}

/* Returns a pointer to the interval for the given axis (0: x, 1: y, 2: z) */
static inline const interval* aabb_axis_interval(const aabb *box, int axis) {
    switch (axis) {
        case 0: return &box->x;
        case 1: return &box->y;
        case 2: return &box->z;
        default: return &box->x;
    }
}

/* Test if a ray hits the AABB.
   't' is a copy of the ray interval [t.min, t.max] and is updated locally. */
static inline bool aabb_hit(const aabb *box, const ray *r, interval t) {
    for (int axis = 0; axis < 3; axis++) {
        const interval *ax = aabb_axis_interval(box, axis);
        double origin = vec3_get(&r->orig, axis);
        double dir = vec3_get(&r->dir, axis);
        double inv_d = 1.0 / dir;
        double t0 = (ax->min - origin) * inv_d;
        double t1 = (ax->max - origin) * inv_d;
        if (t0 > t1) {
            double tmp = t0;
            t0 = t1;
            t1 = tmp;
        }
        if (t0 > t.min) t.min = t0;
        if (t1 < t.max) t.max = t1;
        if (t.max <= t.min)
            return false;
    }
    return true;
}

/* Return the index (0, 1, or 2) of the longest axis of the AABB */
static inline int aabb_longest_axis(const aabb *box) {
    double size_x = interval_size(&box->x);
    double size_y = interval_size(&box->y);
    double size_z = interval_size(&box->z);
    if (size_x > size_y)
        return (size_x > size_z) ? 0 : 2;
    else
        return (size_y > size_z) ? 1 : 2;
}

/* Return a new AABB shifted by the given offset */
static inline aabb aabb_add(const aabb *box, const vec3 offset) {
    aabb result;
    result.x = interval_add(&box->x, offset.x);
    result.y = interval_add(&box->y, offset.y);
    result.z = interval_add(&box->z, offset.z);
    return result;
}

/* Global constants */
static const aabb aabb_empty = {
    .x = { .min = INFINITY,  .max = -INFINITY },
    .y = { .min = INFINITY,  .max = -INFINITY },
    .z = { .min = INFINITY,  .max = -INFINITY }
};

static const aabb aabb_universe = {
    .x = { .min = -INFINITY, .max = INFINITY },
    .y = { .min = -INFINITY, .max = INFINITY },
    .z = { .min = -INFINITY, .max = INFINITY }
};

#endif /* AABB_H */
