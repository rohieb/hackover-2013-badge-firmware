#include "enemy_types.h"
#include "game_state.h"

#include "../ui/sprite.h"
#include "../util/vec2d.h"

static vec2d gladio_enemy_snout(gladio_enemy *self) {
  return vec2d_add(self->base.position, gladio_enemy_type_get(self)->snout);
}

static void tick_move_straight_ahead(gladio_enemy *self, gladio_game_state *state) {
  (void) state;

  self->base.position.x = fixed_point_sub(self->base.position.x, gladio_enemy_type_get(self)->move_speed);
}

static void tick_move_straight_sw(gladio_enemy *self, gladio_game_state *state) {
  (void) state;

  self->base.position = vec2d_add(self->base.position,
                                  vec2d_mul(vec2d_new(FIXED_INT(-1), FIXED_POINT(0, 600)),
                                            gladio_enemy_type_get(self)->move_speed));
}

static void tick_move_straight_nw(gladio_enemy *self, gladio_game_state *state) {
  (void) state;

  self->base.position = vec2d_add(self->base.position,
                                  vec2d_mul(vec2d_new(FIXED_INT(-1), FIXED_POINT(0, -600)),
                                            gladio_enemy_type_get(self)->move_speed));
}

static void tick_move_tumble(gladio_enemy *self, gladio_game_state *state) {
  (void) state;

  int8_t ppos = self->move_counter % 64;
  int8_t factor = ppos >= 32 ? 1 : -1;

  fixed_point diff_y = fixed_point_div(FIXED_INT(factor), FIXED_INT(2));

  self->base.position.x = fixed_point_sub(self->base.position.x, gladio_enemy_type_get(self)->move_speed);
  self->base.position.y = fixed_point_add(self->base.position.y, diff_y);
}

