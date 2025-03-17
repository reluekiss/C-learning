#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "ray.h"
#include "vec3.h"
#include "aabb.h"
#include "onb.h"
#include "common.h"  // For pi, infinity, random_double(), etc.
#include <math.h>
#include <stdlib.h>

/* Sphere structure: supports both stationary and moving spheres.
   For a stationary sphere, set center.dir = vec3_create(0,0,0). */
typedef struct sphere {
    hittable base;      // Must be first for polymorphism.
    ray center;         // Represents center: center.at(time) gives center at given time.
    double radius;
    material *mat;
    aabb bbox;
} sphere;

/* Helper: Compute sphere UV coordinates for a point on unit sphere.
   p is assumed to be a unit vector. */
static void get_sphere_uv(const point3 p, double *u, double *v) {
    double theta = acos(-p.y);
    double phi = atan2(-p.z, p.x) + pi;
    *u = phi / (2 * pi);
    *v = theta / pi;
}

/* Helper: Generate a random vector toward the sphere.
   Returns a vector pointing from the origin toward a random point on the sphere
   as seen from a distance with the given squared distance. */
static vec3 random_to_sphere(double radius, double distance_squared) {
    double r1 = random_double();
    double r2 = random_double();
    double z = 1 + r2 * (sqrt(1 - (radius * radius) / distance_squared) - 1);
    double phi = 2 * pi * r1;
    double x = cos(phi) * sqrt(1 - z * z);
    double y = sin(phi) * sqrt(1 - z * z);
    return vec3_create(x, y, z);
}

/* Sphere hit function */
static bool sphere_hit(const hittable *self, const ray *r, interval t, hit_record *rec) {
    const sphere *s = (const sphere *)self;
    // Get current center at ray time.
    point3 current_center = ray_at(&s->center, r->tm);
    // Compute vector from ray origin to sphere center.
    vec3 oc = vec3_sub(&current_center, &r->orig);
    double a = vec3_length_squared(&r->dir);
    double h = vec3_dot(&r->dir, &oc);
    double c = vec3_length_squared(&oc) - s->radius * s->radius;
    double discriminant = h * h - a * c;
    if (discriminant < 0)
        return false;
    double sqrtd = sqrt(discriminant);
    
    // Find the nearest root in the acceptable range.
    double root = (h - sqrtd) / a;
    if (!interval_surrounds(&t, root)) {
        root = (h + sqrtd) / a;
        if (!interval_surrounds(&t, root))
            return false;
    }
    
    rec->t = root;
    rec->p = ray_at(r, rec->t);
    vec3 temp = vec3_sub(&rec->p, &current_center);
    vec3 outward_normal = vec3_div(&temp, s->radius);
    hit_record_set_face_normal(rec, r, outward_normal);
    get_sphere_uv(outward_normal, &rec->u, &rec->v);
    rec->mat = s->mat;
    return true;
}

/* Sphere bounding box: returns precomputed bbox */
static aabb sphere_bounding_box(const hittable *self) {
    const sphere *s = (const sphere *)self;
    return s->bbox;
}

/* Sphere PDF value (only for stationary spheres).
   Assumes sphere->center is stationary (i.e. center.dir is zero). */
static double sphere_pdf_value_h(const hittable *self, const point3 origin, const vec3 direction) {
    const sphere *s = (const sphere *)self;
    hit_record rec;
    ray temp_ray = ray_create(origin, direction, 0);
    interval t_interval = {0.001, infinity};
    if (!sphere_hit(self, &temp_ray, t_interval, &rec))
        return 0;
    /* Use temporary variables for the sphere center and the difference vector */
    vec3 center_origin = s->center.orig;  
    vec3 diff = vec3_sub(&center_origin, &origin);
    double dist_squared = vec3_length_squared(&diff);
    double cos_theta_max = sqrt(1 - (s->radius * s->radius) / dist_squared);
    double solid_angle = 2 * pi * (1 - cos_theta_max);
    return 1.0 / solid_angle;
}

/* Sphere random function (only for stationary spheres).
   Returns a vector from origin toward a random point on the sphere. */
static vec3 sphere_random(const hittable *self, const point3 origin) {
    const sphere *s = (const sphere *)self;
    vec3 direction = vec3_sub(&s->center.orig, &origin);
    double distance_squared = vec3_length_squared(&direction);
    onb uvw;
    onb_init(&uvw, &direction);
    vec3 random = random_to_sphere(s->radius, distance_squared);
    return onb_transform(&uvw, &random);
}

/* Virtual function table for sphere */
static hittable_vtable sphere_vtable = {
    .hit = sphere_hit,
    .bounding_box = sphere_bounding_box,
    .pdf_value = sphere_pdf_value_h,
    .random = sphere_random,
    .destroy = NULL  // Use default destroy if needed.
};

/* Create a stationary sphere.
   static_center: sphere center.
   radius: sphere radius (non-negative).
   mat: pointer to material.
   Returns pointer to sphere (as hittable*). */
static sphere *sphere_create_stationary(const point3 static_center, double radius, material *mat) {
    sphere *s = (sphere *)malloc(sizeof(sphere));
    if (!s) return NULL;
    s->base.vptr = &sphere_vtable;
    // For a stationary sphere, set center.dir = (0,0,0)
    s->center = ray_create(static_center, vec3_create(0, 0, 0), 0);
    s->radius = (radius < 0) ? 0 : radius;
    s->mat = mat;
    vec3 rvec = vec3_create(radius, radius, radius);
    // Compute bounding box from (center - rvec) to (center + rvec)
    point3 temp1 = vec3_sub(&static_center, &rvec);
    point3 temp2 = vec3_add(&static_center, &rvec);
    s->bbox = aabb_create_from_points(&temp1, &temp2);
    return s;
}

/* Create a moving sphere.
   center1: center at time 0.
   center2: center at time 1.
   radius: sphere radius.
   mat: pointer to material.
   Returns pointer to sphere (as hittable*). */
static sphere *sphere_create_moving(const point3 center1, const point3 center2, double radius, material *mat) {
    sphere *s = (sphere *)malloc(sizeof(sphere));
    if (!s) return NULL;
    s->base.vptr = &sphere_vtable;
    s->center = ray_create(center1, vec3_sub(&center2, &center1), 0);
    s->radius = (radius < 0) ? 0 : radius;
    s->mat = mat;
    vec3 rvec = vec3_create(radius, radius, radius);
    point3 center_at0 = ray_at(&s->center, 0);
    point3 center_at1 = ray_at(&s->center, 1);
    point3 temp1 = vec3_sub(&center_at0, &rvec);
    point3 temp2 = vec3_add(&center_at0, &rvec);
    aabb box1 = aabb_create_from_points(&temp1, &temp2);
    
    point3 temp3 = vec3_sub(&center_at1, &rvec);
    point3 temp4 = vec3_add(&center_at1, &rvec);
    aabb box2 = aabb_create_from_points(&temp3, &temp4);
    
    s->bbox = aabb_enclose(&box1, &box2);
    return s;
}

#endif
