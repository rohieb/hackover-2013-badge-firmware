#include "object.h"

#include "../common/explosion.h"

vec2d gladio_object_explosion_position(rectangle r) {
  vec2d xdiff = vec2d_div(vec2d_sub(r.extent, vec2d_new(FIXED_INT(EXPLOSION_WIDTH),
                                                        FIXED_INT(EXPLOSION_HEIGHT))),
                          FIXED_INT(2));

  return vec2d_add(r.pos, xdiff);
}
