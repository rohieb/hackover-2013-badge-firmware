#include "game_state.h"
#include "gladio.h"

#include <assert.h>
#include <string.h>

gladio_game_state gladio_game_state_new(gladio_game_state_persistent *persistent) {
  badge_rng rng = badge_rng_init(12345);
  gladio_background bg = gladio_background_new(&rng);

  gladio_game_state state;

  memset(&state, 0, sizeof(state));

  state.player     = gladio_player_new();
  state.flags      = GLADIO_PLAYING;
  state.rng        = rng;
  state.background = bg;

  state.persistent = persistent;

  return state;
}

gladio_game_state_persistent gladio_game_state_persistent_new(void) {
  gladio_game_state_persistent persistent_state;

  persistent_state.score = GLADIO_SCORE_MAX;
  persistent_state.lives = 3;

  return persistent_state;
}

void gladio_score_add(gladio_game_state *state, uint32_t score) {
  assert(state->persistent->score <= GLADIO_SCORE_MAX);

  if(score > GLADIO_SCORE_MAX - state->persistent->score) {
    state->persistent->score = GLADIO_SCORE_MAX;
  } else {
    state->persistent->score += score;
  }
}
