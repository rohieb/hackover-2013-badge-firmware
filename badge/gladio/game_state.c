#include "game_state.h"
#include "gladio.h"

gladio_game_state gladio_game_state_new(void) {
  return (gladio_game_state) { 0, 0, GLADIO_PLAYING };
}
