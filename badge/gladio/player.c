#include "player.h"

#include "status.h"
#include "../ui/display.h"
#include "../ui/sprite.h"

badge_sprite const gladio_player_sprite = { GLADIO_PLAYER_WIDTH, GLADIO_PLAYER_HEIGHT, (uint8_t const *) "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" };

gladio_player gladio_player_new(void) {
  return (gladio_player) { { { FIXED_INT(10), FIXED_INT(BADGE_DISPLAY_HEIGHT - GLADIO_STATUS_BAR_HEIGHT) }, 0 }, 3, 3, 0, 0 };
}

void gladio_player_render(badge_framebuffer *fb, gladio_player const *p) {
  badge_framebuffer_blt(fb,
                        fixed_point_cast_int(p->base.position.x),
                        fixed_point_cast_int(p->base.position.y),
                        &gladio_player_sprite,
                        0);
}
