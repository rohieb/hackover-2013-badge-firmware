#ifndef INCLUDED_BADGE_JUMPNRUN_SHOTS_H
#define INCLUDED_BADGE_JUMPNRUN_SHOTS_H

#include "../ui/display.h"
#include "../util/util.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct jumpnrun_shot {
  rectangle old_box;
  rectangle current_box;
  vec2d     inertia;
  uint8_t   tick;
} jumpnrun_shot;

enum {
  JUMPNRUN_SHOT_EXTENT = 3,
  JUMPNRUN_SHOT_TICKS_PER_FRAME = 36,
  JUMPNRUN_SHOT_FRAMES = 2
};

static inline bool jumpnrun_shot_spawned(jumpnrun_shot const *shot) { return fixed_point_ne(shot->inertia.x, FIXED_INT(0)); }
static inline void jumpnrun_shot_despawn(jumpnrun_shot       *shot) { shot->inertia.x = FIXED_INT(0); }

void jumpnrun_shot_process(jumpnrun_shot *shot);

#endif
