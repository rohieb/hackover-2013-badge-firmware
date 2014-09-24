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

  GLADIO_PLAYER_HEIGHT =  7,
  GLADIO_PLAYER_WIDTH  = 13,

  GLADIO_PLAYER_COOLDOWN_PERIOD = 15,

  GLADIO_PLAYER_MAX_HEALTH = 24,
  GLADIO_PLAYER_MAX_CHARGE = 192,
  GLADIO_PLAYER_CHARGE_SHIFT = 3,
  GLADIO_PLAYER_CHARGE_UNIT  = 8
};

gladio_player gladio_player_new(void);

void gladio_player_render(badge_framebuffer *fb, gladio_player const *p);

static inline rectangle gladio_player_rectangle(gladio_player const *p) {
  return gladio_object_rectangle(p->base, vec2d_new(FIXED_INT(GLADIO_PLAYER_WIDTH),
                                                    FIXED_INT(GLADIO_PLAYER_HEIGHT)));
}

#endif
