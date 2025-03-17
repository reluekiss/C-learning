#ifndef PDF_H
#define PDF_H

#include "vec3.h"
#include "hittable_list.h"  // For hittable interface functions
#include "onb.h"
#include "common.h"      // For pi, random_double(), etc.
#include <math.h>
#include <stdlib.h>

/* Base PDF interface */
typedef struct pdf {
    double (*value)(const struct pdf *self, vec3 direction);
    vec3   (*generate)(const struct pdf *self);
    void   (*destroy)(struct pdf *self);
} pdf;

/* ---------------- Sphere PDF ---------------- */
typedef struct sphere_pdf {
    pdf base;
} sphere_pdf;

static double sphere_pdf_value(const pdf *self, vec3 direction) {
    (void)self; (void)direction;
    return 1.0 / (4 * PI);
}

static vec3 sphere_pdf_generate(const pdf *self) {
    (void)self;
    return vec3_random_unit_vector();
}

static void sphere_pdf_destroy(pdf *self) {
    free(self);
}

static pdf *sphere_pdf_create(void) {
    sphere_pdf *sp = (sphere_pdf *)malloc(sizeof(sphere_pdf));
    if (!sp) return NULL;
    sp->base.value    = sphere_pdf_value;
    sp->base.generate = sphere_pdf_generate;
    sp->base.destroy  = sphere_pdf_destroy;
    return (pdf *)sp;
}

/* ---------------- Cosine PDF ---------------- */
typedef struct cosine_pdf {
    pdf base;
    onb uvw;
} cosine_pdf;

static double cosine_pdf_value(const pdf *self, vec3 direction) {
    const cosine_pdf *cp = (const cosine_pdf *)self;
    vec3 unit_dir = vec3_unit_vector(&direction);
    double cosine_theta = vec3_dot(&unit_dir, &cp->uvw.w);
    return fmax(0.0, cosine_theta) / pi;
}

static vec3 cosine_pdf_generate(const pdf *self) {
    const cosine_pdf *cp = (const cosine_pdf *)self;
    vec3 rand_dir = vec3_random_cosine_direction();
    return onb_transform(&cp->uvw, &rand_dir);
}

static void cosine_pdf_destroy(pdf *self) {
    free(self);
}

static pdf *cosine_pdf_create(const vec3 *w) {
    cosine_pdf *cp = (cosine_pdf *)malloc(sizeof(cosine_pdf));
    if (!cp) return NULL;
    onb_init(&cp->uvw, w);
    cp->base.value    = cosine_pdf_value;
    cp->base.generate = cosine_pdf_generate;
    cp->base.destroy  = cosine_pdf_destroy;
    return (pdf *)cp;
}

/* ---------------- Hittable PDF ---------------- */
typedef struct hittable_pdf {
    pdf base;
    hittable *objects;  /* Pointer to a hittable object */
    point3 origin;      /* Origin point */
} hittable_pdf;

static double hittable_pdf_value_fn(const pdf *self, vec3 direction) {
    const hittable_pdf *hp = (const hittable_pdf *)self;
    /* Calls the hittable interface's pdf_value function */
    return hittable_pdf_value(hp->objects, hp->origin, direction);
}

static vec3 hittable_pdf_generate_fn(const pdf *self) {
    const hittable_pdf *hp = (const hittable_pdf *)self;
    return hittable_random(hp->objects, hp->origin);
}

static void hittable_pdf_destroy(pdf *self) {
    free(self);
}

static pdf *hittable_pdf_create(hittable *objects, point3 origin) {
    hittable_pdf *hp = (hittable_pdf *)malloc(sizeof(hittable_pdf));
    if (!hp) return NULL;
    hp->objects = objects;
    hp->origin  = origin;
    hp->base.value    = hittable_pdf_value_fn;
    hp->base.generate = hittable_pdf_generate_fn;
    hp->base.destroy  = hittable_pdf_destroy;
    return (pdf *)hp;
}

/* ---------------- Mixture PDF ---------------- */
typedef struct mixture_pdf {
    pdf base;
    pdf *p[2];
} mixture_pdf;

static double mixture_pdf_value(const pdf *self, vec3 direction) {
    const mixture_pdf *mp = (const mixture_pdf *)self;
    double val0 = mp->p[0]->value(mp->p[0], direction);
    double val1 = mp->p[1]->value(mp->p[1], direction);
    return 0.5 * val0 + 0.5 * val1;
}

static vec3 mixture_pdf_generate(const pdf *self) {
    const mixture_pdf *mp = (const mixture_pdf *)self;
    if (random_double() < 0.5)
        return mp->p[0]->generate(mp->p[0]);
    else
        return mp->p[1]->generate(mp->p[1]);
}

static void mixture_pdf_destroy(pdf *self) {
    mixture_pdf *mp = (mixture_pdf *)self;
    if (mp->p[0]) mp->p[0]->destroy(mp->p[0]);
    if (mp->p[1]) mp->p[1]->destroy(mp->p[1]);
    free(mp);
}

static pdf *mixture_pdf_create(pdf *p0, pdf *p1) {
    mixture_pdf *mp = (mixture_pdf *)malloc(sizeof(mixture_pdf));
    if (!mp) return NULL;
    mp->p[0] = p0;
    mp->p[1] = p1;
    mp->base.value    = mixture_pdf_value;
    mp->base.generate = mixture_pdf_generate;
    mp->base.destroy  = mixture_pdf_destroy;
    return (pdf *)mp;
}

#endif /* PDF_H */

