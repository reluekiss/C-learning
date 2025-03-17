#ifndef VEC3_H
#define VEC3_H

#include "common.h" 
#include <math.h>
#include <stdbool.h>

#define PI 3.1415926535897932385

typedef struct {
    double x, y, z;
} vec3;

static inline vec3 vec3_create(double x, double y, double z) {
    vec3 v = {  x, y, z };
    return v;
}

static inline vec3 vec3_neg(const vec3 *v) {
    return (vec3){-v->x, -v->y, -v->z};
}

static inline void vec3_add_inplace(vec3 *u, const vec3 *v) {
    u->x += v->x;
    u->y += v->y;
    u->z += v->z;
}

static inline void vec3_mul_inplace(vec3 *v, double t) {
    v->x *= t;
    v->y *= t;
    v->z *= t;
}

static inline void vec3_div_inplace(vec3 *v, double t) {
    vec3_mul_inplace(v, 1.0 / t);
}

static inline double vec3_length_squared(const vec3 *v) {
    return v->x*v->x + v->y*v->y + v->z*v->z;
}

static inline double vec3_length(const vec3 *v) {
    return sqrt(vec3_length_squared(v));
}

static inline bool vec3_near_zero(const vec3 *v) {
    double s = 1e-8;
    return (fabs(v->x) < s) && (fabs(v->y) < s) && (fabs(v->z) < s);
}

static inline vec3 vec3_random(void) {
    return (vec3){random_double(), random_double(), random_double()};
}

static inline vec3 vec3_random_range(double min, double max) {
    return (vec3){random_double_range(min, max),
                       random_double_range(min, max),
                       random_double_range(min, max)};
}

/* Alias for clarity */
typedef vec3 point3;

/* Vector Utility Functions */
static inline vec3 vec3_add(const vec3 *u, const vec3 *v) {
    return (vec3){u->x + v->x,
                       u->y + v->y,
                       u->z + v->z};
}

static inline vec3 vec3_sub(const vec3 *u, const vec3 *v) {
    return (vec3){u->x - v->x,
                       u->y - v->y,
                       u->z - v->z};
}

static inline vec3 vec3_mul(const vec3 *u, const vec3 *v) {
    return (vec3){u->x * v->x,
                       u->y * v->y,
                       u->z * v->z};
}

static inline vec3 vec3_scale(const vec3 *v, double t) {
    return (vec3){t * v->x, t * v->y, t * v->z};
}

static inline vec3 vec3_div(const vec3 *v, double t) {
    return vec3_scale(v, 1.0 / t);
}

static inline double vec3_dot(const vec3 *u, const vec3 *v) {
    return u->x*v->x + u->y*v->y + u->z*v->z;
}

static inline vec3 vec3_cross(const vec3 *u, const vec3 *v) {
    return (vec3){u->y * v->z - u->z * v->y,
                       u->z * v->x - u->x * v->z,
                       u->x * v->y - u->y * v->x};
}

static inline vec3 vec3_unit_vector(const vec3 *v) {
    return vec3_div(v, vec3_length(v));
}

static inline vec3 vec3_random_in_unit_disk(void) {
    vec3 p;
    do {
        p = (vec3){random_double_range(-1, 1),
                        random_double_range(-1, 1),
                        0};
    } while (vec3_length_squared(&p) >= 1);
    return p;
}

static inline vec3 vec3_random_unit_vector(void) {
    vec3 p;
    double lensq;
    do {
        p = vec3_random_range(-1, 1);
        lensq = vec3_length_squared(&p);
    } while (lensq <= 1e-160 || lensq > 1.0);
    return vec3_div(&p, sqrt(lensq));
}

static inline vec3 vec3_random_on_hemisphere(const vec3 *normal) {
    vec3 on_unit_sphere = vec3_random_unit_vector();
    if (vec3_dot(&on_unit_sphere, normal) > 0.0)
        return on_unit_sphere;
    else
        return vec3_neg(&on_unit_sphere);
}

static inline vec3 vec3_reflect(const vec3 *v, const vec3 *n) {
    vec3 scaled = vec3_scale(n, 2 * vec3_dot(v, n));
    return vec3_sub(v, &scaled);
}

// Corrected vec3_refract
static inline vec3 vec3_refract(const vec3 *uv, const vec3 *n, double etai_over_etat) {
    vec3 neg_uv = vec3_neg(uv);
    double cos_theta = fmin(vec3_dot(&neg_uv, n), 1.0);
    vec3 scaled_n = vec3_scale(n, cos_theta);
    vec3 temp = vec3_add(uv, &scaled_n);
    vec3 r_out_perp = vec3_scale(&temp, etai_over_etat);
    double r_out_perp_len_sq = vec3_length_squared(&r_out_perp);
    vec3 r_out_parallel = vec3_scale(n, -sqrt(fabs(1.0 - r_out_perp_len_sq)));
    return vec3_add(&r_out_perp, &r_out_parallel);
}

static inline vec3 vec3_random_cosine_direction(void) {
    double r1 = random_double();
    double r2 = random_double();
    double phi = 2 * PI * r1;
    double x = cos(phi) * sqrt(r2);
    double y = sin(phi) * sqrt(r2);
    double z = sqrt(1 - r2);
    return (vec3){x, y, z};
}

#endif /* VEC3_H */
