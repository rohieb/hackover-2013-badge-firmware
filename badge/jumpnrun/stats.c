#include "stats.h"

#include "items.h"
#include "jumpnrun.h"
#include "render.h"

#include "../ui/display.h"
#include "../ui/event.h"
#include "../ui/font.h"

void jumpnrun_show_lives_screen(jumpnrun_game_state const *state) {
  badge_framebuffer fb = { { { 0 } } };

  int8_t y_upper = BADGE_DISPLAY_HEIGHT / 2 - BADGE_FONT_HEIGHT;
  int8_t y_lower = (BADGE_DISPLAY_HEIGHT + BADGE_FONT_HEIGHT) / 2;
  int8_t x_left  = BADGE_DISPLAY_WIDTH / 2 - fixed_point_cast_int(jumpnrun_player_extents().y) - BADGE_FONT_WIDTH - 5;
  int8_t x_mid   = (BADGE_DISPLAY_WIDTH - BADGE_FONT_WIDTH) / 2;
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

  for(uint8_t i = 0; i < 75; ) {
    badge_event_t ev = badge_event_wait();
    if(badge_event_type(ev) == BADGE_EVENT_GAME_TICK) {
      ++i;
    } else if(i > 25) {
      uint8_t old_state = badge_event_old_input_state(ev);
      uint8_t new_state = badge_event_new_input_state(ev);
      uint8_t new_buttons = new_state & (old_state ^ new_state);

      if(new_buttons != 0) break;
    }
  }
}
