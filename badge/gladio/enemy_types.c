#include "enemy_types.h"
#include "game_state.h"

#include "../ui/sprite.h"
#include "../util/vec2d.h"

static vec2d gladio_enemy_snout_left(gladio_enemy *self) {
  rectangle hitbox = gladio_enemy_hitbox(self);
  return vec2d_new(rectangle_left(&hitbox), rectangle_mid_y(&hitbox));
}
/*
static vec2d gladio_enemy_snout_right(gladio_enemy *self) {
  rectangle hitbox = gladio_enemy_hitbox(self);
  return vec2d_new(rectangle_right(&hitbox), rectangle_mid_y(&hitbox));
}

static vec2d gladio_enemy_snout_top(gladio_enemy *self) {
  rectangle hitbox = gladio_enemy_hitbox(self);
  return vec2d_new(rectangle_mid_x(&hitbox), rectangle_top(&hitbox));
}

static vec2d gladio_enemy_snout_bottom(gladio_enemy *self) {
  rectangle hitbox = gladio_enemy_hitbox(self);
  return vec2d_new(rectangle_mid_x(&hitbox), rectangle_bottom(&hitbox));
}
*/

static void tick_move_straight_ahead(gladio_enemy *self, gladio_game_state *state) {
  (void) state;

  self->base.position.x = fixed_point_sub(self->base.position.x, FIXED_INT(1));

  rectangle r = gladio_enemy_hitbox(self);
  if(fixed_point_lt(rectangle_right(&r), FIXED_INT(0))) {
    gladio_enemy_despawn(self);
  }
}

static void tick_move_tumble(gladio_enemy *self, gladio_game_state *state) {
  (void) state;

  int8_t ppos = self->move_counter % 64;
  int8_t factor = ppos >= 32 ? 1 : -1;

  fixed_point diff_y = fixed_point_div(FIXED_INT(factor), FIXED_INT(2));

  self->base.position.x = fixed_point_sub(self->base.position.x, FIXED_INT(1));
  self->base.position.y = fixed_point_add(self->base.position.y, diff_y);

  rectangle r = gladio_enemy_hitbox(self);
  if(fixed_point_lt(rectangle_right(&r), FIXED_INT(0))) {
    gladio_enemy_despawn(self);
  }
}

static void tick_move_zigzag(gladio_enemy *self, int8_t direction) {
  if(self->move_counter < 32) {
    self->base.position.x = fixed_point_sub(self->base.position.x, FIXED_INT(1));
  } else if(self->move_counter < 64) {
    self->base.position.x = fixed_point_add(self->base.position.x, FIXED_POINT(0, 500));
    self->base.position.y = fixed_point_add(self->base.position.y, FIXED_POINT(0, direction * 750));
  } else {
    self->base.position.x = fixed_point_sub(self->base.position.x, FIXED_INT(2));
  }

  rectangle r = gladio_enemy_hitbox(self);
  if(fixed_point_lt(rectangle_right(&r), FIXED_INT(0))) {
    gladio_enemy_despawn(self);
  }
}

static void tick_move_zigzag_down(gladio_enemy *self, gladio_game_state *state) {
  (void) state;
  tick_move_zigzag(self, 1);
}

static void tick_move_zigzag_up(gladio_enemy *self, gladio_game_state *state) {
  (void) state;
  tick_move_zigzag(self, -1);
}

static void tick_shoot_not(gladio_enemy *self, gladio_game_state *state) {
  (void) self;
  (void) state;
}

static void tick_shoot_forward(gladio_enemy *self, gladio_game_state *state) {
  vec2d snout_pos = gladio_enemy_snout_left(self);
  gladio_shot_hostile_spawn(state, snout_pos, vec2d_new(FIXED_POINT(0, -500), FIXED_INT(0)));
}

static void tick_shoot_zigzag(gladio_enemy *self, gladio_game_state *state) {
  if(self->move_counter < 64) {
    vec2d snout_pos = gladio_enemy_snout_left(self);
    gladio_shot_hostile_spawn(state, snout_pos, vec2d_new(FIXED_POINT(-1, -500), FIXED_INT(0)));
  }
}

static void tick_shoot_up(gladio_enemy *self, gladio_game_state *state) {
  vec2d snout_pos = self->base.position;
  gladio_shot_hostile_spawn(state, snout_pos, vec2d_new(FIXED_INT(0), FIXED_INT(-1)));
}

static void tick_shoot_down(gladio_enemy *self, gladio_game_state *state) {
  vec2d snout_pos = vec2d_add(self->base.position,
                              vec2d_new(FIXED_INT(0),
                                        FIXED_INT(gladio_enemy_type_get(self)->sprite.height)));
  gladio_shot_hostile_spawn(state, snout_pos, vec2d_new(FIXED_INT(0), FIXED_INT(1)));
}

static void tick_shoot_targeted(gladio_enemy *self, gladio_game_state *state) {
  vec2d diff = vec2d_sub(state->player.base.position, self->base.position);
  diff = vec2d_div(diff, fixed_point_mul(vec2d_length_approx(diff), FIXED_POINT(1, 500)));

  vec2d snout_pos  = gladio_enemy_snout_left(self);

  gladio_shot_hostile_spawn(state, snout_pos, diff);
  self->cooldown = 48;
}

