#ifndef INCLUDED_BADGE_JUMPNRUN_GAME_STATE_H
#define INCLUDED_BADGE_JUMPNRUN_GAME_STATE_H

#include "player.h"
#include "levels.h"
#include "shots.h"
#include "../ui/display.h"

enum {
  JUMPNRUN_MAX_SHOTS = 2,

  JUMPNRUN_STATE_WON = 1
};

typedef struct jumpnrun_game_state {
  jumpnrun_player   player;
  int               screen_left;
  uint8_t           flags;
  jumpnrun_shot     shots[JUMPNRUN_MAX_SHOTS];
} jumpnrun_game_state;

static inline int jumpnrun_screen_left (jumpnrun_game_state const *state) { return state->screen_left; }
static inline int jumpnrun_screen_right(jumpnrun_game_state const *state) { return jumpnrun_screen_left(state) + BADGE_DISPLAY_WIDTH; }

void jumpnrun_game_state_init   (jumpnrun_game_state *state, jumpnrun_level const *lv);
void jumpnrun_game_state_respawn(jumpnrun_game_state *state, jumpnrun_level const *lv);

void jumpnrun_shot_spawn(jumpnrun_shot *shot, jumpnrun_game_state const *state);

#endif
