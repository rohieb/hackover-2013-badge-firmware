#include "game_state.h"

void jumpnrun_game_state_init(jumpnrun_game_state *state, jumpnrun_level const *lv) {
  memset(state, 0, sizeof(*state));
  jumpnrun_player_spawn(&state->player, lv->start_pos, 99);
}

void jumpnrun_game_state_respawn(jumpnrun_game_state *state, jumpnrun_level const *lv) {
  jumpnrun_player_respawn(&state->player, lv->start_pos);
  state->flags = 0;
  state->screen_left = 0;
  memset(state->shots, 0, sizeof(state->shots));

  for(size_t i = 0; i < lv->header.enemy_count; ++i) {
    jumpnrun_enemy_reset(&lv->enemies[i]);
  }
}

void jumpnrun_shot_spawn(jumpnrun_shot *shot, jumpnrun_game_state const *state) {
  static vec2d const shot_spawn_inertia = { FIXED_POINT_I(0, 800), FIXED_POINT_I(0, -800) };

  shot->tick        = 0;
  shot->inertia     = shot_spawn_inertia;

  if(jumpnrun_moveable_mirrored(&state->player.base)) {
    shot->current_box = rectangle_new((vec2d) { fixed_point_sub(rectangle_left(&state->player.base.hitbox), FIXED_INT(JUMPNRUN_SHOT_EXTENT)), rectangle_top(&state->player.base.hitbox) },
                                      (vec2d) { FIXED_INT(JUMPNRUN_SHOT_EXTENT), FIXED_INT(JUMPNRUN_SHOT_EXTENT) });
    shot->inertia.x   = fixed_point_neg(shot->inertia.x);
  } else {
    shot->current_box = rectangle_new((vec2d) { rectangle_right(&state->player.base.hitbox), rectangle_top(&state->player.base.hitbox) },
                                      (vec2d) { FIXED_INT(JUMPNRUN_SHOT_EXTENT), FIXED_INT(JUMPNRUN_SHOT_EXTENT) });
  }

  shot->old_box = shot->current_box;
  shot->inertia = vec2d_add(shot->inertia, state->player.base.inertia);
}
