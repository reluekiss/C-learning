#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"
#include "hittable.h"
#include "pdf.h"
#include "material.h"
#include "vec3.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Camera structure */
typedef struct {
    /* Public settings */
    double aspect_ratio;      // width / height
    int image_width;          // pixels
    int samples_per_pixel;    // samples per pixel
    int max_depth;            // max ray bounces
    color background;         // background color

    double vfov;              // vertical FOV in degrees
    point3 lookfrom;          // camera position
    point3 lookat;            // target point
    vec3 vup;                 // up direction

    double defocus_angle;     // defocus angle in degrees (0 for pinhole)
    double focus_dist;        // focus distance

    /* Private members computed at initialization */
    int image_height;
    double pixel_samples_scale;
    int sqrt_spp;
    double recip_sqrt_spp;
    point3 center;            // equals lookfrom
    point3 pixel00_loc;       // location of pixel (0,0)
    vec3 pixel_delta_u;       // horizontal pixel delta
    vec3 pixel_delta_v;       // vertical pixel delta
    vec3 u, v, w;             // camera coordinate basis (u: right, v: up, w: backward)
    vec3 defocus_disk_u;      // defocus disk horizontal radius vector
    vec3 defocus_disk_v;      // defocus disk vertical radius vector
} camera;

/* Forward declarations of helper functions.
   These must be implemented elsewhere:
   - vec3_sub, vec3_add, vec3_scale, vec3_div, unit_vector, cross,
     random_double, random_in_unit_disk, ray_create, ray_at, write_color */
 
/* Initialize the camera (computes private members) */
static inline void camera_initialize(camera *cam) {
    cam->image_height = (int)(cam->image_width / cam->aspect_ratio);
    if (cam->image_height < 1) cam->image_height = 1;
    
    cam->sqrt_spp = (int) sqrt((double) cam->samples_per_pixel);
    cam->pixel_samples_scale = 1.0 / (cam->sqrt_spp * cam->sqrt_spp);
    cam->recip_sqrt_spp = 1.0 / cam->sqrt_spp;
    
    cam->center = cam->lookfrom;
    
    double theta = degrees_to_radians(cam->vfov);
    double h = tan(theta / 2);
    double viewport_height = 2 * h * cam->focus_dist;
    double viewport_width  = viewport_height * ((double) cam->image_width / cam->image_height);
    
    vec3 temp_w = vec3_sub(&cam->lookfrom, &cam->lookat);
    cam->w = vec3_unit_vector(&temp_w);

    vec3 temp_u = vec3_cross(&cam->vup, &cam->w);
    cam->u = vec3_unit_vector(&temp_u);

    cam->v = vec3_cross(&cam->w, &cam->u);

    vec3 temp_viewport_u = vec3_scale(&cam->u, viewport_width);
    vec3 temp_viewport_v = vec3_scale(&cam->v, viewport_height);
    vec3 flipped_viewport_v = vec3_scale(&cam->v, -viewport_height);

    vec3 temp_focus = vec3_scale(&cam->w, cam->focus_dist);
    point3 temp_upper_left = vec3_sub(&cam->center, &temp_focus);

    vec3 half_viewport_u = vec3_div(&temp_viewport_u, cam->image_width);
    vec3 half_viewport_v = vec3_div(&flipped_viewport_v, cam->image_height);

    temp_upper_left = vec3_sub(&temp_upper_left, &half_viewport_u);
    temp_upper_left = vec3_sub(&temp_upper_left, &half_viewport_v);

    cam->pixel_delta_u = vec3_div(&temp_viewport_u, cam->image_width);
    cam->pixel_delta_v = vec3_div(&flipped_viewport_v, cam->image_height);

    vec3 temp_sum = vec3_add(&cam->pixel_delta_u, &cam->pixel_delta_v);
    vec3 temp_scale = vec3_scale(&temp_sum, 0.5);
    cam->pixel00_loc = vec3_add(&temp_upper_left, &temp_scale);
    
    /* Compute defocus disk vectors */
    double defocus_radius = cam->focus_dist * tan(degrees_to_radians(cam->defocus_angle / 2));
    cam->defocus_disk_u = vec3_scale(&cam->u, defocus_radius);
    cam->defocus_disk_v = vec3_scale(&cam->v, defocus_radius);
}

/* Returns a stratified random sample in a sub-pixel square */
static inline vec3 camera_sample_square_stratified(const camera *cam, int s_i, int s_j) {
    double px = ((s_i + random_double()) * cam->recip_sqrt_spp) - 0.5;
    double py = ((s_j + random_double()) * cam->recip_sqrt_spp) - 0.5;
    return vec3_create(px, py, 0);
}

/* Returns a random point in the unit square [-0.5,0.5] x [-0.5,0.5] */
static inline vec3 camera_sample_square(const camera *cam) {
    (void)cam;
    return vec3_create(random_double() - 0.5, random_double() - 0.5, 0);
}

