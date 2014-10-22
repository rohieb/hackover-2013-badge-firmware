#ifndef INCLUDED_BADGE_GLADIO_GLADIO_H
#define INCLUDED_BADGE_GLADIO_GLADIO_H

#include "enemy.h"
#include "game_state.h"
#include "player.h"
#include "shot.h"
#include "status.h"

typedef struct gladio_level_number {
  uint8_t world;
  uint8_t level;
} gladio_level_number;

void gladio_play(void);
uint8_t gladio_play_level(char const *fname, gladio_game_state_persistent *persistent_state, gladio_level_number const *lvnum);

#endif
