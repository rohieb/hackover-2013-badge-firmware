#ifndef INCLUDED_BADGE2013_MOCKUP_SPRITE_H
#define INCLUDED_BADGE2013_MOCKUP_SPRITE_H

#include "display.h"
#include <stdint.h>

enum {
  BADGE_SPRITE_MAX_WIDTH = 32
};

typedef struct {
  uint8_t width;
  uint8_t height;
  uint8_t const *data;
} badge_sprite;

enum {
  BADGE_BLT_MIRRORED = 1
};

void badge_framebuffer_blt(badge_framebuffer *fb,
                           int8_t x,
                           int8_t y,
                           badge_sprite const *sprite,
                           unsigned flags);

#endif
