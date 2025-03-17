#ifndef HITTABLE_H
#define HITTABLE_H

#include "aabb.h"
#include "ray.h"
#include "vec3.h"
#include <stdbool.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

/* Forward declaration */
typedef struct material material;

/* Hit record */
typedef struct hit_record {
    point3 p;
    vec3 normal;
    material *mat;
    double t;
    double u;
    double v;
    bool front_face;
} hit_record;

static inline void hit_record_set_face_normal(hit_record *rec, const ray *r, const vec3 outward_normal) {
    rec->front_face = (vec3_dot(&r->dir, &outward_normal) < 0);
    rec->normal = rec->front_face ? outward_normal : vec3_neg(&outward_normal);
}

/* Abstract hittable type using a virtual function table */
typedef struct hittable hittable;

typedef struct hittable_vtable {
    bool (*hit)(const hittable *self, const ray *r, interval t, hit_record *rec);
    aabb (*bounding_box)(const hittable *self);
    double (*pdf_value)(const hittable *self, const point3 origin, const vec3 direction);
    vec3 (*random)(const hittable *self, const point3 origin);
    void (*destroy)(hittable *self);
} hittable_vtable;

struct hittable {
    hittable_vtable *vptr;
};

static inline bool hittable_hit(const hittable *h, const ray *r, interval t, hit_record *rec) {
    return h->vptr->hit(h, r, t, rec);
}

static inline aabb hittable_bounding_box(const hittable *h) {
    return h->vptr->bounding_box(h);
}

static inline double hittable_pdf_value(const hittable *h, const point3 origin, const vec3 direction) {
    return h->vptr->pdf_value ? h->vptr->pdf_value(h, origin, direction) : 0.0;
}

static inline vec3 hittable_random(const hittable *h, const point3 origin) {
    return h->vptr->random ? h->vptr->random(h, origin) : vec3_create(1, 0, 0);
}

/* --- Translate --- */

typedef struct translate {
    hittable base;  /* Base must be first */
    hittable *object;
    vec3 offset;
    aabb bbox;
} translate;

static bool translate_hit(const hittable *self, const ray *r, interval t, hit_record *rec) {
    const translate *tr = (const translate *)self;
    /* Create a moved ray by subtracting the offset */
    ray moved_r = ray_create(vec3_sub(&r->orig, &tr->offset), r->dir, r->tm);
    if (!hittable_hit(tr->object, &moved_r, t, rec))
        return false;
    rec->p = vec3_add(&rec->p, &tr->offset);
    return true;
}

static aabb translate_bounding_box(const hittable *self) {
    const translate *tr = (const translate *)self;
    return tr->bbox;
}

static hittable_vtable translate_vtable = {
    .hit = translate_hit,
    .bounding_box = translate_bounding_box,
    .pdf_value = NULL,
    .random = NULL,
    .destroy = NULL
};

static inline translate *translate_create(hittable *object, vec3 offset) {
    translate *tr = (translate *)malloc(sizeof(translate));
    tr->base.vptr = &translate_vtable;
    tr->object = object;
    tr->offset = offset;
    {
        aabb obj_box = hittable_bounding_box(object);
        /* Shift each axis interval by the corresponding offset */
        tr->bbox.x = interval_add(&obj_box.x, offset.x);
        tr->bbox.y = interval_add(&obj_box.y, offset.y);
        tr->bbox.z = interval_add(&obj_box.z, offset.z);
    }
    return tr;
}

/* --- Rotate_Y --- */

typedef struct rotate_y {
    hittable base;  /* Base must be first */
    hittable *object;
    double sin_theta;
    double cos_theta;
    aabb bbox;
} rotate_y;

static bool rotate_y_hit(const hittable *self, const ray *r, interval t, hit_record *rec) {
    const rotate_y *ry = (const rotate_y *)self;
    /* Rotate ray to object space */
    vec3 orig = r->orig;
    vec3 dir = r->dir;
    double new_x = ry->cos_theta * orig.x - ry->sin_theta * orig.z;
    double new_z = ry->sin_theta * orig.x + ry->cos_theta * orig.z;
    vec3 rotated_orig = vec3_create(new_x, orig.y, new_z);
    new_x = ry->cos_theta * dir.x - ry->sin_theta * dir.z;
    new_z = ry->sin_theta * dir.x + ry->cos_theta * dir.z;
    vec3 rotated_dir = vec3_create(new_x, dir.y, new_z);
    ray rotated_r = ray_create(rotated_orig, rotated_dir, r->tm);
    if (!hittable_hit(ry->object, &rotated_r, t, rec))
        return false;
    /* Rotate hit record back to world space */
    orig = rec->p;
    new_x = ry->cos_theta * orig.x + ry->sin_theta * orig.z;
    new_z = -ry->sin_theta * orig.x + ry->cos_theta * orig.z;
    rec->p = vec3_create(new_x, orig.y, new_z);
    dir = rec->normal;
    new_x = ry->cos_theta * dir.x + ry->sin_theta * dir.z;
    new_z = -ry->sin_theta * dir.x + ry->cos_theta * dir.z;
    rec->normal = vec3_create(new_x, dir.y, new_z);
    return true;
}

static aabb rotate_y_bounding_box(const hittable *self) {
    const rotate_y *ry = (const rotate_y *)self;
    return ry->bbox;
}

static hittable_vtable rotate_y_vtable = {
    .hit = rotate_y_hit,
    .bounding_box = rotate_y_bounding_box,
    .pdf_value = NULL,
    .random = NULL,
    .destroy = NULL
};

static inline rotate_y *rotate_y_create(hittable *object, double angle) {
    rotate_y *ry = (rotate_y *)malloc(sizeof(rotate_y));
    ry->base.vptr = &rotate_y_vtable;
    ry->object = object;
    double radians = degrees_to_radians(angle);
    ry->sin_theta = sin(radians);
    ry->cos_theta = cos(radians);

    aabb obj_box = hittable_bounding_box(object);
    vec3 min_point = vec3_create(INFINITY, INFINITY, INFINITY);
    vec3 max_point = vec3_create(-INFINITY, -INFINITY, -INFINITY);
    for (int i = 0; i < 2; i++) {
        double x = (i == 1) ? obj_box.x.max : obj_box.x.min;
        for (int j = 0; j < 2; j++) {
            double y = (j == 1) ? obj_box.y.max : obj_box.y.min;
            for (int k = 0; k < 2; k++) {
                double z = (k == 1) ? obj_box.z.max : obj_box.z.min;
                double new_x = ry->cos_theta * x + ry->sin_theta * z;
                double new_z = -ry->sin_theta * x + ry->cos_theta * z;
                vec3 tester = vec3_create(new_x, y, new_z);
                if (tester.x < min_point.x) min_point.x = tester.x;
                if (tester.y < min_point.y) min_point.y = tester.y;
                if (tester.z < min_point.z) min_point.z = tester.z;
                if (tester.x > max_point.x) max_point.x = tester.x;
                if (tester.y > max_point.y) max_point.y = tester.y;
                if (tester.z > max_point.z) max_point.z = tester.z;
            }
        }
    }
    ry->bbox = aabb_create_from_points(&min_point, &max_point);
    return ry;
}

#endif /* HITTABLE_H */
