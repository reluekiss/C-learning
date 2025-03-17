#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "aabb.h"
#include "hittable.h"
#include "ray.h"
#include "vec3.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct hittable_list {
    hittable **objects;
    size_t size;
    size_t capacity;
    aabb bbox;
} hittable_list;

#ifndef HITTABLE_LIST_INITIAL_CAPACITY
#define HITTABLE_LIST_INITIAL_CAPACITY 4
#endif

static inline hittable_list *hittable_list_create(void) {
    hittable_list *list = (hittable_list *)malloc(sizeof(hittable_list));
    if (!list) return NULL;
    list->size = 0;
    list->capacity = HITTABLE_LIST_INITIAL_CAPACITY;
    list->objects = (hittable **)malloc(list->capacity * sizeof(hittable *));
    if (!list->objects) {
        free(list);
        return NULL;
    }
    list->bbox = aabb_empty;
    return list;
}

static inline void hittable_list_clear(hittable_list *list) {
    if (list) {
        list->size = 0;
        list->bbox = aabb_empty;
    }
}

static inline bool hittable_list_add(hittable_list *list, hittable *object) {
    if (!list || !object) return false;
    if (list->size == list->capacity) {
        size_t new_capacity = list->capacity * 2;
        hittable **new_objects = (hittable **)realloc(list->objects, new_capacity * sizeof(hittable *));
        if (!new_objects) return false;
        list->objects = new_objects;
        list->capacity = new_capacity;
    }
    list->objects[list->size++] = object;
    aabb obj_box = hittable_bounding_box(object);
    if (list->size == 1)
        list->bbox = obj_box;
    else
        list->bbox = aabb_enclose(&list->bbox, &obj_box);
    return true;
}

static inline hittable_list *hittable_list_create_with_object(hittable *object) {
    hittable_list *list = hittable_list_create();
    if (!list) return NULL;
    if (object)
        (void)hittable_list_add(list, object);
    return list;
}

static inline bool hittable_list_hit(const hittable_list *list, const ray *r, interval t, hit_record *rec) {
    if (!list) return false;
    hit_record temp_rec;
    bool hit_anything = false;
    double closest_so_far = t.max;
    for (size_t i = 0; i < list->size; i++) {
        hittable *object = list->objects[i];
        interval temp_interval = t;
        temp_interval.max = closest_so_far;
        if (hittable_hit(object, r, temp_interval, &temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            *rec = temp_rec;
        }
    }
    return hit_anything;
}

static inline aabb hittable_list_bounding_box(const hittable_list *list) {
    return list ? list->bbox : aabb_empty;
}

static inline double hittable_list_pdf_value(const hittable_list *list, const point3 origin, const vec3 direction) {
    if (!list || list->size == 0) return 0.0;
    double weight = 1.0 / list->size;
    double sum = 0.0;
    for (size_t i = 0; i < list->size; i++) {
        hittable *object = list->objects[i];
        sum += weight * hittable_pdf_value(object, origin, direction);
    }
    return sum;
}

static inline vec3 hittable_list_random(const hittable_list *list, const point3 origin) {
    if (!list || list->size == 0) return vec3_create(1, 0, 0);
    int idx = random_int(0, (int)list->size - 1);
    hittable *object = list->objects[idx];
    return hittable_random(object, origin);
}

static inline void hittable_list_destroy(hittable_list *list) {
    if (!list) return;
    free(list->objects);
    free(list);
}

#endif /* HITTABLE_LIST_H */
