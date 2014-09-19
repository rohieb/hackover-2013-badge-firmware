#ifndef INCLUDED_BADGE_GLADIO_GAME_STATE_H
#define INCLUDED_BADGE_GLADIO_GAME_STATE_H

#include "background.h"
#include "shot.h"
#include "player.h"

#include "../util/random.h"

#include <stdint.h>

enum {
  GLADIO_PLAYING,
  GLADIO_WON,
  GLADIO_LOST,
  GLADIO_ERROR,
};

enum {
  GLADIO_MAX_SHOTS_FRIENDLY = 15,
  GLADIO_MAX_SHOTS_HOSTILE  = 30
};

typedef struct gladio_game_state {
  uint32_t          score;
  uint8_t           tick;
  uint8_t           flags;

  badge_rng         rng;
  gladio_background background;

  gladio_player     player;

  gladio_shot       shots_friendly[GLADIO_MAX_SHOTS_FRIENDLY];
  gladio_shot       shots_hostile [GLADIO_MAX_SHOTS_HOSTILE ];
} gladio_game_state;

gladio_game_state gladio_game_state_new(void);

#endif
