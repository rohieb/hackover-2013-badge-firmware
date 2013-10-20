#include "enemies.h"

#include "collision.h"
#include "game_state.h"
#include "tiles.h"
#include "jumpnrun.h"
#include "moveable.h"
#include "render.h"

static badge_sprite const anim_cat[] = {
  { 8, 5, (uint8_t const *) "\xc7\x3f\xce\x38\x11" },
  { 8, 5, (uint8_t const *) "\xd7\x7d\xc6\x19\x25" }
};

static badge_sprite const anim_mushroom[] = {
  { 7, 7, (uint8_t const *) "\x10\x0c\x9f\xcf\xc7\x40" },
  { 7, 7, (uint8_t const *) "\x20\x18\x1e\x8f\x87\x81" },
  { 7, 7, (uint8_t const *) "\x10\x0c\x9f\xcf\xc7\x40" },
  { 7, 7, (uint8_t const *) "\x08\x86\xdf\xef\x67\x20" },
  { 7, 7, (uint8_t const *) "\x04\xc3\xef\xf7\x33\x10" },
  { 7, 7, (uint8_t const *) "\x04\xc3\xe7\xf3\x31\x10" }
};

static badge_sprite const anim_bunny[] = {
  { 7, 5, (uint8_t const *) "\x60\x30\xbe\x31\x02" },
  { 7, 5, (uint8_t const *) "\x42\x30\xbe\x31\x01" },
  { 7, 5, (uint8_t const *) "\x42\x30\xae\x35\x01" },
  { 7, 5, (uint8_t const *) "\x60\x30\xae\x35\x02" },
  { 7, 5, (uint8_t const *) "\x60\x30\xbe\x31\x01" }
};

static badge_sprite const anim_snake[] = {
  { 10, 6, (uint8_t const *) "\x10\x86\x83\x30\x04\x83\xa2\x07" },
  { 10, 6, (uint8_t const *) "\x10\x86\x83\x20\x0c\xc1\xa2\x07" },
  { 10, 6, (uint8_t const *) "\x10\x86\x83\x20\x08\x82\xe0\x07" },
  { 10, 6, (uint8_t const *) "\x10\x86\x83\x20\x08\x86\xe1\x07" }
};

static badge_sprite const anim_spiral[] = {
  { 10, 10, (uint8_t const *) "\xff\x07\xd8\x6f\xa1\xb5\xf6\x1a\xe8\xbf\x00\xfe\x0f" },
  { 10, 10, (uint8_t const *) "\xff\x07\xd8\x6f\xa1\xb5\xd6\xda\x69\xb0\x7f\x02\x0c" },
  { 10, 10, (uint8_t const *) "\xff\x07\xd8\x6f\xa1\xb5\xd6\x5a\x6a\xaf\x81\xfe\x0b" },
  { 10, 10, (uint8_t const *) "\xff\x07\xd8\x4f\x61\x35\xd5\x55\x54\x5f\x01\xfd\x07" },
  { 10, 10, (uint8_t const *) "\xff\x07\xd0\x7f\x81\xf5\xd6\x5a\x68\xbf\x01\xfe\x0f" },
  { 10, 10, (uint8_t const *) "\x03\xe4\xdf\x60\xb9\xb5\xd6\x5a\x68\xbf\x01\xfe\x0f" },
  { 10, 10, (uint8_t const *) "\xfd\x17\x58\x6f\xa5\xb5\xd6\x5a\x68\xbf\x01\xfe\x0f" },
  { 10, 10, (uint8_t const *) "\xfe\x0b\xa8\xaf\xa2\xba\xca\x6a\x28\xbf\x01\xfe\x0f" }
};

