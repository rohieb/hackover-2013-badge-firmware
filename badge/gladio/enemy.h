#ifndef INCLUDED_BADGE_GLADIO_ENEMY_H
#define INCLUDED_BADGE_GLADIO_ENEMY_H

#include "animation.h"
#include "object.h"
#include "../ui/sprite.h"

typedef struct gladio_enemy {
  gladio_object base;

  uint8_t       hitpoints;
  uint8_t       flags;
  uint8_t       move_counter;
  uint8_t       type;
} gladio_enemy;

struct gladio_player;
struct gladio_shot;

typedef struct gladio_enemy_type {
  gladio_animation animation;

  rectangle hitbox;
  uint8_t   hitpoints;

  void (*collision_player)(struct gladio_enemy *self, struct gladio_player *);
  void (*collision_shots )(struct gladio_enemy *self, struct gladio_shot   *);
} gladio_enemy_type;

gladio_enemy_type const *gladio_get_enemy_type_by_id(uint8_t id);

static inline gladio_enemy_type const *gladio_get_enemy_type(gladio_enemy const *enemy) {
  return gladio_get_enemy_type_by_id(enemy->type);
}

#endif
