#include "enemies.h"

#include "collision.h"
#include "tiles.h"
#include "jumpnrun.h"

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

void jumpnrun_process_enemy(jumpnrun_enemy                   *self,
                            badge_framebuffer                *fb,
                            struct jumpnrun_game_state       *state,
                            struct jumpnrun_level            *lv,
                            struct jumpnrun_tile_range const *visible_tiles,
                            vec2d                            *player_inertia_mod) {
  int const spawn_margin = 1 + self->type->animation_frames[self->base.anim_frame].width;

  if(self->flags & JUMPNRUN_ENEMY_SPAWNED) {
    if(fixed_point_lt(rectangle_left(enemy_box(self)), FIXED_POINT(state->left                       - spawn_margin, 0)) ||
       fixed_point_gt(rectangle_left(enemy_box(self)), FIXED_POINT(state->left + BADGE_DISPLAY_WIDTH + spawn_margin, 0)) ||
       fixed_point_gt(rectangle_top (enemy_box(self)), FIXED_POINT(BADGE_DISPLAY_HEIGHT                            , 0))) {
      self->flags &= ~JUMPNRUN_ENEMY_SPAWNED;
    } else {
      self->type->game_tick(self, state, lv, visible_tiles, player_inertia_mod);

      if(fb) {
        badge_framebuffer_blt(fb,
                              fixed_point_cast_int(rectangle_left(enemy_box(self))) - state->left,
                              fixed_point_cast_int(rectangle_top (enemy_box(self))),
                              enemy_sprite(self),
                              enemy_render_flags(self));
      }
    }
  } else if(self->flags & JUMPNRUN_ENEMY_UNAVAILABLE) {
    if(state->left                                      > fixed_point_cast_int(self->spawn_pos.x) + spawn_margin ||
       state->left + BADGE_DISPLAY_WIDTH + spawn_margin < fixed_point_cast_int(self->spawn_pos.x)) {
      self->flags &= ~JUMPNRUN_ENEMY_UNAVAILABLE;
    }
  } else if((fixed_point_gt(self->spawn_pos.x, FIXED_POINT(state->left                       - spawn_margin, 0)) &&
             fixed_point_lt(self->spawn_pos.x, FIXED_POINT(state->left - spawn_margin / 2, 0))) ||
            (fixed_point_lt(self->spawn_pos.x, FIXED_POINT(state->left + BADGE_DISPLAY_WIDTH + spawn_margin, 0)) &&
             fixed_point_gt(self->spawn_pos.x, FIXED_POINT(state->left + BADGE_DISPLAY_WIDTH, 0)))) {
    // enemy unspawned, available and in spawn zone.
    self->flags               |= JUMPNRUN_ENEMY_SPAWNED | JUMPNRUN_ENEMY_UNAVAILABLE;
    self->base.current_box     = rectangle_new(self->spawn_pos, self->type->extent);
    self->base.inertia         = self->type->spawn_inertia;
    self->base.anim_frame      = 0;
    self->base.tick_minor      = 0;
    self->base.touching_ground = false;
    self->base.jumpable_frames = 0;
  }
}

void enemy_collision_tiles_bounce_horiz(jumpnrun_enemy            *self,
                                        vec2d                     *desired_position,
                                        jumpnrun_level            *lv,
                                        jumpnrun_tile_range const *visible_tiles) {
  vec2d inertia_mod = self->base.inertia;

  collisions_tiles_displace(desired_position,
                            &self->base,
                            lv,
                            visible_tiles,
                            &inertia_mod);

  if(fixed_point_ne(inertia_mod.x, self->base.inertia.x)) {
    self->base.inertia.x = fixed_point_neg(self->base.inertia.x);
  }
}

void enemy_collision_player_deadly(struct jumpnrun_enemy      *self,
                                   struct jumpnrun_game_state *state,
                                   vec2d                      *player_inertia_mod) {
  (void) player_inertia_mod;

  rectangle rect_self = enemy_hitbox(self);

  if(rectangle_intersect(&rect_self, &state->player.current_box)) {
    state->status = JUMPNRUN_DEAD;
  }
}

