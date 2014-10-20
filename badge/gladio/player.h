#ifndef INCLUDED_BADGE_GLADIO_PLAYER_H
#define INCLUDED_BADGE_GLADIO_PLAYER_H

#include "object.h"

#include "../ui/display.h"
#include "../util/rectangle.h"

#include <stdint.h>

typedef struct gladio_player {
  gladio_object base;

  uint8_t       health;
  uint8_t       charge;
  uint8_t       weapon;
  uint8_t       cooldown;
} gladio_player;

enum {
  GLADIO_LIVES_LIMIT   =  5,

  GLADIO_PLAYER_HEIGHT =  9,
  GLADIO_PLAYER_WIDTH  = 15,

  GLADIO_PLAYER_COOLDOWN_PERIOD = 15,

  GLADIO_PLAYER_MAX_HEALTH = 30,
  GLADIO_PLAYER_MAX_CHARGE = 240,
  GLADIO_PLAYER_CHARGE_SHIFT = 3,
  GLADIO_PLAYER_CHARGE_UNIT  = 8
};

gladio_player gladio_player_new(void);

void gladio_player_render(badge_framebuffer *fb, gladio_player const *p);
void gladio_player_damage(gladio_player *p, uint8_t damage);
void gladio_player_heal(gladio_player *p, uint8_t damage);

static inline rectangle gladio_player_rectangle(gladio_player const *p) {
  return gladio_object_rectangle(p->base, vec2d_new(FIXED_INT(GLADIO_PLAYER_WIDTH),
                                                    FIXED_INT(GLADIO_PLAYER_HEIGHT)));
}

static inline rectangle gladio_player_hitbox(gladio_player const *p) {
  return rectangle_new(vec2d_new(fixed_point_add(p->base.position.x, FIXED_INT(1)),
                                 fixed_point_add(p->base.position.y, FIXED_INT(2))),
                       vec2d_new(FIXED_INT(GLADIO_PLAYER_WIDTH  - 1),
                                 FIXED_INT(GLADIO_PLAYER_HEIGHT - 2)));
}

#endif
