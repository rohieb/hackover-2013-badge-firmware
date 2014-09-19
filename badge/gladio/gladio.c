#include "gladio.h"
#include "shot.h"

#include "../ui/display.h"
#include "../ui/event.h"

static fixed_point const speed_player      = FIXED_INT_I(2);
static fixed_point const speed_player_shot = FIXED_INT_I(1);

void gladio_render(gladio_game_state const *state) {
  badge_framebuffer fb = { { { 0 } } };

  gladio_background_render(&fb, &state->background);

  gladio_player_render    (&fb, &state->player);

  for(uint8_t i = 0; i < GLADIO_MAX_SHOTS_FRIENDLY; ++i) { gladio_shot_render(&fb, state->shots_friendly + i); }
  for(uint8_t i = 0; i < GLADIO_MAX_SHOTS_HOSTILE ; ++i) { gladio_shot_render(&fb, state->shots_hostile  + i); }

  gladio_status_render    (&fb, state);

  badge_framebuffer_flush(&fb);
}

void gladio_handle_input(gladio_game_state *state) {
  uint8_t input_state = badge_event_current_input_state();

  if(input_state & BADGE_EVENT_KEY_UP   ) { state->player.base.position.y = fixed_point_max(FIXED_INT(GLADIO_STATUS_BAR_HEIGHT                    + 1), fixed_point_sub(state->player.base.position.y, speed_player)); }
  if(input_state & BADGE_EVENT_KEY_DOWN ) { state->player.base.position.y = fixed_point_min(FIXED_INT(BADGE_DISPLAY_HEIGHT - GLADIO_PLAYER_HEIGHT - 1), fixed_point_add(state->player.base.position.y, speed_player)); }
  if(input_state & BADGE_EVENT_KEY_LEFT ) { state->player.base.position.x = fixed_point_max(FIXED_INT(                                              1), fixed_point_sub(state->player.base.position.x, speed_player)); }
  if(input_state & BADGE_EVENT_KEY_RIGHT) { state->player.base.position.x = fixed_point_min(FIXED_INT(BADGE_DISPLAY_WIDTH  - GLADIO_PLAYER_WIDTH  - 1), fixed_point_add(state->player.base.position.x, speed_player)); }

  if((input_state & BADGE_EVENT_KEY_BTN_A) && state->player.cooldown == 0) {
    rectangle rp = gladio_player_rectangle(&state->player);
    gladio_shot_spawn(state,
                      GLADIO_SHOT_FRIENDLY,
                      vec2d_new(rectangle_right(&rp), rectangle_mid_y(&rp)),
                      vec2d_new(speed_player_shot, FIXED_INT(0)));
    state->player.cooldown = GLADIO_PLAYER_COOLDOWN_PERIOD;
  }
}

void gladio_tick(gladio_game_state *state) {
  ++state->tick;

  if(state->player.cooldown > 0) {
    --state->player.cooldown;
  }

  gladio_handle_input(state);
  gladio_background_tick(&state->background, &state->rng);

  for(uint8_t i = 0; i < GLADIO_MAX_SHOTS_FRIENDLY; ++i) { gladio_shot_tick(state->shots_friendly + i); }
  for(uint8_t i = 0; i < GLADIO_MAX_SHOTS_HOSTILE ; ++i) { gladio_shot_tick(state->shots_hostile  + i); }

  if(state->tick == 3) {
    state->tick = 0;
    gladio_render(state);
  }
}

void gladio_play(void) {
  gladio_game_state state = gladio_game_state_new();

  do {
    badge_event_t ev = badge_event_wait();

    switch(badge_event_type(ev)) {
    case BADGE_EVENT_GAME_TICK:
      gladio_tick(&state);
    }
  } while(state.flags == GLADIO_PLAYING);
}
