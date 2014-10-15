#ifndef INCLUDED_BADGE_UTIL_VEC2D_H
#define INCLUDED_BADGE_UTIL_VEC2D_H

#include "fixed_point.h"

typedef struct {
  fixed_point x;
  fixed_point y;
} vec2d;

static inline vec2d vec2d_new(fixed_point x, fixed_point y) {
  vec2d r = { x, y };
  return r;
}

static inline vec2d vec2d_add(vec2d v1, vec2d v2) {
  vec2d r = {
    fixed_point_add(v1.x, v2.x),
    fixed_point_add(v1.y, v2.y)
  };
  return r;
}

static inline vec2d vec2d_sub(vec2d v1, vec2d v2) {
  vec2d r = {
    fixed_point_sub(v1.x, v2.x),
    fixed_point_sub(v1.y, v2.y)
  };
  return r;
}

static inline vec2d vec2d_div(vec2d v, fixed_point x) {
  return vec2d_new(fixed_point_div(v.x, x),
                   fixed_point_div(v.y, x));
}

static inline vec2d vec2d_neg(vec2d v) {
  vec2d r = {
    fixed_point_neg(v.x),
    fixed_point_neg(v.y)
  };
  return r;
}

static inline uint8_t vec2d_xy_less(vec2d lhs, vec2d rhs) {
  return fixed_point_lt(lhs.x, rhs.x) || (fixed_point_eq(lhs.x, rhs.x) && fixed_point_lt(lhs.y, rhs.y));
}

static inline fixed_point vec2d_length_approx(vec2d v) {
  fixed_point x = fixed_point_abs(v.x);
  fixed_point y = fixed_point_abs(v.y);

  fixed_point a = fixed_point_max(x, y);
  fixed_point b = fixed_point_min(x, y);

  fixed_point a2 = fixed_point_mul(a, a);
  fixed_point b2 = fixed_point_mul(b, b);

  // a * (1 + b^2 / a^2 / 2)
  return fixed_point_mul(a,
                         fixed_point_add(FIXED_INT(1),
                                         fixed_point_div(b2,
                                                         fixed_point_mul(a2, FIXED_INT(2)))));
}

#endif
