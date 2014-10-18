#ifndef INCLUDED_BADGE_GLADIO_ENEMY_TYPES_H
#define INCLUDED_BADGE_GLADIO_ENEMY_TYPES_H

#include "enemy.h"

enum {
  GLADIO_ENEMY_ITEM_HEART_FULL,
  GLADIO_ENEMY_ITEM_HEART_HALF,

  GLADIO_ENEMY_ROCKET,
  GLADIO_ENEMY_FIGHTER,
  GLADIO_ENEMY_CANNON_UP,
  GLADIO_ENEMY_CANNON_DOWN,
  GLADIO_ENEMY_TUMBLER,
  GLADIO_ENEMY_WFIGHTER_DOWN,
  GLADIO_ENEMY_WFIGHTER_UP,
  GLADIO_ENEMY_BACKSTABBER,

  GLADIO_ENEMY_FINAL_BOSS_TOP_GUN,
  GLADIO_ENEMY_FINAL_BOSS_BODY,
  GLADIO_ENEMY_FINAL_BOSS_BOTTOM_GUN
};

struct gladio_game_state;
struct gladio_hot;

typedef struct gladio_enemy_type {
  badge_sprite sprite;
  rectangle    hitbox;
  rectangle    collisionbox;

  uint8_t      hitpoints;

  uint8_t      cooldown_initial;
  uint8_t      cooldown_max;

  fixed_point  move_speed;
  fixed_point  shot_speed;

  int8_t       spawnpos_x;

  void (*tick_move )(struct gladio_enemy *self, struct gladio_game_state *);
  void (*tick_shoot)(struct gladio_enemy *self, struct gladio_game_state *);
  void (*collision_player)(struct gladio_enemy *self, struct gladio_game_state *);
  void (*collision_shots )(struct gladio_enemy *self, struct gladio_shot  *);
} gladio_enemy_type;

gladio_enemy_type const *gladio_enemy_type_by_id(uint8_t id);

static inline gladio_enemy_type const *gladio_enemy_type_get(gladio_enemy const *enemy) {
  return gladio_enemy_type_by_id(enemy->type);
}

#endif
