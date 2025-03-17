#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"
#include "hittable_list.h"
#include "vec3.h"
#include "aabb.h"
#include "ray.h"
#include "common.h"
#include <math.h>
#include <stdlib.h>

/* Quad structure (implements hittable) */
typedef struct quad {
    hittable base;    /* Base must be first */
    point3 Q;
    vec3 u, v;
    vec3 w;           /* Auxiliary vector: w = n / dot(n,n) */
    material *mat;
    aabb bbox;
    vec3 normal;
    double D;
    double area;
} quad;

/* Compute the bounding box of the quad (from its 4 vertices) */
static void quad_set_bounding_box(quad *q) {
    point3 p1 = q->Q;
    point3 temp = vec3_add(&q->u, &q->v);
    point3 p2 = vec3_add(&q->Q, &temp);
    aabb bbox1 = aabb_create_from_points(&p1, &p2);
    point3 p3 = vec3_add(&q->Q, &q->u);
    point3 p4 = vec3_add(&q->Q, &q->v);
    aabb bbox2 = aabb_create_from_points(&p3, &p4);
    q->bbox = aabb_enclose(&bbox1, &bbox2);
}

/* Hit function for quad */
static bool quad_hit(const hittable *self, const ray *r, interval t, hit_record *rec) {
    const quad *q = (const quad *)self;
    double denom = vec3_dot(&q->normal, &r->dir);
    if (fabs(denom) < 1e-8)
        return false;
    double t_val = (q->D - vec3_dot(&q->normal, &r->orig)) / denom;
    if (!interval_contains(&t, t_val))
        return false;
    point3 hit_pt = ray_at(r, t_val);
    vec3 planar_vec = vec3_sub(&hit_pt, &q->Q);
    vec3 temp = vec3_cross(&planar_vec, &q->v);
    double alpha = vec3_dot(&q->w, &temp);
    temp = vec3_cross(&q->u, &planar_vec);
    double beta  = vec3_dot(&q->w, &temp);
    /* Check if (alpha,beta) lies within [0,1] */
    if (alpha < 0 || alpha > 1 || beta < 0 || beta > 1)
        return false;
    rec->t = t_val;
    rec->p = hit_pt;
    rec->mat = q->mat;
    hit_record_set_face_normal(rec, r, q->normal);
    rec->u = alpha;
    rec->v = beta;
    return true;
}

/* Return quad's bounding box */
static aabb quad_bounding_box(const hittable *self) {
    const quad *q = (const quad *)self;
    return q->bbox;
}

/* PDF value for the quad */
static double quad_pdf_value(const hittable *self, const point3 origin, const vec3 direction) {
    hit_record rec;
    ray r = ray_create(origin, direction, 0);
    interval t_interval = {0.001, infinity};
    if (!quad_hit(self, &r, t_interval, &rec))
        return 0;
    double distance_sq = rec.t * rec.t * vec3_length_squared(&direction);
    double cosine = fabs(vec3_dot(&direction, &rec.normal) / vec3_length(&direction));
    return distance_sq / (cosine * ((quad *)self)->area);
}

/* Generate a random direction from origin toward the quad */
static vec3 quad_random(const hittable *self, const point3 origin) {
    const quad *q = (const quad *)self;
    double rand_u = random_double();
    double rand_v = random_double();
    vec3 temp = vec3_scale(&q->u, rand_u);
    vec3 temp1 = vec3_scale(&q->v, rand_v);
    temp = vec3_add(&temp, &temp1);
    point3 random_pt = vec3_add(&q->Q, &temp);
    return vec3_sub(&random_pt, &origin);
}

/* Clean up quad */
static void quad_destroy(hittable *self) {
    free(self);
}

/* Quad virtual function table */
static hittable_vtable quad_vtable = {
    .hit = quad_hit,
    .bounding_box = quad_bounding_box,
    .pdf_value = quad_pdf_value,
    .random = quad_random,
    .destroy = quad_destroy
};

/* Create a new quad */
static quad *quad_create(const point3 Q, const vec3 u, const vec3 v, material *mat) {
    quad *q = (quad *)malloc(sizeof(quad));
    if (!q) return NULL;
    q->base.vptr = &quad_vtable;
    q->Q = Q;
    q->u = u;
    q->v = v;
    q->mat = mat;
    /* Compute plane normal from cross(u,v) */
    vec3 n = vec3_cross(&u, &v);
    q->normal = vec3_unit_vector(&n);
    q->D = vec3_dot(&q->normal, &Q);
    double n_dot = vec3_length_squared(&n);
    q->w = vec3_scale(&n, 1.0 / n_dot);
    q->area = sqrt(n_dot);
    quad_set_bounding_box(q);
    return q;
}

/* Build a box (six-sided) from two opposite vertices */
static hittable_list *box(const point3 a, const point3 b, material *mat) {
    hittable_list *sides = hittable_list_create();
    if (!sides) return NULL;
    point3 min_pt = vec3_create(fmin(a.x, b.x), fmin(a.y, b.y), fmin(a.z, b.z));
    point3 max_pt = vec3_create(fmax(a.x, b.x), fmax(a.y, b.y), fmax(a.z, b.z));
    vec3 dx = vec3_create(max_pt.x - min_pt.x, 0, 0);
    vec3 dy = vec3_create(0, max_pt.y - min_pt.y, 0);
    vec3 dz = vec3_create(0, 0, max_pt.z - min_pt.z);
    /* Front */
    hittable_list_add(sides, (hittable *)quad_create(
        vec3_create(min_pt.x, min_pt.y, max_pt.z), dx, dy, mat));
    /* Right (using -dz) */
    hittable_list_add(sides, (hittable *)quad_create(
        vec3_create(max_pt.x, min_pt.y, max_pt.z), vec3_neg(&dz), dy, mat));
    /* Back (using -dx) */
    hittable_list_add(sides, (hittable *)quad_create(
        vec3_create(max_pt.x, min_pt.y, min_pt.z), vec3_neg(&dx), dy, mat));
    /* Left (using dz) */
    hittable_list_add(sides, (hittable *)quad_create(
        vec3_create(min_pt.x, min_pt.y, min_pt.z), dz, dy, mat));
    /* Top (using -dz) */
    hittable_list_add(sides, (hittable *)quad_create(
        vec3_create(min_pt.x, max_pt.y, max_pt.z), dx, vec3_neg(&dz), mat));
    /* Bottom */
    hittable_list_add(sides, (hittable *)quad_create(
        vec3_create(min_pt.x, min_pt.y, min_pt.z), dx, dz, mat));
    return sides;
}

#endif /* QUAD_H */