void enemy_collision_player_jumpable(jumpnrun_enemy      *self,
                                     jumpnrun_game_state *state,
                                     vec2d               *player_inertia_mod)
{
  rectangle rect_self = enemy_hitbox(self);

  if(rectangle_intersect(&rect_self, &state->player.current_box)) {
    if(fixed_point_gt(state->player.inertia.y, FIXED_POINT(0, 0))) {
      self->flags &= ~JUMPNRUN_ENEMY_SPAWNED;
      player_inertia_mod->y = FIXED_POINT(0, -250);
      state->player.jumpable_frames = 12;
    } else {
      state->status = JUMPNRUN_DEAD;
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
  return;
}

void enemy_tick_straight_ahead(jumpnrun_enemy            *self,
                               jumpnrun_game_state       *state,
                               jumpnrun_level            *lv,
                               jumpnrun_tile_range const *visible_tiles,
                               vec2d                     *player_inertia_mod) {
  int screenpos = fixed_point_cast_int(rectangle_left(&self->base.current_box));

  if(screenpos + JUMPNRUN_MAX_SPAWN_MARGIN <  state->left ||
     screenpos                             >= state->left + BADGE_DISPLAY_WIDTH + JUMPNRUN_MAX_SPAWN_MARGIN) {
    return;
  }

  jumpnrun_passive_movement(&self->base.inertia);

  vec2d new_pos = vec2d_add(enemy_position(self), self->base.inertia);
  self->type->collision_tiles(self, &new_pos, lv, visible_tiles);
  self->type->collision_player(self, state, player_inertia_mod);
  rectangle_move_to(&self->base.current_box, new_pos);

  enemy_animation_advance(self);
}

void enemy_tick_swing_up_and_down(struct jumpnrun_enemy            *self,
                                  struct jumpnrun_game_state       *state,
                                  struct jumpnrun_level            *lv,
                                  struct jumpnrun_tile_range const *visible_tiles,
                                  vec2d                            *player_inertia_mod) {
  int screenpos = fixed_point_cast_int(rectangle_left(&self->base.current_box));

  if(screenpos + JUMPNRUN_MAX_SPAWN_MARGIN <  state->left ||
     screenpos                             >= state->left + BADGE_DISPLAY_WIDTH + JUMPNRUN_MAX_SPAWN_MARGIN) {
    return;
  }

  vec2d new_pos = vec2d_add(enemy_position(self), self->base.inertia);
  self->type->collision_tiles(self, &new_pos, lv, visible_tiles);
  self->type->collision_player(self, state, player_inertia_mod);
  rectangle_move_to(&self->base.current_box, new_pos);

  self->base.inertia.y =
    fixed_point_add(fixed_point_add(self->base.inertia.y,
                                    fixed_point_div(self->type->spawn_inertia.y, FIXED_INT(3))),
                    fixed_point_mul(FIXED_POINT(0, 5),
                                    fixed_point_sub(self->spawn_pos.y,
                                                    enemy_position(self).y)));

  enemy_animation_advance(self);
}

void enemy_tick_stationary(struct jumpnrun_enemy            *self,
                           struct jumpnrun_game_state       *state,
                           struct jumpnrun_level            *lv,
                           struct jumpnrun_tile_range const *visible_tiles,
                           vec2d                            *player_inertia_mod) {
  int screenpos = fixed_point_cast_int(rectangle_left(&self->base.current_box));

  if(screenpos + JUMPNRUN_MAX_SPAWN_MARGIN <  state->left ||
     screenpos                             >= state->left + BADGE_DISPLAY_WIDTH + JUMPNRUN_MAX_SPAWN_MARGIN) {
    return;
  }

  self->type->collision_tiles(self, &self->base.current_box.pos, lv, visible_tiles);
  self->type->collision_player(self, state, player_inertia_mod);

  enemy_animation_advance(self);
}

void enemy_tick_jumper(jumpnrun_enemy            *self,
                       jumpnrun_game_state       *state,
                       jumpnrun_level            *lv,
                       jumpnrun_tile_range const *visible_tiles,
                       vec2d                     *player_inertia_mod) {
  int screenpos = fixed_point_cast_int(rectangle_left(&self->base.current_box));

  if(screenpos + JUMPNRUN_MAX_SPAWN_MARGIN <  state->left ||
     screenpos                             >= state->left + BADGE_DISPLAY_WIDTH + JUMPNRUN_MAX_SPAWN_MARGIN) {
    return;
  }

  jumpnrun_passive_movement(&self->base.inertia);

  vec2d new_pos = vec2d_add(enemy_position(self), self->base.inertia);
  self->type->collision_tiles(self, &new_pos, lv, visible_tiles);
  self->type->collision_player(self, state, player_inertia_mod);
  rectangle_move_to(&self->base.current_box, new_pos);

  if(self->base.touching_ground) {
    self->base.inertia.y = self->type->spawn_inertia.y;
  }

  enemy_animation_advance(self);
}

void enemy_tick_dog(jumpnrun_enemy            *self,
                    jumpnrun_game_state       *state,
                    jumpnrun_level            *lv,
                    jumpnrun_tile_range const *visible_tiles,
                    vec2d                     *player_inertia_mod) {
  int screenpos = fixed_point_cast_int(rectangle_left(&self->base.current_box));

  if(screenpos + JUMPNRUN_MAX_SPAWN_MARGIN <  state->left ||
     screenpos                             >= state->left + BADGE_DISPLAY_WIDTH + JUMPNRUN_MAX_SPAWN_MARGIN) {
    return;
  }

  jumpnrun_passive_movement(&self->base.inertia);

  vec2d new_pos = vec2d_add(enemy_position(self), self->base.inertia);
  self->type->collision_tiles(self, &new_pos, lv, visible_tiles);
  self->type->collision_player(self, state, player_inertia_mod);
  rectangle_move_to(&self->base.current_box, new_pos);

  if(self->base.tick_minor % self->type->animation_ticks_per_frame == 0) {
    switch(self->base.tick_minor / self->type->animation_ticks_per_frame) {
    case 12:
      self->base.tick_minor = 0;
    case 0:
    case 2:
    case 4:
    case 6:
      self->base.anim_frame = 0;
      if(self->flags & JUMPNRUN_ENEMY_FACING_RIGHT) {
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
      if(self->flags & JUMPNRUN_ENEMY_FACING_RIGHT) {
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
  if     (fixed_point_lt(self->base.inertia.x, FIXED_INT(0))) { self->flags &= ~JUMPNRUN_ENEMY_FACING_RIGHT; }
  else if(fixed_point_ne(self->base.inertia.x, FIXED_INT(0))) { self->flags |=  JUMPNRUN_ENEMY_FACING_RIGHT; }
}

void enemy_tick_giraffe(jumpnrun_enemy            *self,
                        jumpnrun_game_state       *state,
                        jumpnrun_level            *lv,
                        jumpnrun_tile_range const *visible_tiles,
                        vec2d                     *player_inertia_mod) {
  int screenpos = fixed_point_cast_int(rectangle_left(&self->base.current_box));

  if(screenpos + JUMPNRUN_MAX_SPAWN_MARGIN <  state->left ||
     screenpos                             >= state->left + BADGE_DISPLAY_WIDTH + JUMPNRUN_MAX_SPAWN_MARGIN) {
    return;
  }

  bool was_on_ground = self->base.touching_ground;

  jumpnrun_passive_movement(&self->base.inertia);

  vec2d new_pos = vec2d_add(enemy_position(self), self->base.inertia);
  self->type->collision_tiles(self, &new_pos, lv, visible_tiles);
  self->type->collision_player(self, state, player_inertia_mod);
  rectangle_move_to(&self->base.current_box, new_pos);

  if(self->base.touching_ground) {
    if(was_on_ground) {
      enemy_animation_advance(self);
      if(self->base.anim_frame == 0) {
        if(self->flags & JUMPNRUN_ENEMY_FACING_RIGHT) {
          self->base.inertia.x = fixed_point_neg(self->type->spawn_inertia.x);
          self->base.inertia.y = self->type->spawn_inertia.y;
        } else {
          self->base.inertia = self->type->spawn_inertia;
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
  if     (fixed_point_lt(self->base.inertia.x, FIXED_INT(0))) { self->flags &= ~JUMPNRUN_ENEMY_FACING_RIGHT; }
  else if(fixed_point_ne(self->base.inertia.x, FIXED_INT(0))) { self->flags |=  JUMPNRUN_ENEMY_FACING_RIGHT; }
}

jumpnrun_enemy_type const jumpnrun_enemy_type_data[JUMPNRUN_ENEMY_TYPE_COUNT] = {
  {
    .animation_ticks_per_frame = 16,
    .animation_length          = ARRAY_SIZE(anim_cat),
    .animation_frames          = anim_cat,
    .extent                    = { FIXED_INT_I(8), FIXED_INT_I(5) },
    .hitbox                    = { { FIXED_INT_I(1), FIXED_INT_I(2) },
                                   { FIXED_INT_I(6), FIXED_INT_I(3) } },
    .spawn_inertia             = { FIXED_POINT_I(0, -200), FIXED_INT_I(0) },
    .collision_tiles           = enemy_collision_tiles_bounce_horiz,
    .collision_player          = enemy_collision_player_jumpable,
    .game_tick                 = enemy_tick_straight_ahead
  }, {
    .animation_ticks_per_frame = 12,
    .animation_length          = ARRAY_SIZE(anim_mushroom),
    .animation_frames          = anim_mushroom,
    .extent                    = { FIXED_INT_I(7), FIXED_INT_I(7) },
    .hitbox                    = { { FIXED_INT_I(1), FIXED_INT_I(1) },
                                   { FIXED_INT_I(5), FIXED_INT_I(4) } },
    .spawn_inertia             = { FIXED_POINT_I(0, -50), FIXED_INT_I(0) },
    .collision_tiles           = enemy_collision_tiles_bounce_horiz,
    .collision_player          = enemy_collision_player_jumpable,
    .game_tick                 = enemy_tick_straight_ahead
  }, {
    .animation_ticks_per_frame = 9,
    .animation_length          = ARRAY_SIZE(anim_bunny),
    .animation_frames          = anim_bunny,
    .extent                    = { FIXED_INT_I(7), FIXED_INT_I(5) },
    .hitbox                    = { { FIXED_INT_I(1), FIXED_INT_I(2) },
                                   { FIXED_INT_I(5), FIXED_INT_I(3) } },
    .spawn_inertia             = { FIXED_POINT_I(0, -80), FIXED_POINT_I(0, -800) },
    .collision_tiles           = enemy_collision_tiles_bounce_horiz,
    .collision_player          = enemy_collision_player_jumpable,
    .game_tick                 = enemy_tick_jumper
  }, {
    .animation_ticks_per_frame = 6,
    .animation_length          = ARRAY_SIZE(anim_snake),
    .animation_frames          = anim_snake,
    .extent                    = { FIXED_INT_I(10), FIXED_INT_I(6) },
    .hitbox                    = { { FIXED_INT_I(1), FIXED_INT_I(4) },
                                   { FIXED_INT_I(8), FIXED_INT_I(2) } },
    .spawn_inertia             = { FIXED_POINT_I(0, -150), FIXED_INT_I(0) },
    .collision_tiles           = enemy_collision_tiles_bounce_horiz,
    .collision_player          = enemy_collision_player_jumpable,
    .game_tick                 = enemy_tick_straight_ahead
  }, {
    .animation_ticks_per_frame = 6,
    .animation_length          = ARRAY_SIZE(anim_spiral),
    .animation_frames          = anim_spiral,
    .extent                    = { FIXED_INT_I(10), FIXED_INT_I(10) },
    .hitbox                    = { { FIXED_INT_I(1), FIXED_INT_I(1) },
                                   { FIXED_INT_I(8), FIXED_INT_I(8) } },
    .spawn_inertia             = { FIXED_INT_I(0), FIXED_POINT_I(0, -200) },
    .collision_tiles           = enemy_collision_tiles_pass_through,
    .collision_player          = enemy_collision_player_deadly,
    .game_tick                 = enemy_tick_swing_up_and_down
  }, {
    .animation_ticks_per_frame = 5,
    .animation_length          = ARRAY_SIZE(anim_rotor),
    .animation_frames          = anim_rotor,
    .extent                    = { FIXED_INT_I(9), FIXED_INT_I(9) },
    .hitbox                    = { { FIXED_INT_I(1), FIXED_INT_I(1) },
                                   { FIXED_INT_I(7), FIXED_INT_I(7) } },
    .spawn_inertia             = { FIXED_INT_I(0), FIXED_POINT_I(0, 0) },
    .collision_tiles           = enemy_collision_tiles_pass_through,
    .collision_player          = enemy_collision_player_deadly,
    .game_tick                 = enemy_tick_stationary
  }, {
    .animation_ticks_per_frame = 16,
    .animation_length          = ARRAY_SIZE(anim_dog),
    .animation_frames          = anim_dog,
    .extent                    = { FIXED_INT_I(8), FIXED_INT_I(5) },
    .hitbox                    = { { FIXED_INT_I(1), FIXED_INT_I(1) },
                                   { FIXED_INT_I(6), FIXED_INT_I(4) } },
    .spawn_inertia             = { FIXED_POINT_I(0, -200), FIXED_POINT_I(0, 0) },
    .collision_tiles           = enemy_collision_tiles_bounce_horiz,
    .collision_player          = enemy_collision_player_jumpable,
    .game_tick                 = enemy_tick_dog
  }, {
    .animation_ticks_per_frame = 36,
    .animation_length          = ARRAY_SIZE(anim_giraffe),
    .animation_frames          = anim_giraffe,
    .extent                    = { FIXED_INT_I(7), FIXED_INT_I(10) },
    .hitbox                    = { { FIXED_INT_I(2), FIXED_INT_I(1) },
                                   { FIXED_INT_I(4), FIXED_INT_I(9) } },
    .spawn_inertia             = { FIXED_POINT_I(0, -150), FIXED_POINT_I(-1, -200) },
    .collision_tiles           = enemy_collision_tiles_bounce_horiz,
    .collision_player          = enemy_collision_player_jumpable,
    .game_tick                 = enemy_tick_giraffe
  }
};
