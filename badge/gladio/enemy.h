#ifndef INCLUDED_BADGE_GLADIO_ENEMY_H
#define INCLUDED_BADGE_GLADIO_ENEMY_H

#include "object.h"
#include "../ui/sprite.h"

enum {
  GLADIO_ENEMY_ACTIVE = 1,
  GLADIO_ENEMY_DYING  = 2
};

typedef struct gladio_enemy {
  gladio_object base;

  uint8_t       type;
  uint8_t       hitpoints;
  uint8_t       flags;
  uint16_t      move_counter;
} gladio_enemy;

struct gladio_player;
struct gladio_shot;
struct gladio_game_state;

typedef struct gladio_enemy_type {
  badge_sprite sprite;
  rectangle    hitbox;
  uint8_t      hitpoints;
  int8_t       spawnpos_x;

  void (*tick_move )(struct gladio_enemy *self, struct gladio_game_state *);
  void (*tick_shoot)(struct gladio_enemy *self, struct gladio_game_state *);
  void (*collision_player)(struct gladio_enemy *self, struct gladio_game_state *);
  void (*collision_shots )(struct gladio_enemy *self, struct gladio_shot  *);
} gladio_enemy_type;

uint8_t gladio_enemy_active(gladio_enemy const *enemy);
uint8_t gladio_enemy_dying (gladio_enemy const *enemy);

gladio_enemy_type const *gladio_get_enemy_type_by_id(uint8_t id);

static inline gladio_enemy_type const *gladio_get_enemy_type(gladio_enemy const *enemy) {
  return gladio_get_enemy_type_by_id(enemy->type);
}

rectangle gladio_enemy_hitbox(gladio_enemy const *enemy);

void gladio_enemy_spawn(struct gladio_game_state *state, uint8_t type, int8_t pos_y);
void gladio_enemy_render(badge_framebuffer *fb, gladio_enemy const *enemy);

void gladio_enemy_tick(struct gladio_game_state *state);

void gladio_enemy_schedule_death(gladio_enemy *enemy);
void gladio_enemy_despawn(gladio_enemy *enemy);

#endif