/* Returns a random point in a disk of given radius */
static inline vec3 camera_sample_disk(const camera *cam, double radius) {
    (void)cam;
    vec3 temp = vec3_random_in_unit_disk();
    return vec3_scale(&temp, radius);
}

/* Returns a random point on the defocus disk */
static inline point3 camera_defocus_disk_sample(const camera *cam) {
    vec3 p = vec3_random_in_unit_disk();  // p.x, p.y in [-0.5, 0.5], p.z = 0
    vec3 term1 = vec3_scale(&cam->defocus_disk_u, p.x);
    vec3 term2 = vec3_scale(&cam->defocus_disk_v, p.y);
    vec3 temp = vec3_add(&term1, &term2);
    return vec3_add(&cam->center, &temp);
}

/* Construct a ray from the camera for a given pixel (i,j) and stratified sample (s_i,s_j) */
static inline ray camera_get_ray(const camera *cam, int i, int j, int s_i, int s_j) {
    vec3 offset = camera_sample_square_stratified(cam, s_i, s_j);
    vec3 right_term = vec3_scale(&cam->pixel_delta_u, i + offset.x);
    vec3 down_term  = vec3_scale(&cam->pixel_delta_v, j + offset.y);
    vec3 temp = vec3_add(&right_term, &down_term);
    point3 pixel_sample = vec3_add(&cam->pixel00_loc, &temp);
    
    point3 ray_origin;
    if (cam->defocus_angle <= 0)
        ray_origin = cam->center;
    else
        ray_origin = camera_defocus_disk_sample(cam);
    
    vec3 ray_direction = vec3_sub(&pixel_sample, &ray_origin);
    double ray_time = random_double();
    
    return ray_create(ray_origin, ray_direction, ray_time);
}
/* Recursive function that computes ray color. (Requires world.hit, material scattering, etc.) */
static inline color ray_color(const ray *r, int depth, const hittable *world, const hittable *lights, const color *background) {
    if (depth <= 0)
        return vec3_create(0, 0, 0);

    hit_record rec;
    interval t_interval = {0.001, infinity};
    if (!hittable_hit(world, r, t_interval, &rec))
        return *background;  // Fixed: dereference background

    scatter_record srec;
    color color_from_emission = rec.mat->vptr->emitted(rec.mat, r, &rec, rec.u, rec.v, &rec.p);
    if (!rec.mat->vptr->scatter(rec.mat, r, &rec, &srec))
        return color_from_emission;

    if (srec.skip_pdf) {
        color recursive_color = ray_color(&srec.skip_pdf_ray, depth - 1, world, lights, background); // Fixed: pass background
        return vec3_mul(&srec.attenuation, &recursive_color);
    }

    pdf *light_pdf = hittable_pdf_create((hittable *)lights, rec.p);
    pdf *mix_pdf = mixture_pdf_create(light_pdf, srec.pdf_ptr);

    ray scattered = ray_create(rec.p, mix_pdf->generate(mix_pdf), r->tm);
    double pdf_val = mix_pdf->value(mix_pdf, scattered.dir);
    double scattering_pdf = rec.mat->vptr->scattering_pdf(rec.mat, r, &rec, &scattered);
    color sample_color = ray_color(&scattered, depth - 1, world, lights, background);
    color temp = vec3_scale(&srec.attenuation, scattering_pdf);
    temp = vec3_mul(&temp, &sample_color);
    color color_from_scatter = vec3_div(&temp, pdf_val);
    color final_color = vec3_add(&color_from_emission, &color_from_scatter);

    mix_pdf->destroy(mix_pdf);

    return final_color;
}

/* Write a color (assumed to be in [0,1] per channel) to out in PPM format.
   (write_color is assumed to be defined elsewhere.) */

/* Render the scene by shooting rays through each pixel */
static inline void camera_render(camera *cam, const hittable *world, const hittable *lights) {
    camera_initialize(cam);
    printf("P3\n%d %d\n255\n", cam->image_width, cam->image_height);
    
    for (int j = 0; j < cam->image_height; j++) {
        fprintf(stderr, "\rScanlines remaining: %d", cam->image_height - j);
        for (int i = 0; i < cam->image_width; i++) {
            color pixel_color = vec3_create(0, 0, 0);
            for (int s_j = 0; s_j < cam->sqrt_spp; s_j++) {
                for (int s_i = 0; s_i < cam->sqrt_spp; s_i++) {
                    ray r = camera_get_ray(cam, i, j, s_i, s_j);
                    color temp = ray_color(&r, cam->max_depth, world, lights, &cam->background);
                    pixel_color = vec3_add(&pixel_color, &temp);
                }
            }
            write_color(stdout, vec3_scale(&pixel_color, cam->pixel_samples_scale));
        }
    }
    fprintf(stderr, "\rDone.\n");
}

#endif /* CAMERA_H */
