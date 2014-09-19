#include "shot.h"

#include "game_state.h"

#include <string.h>

uint8_t gladio_shot_active(gladio_shot const *shot) {
  return
    fixed_point_eq(FIXED_INT(0), shot->inertia.x) &&
    fixed_point_eq(FIXED_INT(0), shot->inertia.y);
}

void gladio_shot_despawn(gladio_shot *shot) {
  memset(shot, 0, sizeof(gladio_shot));
}

void gladio_shot_spawn(struct gladio_game_state *state, uint8_t shot_type, vec2d position, vec2d movement) {
  gladio_shot *shots     = shot_type == GLADIO_SHOT_FRIENDLY ? state->shots_friendly     : state->shots_hostile;
  uint8_t      shots_max = shot_type == GLADIO_SHOT_FRIENDLY ? GLADIO_MAX_SHOTS_FRIENDLY : GLADIO_MAX_SHOTS_HOSTILE;

  for(uint8_t i = 0; i < shots_max; ++i) {
    if(!gladio_shot_active(shots + i)) {
      shots[i].base.position = position;
      shots[i].base.anim_pos = 0;
      shots[i].type          = 0;
      shots[i].inertia       = movement;
      break;
    }
  }
}
