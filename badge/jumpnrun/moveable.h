#ifndef INCLUDED_BADGE_JUMPNRUN_MOVEABLE_H
#define INCLUDED_BADGE_JUMPNRUN_MOVEABLE_H

#include "../util/util.h"
#include <stdint.h>

enum {
  // Do not collide with JUMPNRUN_ENEMY_* and JUMPNRUN_PLAYER_* flags.
  JUMPNRUN_MOVEABLE_TOUCHING_GROUND = 1,
  JUMPNRUN_MOVEABLE_MIRRORED        = 2,
  JUMPNRUN_MOVEABLE_DYING           = 4
};

typedef struct jumpnrun_moveable {
  rectangle hitbox;
  vec2d     inertia;

  uint8_t   tick_minor;
  uint8_t   anim_frame;

  uint8_t   flags;
  uint8_t   jumpable_frames;
} jumpnrun_moveable;

static inline bool jumpnrun_moveable_touching_ground(jumpnrun_moveable const *self) { return self->flags & JUMPNRUN_MOVEABLE_TOUCHING_GROUND; }
static inline bool jumpnrun_moveable_mirrored       (jumpnrun_moveable const *self) { return self->flags & JUMPNRUN_MOVEABLE_MIRRORED       ; }

#endif
