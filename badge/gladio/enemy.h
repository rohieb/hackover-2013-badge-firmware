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
  uint16_t      move_counter;
  uint8_t       cooldown;
  uint8_t       flags;
} gladio_enemy;

struct gladio_player;
struct gladio_shot;
struct gladio_game_state;

uint8_t gladio_enemy_active(gladio_enemy const *enemy);
uint8_t gladio_enemy_dying (gladio_enemy const *enemy);

rectangle gladio_enemy_hitbox(gladio_enemy const *enemy);

void gladio_enemy_spawn(struct gladio_game_state *state, uint8_t type, int8_t pos_y);
void gladio_enemy_render(badge_framebuffer *fb, gladio_enemy const *enemy);

void gladio_enemy_tick(struct gladio_game_state *state);

void gladio_enemy_schedule_death(gladio_enemy *enemy);
void gladio_enemy_despawn(gladio_enemy *enemy);

#endif
