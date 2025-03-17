#ifndef TEXTURE_H
#define TEXTURE_H

#include "color.h"
#include "perlin.h"
#include "rtw_stb_image.h"
#include "interval.h"
#include "vec3.h"
#include <math.h>
#include <stdlib.h>

/* Base texture interface */
typedef struct texture {
    /* Returns a color given texture coordinates (u,v) and a point p. */
    color (*value)(const struct texture *self, double u, double v, const point3 *p);
    /* Destructor for the texture object */
    void (*destroy)(struct texture *self);
} texture;

/* ---------------- Solid Color ---------------- */
typedef struct {
    texture base;
    color albedo;
} solid_color;

static color solid_color_value(const texture *self, double u, double v, const point3 *p) {
    (void)u; (void)v; (void)p;
    const solid_color *sc = (const solid_color *)self;
    return sc->albedo;
}

static void solid_color_destroy(texture *self) {
    free(self);
}

static texture *solid_color_create(color albedo) {
    solid_color *sc = (solid_color *)malloc(sizeof(solid_color));
    if (!sc) return NULL;
    sc->albedo = albedo;
    sc->base.value = solid_color_value;
    sc->base.destroy = solid_color_destroy;
    return (texture *)sc;
}

static texture *solid_color_create_rgb(double red, double green, double blue) {
    return solid_color_create(vec3_create(red, green, blue));
}

/* ---------------- Checker Texture ---------------- */
typedef struct {
    texture base;
    double inv_scale;
    texture *even;
    texture *odd;
} checker_texture;

static color checker_texture_value(const texture *self, double u, double v, const point3 *p) {
    const checker_texture *ct = (const checker_texture *)self;
    int xInt = (int)floor(ct->inv_scale * p->x);
    int yInt = (int)floor(ct->inv_scale * p->y);
    int zInt = (int)floor(ct->inv_scale * p->z);
    int sum = xInt + yInt + zInt;
    if (sum % 2 == 0)
        return ct->even->value(ct->even, u, v, p);
    else
        return ct->odd->value(ct->odd, u, v, p);
}

static void checker_texture_destroy(texture *self) {
    checker_texture *ct = (checker_texture *)self;
    if (ct->even) ct->even->destroy(ct->even);
    if (ct->odd)  ct->odd->destroy(ct->odd);
    free(ct);
}

static texture *checker_texture_create(double scale, texture *even, texture *odd) {
    checker_texture *ct = (checker_texture *)malloc(sizeof(checker_texture));
    if (!ct) return NULL;
    ct->inv_scale = 1.0 / scale;
    ct->even = even;
    ct->odd = odd;
    ct->base.value = checker_texture_value;
    ct->base.destroy = checker_texture_destroy;
    return (texture *)ct;
}

static texture *checker_texture_create_colors(double scale, color c1, color c2) {
    texture *even = solid_color_create(c1);
    texture *odd  = solid_color_create(c2);
    return checker_texture_create(scale, even, odd);
}

/* ---------------- Image Texture ---------------- */
typedef struct {
    texture base;
    rtw_image image;
} image_texture;

static color image_texture_value(const texture *self, double u, double v, const point3 *p) {
    (void)p;
    const image_texture *it = (const image_texture *)self;
    if (rtw_image_height(&it->image) <= 0)
        return vec3_create(0, 1, 1);  /* Debug: cyan */
    interval i01 = interval_create(0, 1);
    u = interval_clamp(&i01, u);
    v = 1.0 - interval_clamp(&i01, v);  /* Flip V */
    int i = (int)(u * rtw_image_width(&it->image));
    int j = (int)(v * rtw_image_height(&it->image));
    const unsigned char *pixel = rtw_pixel_data(&it->image, i, j);
    double scale = 1.0 / 255.0;
    return vec3_create(scale * pixel[0],
                       scale * pixel[1],
                       scale * pixel[2]);
}

static void image_texture_destroy(texture *self) {
    image_texture *it = (image_texture *)self;
    rtw_image_destroy(&it->image);
    free(it);
}

static texture *image_texture_create(const char *filename) {
    image_texture *it = (image_texture *)malloc(sizeof(image_texture));
    if (!it) return NULL;
    if (!rtw_image_load(&it->image, filename)) {
        free(it);
        /* Fallback: return solid cyan */
        return solid_color_create_rgb(0, 1, 1);
    }
    it->base.value = image_texture_value;
    it->base.destroy = image_texture_destroy;
    return (texture *)it;
}

/* ---------------- Noise Texture ---------------- */
typedef struct {
    texture base;
    perlin noise;
    double scale;
} noise_texture;

static color noise_texture_value(const texture *self, double u, double v, const point3 *p) {
    (void)u; (void)v;
    const noise_texture *nt = (const noise_texture *)self;
    double turb = perlin_turb(&nt->noise, p, 7);
    double s = sin(nt->scale * p->z + 10 * turb);
    double factor = 1 + s;
    color base_color = vec3_create(0.5, 0.5, 0.5);
    return vec3_scale(&base_color, factor);
}

static void noise_texture_destroy(texture *self) {
    free(self);
}

static texture *noise_texture_create(double scale) {
    noise_texture *nt = (noise_texture *)malloc(sizeof(noise_texture));
    if (!nt) return NULL;
    nt->scale = scale;
    perlin_init(&nt->noise);
    nt->base.value = noise_texture_value;
    nt->base.destroy = noise_texture_destroy;
    return (texture *)nt;
}

#endif