static badge_sprite const anim_rotor[] = {
  { 9, 9, (uint8_t const *) "\x00\x00\x00\x00\xf0\xe1\x42\x0c\x18\x70\x00" },
  { 9, 9, (uint8_t const *) "\x1c\x30\x60\x80\xf0\xe1\x40\x00\x00\x00\x00" },
  { 9, 9, (uint8_t const *) "\x1c\x30\x60\x84\x0e\x1f\x00\x00\x00\x00\x00" },
  { 9, 9, (uint8_t const *) "\x00\x00\x00\x04\x0e\x1f\x02\x0c\x18\x70\x00" }
};

static badge_sprite const anim_dog[] = {
  { 8, 5, (uint8_t const *) "\xc1\xf8\xc6\xb8\x08" },
  { 8, 5, (uint8_t const *) "\xc1\xba\xce\x99\x0c" },
  { 8, 5, (uint8_t const *) "\xc1\xf8\xc6\xb8\x10" },
  { 8, 5, (uint8_t const *) "\xc1\xf8\xc6\xb8\x20" }
};

static badge_sprite const anim_giraffe[] = {
  { 8, 10, (uint8_t const *) "\x00\x10\x60\x80\xff\xc1\x00\x03\x3c\x08" },
  { 8, 10, (uint8_t const *) "\x00\x10\x60\xa0\x7f\xc1\x00\x03\x1c\x88" },
  { 9, 10, (uint8_t const *) "\x00\x10\x68\x90\x3f\xc1\x00\x03\x0c\x48\x00\x02" },
  { 8, 10, (uint8_t const *) "\x00\x10\x60\xb0\x3f\xc1\x00\x03\x0c\xc8" },
  { 8, 10, (uint8_t const *) "\x08\x30\xc0\x81\xfc\xc0\x00\x03\x3c\x08" },
  { 8, 10, (uint8_t const *) "\x00\x10\x60\x80\xff\xc1\x00\x03\x3c\x08" }
};

static badge_sprite const anim_bird[] = {
  { 9, 7, (uint8_t const *) "\x10\x0c\x05\xf7\x4a\xa9\x58\x10" },
  { 9, 7, (uint8_t const *) "\x10\x0c\x05\x87\x47\xa2\x70\x10" }
/*
  { 10, 8, (uint8_t const *) "\x10\x10\x18\x14\x38\x2e\x29\x29\x2e\x10" },
  { 10, 8, (uint8_t const *) "\x10\x10\x18\x14\x38\x78\xa8\xa8\x78\x10" }
*/
};

static void enemy_animation_advance(jumpnrun_enemy *enemy) {
  ++enemy->base.tick_minor;
  if(enemy->base.tick_minor == enemy->type->animation_ticks_per_frame) {
    enemy->base.tick_minor = 0;

    ++enemy->base.anim_frame;
    if(enemy->base.anim_frame >= enemy->type->animation_length) {
      enemy->base.anim_frame = 0;
    }
  }
}

static void enemy_spawn(jumpnrun_enemy *self) {
  // enemy unspawned, available and in spawn zone.
  self->base.flags           = JUMPNRUN_ENEMY_SPAWNED | JUMPNRUN_ENEMY_UNAVAILABLE;
  self->base.anim_frame      = 0;
  self->base.tick_minor      = 0;
  self->base.jumpable_frames = 0;
}

void jumpnrun_enemy_despawn(jumpnrun_enemy *self) {
  // Despawned enemies are reset to their spawn position, so enemy_in_spawn_area will determine whether the spawn point is in the spawn area.
  self->base.flags  &= ~(JUMPNRUN_ENEMY_SPAWNED | JUMPNRUN_ENEMY_MOVING);
  self->base.hitbox  = rectangle_new(self->spawn_pos, self->type->hitbox.extent);
  self->base.inertia = self->type->spawn_inertia;
}

void jumpnrun_enemy_reset  (jumpnrun_enemy *self) {
  jumpnrun_enemy_despawn(self);
  self->base.flags &= ~JUMPNRUN_ENEMY_UNAVAILABLE;
}

static void enemy_bounce(jumpnrun_enemy *self) {
  self->base.inertia.x = fixed_point_neg(self->base.inertia.x);
}

