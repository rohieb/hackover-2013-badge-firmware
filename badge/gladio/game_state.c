#include "game_state.h"
#include "gladio.h"

gladio_game_state gladio_game_state_new(void) {
  badge_rng rng = badge_rng_init(12345);
  gladio_background bg = gladio_background_new(&rng);

  return (gladio_game_state) { 0, 0, GLADIO_PLAYING, rng, bg };
}
