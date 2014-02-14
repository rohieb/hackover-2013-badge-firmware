#include "jumpnrun.h"
#include "display.h"
#include "event.h"
#include "main_loop.h"

#include <jumpnrun/game_state.h>
#include <jumpnrun/jumpnrun.h>
#include <jumpnrun/render.h>
#include <ui/display.h>
#include <ui/event.h>
#include <ui/font.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum {
  JNR_STATE_SCREEN_PLAYING,
  JNR_STATE_SCREEN_LIVES,
  JNR_STATE_SCREEN_LOST,
  JNR_STATE_SCREEN_WON,
};

static jumpnrun_level      mock_jnr_level;
static jumpnrun_game_state mock_jnr_state;
static int                 mock_jnr_loop_state;
static int                 mock_jnr_counter;

enum {
  COUNTER_MAX     = 75,
  COUNTER_BARRIER = 25
};

static void mock_show_lives_screen(jumpnrun_game_state const *state) {
  badge_framebuffer fb = { { { 0 } } };

  int8_t y_upper = BADGE_DISPLAY_HEIGHT / 2 - BADGE_FONT_HEIGHT;
  int8_t y_lower = (BADGE_DISPLAY_HEIGHT + BADGE_FONT_HEIGHT) / 2;
  int8_t x_mid   = (BADGE_DISPLAY_WIDTH  - BADGE_FONT_WIDTH ) / 2;
  int8_t x_left  = x_mid - BADGE_FONT_WIDTH - 5;
  int8_t x_right = x_mid + BADGE_FONT_WIDTH + 5;

  char buf[] = "x";
  if(state->player.keys != 0) {
    y_upper -= BADGE_FONT_HEIGHT;
  }

  jumpnrun_render_player_symbol(&fb, x_left, y_upper);
  badge_framebuffer_render_text  (&fb, x_mid  , y_upper, buf);
  badge_framebuffer_render_number(&fb, x_right, y_upper, state->player.lives);

  if(state->player.keys != 0) {
    jumpnrun_render_key_symbol(&fb, x_left, y_lower);
    badge_framebuffer_render_text  (&fb, x_mid  , y_lower, buf);
    badge_framebuffer_render_number(&fb, x_right, y_lower, state->player.keys);
  }

  badge_framebuffer_flush(&fb);
}

static void mock_show_message(char const *msg) {
  badge_framebuffer fb = { { { 0 } } };

  size_t len = strlen(msg);
  size_t wid = len * BADGE_FONT_WIDTH;
  size_t off = (BADGE_DISPLAY_WIDTH - wid + 1) / 2;

  badge_framebuffer_render_text(&fb, off, BADGE_DISPLAY_HEIGHT / 2 - BADGE_FONT_WIDTH, msg);
  badge_framebuffer_flush(&fb);
}

uint8_t ev_new_buttons(badge_event_t ev) {
  uint8_t old_state = badge_event_old_input_state(ev);
  uint8_t new_state = badge_event_new_input_state(ev);
  return new_state & (old_state ^ new_state);
}

int poll_wait(void) {
  if(mock_jnr_counter == COUNTER_MAX) {
    mock_jnr_counter = 0;
    return 0;
  }

  if(mock_jnr_counter > COUNTER_BARRIER) {
    badge_event_t ev;
    if(mock_event_poll(&ev)) {
      if(ev_new_buttons(ev)) {
        mock_jnr_counter = 0;
        return 0;
      }
    }
  }

  ++mock_jnr_counter;
  return 1;
}

static void mock_jumpnrun_level_free(jumpnrun_level *lv) {
  free(lv->tiles);
  free(lv->items);
  free(lv->enemies);

  memset(lv, 0, sizeof(*lv));
}

static int mock_jumpnrun_start_level_fd(FILE *fd) {
  mock_jumpnrun_level_free(&mock_jnr_level);

  int err = jumpnrun_load_level_header_from_file(&mock_jnr_level, fd);

  if(err != 0) {
    return err;
  }

  mock_jnr_level.tiles   = malloc(sizeof(*mock_jnr_level.tiles  ) * mock_jnr_level.header. tile_count);
  mock_jnr_level.items   = malloc(sizeof(*mock_jnr_level.items  ) * mock_jnr_level.header. item_count);
  mock_jnr_level.enemies = malloc(sizeof(*mock_jnr_level.enemies) * mock_jnr_level.header.enemy_count);

  if(mock_jnr_level.tiles   == NULL ||
     mock_jnr_level.items   == NULL ||
     mock_jnr_level.enemies == NULL)
  {
    mock_jumpnrun_level_free(&mock_jnr_level);
    return JUMPNRUN_ERROR;
  }

  err = jumpnrun_load_level_from_file(&mock_jnr_level, fd);
  if(err != 0) {
    jumpnrun_game_state_init   (&mock_jnr_state, &mock_jnr_level);
    mock_jnr_loop_state = JNR_STATE_SCREEN_LIVES;
  }

  return err;
}

int mock_jumpnrun_start_level(char const *fname) {
  char buf[256];

  snprintf(buf, sizeof(buf), "/hackio/%s", fname);
  buf[sizeof(buf) - 1] = '\0';

  FILE *fd = fopen(buf, "rb");

  if(fd == NULL) {
    puts(buf);
    return JUMPNRUN_ERROR;
  }

  int err = mock_jumpnrun_start_level_fd(fd);
  fclose(fd);
  return err;
}

int mock_jumpnrun_tick(void) {
  switch(mock_jnr_loop_state) {
  case JNR_STATE_SCREEN_PLAYING:
  {
    badge_event_t ev;

    while(mock_event_poll(&ev)) {
      if(badge_event_type(ev) == BADGE_EVENT_USER_INPUT) {
        jumpnrun_handle_input_event(ev, &mock_jnr_state);
      }
    }

    jumpnrun_level_tick(&mock_jnr_level, &mock_jnr_state);
    break;
  }

  case JNR_STATE_SCREEN_LIVES:
    if(mock_jnr_counter == 0) { mock_show_lives_screen(&mock_jnr_state); }
    if(!poll_wait()) {
      jumpnrun_game_state_respawn(&mock_jnr_state, &mock_jnr_level);
      mock_jnr_loop_state = JNR_STATE_SCREEN_PLAYING;
    }
    break;

  case JNR_STATE_SCREEN_LOST:
    if(mock_jnr_counter == 0) { mock_show_message("GAME OVER"); }
    if(!poll_wait()) { return MOCK_JUMPNRUN_GAMEOVER; }
    break;

  case JNR_STATE_SCREEN_WON:
    if(mock_jnr_counter == 0) { mock_show_message("YOU ROCK!!1!"); }
    if(!poll_wait()) { return MOCK_JUMPNRUN_GAMEOVER; }
    break;
  }

  return MOCK_JUMPNRUN_CONTINUE;
}
