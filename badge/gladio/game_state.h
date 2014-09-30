#ifndef INCLUDED_BADGE_GLADIO_GAME_STATE_H
#define INCLUDED_BADGE_GLADIO_GAME_STATE_H

#include "background.h"
#include "enemy.h"
#include "shot.h"
#include "player.h"

#include "../util/random.h"

#include <stdint.h>

enum {
  GLADIO_PLAYING = 0,
  GLADIO_WON     = 1,
  GLADIO_LOST    = 2,
  GLADIO_ERROR   = 3,
  GLADIO_CONTINUATION_MASK = 0x3,

  GLADIO_SCHEDULE_SHOT_FRONT   = 4,
  GLADIO_SCHEDULE_SHOT_SIDEGUN = 8,
  GLADIO_SCHEDULE_SHOT_MASK    = 0xc
};

enum {
  GLADIO_MAX_SHOTS_FRIENDLY = 18,
  GLADIO_MAX_SHOTS_HOSTILE  = 30,
  GLADIO_MAX_ENEMIES        = 20
};

struct gladio_level;

typedef struct gladio_game_state {
  uint32_t          score;
  uint8_t           tick;
  uint8_t           flags;

  uint16_t          level_pos;
/*
  gladio_level     *level;
*/

  badge_rng         rng;
  gladio_background background;

  gladio_player     player;

  gladio_shot       shots_hostile [GLADIO_MAX_SHOTS_HOSTILE ];
  gladio_shot       shots_friendly[GLADIO_MAX_SHOTS_FRIENDLY];

  gladio_enemy      active_enemies[GLADIO_MAX_ENEMIES];
} gladio_game_state;

gladio_game_state gladio_game_state_new(void);

#endif
