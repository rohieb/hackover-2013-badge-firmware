#include "gladio.h"

#include "../ui/display.h"
#include "../ui/event.h"

void gladio_render(gladio_game_state const *state,
                   gladio_player     const *player) {
  badge_framebuffer fb = { { { 0 } } };

  gladio_player_render(&fb, player);
  gladio_status_render(&fb, player, state);

  badge_framebuffer_flush(&fb);
}

void gladio_handle_input(gladio_player *player) {
  uint8_t input_state = badge_event_current_input_state();

  if(input_state & BADGE_EVENT_KEY_UP   ) { player->base.position.y = fixed_point_max(FIXED_INT(GLADIO_STATUS_BAR_HEIGHT                    + 1), fixed_point_sub(player->base.position.y, FIXED_POINT(1, 200))); }
  if(input_state & BADGE_EVENT_KEY_DOWN ) { player->base.position.y = fixed_point_min(FIXED_INT(BADGE_DISPLAY_HEIGHT - GLADIO_PLAYER_HEIGHT - 1), fixed_point_add(player->base.position.y, FIXED_POINT(1, 200))); }
  if(input_state & BADGE_EVENT_KEY_LEFT ) { player->base.position.x = fixed_point_max(FIXED_INT(                                              1), fixed_point_sub(player->base.position.x, FIXED_POINT(1, 200))); }
  if(input_state & BADGE_EVENT_KEY_RIGHT) { player->base.position.x = fixed_point_min(FIXED_INT(BADGE_DISPLAY_WIDTH  - GLADIO_PLAYER_WIDTH  - 1), fixed_point_add(player->base.position.x, FIXED_POINT(1, 200))); }
}

void gladio_tick(gladio_game_state *state,
                 gladio_player     *player) {
  ++state->tick;

  gladio_handle_input(player);

  if(state->tick == 3) {
    state->tick = 0;
    gladio_render(state, player);
  }
}

void gladio_play(void) {
  gladio_player     player = gladio_player_new();
  gladio_game_state state  = gladio_game_state_new();

  do {
    badge_event_t ev = badge_event_wait();

    switch(badge_event_type(ev)) {
    case BADGE_EVENT_GAME_TICK:
      gladio_tick(&state, &player);
    }
  } while(state.flags == GLADIO_PLAYING);
}
