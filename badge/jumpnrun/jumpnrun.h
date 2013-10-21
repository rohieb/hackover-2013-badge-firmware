#ifndef INCLUDED_BADGE2013_JUMPNRUN_H
#define INCLUDED_BADGE2013_JUMPNRUN_H

#include "enemies.h"
#include "items.h"
#include "levels.h"
#include "shots.h"
#include "tiles.h"

#include "../ui/sprite.h"
#include "../util/util.h"

#include <stdbool.h>
#include <stddef.h>

enum {
  JUMPNRUN_PLAYING,
  JUMPNRUN_WON,
  JUMPNRUN_LOST,
  JUMPNRUN_ERROR
};

enum {
  JUMPNRUN_MAX_SPAWN_MARGIN = BADGE_SPRITE_MAX_WIDTH + 1,
  JUMPNRUN_MAX_SPAWNED_ENEMIES = 10
};

vec2d jumpnrun_player_extents(void);
void jumpnrun_apply_gravity(vec2d *inertia);
void jumpnrun_passive_movement(vec2d *inertia);
badge_sprite const *jumpnrun_hacker_symbol(void);

uint8_t jumpnrun_play_level(char const *lvname);
void jumpnrun_play(void);

#endif
