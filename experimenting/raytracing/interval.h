#ifndef INTERVAL_H
#define INTERVAL_H

#include <math.h>
#include <stdbool.h>

typedef struct {
    double min;
    double max;
} interval;

static inline interval interval_empty_create(void) {
    return (interval){ INFINITY, -INFINITY };
}

static inline interval interval_create(double min, double max) {
    return (interval){ min, max };
}

static inline interval interval_enclose(interval a, interval b) {
    return (interval){
        a.min <= b.min ? a.min : b.min,
        a.max >= b.max ? a.max : b.max
    };
}

static inline double interval_size(const interval *ival) {
    return ival->max - ival->min;
}

static inline bool interval_contains(const interval *ival, double x) {
    return (ival->min <= x) && (x <= ival->max);
}

static inline bool interval_surrounds(const interval *ival, double x) {
    return (ival->min < x) && (x < ival->max);
}

static inline double interval_clamp(const interval *ival, double x) {
    if (x < ival->min) return ival->min;
    if (x > ival->max) return ival->max;
    return x;
}

static inline interval interval_expand(const interval *ival, double delta) {
    double padding = delta / 2;
    return interval_create(ival->min - padding, ival->max + padding);
}

static inline interval interval_add(const interval *ival, double displacement) {
    return interval_create(ival->min + displacement, ival->max + displacement);
}

static const interval interval_empty    = { INFINITY, -INFINITY };
static const interval interval_universe = { -INFINITY, INFINITY };

#endif /* INTERVAL_H */
