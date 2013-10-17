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

typedef struct jumpnrun_game_state {
  jumpnrun_moveable player;

  uint8_t status;
  int     left;

  size_t  spawned_enemies_counter;
  size_t  spawned_enemies[JUMPNRUN_MAX_SPAWNED_ENEMIES];
} jumpnrun_game_state;

rectangle hacker_rect_current(jumpnrun_game_state const *state);

void jumpnrun_passive_movement(vec2d *inertia);

uint8_t jumpnrun_play(char const *lvname);

#endif
