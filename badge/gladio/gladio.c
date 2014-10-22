#include "gladio.h"
#include "level.h"
#include "shot.h"
#include "enemy_types.h"

#include "../ui/display.h"
#include "../ui/event.h"
#include "../ui/font.h"

static fixed_point const speed_player_y = FIXED_POINT_I(1, 500);
static fixed_point const speed_player_x = FIXED_POINT_I(1, 500);

void gladio_render(gladio_game_state const *state) {
  badge_framebuffer fb = { { { 0 } } };

  gladio_background_render(&fb, &state->persistent->background);

  for(uint8_t i = 0; i < GLADIO_MAX_ENEMIES; ++i) {
    gladio_enemy const *e = &state->active_enemies[i];
    if(gladio_enemy_active(e)) {
      gladio_enemy_render(&fb, e);
    }
  }

  gladio_player_render    (&fb, &state->player);

  for(uint8_t i = 0; i < GLADIO_MAX_SHOTS_FRIENDLY; ++i) { gladio_shot_render(&fb, state->shots_friendly + i); }
  for(uint8_t i = 0; i < GLADIO_MAX_SHOTS_HOSTILE ; ++i) { gladio_shot_render(&fb, state->shots_hostile  + i); }

  gladio_status_render   (&fb, state);

  badge_framebuffer_flush(&fb);
}

void gladio_handle_input(gladio_game_state *state) {
  uint8_t input_state = badge_event_current_input_state();

  if(gladio_player_moveable(&state->player)) {
    if(input_state & BADGE_EVENT_KEY_UP   ) { state->player.base.position.y = fixed_point_max(FIXED_INT(GLADIO_STATUS_BAR_HEIGHT                    + 1), fixed_point_sub(state->player.base.position.y, speed_player_y)); }
    if(input_state & BADGE_EVENT_KEY_DOWN ) { state->player.base.position.y = fixed_point_min(FIXED_INT(BADGE_DISPLAY_HEIGHT - GLADIO_PLAYER_HEIGHT - 1), fixed_point_add(state->player.base.position.y, speed_player_y)); }
    if(input_state & BADGE_EVENT_KEY_LEFT ) { state->player.base.position.x = fixed_point_max(FIXED_INT(                                              1), fixed_point_sub(state->player.base.position.x, speed_player_x)); }
    if(input_state & BADGE_EVENT_KEY_RIGHT) { state->player.base.position.x = fixed_point_min(FIXED_INT(BADGE_DISPLAY_WIDTH  - GLADIO_PLAYER_WIDTH  - 1), fixed_point_add(state->player.base.position.x, speed_player_x)); }
  }

  if(state->tick_minor == 3
     && state->player.charge < GLADIO_PLAYER_MAX_CHARGE
     && (input_state & BADGE_EVENT_KEY_BTN_B) == 0)
  {
    state->player.charge += (input_state & BADGE_EVENT_KEY_BTN_A) ? 1 : 2;
  }

  if(state->player.cooldown == 0) {
    rectangle rp = gladio_player_rectangle(&state->player);
    uint8_t spawnmode = 0;

    if(gladio_player_moveable(&state->player)) {
      if((input_state & BADGE_EVENT_KEY_BTN_A) || (state->flags & GLADIO_SCHEDULE_SHOT_FRONT)) {
        spawnmode |= GLADIO_SHOT_FRIENDLY_FRONT;
        state->player.cooldown = GLADIO_PLAYER_COOLDOWN_PERIOD;
      }

      if(((input_state & BADGE_EVENT_KEY_BTN_B) || (state->flags & GLADIO_SCHEDULE_SHOT_SIDEGUN)) && state->player.charge >= GLADIO_PLAYER_CHARGE_UNIT) {
        spawnmode |= GLADIO_SHOT_FRIENDLY_SIDEGUN;
        state->player.cooldown = GLADIO_PLAYER_COOLDOWN_PERIOD;
        state->player.charge -= GLADIO_PLAYER_CHARGE_UNIT;
      }
    }

    gladio_shot_friendly_spawn(state, spawnmode, vec2d_new(rectangle_right(&rp), rectangle_mid_y(&rp)));
    state->flags &= ~GLADIO_SCHEDULE_SHOT_MASK;
  }
}

void gladio_tick(gladio_game_state *state) {
  ++state->tick_minor;

  if(state->player.cooldown > 0) {
    --state->player.cooldown;
  }

  gladio_handle_input(state);
  gladio_background_tick(state);

  gladio_shot_friendly_move(state);

  uint8_t enemies_active = 0;

  enemies_active |= gladio_shot_hostile_tick(state);
  enemies_active |= gladio_enemy_tick(state);

  if(!enemies_active &&
     state->level->pos == state->level->len &&
     state->player.status != GLADIO_PLAYER_WINNING)
  {
    state->player.status = GLADIO_PLAYER_WINNING;
    state->player.status_cooldown = 48;
  }

  if(state->tick_minor == 3) {
    gladio_player_status_tick(state);

    state->tick_minor = 0;

    while(state->level->pos < state->level->len &&
          state->level->specs[state->level->pos].time <= state->tick_major) {
      gladio_enemy_spawn(state,
                         state->level->specs[state->level->pos].type,
                         state->level->specs[state->level->pos].pos_y);
      ++state->level->pos;
    }
    ++state->tick_major;

    gladio_render(state);
  }
}

void gladio_game_over(gladio_game_state *state) {
  uint16_t i = 750;

  do {
    badge_event_t ev;

    ev = badge_event_wait();

    switch(badge_event_type(ev)) {
    case BADGE_EVENT_USER_INPUT:
      if(i < 700 && badge_event_new_buttons(ev)) {
        i = 0;
      }
      break;
    case BADGE_EVENT_GAME_TICK:
      {
        badge_framebuffer fb = { { { 0 } } };

        gladio_background_tick(state);

        gladio_background_render(&fb, &state->persistent->background);
        gladio_status_render(&fb, state);

        badge_framebuffer_render_text(&fb, 16, 31, "GAME OVER");

        badge_framebuffer_flush(&fb);
        --i;
      }
    }
  } while(i != 0);
}

uint8_t gladio_play_level(char const *fname, gladio_game_state_persistent *persistent_state) {
  // Nur zum Testen. Dateifoo kommt später.
  gladio_level lv;

  GLADIO_LEVEL_LOAD(lv, fname);

  gladio_game_state state = gladio_game_state_new(persistent_state);
  state.level = &lv;
  state.tick_major = 0;

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
  } while(state.player.status != GLADIO_PLAYER_LOST && !gladio_player_won(&state.player));

  if(state.player.status == GLADIO_PLAYER_LOST) {
    gladio_game_over(&state);
  }

  return state.player.status;
}

void gladio_play(void) {
  gladio_game_state_persistent persistent_state = gladio_game_state_persistent_new();
  uint8_t status;

  do {
    status = gladio_play_level("test.lvl", &persistent_state);
  } while(status != GLADIO_PLAYER_LOST);
}
