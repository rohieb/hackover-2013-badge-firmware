#ifndef INCLUDED_BADGE_JUMPNRUN_MOVEABLE_H
#define INCLUDED_BADGE_JUMPNRUN_MOVEABLE_H

#include "../util/util.h"

typedef struct jumpnrun_moveable {
  rectangle current_box;
  vec2d     inertia;

  uint8_t  tick_minor;
  uint8_t   anim_frame;
  uint8_t   anim_direction;

  bool      touching_ground;
  uint8_t   jumpable_frames;
} jumpnrun_moveable;

#endif
