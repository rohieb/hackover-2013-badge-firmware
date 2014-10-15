#ifndef INCLUDED_BADGE_COMMON_EXPLOSION_H
#define INCLUDED_BADGE_COMMON_EXPLOSION_H

#include "../ui/display.h"
#include "../util/vec2d.h"
#include <stdint.h>

enum {
  EXPLOSION_FRAMES          =  4,
  EXPLOSION_TICKS_PER_FRAME = 15,
  EXPLOSION_TICKS           = EXPLOSION_FRAMES * EXPLOSION_TICKS_PER_FRAME,

  EXPLOSION_WIDTH  = 7,
  EXPLOSION_HEIGHT = 5
};

void common_render_explosion(badge_framebuffer *fb,
                             int8_t x,
                             int8_t y,
                             uint8_t tick);

#endif
