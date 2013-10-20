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
  JUMPNRUN_PLAYER_TICKS_PER_FRAME = 6,
  JUMPNRUN_PLAYER_FRAMES = 4
};

typedef struct jumpnrun_player {
  jumpnrun_moveable base;

  uint8_t lives;
  uint8_t keys;
} jumpnrun_player;

void jumpnrun_player_spawn  (jumpnrun_player *self, vec2d spawn_pos, uint8_t lives);
void jumpnrun_player_respawn(jumpnrun_player *self, vec2d spawn_pos);
void jumpnrun_player_kill   (jumpnrun_player *self);
void jumpnrun_player_despawn(jumpnrun_player *self);

void jumpnrun_player_advance_animation(jumpnrun_player *self);

static inline bool jumpnrun_player_alive(jumpnrun_player const *self) {
  return (self->base.flags & (JUMPNRUN_PLAYER_DEAD | JUMPNRUN_MOVEABLE_DYING)) == 0;
}

#endif
