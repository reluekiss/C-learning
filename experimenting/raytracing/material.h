#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"
#include "pdf.h"
#include "texture.h"
#include "ray.h"
#include "vec3.h"
#include "common.h"
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

/* --- Scatter Record --- */
typedef struct scatter_record {
    color attenuation;
    pdf *pdf_ptr;
    bool skip_pdf;
    ray skip_pdf_ray;
} scatter_record;

/* --- Material Virtual Table and Base --- */
typedef struct material material;

typedef struct material_vtable {
    color (*emitted)(const material *self, const ray *r_in, const hit_record *rec,
                     double u, double v, const point3 *p);
    bool  (*scatter)(const material *self, const ray *r_in, const hit_record *rec,
                     scatter_record *srec);
    double (*scattering_pdf)(const material *self, const ray *r_in, const hit_record *rec,
                             const ray *scattered);
    void (*destroy)(material *self);
} material_vtable;

struct material {
    material_vtable *vptr;
};

/* Default emitted: returns black */
static color material_emitted_default(const material *self, const ray *r_in,
                                        const hit_record *rec, double u, double v,
                                        const point3 *p) {
    (void)self; (void)r_in; (void)rec; (void)u; (void)v; (void)p;
    return vec3_create(0, 0, 0);
}

/* --- Lambertian --- */
typedef struct lambertian {
    material base;
    texture *tex;
} lambertian;

static bool lambertian_scatter(const material *self, const ray *r_in,
                               const hit_record *rec, scatter_record *srec) {
    const lambertian *lam = (const lambertian *)self;
    srec->attenuation = lam->tex->value(lam->tex, rec->u, rec->v, &rec->p);
    srec->pdf_ptr = cosine_pdf_create(&rec->normal);
    srec->skip_pdf = false;
    return true;
}

static double lambertian_scattering_pdf(const material *self, const ray *r_in,
                                          const hit_record *rec, const ray *scattered) {
    (void)r_in;
    const lambertian *lam = (const lambertian *)self;
    vec3 unit_dir = vec3_unit_vector(&scattered->dir);
    double cosine = vec3_dot(&rec->normal, &unit_dir);
    return cosine < 0 ? 0 : cosine / pi;
}

static void lambertian_destroy(material *self) {
    lambertian *lam = (lambertian *)self;
    if(lam->tex)
        lam->tex->destroy(lam->tex);
    free(lam);
}

static material_vtable lambertian_vtable = {
    .emitted = material_emitted_default,
    .scatter = lambertian_scatter,
    .scattering_pdf = lambertian_scattering_pdf,
    .destroy = lambertian_destroy
};

static material *lambertian_create_from_texture(texture *tex) {
    lambertian *lam = (lambertian *)malloc(sizeof(lambertian));
    if (!lam) return NULL;
    lam->base.vptr = &lambertian_vtable;
    lam->tex = tex;
    return (material *)lam;
}

static material *lambertian_create_from_color(color albedo) {
    texture *tex = solid_color_create(albedo);
    return lambertian_create_from_texture(tex);
}

/* --- Metal --- */
typedef struct metal {
    material base;
    color albedo;
    double fuzz;
} metal;

static bool metal_scatter(const material *self, const ray *r_in,
                          const hit_record *rec, scatter_record *srec) {
    const metal *met = (const metal *)self;
    vec3 reflected = vec3_reflect(&r_in->dir, &rec->normal);
    vec3 unit_reflected = vec3_unit_vector(&reflected);
    vec3 temp_ruv = vec3_random_unit_vector();  // Use vec3_random_unit_vector, if that's the correct function.
    vec3 scaled_ruv = vec3_scale(&temp_ruv, met->fuzz);
    reflected = vec3_add(&unit_reflected, &scaled_ruv);
    srec->attenuation = met->albedo;
    srec->pdf_ptr = NULL;
    srec->skip_pdf = true;
    srec->skip_pdf_ray = (ray){rec->p, reflected, r_in->tm};
    return true;
}

static double metal_scattering_pdf(const material *self, const ray *r_in,
                                   const hit_record *rec, const ray *scattered) {
    (void)self; (void)r_in; (void)rec; (void)scattered;
    return 0;
}

static void metal_destroy(material *self) {
    free(self);
}

static material_vtable metal_vtable = {
    .emitted = material_emitted_default,
    .scatter = metal_scatter,
    .scattering_pdf = metal_scattering_pdf,
    .destroy = metal_destroy
};

static material *metal_create(color albedo, double fuzz) {
    metal *met = (metal *)malloc(sizeof(metal));
    if (!met) return NULL;
    met->base.vptr = &metal_vtable;
    met->albedo = albedo;
    met->fuzz = (fuzz < 1) ? fuzz : 1;
    return (material *)met;
}

/* --- Dielectric --- */
typedef struct dielectric {
    material base;
    double refraction_index;
} dielectric;

