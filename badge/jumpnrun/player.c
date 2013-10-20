#include "player.h"
#include "jumpnrun.h"
#include <stdlib.h>

void jumpnrun_player_respawn(jumpnrun_player *self, vec2d spawn_pos) {
  memset(&self->base, 0, sizeof(self->base));
  self->base.hitbox = rectangle_new(spawn_pos, jumpnrun_player_extents());
}

void jumpnrun_player_spawn  (jumpnrun_player *self, vec2d spawn_pos, uint8_t lives) {
  memset(self, 0, sizeof(*self));
  jumpnrun_player_respawn(self, spawn_pos);
  self->lives = lives;
}
