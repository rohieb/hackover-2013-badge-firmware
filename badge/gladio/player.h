#ifndef INCLUDED_BADGE_GLADIO_PLAYER_H
#define INCLUDED_BADGE_GLADIO_PLAYER_H

#include "object.h"

#include "../util/rectangle.h"

#include <stdint.h>

typedef struct {
  gladio_object base;

  uint8_t       lives;
  uint8_t       weapon;
} gladio_player;

#endif
