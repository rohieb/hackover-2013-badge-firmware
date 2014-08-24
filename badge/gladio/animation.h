#ifndef INCLUDED_BADGE_GLADIO_ANIMATION_H
#define INCLUDED_BADGE_GLADIO_ANIMATION_H

#include "../ui/sprite.h"

#include <stdint.h>

typedef struct {
  uint8_t             animation_ticks_per_frame;
  uint8_t             animation_length;
  badge_sprite const *animation_frames;
} gladio_animation;

#endif
