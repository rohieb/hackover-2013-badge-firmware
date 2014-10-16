#include "enemy_types.h"
#include "game_state.h"

#include "../ui/sprite.h"
#include "../util/vec2d.h"

static inline vec2d gladio_enemy_snout_position(gladio_enemy *self) {
  rectangle hitbox = gladio_enemy_hitbox(self);
  return vec2d_new(self->base.position.x, rectangle_mid_y(&hitbox));
}

static void tick_move_straight_ahead(gladio_enemy *self, gladio_game_state *state) {
  (void) state;

  self->base.position.x = fixed_point_sub(self->base.position.x, FIXED_INT(1));

  rectangle r = gladio_enemy_hitbox(self);
  if(fixed_point_lt(rectangle_right(&r), FIXED_INT(0))) {
    gladio_enemy_despawn(self);
  }
}

static void tick_shoot_not(gladio_enemy *self, gladio_game_state *state) {
  (void) self;
  (void) state;
}

static void tick_shoot_targeted(gladio_enemy *self, gladio_game_state *state) {
  if(self->move_counter == 16) {
    vec2d diff = vec2d_sub(state->player.base.position, self->base.position);
    diff = vec2d_div(diff, fixed_point_mul(vec2d_length_approx(diff), FIXED_POINT(1, 500)));

    vec2d snout_pos  = gladio_enemy_snout_position(self);

    gladio_shot_hostile_spawn(state, snout_pos, diff);
  } else if(self->move_counter == 48) {
    self->move_counter = 0;
  }
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


static gladio_enemy_type const enemy_types[] = {
  {
    { 12, 7, (uint8_t const *) "\x08\x04\xc7\x26\x12\xf9\x38\xbe\x75\x32\x09" },
    { { FIXED_INT_I( 0), FIXED_INT_I(0) }, { FIXED_INT_I(12), FIXED_INT_I(7) } },
    { { FIXED_INT_I( 1), FIXED_INT_I(1) }, { FIXED_INT_I(10), FIXED_INT_I(5) } },
    10,
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
    BADGE_DISPLAY_WIDTH,
    tick_move_straight_ahead,
    tick_shoot_targeted,
    collision_player_simple,
    collision_shots_instant_death
  }
};

gladio_enemy_type const *gladio_enemy_type_by_id(uint8_t id) {
  return &enemy_types[id];
}
