#ifndef INCLUDED_BADGE_GLADIO_GAME_STATE_H
#define INCLUDED_BADGE_GLADIO_GAME_STATE_H

#include "background.h"
#include "../util/random.h"

#include <stdint.h>

enum {
  GLADIO_PLAYING,
  GLADIO_WON,
  GLADIO_LOST,
  GLADIO_ERROR,
};

typedef struct {
  uint32_t score;
  uint8_t  tick;
  uint8_t  flags;

  badge_rng         rng;
  gladio_background background;
} gladio_game_state;

gladio_game_state gladio_game_state_new(void);

#endif
