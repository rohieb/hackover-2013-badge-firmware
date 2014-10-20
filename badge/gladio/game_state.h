#ifndef INCLUDED_BADGE_GLADIO_GAME_STATE_H
#define INCLUDED_BADGE_GLADIO_GAME_STATE_H

#include "background.h"
#include "enemy.h"
#include "shot.h"
#include "player.h"

#include "../util/random.h"

#include <stdint.h>

enum {
  GLADIO_PLAYING     = 0,
  GLADIO_WON         = 1,
  GLADIO_LOST        = 2,
  GLADIO_STATE_ERROR = 3,
  GLADIO_CONTINUATION_MASK = 0x3,

  GLADIO_SCHEDULE_SHOT_FRONT   = 4,
  GLADIO_SCHEDULE_SHOT_SIDEGUN = 8,
  GLADIO_SCHEDULE_SHOT_MASK    = 0xc,

  GLADIO_SCORE_MAX = 999999,
  GLADIO_SCORE_PER_LIFE = 1000,
};

enum {
  GLADIO_MAX_SHOTS_FRIENDLY = 18,
  GLADIO_MAX_SHOTS_HOSTILE  = 30,
  GLADIO_MAX_ENEMIES        = 20
};

struct gladio_level;

typedef struct gladio_game_state_persistent {
  uint32_t score;
  uint32_t next_life;
  uint8_t lives;
} gladio_game_state_persistent;

typedef struct gladio_game_state {
  gladio_game_state_persistent *persistent;

  uint16_t             tick_major;
  uint8_t              tick_minor;
  uint8_t              flags;

  struct gladio_level *level;

  gladio_player        player;

  // generic scratch space for boss patterns. We assume there's only one boss
  // active at a time for this, possibly consisting of multiple parts that need
  // common data for special actions.
  uint32_t             boss_data;

  badge_rng            rng;
  gladio_background    background;

  gladio_shot          shots_hostile [GLADIO_MAX_SHOTS_HOSTILE ];
  gladio_shot          shots_friendly[GLADIO_MAX_SHOTS_FRIENDLY];

  gladio_enemy         active_enemies[GLADIO_MAX_ENEMIES];
} gladio_game_state;

gladio_game_state gladio_game_state_new(gladio_game_state_persistent *persistent);
gladio_game_state_persistent gladio_game_state_persistent_new(void);

void gladio_score_add(gladio_game_state *state, uint32_t score);

#endif