static inline vec2d enemy_pos_hitbox (jumpnrun_enemy const *self) { return self->base.hitbox.pos; }
static inline vec2d enemy_pos_display(jumpnrun_enemy const *self) { return vec2d_sub(enemy_pos_hitbox(self), self->type->hitbox.pos); }
static inline vec2d enemy_pos_desired(jumpnrun_enemy const *self) { return vec2d_add(enemy_pos_hitbox(self), self->base.inertia); }

static inline void enemy_move_to(jumpnrun_enemy *self, vec2d new_pos) {
  rectangle_move_to(&self->base.hitbox, new_pos);
}

static inline bool enemy_in_area(jumpnrun_enemy const *self, jumpnrun_game_state *state, int margin) {
  return
    fixed_point_gt(rectangle_left (enemy_hitbox(self)), FIXED_INT(jumpnrun_screen_left (state) - margin)) &&
    fixed_point_lt(rectangle_right(enemy_hitbox(self)), FIXED_INT(jumpnrun_screen_right(state) + margin));
}

static inline bool enemy_on_screen(jumpnrun_enemy const *self, jumpnrun_game_state *state) {
  return enemy_in_area(self, state, self->type->animation_frames[self->base.anim_frame].width);
}

static inline bool enemy_in_spawn_area(jumpnrun_enemy const *self, jumpnrun_game_state *state) {
  return enemy_in_area(self, state, JUMPNRUN_MAX_SPAWN_MARGIN);
}

void jumpnrun_process_enemy(jumpnrun_enemy                   *self,
                            badge_framebuffer                *fb,
                            struct jumpnrun_game_state       *state,
                            struct jumpnrun_level            *lv,
                            struct jumpnrun_tile_range const *visible_tiles,
                            vec2d                            *player_inertia_mod) {
  // Despawned enemies are reset to their spawn position, so enemy_in_spawn_area will determine whether the spawn point is in the spawn area.
  if(self->base.flags & JUMPNRUN_ENEMY_SPAWNED) {
    if(!enemy_in_spawn_area(self, state) || fixed_point_gt(rectangle_top (enemy_hitbox(self)), FIXED_INT(BADGE_DISPLAY_HEIGHT))) {
      jumpnrun_enemy_despawn(self);
    } else {
      if((self->base.flags & JUMPNRUN_ENEMY_MOVING) || enemy_on_screen(self, state)) {
        self->base.flags |= JUMPNRUN_ENEMY_MOVING;

        self->type->move_tick(self, state, lv, visible_tiles, player_inertia_mod);
        self->type->collision_shots(self, state);
        if     (fixed_point_lt(self->base.inertia.x, FIXED_INT(0))) { self->base.flags &= ~JUMPNRUN_MOVEABLE_MIRRORED; }
        else if(fixed_point_ne(self->base.inertia.x, FIXED_INT(0))) { self->base.flags |=  JUMPNRUN_MOVEABLE_MIRRORED; }
      }

      if(fb) {
        jumpnrun_render_enemy(fb, state, self);
      }
    }
  } else if(self->base.flags & JUMPNRUN_ENEMY_UNAVAILABLE) {
    if(!enemy_in_spawn_area(self, state)) {
      self->base.flags &= ~JUMPNRUN_ENEMY_UNAVAILABLE;
    }
  } else if(enemy_in_spawn_area(self, state)) {
    // enemy unspawned, available and in spawn zone.
    enemy_spawn(self);
  }
}

void enemy_collision_tiles_bounce_horiz(jumpnrun_enemy            *self,
                                        vec2d                     *desired_position,
                                        jumpnrun_level            *lv,
                                        jumpnrun_tile_range const *visible_tiles) {
  vec2d inertia_mod = self->base.inertia;

  bool killed = collisions_tiles_displace(desired_position,
                                          &self->base,
                                          lv,
                                          visible_tiles,
                                          &inertia_mod);

  if(killed) {
    jumpnrun_enemy_despawn(self);
  } else if(fixed_point_ne(inertia_mod.x, self->base.inertia.x)) {
    enemy_bounce(self);
  }
}

