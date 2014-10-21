#include "background.h"
#include "game_state.h"
#include "status.h"

static gladio_background_star gladio_background_star_new(badge_rng *rng, uint8_t i) {
  gladio_background_star s;

  uint8_t soff   = GLADIO_STATUS_BAR_HEIGHT + 3;
  uint8_t height = BADGE_DISPLAY_HEIGHT     - soff;
  uint8_t minh   = soff +  i      * height / GLADIO_BACKGROUND_STARS + 1;
  uint8_t maxh   = soff + (i + 1) * height / GLADIO_BACKGROUND_STARS - 1;

  s.y  = minh + badge_rng_u8(rng) % (maxh - minh);
  s.x  =                 fixed_point_mod(badge_rng_fixed_point(rng), FIXED_INT(BADGE_DISPLAY_WIDTH))               ;
  s.dx = fixed_point_add(fixed_point_mod(badge_rng_fixed_point(rng), FIXED_INT(2                  )), FIXED_INT(1));

  return s;
}

gladio_background gladio_background_new(badge_rng *rng) {
  gladio_background bg;

  for(uint8_t i = 0; i < GLADIO_BACKGROUND_STARS; ++i) {
    bg.stars[i] = gladio_background_star_new(rng, i);
  }

  return bg;
}

void gladio_background_tick(gladio_game_state *state) {
  gladio_background *bg  = &state->persistent->background;
  badge_rng         *rng = &state->persistent->rng;

  for(uint8_t i = 0; i < GLADIO_BACKGROUND_STARS; ++i) {
    bg->stars[i].x = fixed_point_sub(bg->stars[i].x, bg->stars[i].dx);

    if(fixed_point_lt(bg->stars[i].x, FIXED_INT(0))) {
      bg->stars[i] = gladio_background_star_new(rng, i);
      bg->stars[i].x = FIXED_INT(BADGE_DISPLAY_WIDTH);
    }
  }
}

void gladio_background_render(badge_framebuffer *fb, gladio_background const *bg) {
  for(uint8_t i = 0; i < GLADIO_BACKGROUND_STARS; ++i) {
    badge_framebuffer_pixel_on(fb,
                               fixed_point_cast_int(bg->stars[i].x),
                               bg->stars[i].y);
  }
}
