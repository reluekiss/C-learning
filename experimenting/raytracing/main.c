#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "quad.h"
#include "sphere.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    /* Create world */
    hittable_list *world = hittable_list_create();
    if (!world) exit(1);

    material *red   = lambertian_create_from_color(vec3_create(0.65, 0.05, 0.05));
    material *white = lambertian_create_from_color(vec3_create(0.73, 0.73, 0.73));
    material *green = lambertian_create_from_color(vec3_create(0.12, 0.45, 0.15));
    material *light = diffuse_light_create_from_color(vec3_create(15, 15, 15));

    /* Cornell box sides */
    hittable_list_add(world, (hittable *)quad_create(vec3_create(555, 0, 0),
                                                     vec3_create(0, 0, 555),
                                                     vec3_create(0, 555, 0), green));
    hittable_list_add(world, (hittable *)quad_create(vec3_create(0, 0, 555),
                                                     vec3_create(0, 0, -555),
                                                     vec3_create(0, 555, 0), red));
    hittable_list_add(world, (hittable *)quad_create(vec3_create(0, 555, 0),
                                                     vec3_create(555, 0, 0),
                                                     vec3_create(0, 0, 555), white));
    hittable_list_add(world, (hittable *)quad_create(vec3_create(0, 0, 555),
                                                     vec3_create(555, 0, 0),
                                                     vec3_create(0, 0, -555), white));
    hittable_list_add(world, (hittable *)quad_create(vec3_create(555, 0, 555),
                                                     vec3_create(-555, 0, 0),
                                                     vec3_create(0, 555, 0), white));

    /* Light on ceiling */
    hittable_list_add(world, (hittable *)quad_create(vec3_create(213, 554, 227),
                                                     vec3_create(130, 0, 0),
                                                     vec3_create(0, 0, 105), light));

    /* Box */
    hittable *box1 = box(vec3_create(0, 0, 0), vec3_create(165, 330, 165), white);
    box1 = (hittable *)rotate_y_create(box1, 15);
    box1 = (hittable *)translate_create(box1, vec3_create(265, 0, 295));
    hittable_list_add(world, box1);

    /* Glass sphere */
    material *glass = dielectric_create(1.5);
    hittable_list_add(world, (hittable *)sphere_create_stationary(vec3_create(190, 90, 190), 90, glass));

    /* Light sources */
    hittable_list *lights = hittable_list_create();
    hittable_list_add(lights, (hittable *)quad_create(vec3_create(343, 554, 332),
                                                       vec3_create(-130, 0, 0),
                                                       vec3_create(0, 0, -105), NULL));
    hittable_list_add(lights, (hittable *)sphere_create_stationary(vec3_create(190, 90, 190), 90, NULL));

    /* Set up camera */
    camera cam;
    cam.aspect_ratio = 1.0;
    cam.image_width = 600;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = vec3_create(0, 0, 0);
    cam.vfov = 40;
    cam.lookfrom = vec3_create(278, 278, -800);
    cam.lookat = vec3_create(278, 278, 0);
    cam.vup = vec3_create(0, 1, 0);
    cam.defocus_angle = 0;

    /* Render scene (writes PPM to stdout) */
    camera_render(&cam, (hittable *)world, (hittable *)lights);

    /* Free resources as needed... */
    /* (Assuming you have implemented proper destroy/free functions for your objects.) */
    
    return 0;
}
