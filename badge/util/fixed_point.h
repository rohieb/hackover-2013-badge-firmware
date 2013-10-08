#ifndef INCLUDED_FIXED_POINT_H
#define INCLUDED_FIXED_POINT_H

#include <stdint.h>
#include <stdbool.h>

/* Unterstes Byte als Nachkomma. 16 Bit Nutzlast! Sonst geht Multiplikation flöten. */
typedef struct { int32_t data; } fixed_point;

static inline fixed_point fixed_point_add(fixed_point x, fixed_point y) { fixed_point r = { x.data + y.data         }; return r; }
static inline fixed_point fixed_point_sub(fixed_point x, fixed_point y) { fixed_point r = { x.data - y.data         }; return r; }
static inline fixed_point fixed_point_mul(fixed_point x, fixed_point y) { fixed_point r = { x.data * y.data / 256   }; return r; }
static inline fixed_point fixed_point_div(fixed_point x, fixed_point y) { fixed_point r = { (x.data * 256) / y.data }; return r; }

static inline fixed_point fixed_point_neg(fixed_point x) { fixed_point r = { -x.data                       }; return r; }
static inline fixed_point fixed_point_abs(fixed_point x) { fixed_point r = { x.data < 0 ? -x.data : x.data }; return r; }

static inline bool fixed_point_lt(fixed_point x, fixed_point y) { return x.data <  y.data; }
static inline bool fixed_point_gt(fixed_point x, fixed_point y) { return x.data >  y.data; }
static inline bool fixed_point_le(fixed_point x, fixed_point y) { return x.data <= y.data; }
static inline bool fixed_point_ge(fixed_point x, fixed_point y) { return x.data >= y.data; }
static inline bool fixed_point_eq(fixed_point x, fixed_point y) { return x.data == y.data; }
static inline bool fixed_point_ne(fixed_point x, fixed_point y) { return x.data != y.data; }

#define FIXED_POINT_I(x, y) { ((x) * 256) + ((y) * 256 / 1000) }

static inline fixed_point FIXED_POINT(unsigned x, unsigned y) {
  fixed_point r = { ((int) x * 256) + ((int) y * 256 / 1000) };
  return r;
}

static inline int fixed_point_cast_int(fixed_point x) { return x.data / 256; }

static inline fixed_point fixed_point_min(fixed_point x, fixed_point y) { return fixed_point_lt(x, y) ? x : y; }
static inline fixed_point fixed_point_max(fixed_point x, fixed_point y) { return fixed_point_gt(x, y) ? x : y; }

#endif