static void tick_move_zigzag(gladio_enemy *self, int8_t direction) {
  if(self->move_counter < 32) {
    self->base.position.x = fixed_point_sub(self->base.position.x, gladio_enemy_type_get(self)->move_speed);
  } else if(self->move_counter < 64) {
    self->base.position.x = fixed_point_add(self->base.position.x, FIXED_POINT(0, 500));
    self->base.position.y = fixed_point_add(self->base.position.y, FIXED_POINT(0, direction * 750));
  } else {
    self->base.position.x = fixed_point_sub(self->base.position.x, fixed_point_mul(gladio_enemy_type_get(self)->move_speed, FIXED_INT(2)));
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

static void tick_move_backstabber(gladio_enemy *self, gladio_game_state *state) {
  (void) state;
  self->base.position.x = fixed_point_add(self->base.position.x, gladio_enemy_type_get(self)->move_speed);
}

static void tick_move_middleboss(gladio_enemy *self, gladio_game_state *state) {
  (void) state;

  if(self->move_counter < 128) {
    self->base.position.x = fixed_point_sub(self->base.position.x, FIXED_POINT(0, 200));
  } else if(self->move_counter % 128 < 64) {
    self->base.position.y = fixed_point_add(self->base.position.y, FIXED_POINT(0, 100));
  } else {
    self->base.position.y = fixed_point_sub(self->base.position.y, FIXED_POINT(0, 100));
  }
}

static void tick_move_finalboss(gladio_enemy *self, gladio_game_state *state) {
  (void) state;

  if(self->move_counter < 384) {
    self->base.position.x = fixed_point_sub(self->base.position.x, FIXED_POINT(0, 200));
  } else {
    self->base.position.x = fixed_point_add(self->base.position.x, FIXED_POINT(0, 200));
    if(self->move_counter >= 640) {
      self->move_counter -= 512;
    }
  }

  if(self->move_counter % 128 < 64) {
    self->base.position.y = fixed_point_add(self->base.position.y, FIXED_POINT(0, 200));
  } else {
    self->base.position.y = fixed_point_sub(self->base.position.y, FIXED_POINT(0, 200));
  }
}

static void tick_shoot_not(gladio_enemy *self, gladio_game_state *state) {
  (void) self;
  (void) state;
}

static void tick_shoot_forward(gladio_enemy *self, gladio_game_state *state) {
  vec2d snout_pos = gladio_enemy_snout(self);
  gladio_shot_hostile_spawn(state, snout_pos, vec2d_new(fixed_point_neg(gladio_enemy_type_get(self)->shot_speed), FIXED_INT(0)));
}

static void tick_shoot_rockets(gladio_enemy *self, gladio_game_state *state) {
  vec2d snout_pos = gladio_enemy_snout(self);
  vec2d offset = gladio_enemy_type_by_id(GLADIO_ENEMY_ROCKET)->hitbox.extent;

  offset.y = fixed_point_div(offset.y, FIXED_INT(2));

  vec2d spawnpos = vec2d_sub(snout_pos, offset);

  gladio_enemy_spawn_at(state,
                        GLADIO_ENEMY_ROCKET,
                        fixed_point_cast_int(spawnpos.x),
                        fixed_point_cast_int(spawnpos.y));
}

static void tick_shoot_zigzag(gladio_enemy *self, gladio_game_state *state) {
  if(self->move_counter < 64) {
    vec2d snout_pos = gladio_enemy_snout(self);
    gladio_shot_hostile_spawn(state, snout_pos, vec2d_new(fixed_point_neg(gladio_enemy_type_get(self)->shot_speed), FIXED_INT(0)));
  }
}

static void tick_shoot_up(gladio_enemy *self, gladio_game_state *state) {
  vec2d snout_pos = gladio_enemy_snout(self);
  gladio_shot_hostile_spawn(state, snout_pos, vec2d_new(FIXED_INT(0), fixed_point_neg(gladio_enemy_type_get(self)->shot_speed)));
}

static void tick_shoot_down(gladio_enemy *self, gladio_game_state *state) {
  vec2d snout_pos = gladio_enemy_snout(self);
  gladio_shot_hostile_spawn(state, snout_pos, vec2d_new(FIXED_INT(0), gladio_enemy_type_get(self)->shot_speed));
}

static void tick_shoot_targeted(gladio_enemy *self, gladio_game_state *state) {
  vec2d diff = vec2d_sub(state->player.base.position, self->base.position);
  diff = vec2d_div(diff, fixed_point_div(vec2d_length_approx(diff), gladio_enemy_type_get(self)->shot_speed));

  vec2d snout_pos  = gladio_enemy_snout(self);

  gladio_shot_hostile_spawn(state, snout_pos, diff);
  self->cooldown = 48;
}

static void tick_shoot_targeted_with_pause(gladio_enemy *self, gladio_game_state *state) {
  if((self->move_counter & 0xff) >= 96) {
    tick_shoot_targeted(self, state);
  }
}

static void tick_shoot_finalboss_topgun(gladio_enemy *self, gladio_game_state *state) {
  vec2d pos = gladio_enemy_snout(self);
  vec2d direction = vec2d_new(FIXED_POINT(0, -700), FIXED_POINT(0, 700));
  gladio_shot_hostile_spawn(state, pos, direction);
}

static void tick_shoot_finalboss_bottomgun(gladio_enemy *self, gladio_game_state *state) {
  vec2d pos = gladio_enemy_snout(self);
  vec2d direction = vec2d_new(FIXED_POINT(0, -700), FIXED_POINT(0, -700));
  gladio_shot_hostile_spawn(state, pos, direction);
}

static void collision_player_simple(gladio_enemy *self, gladio_game_state *state) {
  --self->hitpoints;
  gladio_player_damage(state, 2);

  if(self->hitpoints == 0) {
    gladio_enemy_schedule_death(self);
  }
}

static void collision_player_heart(gladio_enemy *self, gladio_game_state *state) {
  gladio_player_heal(&state->player, self->hitpoints);
  gladio_enemy_schedule_death(self);
}

static void collision_shots_ignore(gladio_enemy *self, gladio_shot *shot) {
  (void) self;
  (void) shot;
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

  gladio_shot_despawn_later(shot);
}

static gladio_enemy_type const enemy_types[] = {
  {
    // heart_half
    { 5, 5, (uint8_t const *) "\xe6\xf9\x67" },
    { { FIXED_INT_I( 0), FIXED_INT_I(0) }, { FIXED_INT_I(5), FIXED_INT_I(5) } },
    { { FIXED_INT_I( 0), FIXED_INT_I(0) }, { FIXED_INT_I(5), FIXED_INT_I(5) } },
    { FIXED_INT_I(0), FIXED_INT_I(0) },
    0,
    6,
    255, 255,
    FIXED_POINT_I(0, 250),
    FIXED_INT_I(0),
    BADGE_DISPLAY_WIDTH,
    tick_move_straight_ahead,
    tick_shoot_not,
    collision_player_heart,
    collision_shots_ignore
  }, {
    // heart_full
    { 5, 5, (uint8_t const *) "\x26\xc9\x64" },
    { { FIXED_INT_I( 0), FIXED_INT_I(0) }, { FIXED_INT_I(5), FIXED_INT_I(5) } },
    { { FIXED_INT_I( 0), FIXED_INT_I(0) }, { FIXED_INT_I(5), FIXED_INT_I(5) } },
    { FIXED_INT_I(0), FIXED_INT_I(0) },
    0,
    3,
    255, 255,
    FIXED_POINT_I(0, 250),
    FIXED_INT_I(0),
    BADGE_DISPLAY_WIDTH,
    tick_move_straight_ahead,
    tick_shoot_not,
    collision_player_heart,
    collision_shots_ignore
  }, {
    // rocket
    { 12, 7, (uint8_t const *) "\x08\x04\xc7\x26\x12\xf9\x38\xbe\x75\x32\x09" },
    { { FIXED_INT_I( 0), FIXED_INT_I(0) }, { FIXED_INT_I(12), FIXED_INT_I(7) } },
    { { FIXED_INT_I( 1), FIXED_INT_I(1) }, { FIXED_INT_I(10), FIXED_INT_I(5) } },
    { FIXED_INT_I(0), FIXED_POINT_I(3, 500) },
    5,
    2,
    255, 255,
    FIXED_POINT_I(0, 500),
    FIXED_INT_I(0),
    BADGE_DISPLAY_WIDTH,
    tick_move_straight_ahead,
    tick_shoot_not,
    collision_player_simple,
    collision_shots_instant_death
  }, {
    // rocket_back
    { 12, 7, (uint8_t const *) "\xc9\xe4\xda\xc7\xf1\x89\x44\x36\x0e\x02\x01" },
    { { FIXED_INT_I(0), FIXED_INT_I(0) }, { FIXED_INT_I(12), FIXED_INT_I(7) } },
    { { FIXED_INT_I(1), FIXED_INT_I(1) }, { FIXED_INT_I(10), FIXED_INT_I(5) } },
    { FIXED_INT_I(12), FIXED_POINT_I(3, 500) },
    10,
    2,
    255, 255,
    FIXED_POINT_I(1, 0),
    FIXED_POINT_I(0, 750),
    -12,
    tick_move_backstabber,
    tick_shoot_not,
    collision_player_simple,
    collision_shots_instant_death
  }, {
    // kamikaze_sw
    { 11, 9, (uint8_t const *) "\x10\x20\xe0\xc0\xc1\xa6\xed\xff\xbe\x38\xa8\x18\x03" },
    { { FIXED_INT_I( 0), FIXED_INT_I(0) }, { FIXED_INT_I(11), FIXED_INT_I(9) } },
    { { FIXED_INT_I( 1), FIXED_INT_I(1) }, { FIXED_INT_I( 9), FIXED_INT_I(7) } },
    { FIXED_INT_I(0), FIXED_POINT_I(4, 500) },
    10,
    3,
    255, 255,
    FIXED_POINT_I(0, 500),
    FIXED_INT_I(1),
    BADGE_DISPLAY_WIDTH,
    tick_move_straight_sw,
    tick_shoot_not,
    collision_player_simple,
    collision_shots_simple
  }, {
    // kamikaze_nw
    { 11, 9, (uint8_t const *) "\x10\x20\xe0\xc0\xc1\xa6\xed\xff\xbe\x38\xa8\x18\x03" },
    { { FIXED_INT_I( 0), FIXED_INT_I(0) }, { FIXED_INT_I(11), FIXED_INT_I(9) } },
    { { FIXED_INT_I( 1), FIXED_INT_I(1) }, { FIXED_INT_I( 9), FIXED_INT_I(7) } },
    { FIXED_INT_I(0), FIXED_POINT_I(4, 500) },
    10,
    3,
    255, 255,
    FIXED_POINT_I(0, 500),
    FIXED_INT_I(1),
    BADGE_DISPLAY_WIDTH,
    tick_move_straight_nw,
    tick_shoot_not,
    collision_player_simple,
    collision_shots_simple
  }, {
    // fighter
    { 11, 9, (uint8_t const *) "\x10\x20\x40\xc0\x81\x83\x4d\xdb\xff\xff\xd9\xa0\x00" },
    { { FIXED_INT_I(0), FIXED_INT_I(0) }, { FIXED_INT_I(11), FIXED_INT_I(9) } },
    { { FIXED_INT_I(1), FIXED_INT_I(1) }, { FIXED_INT_I( 9), FIXED_INT_I(7) } },
    { FIXED_INT_I(0), FIXED_POINT_I(4, 500) },
    20,
    3,
    16, 48,
    FIXED_POINT_I(0, 500),
    FIXED_POINT_I(0, 666),
    BADGE_DISPLAY_WIDTH,
    tick_move_straight_ahead,
    tick_shoot_targeted,
    collision_player_simple,
    collision_shots_simple
  }, {
    // cannon_up
    { 9, 5, (uint8_t const *) "\xc3\x30\xcc\x31\x33\x03" },
    { { FIXED_INT_I(0), FIXED_INT_I(0) }, { FIXED_INT_I(9), FIXED_INT_I(5) } },
    { { FIXED_INT_I(1), FIXED_INT_I(1) }, { FIXED_INT_I(7), FIXED_INT_I(3) } },
    { FIXED_INT_I(0), FIXED_INT_I(0) },
    50,
    16,
    16, 36,
    FIXED_POINT_I(0, 500),
    FIXED_INT_I(1),
    BADGE_DISPLAY_WIDTH,
    tick_move_straight_ahead,
    tick_shoot_up,
    collision_player_simple,
    collision_shots_simple
  }, {
    // cannon_down
    { 9, 5, (uint8_t const *) "\x98\x99\x71\x86\x61\x18" },
    { { FIXED_INT_I(0), FIXED_INT_I(0) }, { FIXED_INT_I(9), FIXED_INT_I(5) } },
    { { FIXED_INT_I(1), FIXED_INT_I(1) }, { FIXED_INT_I(7), FIXED_INT_I(3) } },
    { FIXED_INT_I(0), FIXED_INT_I(5) },
    50,
    16,
    16, 36,
    FIXED_POINT_I(0, 500),
    FIXED_INT_I(1),
    BADGE_DISPLAY_WIDTH,
    tick_move_straight_ahead,
    tick_shoot_down,
    collision_player_simple,
    collision_shots_simple
  }, {
    // tumbler
    { 8, 9, (uint8_t const *) "\x82\x06\xaf\xce\xd9\xb7\xef\xff\x9c" },
    { { FIXED_INT_I(0), FIXED_INT_I(0) }, { FIXED_INT_I(8), FIXED_INT_I(9) } },
    { { FIXED_INT_I(1), FIXED_INT_I(1) }, { FIXED_INT_I(6), FIXED_INT_I(7) } },
    { FIXED_INT_I(0), FIXED_POINT_I(4, 500) },
    15,
    3,
    16, 48,
    FIXED_POINT_I(0, 500),
    FIXED_POINT_I(0, 750),
    BADGE_DISPLAY_WIDTH,
    tick_move_tumble,
    tick_shoot_forward,
    collision_player_simple,
    collision_shots_simple
  }, {
    // wfighter_down
    { 6, 7, (uint8_t const *) "\xc1\xf1\xfa\x6f\xa3" },
    { { FIXED_INT_I(0), FIXED_INT_I(0) }, { FIXED_INT_I(6), FIXED_INT_I(7) } },
    { { FIXED_INT_I(1), FIXED_INT_I(1) }, { FIXED_INT_I(4), FIXED_INT_I(5) } },
    { FIXED_INT_I(0), FIXED_POINT_I(3, 500) },
    30,
    2,
    32, 6,
    FIXED_POINT_I(0, 500),
    FIXED_POINT_I(0, 750),
    BADGE_DISPLAY_WIDTH,
    tick_move_zigzag_down,
    tick_shoot_zigzag,
    collision_player_simple,
    collision_shots_simple
  }, {
    // wfighter_up
    { 6, 7, (uint8_t const *) "\xc1\xf1\xfa\x6f\xa3" },
    { { FIXED_INT_I(0), FIXED_INT_I(0) }, { FIXED_INT_I(6), FIXED_INT_I(7) } },
    { { FIXED_INT_I(1), FIXED_INT_I(1) }, { FIXED_INT_I(4), FIXED_INT_I(5) } },
    { FIXED_INT_I(0), FIXED_POINT_I(3, 500) },
    30,
    2,
    32, 6,
    FIXED_POINT_I(0, 500),
    FIXED_POINT_I(0, 750),
    BADGE_DISPLAY_WIDTH,
    tick_move_zigzag_up,
    tick_shoot_zigzag,
    collision_player_simple,
    collision_shots_simple
  }, {
    // backstabber
    { 12, 10, (uint8_t const *) "\x01\x26\x99\x67\xb3\xcf\x37\xdb\x2f\x2d\x30\xc0\x00\x03\x0c" },
    { { FIXED_INT_I(0), FIXED_INT_I(0) }, { FIXED_INT_I(12), FIXED_INT_I(10) } },
    { { FIXED_INT_I(1), FIXED_INT_I(1) }, { FIXED_INT_I(10), FIXED_INT_I( 8) } },
    { FIXED_INT_I(0), FIXED_INT_I(5) },
    15,
    2,
    32, 32,
    FIXED_POINT_I(0, 500),
    FIXED_POINT_I(0, 750),
    -12,
    tick_move_backstabber,
    tick_shoot_not,
    collision_player_simple,
    collision_shots_simple
  }, {
    // finalboss_topgun
    { 22, 25, (uint8_t const *) "\x80\x00\x00\x00\x01\x00\x00\x02\x00\x00\x0c\x00\x00\x38\x00\x00\xf0\x00\x00\xc1\x07\x00\x02\x7f\x00\x06\xce\x00\x0c\x0c\x01\x1c\x08\x06\x18\x10\x0f\x70\x20\xff\xc0\x43\xe3\x01\x8f\xd6\x3e\xb8\x8d\xf1\xe0\xf3\x81\x81\xcf\x81\x03\x3c\x80\x03\xe0\xf1\x03\x80\xf7\x03\x00\x7c" },
    { { FIXED_INT_I(0), FIXED_INT_I(0) }, { FIXED_INT_I(22), FIXED_INT_I(25) } },
    { { FIXED_INT_I(7), FIXED_INT_I(1) }, { FIXED_INT_I(12), FIXED_INT_I(18) } },
    { FIXED_INT_I(3), FIXED_INT_I(4) },
    500,
    32,
    32, 32,
    FIXED_POINT_I(0, 200),
    FIXED_POINT_I(0, 750),
    BADGE_DISPLAY_WIDTH + 8,
    tick_move_finalboss,
    tick_shoot_finalboss_topgun,
    collision_player_simple,
    collision_shots_simple
  }, {
    // finalboss_body
    { 25, 21, (uint8_t const *) "\x00\x04\x00\xc0\x01\x00\x6c\x00\xc0\x18\x00\xfe\x0f\xe0\xff\x03\x1e\xf0\xe0\x01\x3c\x9e\x3f\xcf\xf9\xcf\x9d\x83\xf3\x33\x60\x7e\xe6\xcc\xcf\x94\xf9\x99\x33\x3f\x03\xe6\xe7\xe0\xdc\xf9\xcf\x79\xfe\x3c\x1e\xc0\x83\x07\x3c\xe0\xff\x03\xf8\x3f\x00\xfc\x01\x00\x0e" },
    { { FIXED_INT_I(0), FIXED_INT_I(0) }, { FIXED_INT_I(25), FIXED_INT_I(21) } },
    { { FIXED_INT_I(3), FIXED_INT_I(3) }, { FIXED_INT_I(19), FIXED_INT_I(15) } },
    { FIXED_INT_I(0), FIXED_POINT_I(10, 500) },
    1000,
    32,
    32, 96,
    FIXED_POINT_I(0, 200),
    FIXED_POINT_I(0, 750),
    BADGE_DISPLAY_WIDTH,
    tick_move_finalboss,
    tick_shoot_rockets,
    collision_player_simple,
    collision_shots_simple
  }, {
    // finalboss_bottomgun
    { 22, 24, (uint8_t const *) "\x00\x00\x01\x00\x00\x01\x00\x00\x01\x00\x80\x01\x00\xc0\x01\x00\xe0\x01\x00\xf8\x20\x00\x7e\x20\x80\x73\x60\x80\x61\x60\x80\x41\xe0\xc0\x47\xc0\xf8\x4f\xe0\xf8\x58\x78\xd8\x5a\x3c\xc7\xd8\x0e\x81\xcf\x07\x03\xe7\x03\x07\xf0\x00\x7e\x3c\x00\xfc\x1e\x00\xc0\x07" },
    { { FIXED_INT_I(0), FIXED_INT_I(0) }, { FIXED_INT_I(22), FIXED_INT_I(24) } },
    { { FIXED_INT_I(8), FIXED_INT_I(4) }, { FIXED_INT_I(12), FIXED_INT_I(17) } },
    { FIXED_INT_I(3), FIXED_INT_I(20) },
    500,
    32,
    16, 32,
    FIXED_POINT_I(0, 200),
    FIXED_POINT_I(0, 750),
    BADGE_DISPLAY_WIDTH + 8,
    tick_move_finalboss,
    tick_shoot_finalboss_bottomgun,
    collision_player_simple,
    collision_shots_simple
  }, {
    // midboss_topgun
    { 27, 20, (uint8_t const *) "\x00\x40\x00\x00\x0e\x00\x70\x00\x80\x03\x00\x1c\x00\xe0\x00\x00\x07\x00\x38\x00\xc0\x01\x20\x0e\x00\x67\x00\x70\x06\x00\x67\x00\x60\x07\x00\x3c\x00\x80\x03\x00\x70\x00\x00\x0e\x00\xc0\x01\x00\x38\x00\x00\x07\x00\xe0\x00\x00\x1c\x00\x80\x03\x00\xf0\x03\x00\x7e\x00\x80\x0f" },
    { { FIXED_INT_I(5), FIXED_INT_I(4) }, { FIXED_INT_I(22), FIXED_INT_I(16) } },
    { { FIXED_INT_I(5), FIXED_INT_I(4) }, { FIXED_INT_I(22), FIXED_INT_I(16) } },
    { FIXED_INT_I(0), FIXED_INT_I(15) },
    500,
    32,
    32, 32,
    FIXED_POINT_I(0, 200),
    FIXED_POINT_I(0, 750),
    BADGE_DISPLAY_WIDTH,
    tick_move_middleboss,
    tick_shoot_targeted_with_pause,
    collision_player_simple,
    collision_shots_simple
  }, {
    // midboss_body
    { 24, 21, (uint8_t const *) "\x00\x0a\x00\x78\x0f\xc0\x83\x07\x9c\xcf\xc1\xfc\x67\xf8\xff\x8f\xff\xff\xf3\xff\x7f\xfe\xff\xef\xff\xff\xff\xff\xff\xff\xff\xef\xff\xff\xfc\xff\x9f\xff\xff\xe3\xff\x3f\xfc\xff\x07\xff\x7f\xc0\xff\x07\xe0\x3f\x00\xe0\x00\xc0\xff\x01\xfc\x7e\xc0\x8f\x1f" },
    { { FIXED_INT_I(1), FIXED_INT_I(1) }, { FIXED_INT_I(23), FIXED_INT_I(19) } },
    { { FIXED_INT_I(2), FIXED_INT_I(2) }, { FIXED_INT_I(21), FIXED_INT_I(17) } },
    { FIXED_INT_I(0), FIXED_POINT_I(10, 500) },
    1000,
    32,
    32, 32,
    FIXED_POINT_I(0, 200),
    FIXED_POINT_I(0, 750),
    BADGE_DISPLAY_WIDTH + 3,
    tick_move_middleboss,
    tick_shoot_targeted_with_pause,
    collision_player_simple,
    collision_shots_simple
  }, {
    // midboss_bottomgun
    { 26, 20, (uint8_t const *) "\x40\x00\x00\x0c\x00\xc0\x01\x00\x38\x00\x00\x07\x00\xe0\x00\x00\x1c\x00\x80\x03\x00\x70\x04\x00\xe6\x00\x60\x0e\x00\xe6\x00\xe0\x06\x00\x3c\x00\xc0\x01\x00\x0e\x00\x70\x00\x80\x03\x00\x1c\x00\xe0\x00\x00\x07\x00\x38\x00\xc0\x01\xc0\x0f\x00\x7e\x00\xf0\x01\x00" },
    { { FIXED_INT_I(5), FIXED_INT_I(5) }, { FIXED_INT_I(21), FIXED_INT_I(15) } },
    { { FIXED_INT_I(5), FIXED_INT_I(4) }, { FIXED_INT_I(21), FIXED_INT_I(15) } },
    { FIXED_INT_I(0), FIXED_INT_I(5) },
    500,
    32,
    32, 32,
    FIXED_POINT_I(0, 200),
    FIXED_POINT_I(0, 750),
    BADGE_DISPLAY_WIDTH,
    tick_move_middleboss,
    tick_shoot_targeted_with_pause,
    collision_player_simple,
    collision_shots_simple
  }
};

gladio_enemy_type const *gladio_enemy_type_by_id(uint8_t id) {
  return &enemy_types[id];
}