void enemy_collision_player_deadly(struct jumpnrun_enemy      *self,
                                   struct jumpnrun_game_state *state,
                                   vec2d                      *player_inertia_mod) {
  (void) player_inertia_mod;

  if(rectangle_intersect(enemy_hitbox(self), &state->player.base.hitbox)) {
    state->player.base.flags |= JUMPNRUN_PLAYER_DEAD;
  }
}

void enemy_collision_player_jumpable(jumpnrun_enemy      *self,
                                     jumpnrun_game_state *state,
                                     vec2d               *player_inertia_mod)
{
  if(rectangle_intersect(enemy_hitbox(self), &state->player.base.hitbox)) {
    if(fixed_point_lt(rectangle_top(&state->player.base.hitbox), rectangle_top(enemy_hitbox(self))) &&
       fixed_point_gt(state->player.base.inertia.y, FIXED_INT(0)))
    {
      jumpnrun_enemy_despawn(self);
      player_inertia_mod->y = FIXED_POINT(0, -250);
      state->player.base.jumpable_frames = 12;
    } else {
      state->player.base.flags |= JUMPNRUN_PLAYER_DEAD;
    }
  }
}

void enemy_collision_tiles_pass_through(struct jumpnrun_enemy             *self,
                                        vec2d                             *desired_position,
                                        struct jumpnrun_level             *lv,
                                        struct jumpnrun_tile_range  const *visible_tiles) {
  (void) self;
  (void) desired_position;
  (void) lv;
  (void) visible_tiles;
}

void enemy_collision_shots_die(struct jumpnrun_enemy      *self,
                               struct jumpnrun_game_state *state) {
  for(uint8_t i = 0; i < JUMPNRUN_MAX_SHOTS; ++i) {
    jumpnrun_shot *shot = &state->shots[i];

    if(jumpnrun_shot_spawned(shot)) {
      if(rectangle_intersect(enemy_hitbox(self), &shot->current_box)) {
        self->base.flags &= ~JUMPNRUN_ENEMY_SPAWNED;
        jumpnrun_shot_despawn(shot);
      }
    }
  }
}

void enemy_collision_shots_bounce(struct jumpnrun_enemy      *self,
                                  struct jumpnrun_game_state *state) {
  for(uint8_t i = 0; i < JUMPNRUN_MAX_SHOTS; ++i) {
    jumpnrun_shot *shot = &state->shots[i];

    if(jumpnrun_shot_spawned(shot)) {
      if(rectangle_intersect(enemy_hitbox(self), &shot->current_box)) {
        if(fixed_point_gt(shot->inertia.x, FIXED_INT(0))) {
          rectangle_move_to_x(&shot->current_box, fixed_point_sub(rectangle_left(enemy_hitbox(self)), rectangle_width(&shot->current_box)));
        } else {
          rectangle_move_to_x(&shot->current_box, rectangle_right(enemy_hitbox(self)));
        }

        shot->inertia.x = fixed_point_neg(shot->inertia.x);
      }
    }
  }
}

void enemy_collision_shots_dontcare(struct jumpnrun_enemy      *self,
                                    struct jumpnrun_game_state *state) {
  (void) self;
  (void) state;
}

static void enemy_tick_common(jumpnrun_enemy            *self,
                               jumpnrun_game_state       *state,
                               jumpnrun_level            *lv,
                               jumpnrun_tile_range const *visible_tiles,
                                vec2d                     *player_inertia_mod) {
  vec2d new_pos = enemy_pos_desired(self);
  self->type->collision_tiles (self, &new_pos, lv, visible_tiles);
  self->type->collision_player(self, state, player_inertia_mod);
  enemy_move_to(self, new_pos);
}

