#ifndef INCLUDED_BADGE_JUMPNRUN_PLAYER_H
#define INCLUDED_BADGE_JUMPNRUN_PLAYER_H

#include "moveable.h"
#include "../ui/display.h"
#include "../util/util.h"

enum {
  // Do not collide with JUMPNRUN_MOVEABLE_* flags
  JUMPNRUN_PLAYER_DEAD = 128
};

enum {
  JUMPNRUN_PLAYER_FRAMES = 4
};

typedef struct jumpnrun_player {
  jumpnrun_moveable base;

  uint8_t lives;
  uint8_t keys;
} jumpnrun_player;

void jumpnrun_player_spawn        (jumpnrun_player *self, vec2d spawn_pos, uint8_t lives);
void jumpnrun_player_respawn      (jumpnrun_player *self, vec2d spawn_pos);

#endif
