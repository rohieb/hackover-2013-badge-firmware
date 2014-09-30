#include "gladio.h"
#include "shot.h"

#include "../ui/display.h"
#include "../ui/event.h"

static fixed_point const speed_player      = FIXED_INT_I(2);

void gladio_render(gladio_game_state const *state) {
  badge_framebuffer fb = { { { 0 } } };

  gladio_background_render(&fb, &state->background);

  for(uint8_t i = 0; i < GLADIO_MAX_ENEMIES; ++i) {
    gladio_enemy const *e = &state->active_enemies[i];
    if(gladio_enemy_active(e)) {
      gladio_enemy_render(&fb, e);
    }
  }

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

  if(state->tick == 3
     && state->player.charge < GLADIO_PLAYER_MAX_CHARGE
     && (input_state & BADGE_EVENT_KEY_BTN_B) == 0)
  {
    state->player.charge += (input_state & BADGE_EVENT_KEY_BTN_A) ? 1 : 2;
  }

  if(state->player.cooldown == 0) {
    rectangle rp = gladio_player_rectangle(&state->player);
    uint8_t spawnmode = 0;

    if((input_state & BADGE_EVENT_KEY_BTN_A) || (state->flags & GLADIO_SCHEDULE_SHOT_FRONT)) {
      spawnmode |= GLADIO_SHOT_FRIENDLY_FRONT;
      state->player.cooldown = GLADIO_PLAYER_COOLDOWN_PERIOD;
    }

    if(((input_state & BADGE_EVENT_KEY_BTN_B) || (state->flags & GLADIO_SCHEDULE_SHOT_SIDEGUN)) && state->player.charge >= GLADIO_PLAYER_CHARGE_UNIT) {
      spawnmode |= GLADIO_SHOT_FRIENDLY_SIDEGUN;
      state->player.cooldown = GLADIO_PLAYER_COOLDOWN_PERIOD;
      state->player.charge -= GLADIO_PLAYER_CHARGE_UNIT;
    }

    gladio_shot_friendly_spawn(state, spawnmode, vec2d_new(rectangle_right(&rp), rectangle_mid_y(&rp)));
    state->flags &= ~GLADIO_SCHEDULE_SHOT_MASK;
  }
}

void gladio_tick(gladio_game_state *state) {
  ++state->tick;

  if(state->player.cooldown > 0) {
    --state->player.cooldown;
  }

  gladio_handle_input(state);
  gladio_background_tick(&state->background, &state->rng);

  for(uint8_t i = 0; i < GLADIO_MAX_SHOTS_FRIENDLY && gladio_shot_active(&state->shots_friendly[i]); ++i) { gladio_shot_tick(state->shots_friendly + i); }
  for(uint8_t i = 0; i < GLADIO_MAX_SHOTS_HOSTILE  && gladio_shot_active(&state->shots_hostile [i]); ++i) { gladio_shot_tick(state->shots_hostile  + i); }

  for(uint8_t i = 0; i < GLADIO_MAX_ENEMIES; ++i) {
    gladio_enemy *e = &state->active_enemies[i];
    if(gladio_enemy_active(e)) {
      gladio_get_enemy_type(e)->tick(e, state);
    }
  }

  if(!gladio_enemy_active(&state->active_enemies[0])) {
    gladio_enemy_spawn(state, 0, 10);
  }

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
    case BADGE_EVENT_USER_INPUT:
    {
      uint8_t new_buttons = badge_event_new_buttons(ev);

      if(new_buttons & BADGE_EVENT_KEY_BTN_A) { state.flags |= GLADIO_SCHEDULE_SHOT_FRONT  ; }
      if(new_buttons & BADGE_EVENT_KEY_BTN_B) { state.flags |= GLADIO_SCHEDULE_SHOT_SIDEGUN; }
      break;
    }
    case BADGE_EVENT_GAME_TICK:
      gladio_tick(&state);
    }
  } while((state.flags & GLADIO_CONTINUATION_MASK) == GLADIO_PLAYING);
}
