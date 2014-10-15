#include "player.h"

#include "status.h"
#include "../ui/display.h"
#include "../ui/sprite.h"

static badge_sprite const gladio_player_sprite = { GLADIO_PLAYER_WIDTH, GLADIO_PLAYER_HEIGHT, (uint8_t const *) "\x29\xdf\xff\xaf\x5a\xb5\x6f\xdf\xb6\x6d\x51\xe0\xc0\x01\x01\x02\x04" };

gladio_player gladio_player_new(void) {
  return (gladio_player) { { { FIXED_INT(10), FIXED_INT(BADGE_DISPLAY_HEIGHT - GLADIO_STATUS_BAR_HEIGHT) }, 0 }, GLADIO_PLAYER_MAX_HEALTH, 0, 0, 0 };
}

void gladio_player_render(badge_framebuffer *fb, gladio_player const *p) {
  badge_framebuffer_blt(fb,
                        fixed_point_cast_int(p->base.position.x),
                        fixed_point_cast_int(p->base.position.y),
                        &gladio_player_sprite,
                        0);
}

void gladio_player_damage(gladio_player *p, uint8_t damage) {
  if(p->health > damage) {
    p->health -= damage;
  } else {
    p->health = 0;
  }
}