void enemy_tick_straight_ahead(jumpnrun_enemy            *self,
                               jumpnrun_game_state       *state,
                               jumpnrun_level            *lv,
                               jumpnrun_tile_range const *visible_tiles,
                               vec2d                     *player_inertia_mod) {
  jumpnrun_passive_movement(&self->base.inertia);
  enemy_tick_common(self, state, lv, visible_tiles, player_inertia_mod);
  enemy_animation_advance(self);
}

void enemy_tick_straight_follow(jumpnrun_enemy            *self,
                                jumpnrun_game_state       *state,
                                jumpnrun_level            *lv,
                                jumpnrun_tile_range const *visible_tiles,
                                vec2d                     *player_inertia_mod) {
  jumpnrun_passive_movement(&self->base.inertia);
  enemy_tick_common(self, state, lv, visible_tiles, player_inertia_mod);

  if(fixed_point_lt(rectangle_right(&state->player.base.hitbox), rectangle_left(enemy_hitbox(self)))) {
    self->base.inertia.x = self->type->spawn_inertia.x;
  } else if(fixed_point_gt(rectangle_left(&state->player.base.hitbox), rectangle_right(enemy_hitbox(self)))) {
    self->base.inertia.x = fixed_point_neg(self->type->spawn_inertia.x);
  }

  enemy_animation_advance(self);
}

void enemy_tick_swing_up_and_down(struct jumpnrun_enemy            *self,
                                  struct jumpnrun_game_state       *state,
                                  struct jumpnrun_level            *lv,
                                  struct jumpnrun_tile_range const *visible_tiles,
                                  vec2d                            *player_inertia_mod) {
  enemy_tick_common(self, state, lv, visible_tiles, player_inertia_mod);

  self->base.inertia.y =
    fixed_point_add(fixed_point_add(self->base.inertia.y,
                                    fixed_point_div(self->type->spawn_inertia.y, FIXED_INT(3))),
                    fixed_point_mul(FIXED_POINT(0, 5),
                                    fixed_point_sub(self->spawn_pos.y,
                                                    enemy_pos_display(self).y)));

  enemy_animation_advance(self);
}

void enemy_tick_stationary(struct jumpnrun_enemy            *self,
                           struct jumpnrun_game_state       *state,
                           struct jumpnrun_level            *lv,
                           struct jumpnrun_tile_range const *visible_tiles,
                           vec2d                            *player_inertia_mod) {
  self->type->collision_tiles(self, &self->base.hitbox.pos, lv, visible_tiles);
  self->type->collision_player(self, state, player_inertia_mod);

  enemy_animation_advance(self);
}

void enemy_tick_jumper(jumpnrun_enemy            *self,
                       jumpnrun_game_state       *state,
                       jumpnrun_level            *lv,
                       jumpnrun_tile_range const *visible_tiles,
                       vec2d                     *player_inertia_mod) {
  jumpnrun_passive_movement(&self->base.inertia);
  enemy_tick_common(self, state, lv, visible_tiles, player_inertia_mod);

  if(jumpnrun_moveable_touching_ground(&self->base)) {
    self->base.inertia.y = self->type->spawn_inertia.y;
  }

  enemy_animation_advance(self);
}

