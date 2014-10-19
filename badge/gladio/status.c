#include "status.h"

#include "../ui/sprite.h"

#include <string.h>


static badge_sprite const heart_full  = { 5, 5, (uint8_t const *) "\xe6\xf9\x67" };
static badge_sprite const heart_empty = { 5, 5, (uint8_t const *) "\x26\xc9\x64" };

static badge_sprite const scorefont[] = {
  { 3, 5, (uint8_t const *) "\x2e\x3a" },
  { 3, 5, (uint8_t const *) "\xf2\x43" },
  { 3, 5, (uint8_t const *) "\xb9\x4a" },
  { 3, 5, (uint8_t const *) "\xb1\x2a" },
  { 3, 5, (uint8_t const *) "\x0f\x71" },
  { 3, 5, (uint8_t const *) "\xb7\x26" },
  { 3, 5, (uint8_t const *) "\xbe\x76" },
  { 3, 5, (uint8_t const *) "\xa1\x0f" },
  { 3, 5, (uint8_t const *) "\xbf\x7e" },
  { 3, 5, (uint8_t const *) "\xb7\x3e" }
};

void gladio_render_score(badge_framebuffer *fb, uint32_t score) {
  uint8_t pos_x = BADGE_DISPLAY_WIDTH;

  do {
    pos_x -= scorefont[0].width + 1;
    badge_framebuffer_blt(fb, pos_x, 1, &scorefont[score % 10], 0);
    score /= 10;
  } while(score != 0);
}


void gladio_status_render(badge_framebuffer       *fb,
                          gladio_game_state const *state) {
  (void) state;

  memset(fb->data[0], 0x80, BADGE_DISPLAY_WIDTH);

/*
  uint8_t hearts;

  for(hearts = 0; hearts < state->player.lives; ++hearts) {
    badge_framebuffer_blt(fb, 1 + (heart_full .width + 1) * hearts, 1, &heart_full, 0);
  }

  for(; hearts < state->player.max_lives; ++hearts) {
    badge_framebuffer_blt(fb, 1 + (heart_empty.width + 1) * hearts, 1, &heart_empty, 0);
  }
*/

  for(uint8_t i = 0; i < state->player.health; ++i) {
    fb->data[0][2 + 2 * i] |= 0x06;
  }

  for(uint8_t i = 0; i < (state->player.charge >> GLADIO_PLAYER_CHARGE_SHIFT); ++i) {
    fb->data[0][2 + 2 * i] |= 0x30;
  }

  gladio_render_score(fb, state->persistent->score);
}