static double dielectric_reflectance(double cosine, double ref_idx) {
    double r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

static bool dielectric_scatter(const material *self, const ray *r_in,
                               const hit_record *rec, scatter_record *srec) {
    const dielectric *diel = (const dielectric *)self;
    srec->attenuation = (color){1.0, 1.0, 1.0};
    srec->pdf_ptr = NULL;
    srec->skip_pdf = true;
    double ri = rec->front_face ? (1.0 / diel->refraction_index) : diel->refraction_index;
    vec3 unit_direction = vec3_unit_vector(&r_in->dir);
    vec3 neg_unit = vec3_neg(&unit_direction);
    double cos_theta = fmin(vec3_dot(&neg_unit, &rec->normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
    bool cannot_refract = ri * sin_theta > 1.0;
    vec3 direction;
    if (cannot_refract || dielectric_reflectance(cos_theta, ri) > random_double())
        direction = vec3_reflect(&unit_direction, &rec->normal);
    else
        direction = vec3_refract(&unit_direction, &rec->normal, ri);
    srec->skip_pdf_ray = (ray){rec->p, direction, r_in->tm};
    return true;
}

static double dielectric_scattering_pdf(const material *self, const ray *r_in,
                                        const hit_record *rec, const ray *scattered) {
    (void)self; (void)r_in; (void)rec; (void)scattered;
    return 0;
}

static void dielectric_destroy(material *self) {
    free(self);
}

static material_vtable dielectric_vtable = {
    .emitted = material_emitted_default,
    .scatter = dielectric_scatter,
    .scattering_pdf = dielectric_scattering_pdf,
    .destroy = dielectric_destroy
};

static material *dielectric_create(double refraction_index) {
    dielectric *diel = (dielectric *)malloc(sizeof(dielectric));
    if (!diel) return NULL;
    diel->base.vptr = &dielectric_vtable;
    diel->refraction_index = refraction_index;
    return (material *)diel;
}

/* --- Diffuse Light --- */
typedef struct diffuse_light {
    material base;
    texture *tex;
} diffuse_light;

static color diffuse_light_emitted(const material *self, const ray *r_in,
                                   const hit_record *rec, double u, double v,
                                   const point3 *p) {
    (void)r_in;
    const diffuse_light *dl = (const diffuse_light *)self;
    if (!rec->front_face)
        return vec3_create(0, 0, 0);
    return dl->tex->value(dl->tex, u, v, p);
}

static bool diffuse_light_scatter(const material *self, const ray *r_in,
                                  const hit_record *rec, scatter_record *srec) {
    (void)self; (void)r_in; (void)rec; (void)srec;
    return false;
}

static double diffuse_light_scattering_pdf(const material *self, const ray *r_in,
                                           const hit_record *rec, const ray *scattered) {
    (void)self; (void)r_in; (void)rec; (void)scattered;
    return 0;
}

static void diffuse_light_destroy(material *self) {
    diffuse_light *dl = (diffuse_light *)self;
    if (dl->tex)
        dl->tex->destroy(dl->tex);
    free(dl);
}

static material_vtable diffuse_light_vtable = {
    .emitted = diffuse_light_emitted,
    .scatter = diffuse_light_scatter,
    .scattering_pdf = diffuse_light_scattering_pdf,
    .destroy = diffuse_light_destroy
};

static material *diffuse_light_create_from_texture(texture *tex) {
    diffuse_light *dl = (diffuse_light *)malloc(sizeof(diffuse_light));
    if (!dl) return NULL;
    dl->base.vptr = &diffuse_light_vtable;
    dl->tex = tex;
    return (material *)dl;
}

static material *diffuse_light_create_from_color(color emit) {
    texture *tex = solid_color_create(emit);
    return diffuse_light_create_from_texture(tex);
}

/* --- Isotropic --- */
typedef struct isotropic {
    material base;
    texture *tex;
} isotropic;

static bool isotropic_scatter(const material *self, const ray *r_in,
                              const hit_record *rec, scatter_record *srec) {
    const isotropic *iso = (const isotropic *)self;
    srec->attenuation = iso->tex->value(iso->tex, rec->u, rec->v, &rec->p);
    srec->pdf_ptr = sphere_pdf_create();
    srec->skip_pdf = false;
    return true;
}

static double isotropic_scattering_pdf(const material *self, const ray *r_in,
                                       const hit_record *rec, const ray *scattered) {
    (void)self; (void)r_in; (void)rec; (void)scattered;
    return 1.0 / (4 * pi);
}

static void isotropic_destroy(material *self) {
    isotropic *iso = (isotropic *)self;
    if (iso->tex)
        iso->tex->destroy(iso->tex);
    free(iso);
}

static material_vtable isotropic_vtable = {
    .emitted = material_emitted_default,
    .scatter = isotropic_scatter,
    .scattering_pdf = isotropic_scattering_pdf,
    .destroy = isotropic_destroy
};

static material *isotropic_create_from_texture(texture *tex) {
    isotropic *iso = (isotropic *)malloc(sizeof(isotropic));
    if (!iso) return NULL;
    iso->base.vptr = &isotropic_vtable;
    iso->tex = tex;
    return (material *)iso;
}

static material *isotropic_create_from_color(color albedo) {
    texture *tex = solid_color_create(albedo);
    return isotropic_create_from_texture(tex);
}

#endif /* MATERIAL_H */
