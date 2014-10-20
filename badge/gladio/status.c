#include "status.h"

#include "../ui/sprite.h"

#include <string.h>

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

void gladio_render_number(badge_framebuffer *fb, uint8_t pos_x_right, uint32_t num) {
  uint8_t pos_x = pos_x_right;

  do {
    pos_x -= scorefont[0].width + 1;
    badge_framebuffer_blt(fb, pos_x, 1, &scorefont[num % 10], 0);
    num /= 10;
  } while(num != 0);
}

void gladio_render_score(badge_framebuffer *fb, uint32_t score) {
  gladio_render_number(fb, BADGE_DISPLAY_WIDTH, score);
}

void gladio_render_lives(badge_framebuffer *fb, uint8_t lives) {
  if(lives > 99) {
    lives = 99;
  }

  gladio_render_number(fb, 9, lives);
}

void gladio_status_render(badge_framebuffer       *fb,
                          gladio_game_state const *state) {
  (void) state;

  memset(fb->data[0], 0x80, BADGE_DISPLAY_WIDTH);

  gladio_render_lives(fb, state->persistent->lives);

  for(uint8_t i = 0; i < state->player.health; ++i) {
    fb->data[0][10 + 2 * i] |= 0x06;
  }

  for(uint8_t i = 0; i < (state->player.charge >> GLADIO_PLAYER_CHARGE_SHIFT); ++i) {
    fb->data[0][10 + 2 * i] |= 0x30;
  }

  gladio_render_score(fb, state->persistent->score);
}