void enemy_tick_dog(jumpnrun_enemy            *self,
                    jumpnrun_game_state       *state,
                    jumpnrun_level            *lv,
                    jumpnrun_tile_range const *visible_tiles,
                    vec2d                     *player_inertia_mod) {
  jumpnrun_passive_movement(&self->base.inertia);
  enemy_tick_common(self, state, lv, visible_tiles, player_inertia_mod);

  if(self->base.tick_minor % self->type->animation_ticks_per_frame == 0) {
    switch(self->base.tick_minor / self->type->animation_ticks_per_frame) {
    case 12:
      self->base.tick_minor = 0;
    case 0:
    case 2:
    case 4:
    case 6:
      self->base.anim_frame = 0;
      if(self->base.flags & JUMPNRUN_MOVEABLE_MIRRORED) {
        self->base.inertia.x = fixed_point_neg(self->type->spawn_inertia.x);
      } else {
        self->base.inertia.x = self->type->spawn_inertia.x;
      }

      break;

    case 1:
    case 3:
    case 5:
    case 7:
      self->base.anim_frame = 1;
      if(self->base.flags & JUMPNRUN_MOVEABLE_MIRRORED) {
        self->base.inertia.x = fixed_point_neg(self->type->spawn_inertia.x);
      } else {
        self->base.inertia.x = self->type->spawn_inertia.x;
      }
      break;

    case 8:
    case 10:
      self->base.anim_frame = 2;
      self->base.inertia.x = FIXED_INT(0);
      break;

    case 9:
    case 11:
      self->base.anim_frame = 3;
      self->base.inertia.x = FIXED_INT(0);
      break;
    }
  }

  ++self->base.tick_minor;
}

void enemy_tick_giraffe(jumpnrun_enemy            *self,
                        jumpnrun_game_state       *state,
                        jumpnrun_level            *lv,
                        jumpnrun_tile_range const *visible_tiles,
                        vec2d                     *player_inertia_mod) {
  bool was_on_ground = jumpnrun_moveable_touching_ground(&self->base);

  jumpnrun_passive_movement(&self->base.inertia);
  enemy_tick_common(self, state, lv, visible_tiles, player_inertia_mod);

  if(jumpnrun_moveable_touching_ground(&self->base)) {
    if(was_on_ground) {
      enemy_animation_advance(self);
      if(self->base.anim_frame == 0) {
        self->base.inertia = self->type->spawn_inertia;

        if(fixed_point_gt(rectangle_mid_x(&state->player.base.hitbox), rectangle_mid_x(enemy_hitbox(self)))) {
          self->base.inertia.x = fixed_point_neg(self->base.inertia.x);
        }
      }
    } else {
      self->base.tick_minor = 0;
      self->base.anim_frame = 3;
      self->base.inertia.x  = FIXED_INT(0);
    }
  } else if(was_on_ground) {
    self->base.tick_minor = 0;
    self->base.anim_frame = 1;
  } else if(self->base.anim_frame == 1) {
    enemy_animation_advance(self);
  }

  ++self->base.tick_minor;
}

void enemy_tick_fly_straight(struct jumpnrun_enemy            *self,
                             struct jumpnrun_game_state       *state,
                             struct jumpnrun_level            *lv,
                             struct jumpnrun_tile_range const *visible_tiles,
                             vec2d                            *player_inertia_mod) {
  enemy_tick_common(self, state, lv, visible_tiles, player_inertia_mod);
  enemy_animation_advance(self);
}

void enemy_tick_fly_straight_and_dip(struct jumpnrun_enemy            *self,
                                     struct jumpnrun_game_state       *state,
                                     struct jumpnrun_level            *lv,
                                     struct jumpnrun_tile_range const *visible_tiles,
                                     vec2d                            *player_inertia_mod) {
  if(fixed_point_lt(fixed_point_abs(fixed_point_sub(enemy_pos_hitbox(self).x,
                                                    state->player.base.hitbox.pos.x)),
                    FIXED_INT(20))) {
    self->base.flags |= JUMPNRUN_ENEMY_EVENT_TRIGGER1;
  }

  if(self->base.flags & JUMPNRUN_ENEMY_EVENT_TRIGGER1) {
    self->base.inertia.y =
      fixed_point_add(fixed_point_add(self->base.inertia.y,
                                      fixed_point_div(self->type->spawn_inertia.y, FIXED_INT(3))),
                      fixed_point_mul(FIXED_POINT(0, 5),
                                      fixed_point_sub(self->spawn_pos.y,
                                                      enemy_pos_hitbox(self).y)));
  } else {
    self->base.inertia.y = FIXED_INT(0);
  }

  enemy_tick_common(self, state, lv, visible_tiles, player_inertia_mod);
  enemy_animation_advance(self);
}

