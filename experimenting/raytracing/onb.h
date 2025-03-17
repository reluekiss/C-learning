#ifndef ONB_H
#define ONB_H

#include "vec3.h"
#include <math.h>

typedef struct {
    vec3 u, v, w;
} onb;

static inline void onb_init(onb *ob, const vec3 *n) {
    ob->w = vec3_unit_vector(n);
    vec3 a = (fabs(ob->w.x) > 0.9) ? (vec3){0, 1, 0} : (vec3){1, 0, 0};
    vec3 temp = vec3_cross(&ob->w, &a);
    ob->v = vec3_unit_vector(&temp);
    ob->u = vec3_cross(&ob->w, &ob->v);
}

static inline vec3 onb_transform(const onb *ob, const vec3 *v) {
    vec3 u_scaled = vec3_scale(&ob->u, v->x);
    vec3 v_scaled = vec3_scale(&ob->v, v->y);
    vec3 w_scaled = vec3_scale(&ob->w, v->z);
    vec3 temp = vec3_add(&u_scaled, &v_scaled);
    return vec3_add(&temp, &w_scaled);
}

#endif /* ONB_H */