static void collision_player_simple(gladio_enemy *self, gladio_game_state *state) {
  --self->hitpoints;
  gladio_player_damage(&state->player, 1);

  if(self->hitpoints == 0) {
    gladio_enemy_schedule_death(self);
  }
}

static void collision_shots_instant_death(gladio_enemy *self, gladio_shot *shot) {
  gladio_enemy_schedule_death(self);
  gladio_shot_despawn_later(shot);
}

static void collision_shots_simple(gladio_enemy *self, gladio_shot *shot) {
  if(self->hitpoints < 2) {
    collision_shots_instant_death(self, shot);
  } else {
    --self->hitpoints;
  }
}

static gladio_enemy_type const enemy_types[] = {
  {
    { 12, 7, (uint8_t const *) "\x08\x04\xc7\x26\x12\xf9\x38\xbe\x75\x32\x09" },
    { { FIXED_INT_I( 0), FIXED_INT_I(0) }, { FIXED_INT_I(12), FIXED_INT_I(7) } },
    { { FIXED_INT_I( 1), FIXED_INT_I(1) }, { FIXED_INT_I(10), FIXED_INT_I(5) } },
    10,
    16, 48,
    BADGE_DISPLAY_WIDTH,
    tick_move_straight_ahead,
    tick_shoot_not,
    collision_player_simple,
    collision_shots_instant_death
  }, {
    { 11, 9, (uint8_t const *) "\x10\x20\x40\xc0\x81\x83\x4d\xdb\xff\xff\xd9\xa0\x00" },
    { { FIXED_INT_I(0), FIXED_INT_I(0) }, { FIXED_INT_I(11), FIXED_INT_I(9) } },
    { { FIXED_INT_I(1), FIXED_INT_I(1) }, { FIXED_INT_I( 9), FIXED_INT_I(7) } },
    10,
    16, 48,
    BADGE_DISPLAY_WIDTH,
    tick_move_straight_ahead,
    tick_shoot_targeted,
    collision_player_simple,
    collision_shots_instant_death
  }, {
    { 9, 5, (uint8_t const *) "\xc3\x30\xcc\x31\x33\x03" },
    { { FIXED_INT_I(0), FIXED_INT_I(0) }, { FIXED_INT_I(9), FIXED_INT_I(5) } },
    { { FIXED_INT_I(1), FIXED_INT_I(1) }, { FIXED_INT_I(7), FIXED_INT_I(3) } },
    16,
    12, 36,
    BADGE_DISPLAY_WIDTH,
    tick_move_straight_ahead,
    tick_shoot_up,
    collision_player_simple,
    collision_shots_simple
  }, {
    { 9, 5, (uint8_t const *) "\x98\x99\x71\x86\x61\x18" },
    { { FIXED_INT_I(0), FIXED_INT_I(0) }, { FIXED_INT_I(9), FIXED_INT_I(5) } },
    { { FIXED_INT_I(1), FIXED_INT_I(1) }, { FIXED_INT_I(7), FIXED_INT_I(3) } },
    16,
    12, 36,
    BADGE_DISPLAY_WIDTH,
    tick_move_straight_ahead,
    tick_shoot_down,
    collision_player_simple,
    collision_shots_simple
  }, {
    { 8, 9, (uint8_t const *) "\x82\x06\xaf\xce\xd9\xb7\xef\xff\x9c" },
    { { FIXED_INT_I(0), FIXED_INT_I(0) }, { FIXED_INT_I(8), FIXED_INT_I(9) } },
    { { FIXED_INT_I(1), FIXED_INT_I(1) }, { FIXED_INT_I(6), FIXED_INT_I(7) } },
    10,
    16, 48,
    BADGE_DISPLAY_WIDTH,
    tick_move_tumble,
    tick_shoot_forward,
    collision_player_simple,
    collision_shots_simple
  }, {
    { 6, 7, (uint8_t const *) "\xc1\xf1\xfa\x6f\xa3" },
    { { FIXED_INT_I(0), FIXED_INT_I(0) }, { FIXED_INT_I(6), FIXED_INT_I(7) } },
    { { FIXED_INT_I(1), FIXED_INT_I(1) }, { FIXED_INT_I(4), FIXED_INT_I(5) } },
    10,
    32, 6,
    BADGE_DISPLAY_WIDTH,
    tick_move_zigzag_down,
    tick_shoot_zigzag,
    collision_player_simple,
    collision_shots_simple
  }, {
    { 6, 7, (uint8_t const *) "\xc1\xf1\xfa\x6f\xa3" },
    { { FIXED_INT_I(0), FIXED_INT_I(0) }, { FIXED_INT_I(6), FIXED_INT_I(7) } },
    { { FIXED_INT_I(1), FIXED_INT_I(1) }, { FIXED_INT_I(4), FIXED_INT_I(5) } },
    10,
    32, 6,
    BADGE_DISPLAY_WIDTH,
    tick_move_zigzag_up,
    tick_shoot_zigzag,
    collision_player_simple,
    collision_shots_simple
  }
};

gladio_enemy_type const *gladio_enemy_type_by_id(uint8_t id) {
  return &enemy_types[id];
}
