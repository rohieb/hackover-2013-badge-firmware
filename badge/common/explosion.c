#include "explosion.h"

#include "../ui/sprite.h"

static badge_sprite const anim_explosion[EXPLOSION_FRAMES] = {
  { EXPLOSION_WIDTH, EXPLOSION_HEIGHT, (uint8_t const *) "\x00\x28\xa0\x00" },
  { EXPLOSION_WIDTH, EXPLOSION_HEIGHT, (uint8_t const *) "\x51\x29\xa0\x54\x04" },
  { EXPLOSION_WIDTH, EXPLOSION_HEIGHT, (uint8_t const *) "\x51\x01\x00\x54\x04" },
  { EXPLOSION_WIDTH, EXPLOSION_HEIGHT, (uint8_t const *) "\x11\x00\x00\x40\x04" }
};

void common_render_explosion(badge_framebuffer *fb,
                             int8_t x,
                             int8_t y,
                             uint8_t tick) {
  badge_sprite const *sprite = &anim_explosion[tick / EXPLOSION_TICKS_PER_FRAME];

  badge_framebuffer_blt(fb,
                        x, y,
                        sprite, 0);
}
