#include "object.h"
#include "status.h"

#include "../common/explosion.h"
#include "../ui/display.h"

vec2d gladio_object_explosion_position(rectangle r) {
  vec2d xdiff = vec2d_div(vec2d_sub(r.extent, vec2d_new(FIXED_INT(EXPLOSION_WIDTH),
                                                        FIXED_INT(EXPLOSION_HEIGHT))),
                          FIXED_INT(2));

  return vec2d_add(r.pos, xdiff);
}

uint8_t gladio_object_off_playfield(rectangle r) {
  enum {
    GLADIO_PLAYFIELD_MARGIN = 16,
    GLADIO_PLAYFIELD_LEFT   = -GLADIO_PLAYFIELD_MARGIN,
    GLADIO_PLAYFIELD_RIGHT  = BADGE_DISPLAY_WIDTH      + GLADIO_PLAYFIELD_MARGIN,
    GLADIO_PLAYFIELD_TOP    = GLADIO_STATUS_BAR_HEIGHT - GLADIO_PLAYFIELD_MARGIN,
    GLADIO_PLAYFIELD_BOTTOM = BADGE_DISPLAY_HEIGHT     + GLADIO_PLAYFIELD_MARGIN
  };

  return
    0
    || fixed_point_gt(rectangle_left  (&r), FIXED_INT(GLADIO_PLAYFIELD_RIGHT ))
    || fixed_point_gt(rectangle_top   (&r), FIXED_INT(GLADIO_PLAYFIELD_BOTTOM))
    || fixed_point_lt(rectangle_right (&r), FIXED_INT(GLADIO_PLAYFIELD_LEFT  ))
    || fixed_point_lt(rectangle_bottom(&r), FIXED_INT(GLADIO_PLAYFIELD_TOP  ))
    ;
}
