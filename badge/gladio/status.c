#include "status.h"

#include "../ui/sprite.h"

#include <string.h>


static badge_sprite const heart_full  = { 5, 5, (uint8_t const *) "\xe6\xf9\x67" };
static badge_sprite const heart_empty = { 5, 5, (uint8_t const *) "\x26\xc9\x64" };

#if 0
  static glyph_t const font_digits[] PROGMEM = {
    GLYPH_C(011101000101110), /* 0 */
    GLYPH_C(000011111110001), /* 1 */
    GLYPH_C(010011010110011), /* 2 */
    GLYPH_C(010101010110001), /* 3 */
    GLYPH_C(001110001011110), /* 4 */
    GLYPH_C(100101010111101), /* 5 */
    GLYPH_C(101111010101111), /* 6 */
    GLYPH_C(110001011110000), /* 7 */
    GLYPH_C(111111010111111), /* 8 */
    GLYPH_C(111101010111101)  /* 9 */
  };
#endif

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


void gladio_render_status_bar(badge_framebuffer       *fb,
                              gladio_player     const *player,
                              gladio_game_state const *state) {
  (void) state;

  memset(fb->data[0], 0x80, BADGE_DISPLAY_WIDTH);

  uint8_t hearts;

  for(hearts = 0; hearts < player->lives; ++hearts) {
    badge_framebuffer_blt(fb, 1 + (heart_full .width + 1) * hearts, 1, &heart_full, 0);
  }

  for(; hearts < 5; ++hearts) {
    badge_framebuffer_blt(fb, 1 + (heart_empty.width + 1) * hearts, 1, &heart_empty, 0);
  }

  gladio_render_score(fb, state->score);
}
