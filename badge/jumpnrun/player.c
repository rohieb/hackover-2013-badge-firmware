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

void jumpnrun_player_kill   (jumpnrun_player *self) {
  if((self->base.flags & JUMPNRUN_MOVEABLE_DYING) == 0) {
    self->base.flags |= JUMPNRUN_MOVEABLE_DYING;
    self->base.tick_minor = 0;
  }
}

void jumpnrun_player_despawn(jumpnrun_player *self) {
  self->base.flags |=  JUMPNRUN_PLAYER_DEAD;
  self->base.flags &= ~JUMPNRUN_MOVEABLE_DYING;
  self->base.inertia = (vec2d) { FIXED_INT(0), FIXED_INT(0) };
}

void jumpnrun_player_advance_animation(jumpnrun_player *self) {
  ++self->base.tick_minor;
}
