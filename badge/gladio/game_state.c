#include "game_state.h"
#include "gladio.h"

#include <string.h>

gladio_game_state gladio_game_state_new(void) {
  badge_rng rng = badge_rng_init(12345);
  gladio_background bg = gladio_background_new(&rng);

  gladio_game_state state;

  memset(&state, 0, sizeof(state));

  state.player     = gladio_player_new();
  state.flags      = GLADIO_PLAYING;
  state.rng        = rng;
  state.background = bg;

  return state;
}
