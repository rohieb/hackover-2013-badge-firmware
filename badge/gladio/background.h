#ifndef INCLUDED_BADGE_GLADIO_BACKGROUND_H
#define INCLUDED_BADGE_GLADIO_BACKGROUND_H

#include "../ui/display.h"
#include "../util/fixed_point.h"
#include "../util/random.h"

enum {
  GLADIO_BACKGROUND_STARS = 5
};

typedef struct gladio_background_star {
  fixed_point x;
  int8_t      y;
  fixed_point dx;
} gladio_background_star;

typedef struct gladio_background {
  gladio_background_star stars[GLADIO_BACKGROUND_STARS];
} gladio_background;

struct gladio_game_state;

gladio_background gladio_background_new   (badge_rng *rng);
void              gladio_background_tick  (struct gladio_game_state *state);
void              gladio_background_render(badge_framebuffer *fb, gladio_background const *bg);

#endif
