#include "enemy.h"
#include "game_state.h"
#include "player.h"

static badge_sprite const anim_dummy = { 7, 5, (uint8_t const *) "\xff\xff\xff\xff\xff" };

static void tick_straight_ahead(gladio_enemy *self, gladio_game_state *state) {
  (void) state;

  self->base.position.x = fixed_point_sub(self->base.position.x, FIXED_INT(1));

  if(fixed_point_lt(rectangle_right(&gladio_get_enemy_type(self)->hitbox), FIXED_INT(0))) {
    self->flags = 0;
  }
}

static void collision_player_dummy(gladio_enemy *self, gladio_player *player) {
  (void) self;
  (void) player;
}

static void collision_shots_dummy(gladio_enemy *self, gladio_shot *shot) {
  (void) self;
  (void) shot;
}

static gladio_enemy_type const enemy_types[] = {
  {
    { 100, 1, &anim_dummy },
    { { FIXED_INT_I(1), FIXED_INT_I(1) }, { FIXED_INT_I(5), FIXED_INT_I(3) } },
    100,
    BADGE_DISPLAY_WIDTH,
    tick_straight_ahead,
    collision_player_dummy,
    collision_shots_dummy
  }
};

gladio_enemy_type const *gladio_get_enemy_type_by_id(uint8_t id) {
  return &enemy_types[id];
}

rectangle gladio_enemy_hitbox(gladio_enemy const *enemy) {
  rectangle r = enemy_types[enemy->type].hitbox;
  rectangle_move_rel(&r, enemy->base.position);
  return r;
}

uint8_t gladio_enemy_active(gladio_enemy const *enemy) {
  return enemy->flags & GLADIO_ENEMY_ACTIVE;
}

void gladio_spawn_enemy(struct gladio_game_state *state, uint8_t type_id, int8_t pos_y) {
  for(uint8_t i = 0; i < GLADIO_MAX_ENEMIES; ++i) {
    gladio_enemy *dest = &state->active_enemies[i];

    if(!gladio_enemy_active(dest)) {
      gladio_enemy_type const *type = gladio_get_enemy_type_by_id(type_id);

      dest->base         = gladio_object_new(vec2d_new(FIXED_INT(type->spawnpos_x), FIXED_INT(pos_y)));
      dest->type         = type_id;
      dest->hitpoints    = type->hitpoints;
      dest->flags        = GLADIO_ENEMY_ACTIVE;
      dest->move_counter = 0;
      break;
    }
  }
}
