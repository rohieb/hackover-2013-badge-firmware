#include "display.h"

#include <SDL/SDL.h>

enum {
  SCREEN_SCALE = 4,
  WIDTH  = BADGE_DISPLAY_WIDTH  * SCREEN_SCALE,
  HEIGHT = BADGE_DISPLAY_HEIGHT * SCREEN_SCALE
};

static SDL_Surface *screen;

void mock_display_init(void) {
  SDL_Init(SDL_INIT_VIDEO);

  screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_SWSURFACE);
  SDL_FillRect(screen, 0, SDL_MapRGBA(screen->format, 0xff, 0xff, 0xff, 0xff));
  SDL_Flip(screen);
}

void badge_framebuffer_flush(badge_framebuffer const *fb) {
  SDL_Rect rect = { .x = 0, .y = 0, .w = SCREEN_SCALE, .h = SCREEN_SCALE };

  SDL_FillRect(screen, 0, 0xffffffff);

  for(int i = 0; i < BADGE_DISPLAY_WIDTH; ++i) {
    for(int j = 0; j < BADGE_DISPLAY_HEIGHT; ++j) {
      if(badge_framebuffer_pixel(fb, i, j)) {
        rect.x = i * SCREEN_SCALE;
        rect.y = j * SCREEN_SCALE;

        SDL_FillRect(screen, &rect, 0xff000000);
      }
    }
  }

  SDL_Flip(screen);
}
