#include "stats.h"
#include "jumpnrun.h"
#include "items.h"
#include "../ui/display.h"
#include "../ui/font.h"

void jumpnrun_show_lives_screen(jumpnrun_game_state const *state) {
  badge_framebuffer fb = { { { 0 } } };

  badge_sprite const *hacker =  jumpnrun_hacker_symbol();
  badge_sprite const *key    = &jumpnrun_item_type_data[JUMPNRUN_ITEM_TYPE_KEY].sprite;

  int8_t y_upper = BADGE_DISPLAY_HEIGHT / 2 - BADGE_FONT_HEIGHT;
  int8_t y_lower = (BADGE_DISPLAY_HEIGHT + BADGE_FONT_HEIGHT) / 2;
  int8_t x_left  = BADGE_DISPLAY_WIDTH / 2 - hacker->width - BADGE_FONT_WIDTH - 5;
  int8_t x_mid   = (BADGE_DISPLAY_WIDTH - BADGE_FONT_WIDTH) / 2;
  int8_t x_right = x_mid + BADGE_FONT_WIDTH + 5;

  char buf[] = "x";
  if(state->keys != 0) {
    y_upper -= BADGE_FONT_HEIGHT;
  }

  badge_framebuffer_blt          (&fb, x_left , y_upper, hacker, 0);
  badge_framebuffer_render_text  (&fb, x_mid  , y_upper, buf);
  badge_framebuffer_render_number(&fb, x_right, y_upper, state->lives);

  if(state->keys != 0) {
    badge_framebuffer_blt          (&fb, x_left , y_lower, key, 0);
    badge_framebuffer_render_text  (&fb, x_mid  , y_lower, buf);
    badge_framebuffer_render_number(&fb, x_right, y_lower, state->keys);
  }

  badge_framebuffer_flush(&fb);
}
