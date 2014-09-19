#ifndef INCLUDED_BADGE_GLADIO_PLAYER_H
#define INCLUDED_BADGE_GLADIO_PLAYER_H

#include "object.h"

#include "../ui/display.h"
#include "../util/rectangle.h"

#include <stdint.h>

typedef struct gladio_player {
  gladio_object base;

  uint8_t       lives;
  uint8_t       max_lives;
  uint8_t       weapon;
  uint8_t       cooldown;
} gladio_player;

enum {
  GLADIO_LIVES_LIMIT   =  5,

  GLADIO_PLAYER_HEIGHT =  7,
  GLADIO_PLAYER_WIDTH  = 13,

  GLADIO_PLAYER_COOLDOWN_PERIOD = 15
};

gladio_player gladio_player_new(void);

void gladio_player_render(badge_framebuffer *fb, gladio_player const *p);

#endif
