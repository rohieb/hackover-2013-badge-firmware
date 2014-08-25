#include "status.h"

#include "../ui/sprite.h"

#include <string.h>


static badge_sprite const heart_full  = { 5, 5, (uint8_t const *) "\xe6\xf9\x67" };
static badge_sprite const heart_empty = { 5, 5, (uint8_t const *) "\x26\xc9\x64" };

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
}
