#ifndef INCLUDED_BADGE2013_JUMPNRUN_H
#define INCLUDED_BADGE2013_JUMPNRUN_H

#include "enemies.h"
#include "items.h"
#include "levels.h"
#include "tiles.h"

#include "../ui/sprite.h"
#include "../util/util.h"

#include <stdbool.h>
#include <stddef.h>

enum {
  JUMPNRUN_PLAYING,
  JUMPNRUN_DEAD,
  JUMPNRUN_WON,
  JUMPNRUN_ERROR
};

enum {
  JUMPNRUN_MAX_SPAWN_MARGIN = BADGE_SPRITE_MAX_WIDTH + 1,
  JUMPNRUN_MAX_SPAWNED_ENEMIES = 10
};

typedef struct jumpnrun_shot {
  rectangle old_box;
  rectangle current_box;
  vec2d     inertia;
  uint8_t   tick;
} jumpnrun_shot;

static inline bool jumpnrun_shot_spawned(jumpnrun_shot const *shot) { return fixed_point_ne(shot->inertia.x, FIXED_INT(0)); }
static inline void jumpnrun_shot_despawn(jumpnrun_shot       *shot) { shot->inertia.x = FIXED_INT(0); }

enum {
  JUMPNRUN_MAX_SHOTS = 2
};

typedef struct jumpnrun_game_state {
  jumpnrun_moveable player;

  uint8_t status;
  int     left;
  uint8_t lives;
  uint8_t keys;

  jumpnrun_shot shots[JUMPNRUN_MAX_SHOTS];
} jumpnrun_game_state;

vec2d hacker_extents(void);
void jumpnrun_passive_movement(vec2d *inertia);
badge_sprite const *jumpnrun_hacker_symbol(void);

uint8_t jumpnrun_play(char const *lvname);

#endif