jumpnrun_enemy_type const jumpnrun_enemy_type_data[JUMPNRUN_ENEMY_TYPE_COUNT] = {
  {
    .animation_ticks_per_frame = 18,
    .animation_length          = ARRAY_SIZE(anim_cat),
    .animation_frames          = anim_cat,
    .hitbox                    = { { FIXED_INT_I(1), FIXED_INT_I(2) },
                                   { FIXED_INT_I(6), FIXED_INT_I(3) } },
    .spawn_inertia             = { FIXED_POINT_I(0, -200), FIXED_INT_I(0) },
    .collision_tiles           = enemy_collision_tiles_bounce_horiz,
    .collision_player          = enemy_collision_player_jumpable,
    .collision_shots           = enemy_collision_shots_die,
    .move_tick                 = enemy_tick_straight_ahead
  }, {
    .animation_ticks_per_frame = 12,
    .animation_length          = ARRAY_SIZE(anim_mushroom),
    .animation_frames          = anim_mushroom,
    .hitbox                    = { { FIXED_INT_I(1), FIXED_INT_I(1) },
                                   { FIXED_INT_I(5), FIXED_INT_I(6) } },
    .spawn_inertia             = { FIXED_POINT_I(0, -80), FIXED_INT_I(0) },
    .collision_tiles           = enemy_collision_tiles_bounce_horiz,
    .collision_player          = enemy_collision_player_jumpable,
    .collision_shots           = enemy_collision_shots_die,
    .move_tick                 = enemy_tick_straight_follow
  }, {
    .animation_ticks_per_frame = 9,
    .animation_length          = ARRAY_SIZE(anim_bunny),
    .animation_frames          = anim_bunny,
    .hitbox                    = { { FIXED_INT_I(1), FIXED_INT_I(2) },
                                   { FIXED_INT_I(5), FIXED_INT_I(3) } },
    .spawn_inertia             = { FIXED_POINT_I(0, -80), FIXED_POINT_I(0, -800) },
    .collision_tiles           = enemy_collision_tiles_bounce_horiz,
    .collision_player          = enemy_collision_player_jumpable,
    .collision_shots           = enemy_collision_shots_die,
    .move_tick                 = enemy_tick_jumper
  }, {
    .animation_ticks_per_frame = 6,
    .animation_length          = ARRAY_SIZE(anim_snake),
    .animation_frames          = anim_snake,
    .hitbox                    = { { FIXED_INT_I(1), FIXED_INT_I(4) },
                                   { FIXED_INT_I(8), FIXED_INT_I(2) } },
    .spawn_inertia             = { FIXED_POINT_I(0, -150), FIXED_INT_I(0) },
    .collision_tiles           = enemy_collision_tiles_bounce_horiz,
    .collision_player          = enemy_collision_player_jumpable,
    .collision_shots           = enemy_collision_shots_die,
    .move_tick                 = enemy_tick_straight_ahead
  }, {
    .animation_ticks_per_frame = 6,
    .animation_length          = ARRAY_SIZE(anim_spiral),
    .animation_frames          = anim_spiral,
    .hitbox                    = { { FIXED_INT_I(1), FIXED_INT_I(1) },
                                   { FIXED_INT_I(8), FIXED_INT_I(8) } },
    .spawn_inertia             = { FIXED_INT_I(0), FIXED_POINT_I(0, -200) },
    .collision_tiles           = enemy_collision_tiles_pass_through,
    .collision_player          = enemy_collision_player_deadly,
    .collision_shots           = enemy_collision_shots_dontcare,
    .move_tick                 = enemy_tick_swing_up_and_down
  }, {
    .animation_ticks_per_frame = 5,
    .animation_length          = ARRAY_SIZE(anim_rotor),
    .animation_frames          = anim_rotor,
    .hitbox                    = { { FIXED_INT_I(1), FIXED_INT_I(1) },
                                   { FIXED_INT_I(7), FIXED_INT_I(7) } },
    .spawn_inertia             = { FIXED_INT_I(0), FIXED_POINT_I(0, 0) },
    .collision_tiles           = enemy_collision_tiles_pass_through,
    .collision_player          = enemy_collision_player_deadly,
    .collision_shots           = enemy_collision_shots_dontcare,
    .move_tick                 = enemy_tick_stationary
  }, {
    .animation_ticks_per_frame = 18,
    .animation_length          = ARRAY_SIZE(anim_dog),
    .animation_frames          = anim_dog,
    .hitbox                    = { { FIXED_INT_I(1), FIXED_INT_I(1) },
                                   { FIXED_INT_I(6), FIXED_INT_I(4) } },
    .spawn_inertia             = { FIXED_POINT_I(0, -200), FIXED_POINT_I(0, 0) },
    .collision_tiles           = enemy_collision_tiles_bounce_horiz,
    .collision_player          = enemy_collision_player_jumpable,
    .collision_shots           = enemy_collision_shots_die,
    .move_tick                 = enemy_tick_dog
  }, {
    .animation_ticks_per_frame = 36,
    .animation_length          = ARRAY_SIZE(anim_giraffe),
    .animation_frames          = anim_giraffe,
    .hitbox                    = { { FIXED_INT_I(2), FIXED_INT_I(1) },
                                   { FIXED_INT_I(4), FIXED_INT_I(9) } },
    .spawn_inertia             = { FIXED_POINT_I(0, -150), FIXED_POINT_I(-1, -200) },
    .collision_tiles           = enemy_collision_tiles_bounce_horiz,
    .collision_player          = enemy_collision_player_jumpable,
    .collision_shots           = enemy_collision_shots_bounce,
    .move_tick                 = enemy_tick_giraffe
  }, {
    .animation_ticks_per_frame = 24,
    .animation_length          = ARRAY_SIZE(anim_bird),
    .animation_frames          = anim_bird,
    .hitbox                    = { { FIXED_INT_I(1), FIXED_INT_I(3) },
                                   { FIXED_INT_I(7), FIXED_INT_I(3) } },
    .spawn_inertia             = { FIXED_POINT_I(0, -400), FIXED_POINT_I(0, -150) },
    .collision_tiles           = enemy_collision_tiles_bounce_horiz,
    .collision_player          = enemy_collision_player_jumpable,
    .collision_shots           = enemy_collision_shots_die,
    .move_tick                 = enemy_tick_swing_up_and_down
  }, {
    .animation_ticks_per_frame = 24,
    .animation_length          = ARRAY_SIZE(anim_bird),
    .animation_frames          = anim_bird,
    .hitbox                    = { { FIXED_INT_I(1), FIXED_INT_I(3) },
                                   { FIXED_INT_I(7), FIXED_INT_I(3) } },
    .spawn_inertia             = { FIXED_POINT_I(0, -400), FIXED_INT_I(0) },
    .collision_tiles           = enemy_collision_tiles_bounce_horiz,
    .collision_player          = enemy_collision_player_jumpable,
    .collision_shots           = enemy_collision_shots_die,
    .move_tick                 = enemy_tick_fly_straight
  }, {
    .animation_ticks_per_frame = 24,
    .animation_length          = ARRAY_SIZE(anim_bird),
    .animation_frames          = anim_bird,
    .hitbox                    = { { FIXED_INT_I(1), FIXED_INT_I(3) },
                                   { FIXED_INT_I(7), FIXED_INT_I(3) } },
    .spawn_inertia             = { FIXED_POINT_I(0, -400), FIXED_POINT_I(0, 200) },
    .collision_tiles           = enemy_collision_tiles_bounce_horiz,
    .collision_player          = enemy_collision_player_jumpable,
    .collision_shots           = enemy_collision_shots_die,
    .move_tick                 = enemy_tick_fly_straight_and_dip
  }
};
