#ifndef INCLUDED_RECTANGLE_H
#define INCLUDED_RECTANGLE_H

#include "fixed_point.h"
#include "vec2d.h"
#include "../ui/display.h"

typedef struct {
  vec2d pos;
  vec2d extent;
} rectangle;

static inline rectangle   rectangle_new(vec2d pos, vec2d extent) { rectangle r = { pos, extent }; return r; }

static inline fixed_point rectangle_top   (rectangle const *r) { return r->pos.y; }
static inline fixed_point rectangle_left  (rectangle const *r) { return r->pos.x; }
static inline fixed_point rectangle_bottom(rectangle const *r) { return fixed_point_add(rectangle_top (r), r->extent.y); }
static inline fixed_point rectangle_right (rectangle const *r) { return fixed_point_add(rectangle_left(r), r->extent.x); }

static inline fixed_point rectangle_width (rectangle const *r) { return r->extent.x; }
static inline fixed_point rectangle_height(rectangle const *r) { return r->extent.y; }

static inline fixed_point rectangle_mid_x (rectangle const *r) { return fixed_point_add(r->pos.x, fixed_point_div(r->extent.x, FIXED_INT(2))); }
static inline fixed_point rectangle_mid_y (rectangle const *r) { return fixed_point_add(r->pos.y, fixed_point_div(r->extent.y, FIXED_INT(2))); }

static inline vec2d       rectangle_mid   (rectangle const *r) { vec2d v = { rectangle_mid_x(r), rectangle_mid_y(r) }; return v; }

static inline void rectangle_move_to  (rectangle *r, vec2d       new_pos) { r->pos = new_pos; }
static inline void rectangle_move_to_x(rectangle *r, fixed_point new_x  ) { r->pos.x = new_x; }
static inline void rectangle_move_to_y(rectangle *r, fixed_point new_y  ) { r->pos.x = new_y; }

static inline void rectangle_move_rel (rectangle *r, vec2d       vec    ) { r->pos = vec2d_add(r->pos, vec); }
static inline void rectangle_expand   (rectangle *r, vec2d       extent ) { r->extent = extent; }

static inline bool rectangle_intersect(rectangle const *r1,
                                       rectangle const *r2) {
  return (fixed_point_lt(rectangle_top   (r1), rectangle_bottom(r2)) &&
          fixed_point_gt(rectangle_bottom(r1), rectangle_top   (r2)) &&
          fixed_point_lt(rectangle_left  (r1), rectangle_right (r2)) &&
          fixed_point_gt(rectangle_right (r1), rectangle_left  (r2)));
}

static inline uint8_t rectangle_onscreen(rectangle const *r) {
  return
    1
    && fixed_point_ge(rectangle_right (r), FIXED_INT(0))
    && fixed_point_ge(rectangle_bottom(r), FIXED_INT(0))
    && fixed_point_le(rectangle_left  (r), FIXED_INT(BADGE_DISPLAY_WIDTH + 1))
    && fixed_point_le(rectangle_top   (r), FIXED_INT(BADGE_DISPLAY_WIDTH + 1))
    ;
}

static inline uint8_t rectangle_contains(rectangle const *r, vec2d p) {
  return
    1
    && fixed_point_ge(p.x, rectangle_left  (r))
    && fixed_point_le(p.x, rectangle_right (r))
    && fixed_point_ge(p.y, rectangle_top   (r))
    && fixed_point_le(p.y, rectangle_bottom(r))
    ;
}

#endif
