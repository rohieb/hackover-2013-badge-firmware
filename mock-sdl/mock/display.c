#include "display.h"

#include <SDL/SDL.h>

enum {
  SCREEN_SCALE = 1,
  WIDTH  = BADGE_DISPLAY_WIDTH  * SCREEN_SCALE,
  HEIGHT = BADGE_DISPLAY_HEIGHT * SCREEN_SCALE
};

static SDL_Surface *screen;

void mock_display_init(void) {
  SDL_Init(SDL_INIT_VIDEO);

  screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_SWSURFACE);
  SDL_FillRect(screen, 0, 0xff);
  SDL_Flip(screen);
}

void badge_framebuffer_flush(badge_framebuffer const *fb) {
  SDL_FillRect(screen, 0, 0xffffffff);
  Uint32 (*pixels)[BADGE_DISPLAY_WIDTH] = (Uint32 (*)[BADGE_DISPLAY_WIDTH]) screen->pixels;

  SDL_LockSurface(screen);

  for(int j = 0; j < BADGE_DISPLAY_HEIGHT; ++j) {
    for(int i = 0; i < BADGE_DISPLAY_WIDTH; ++i) {
      if(badge_framebuffer_pixel(fb, i, j)) {
        pixels[j][i] = 0xff000000;
      }
    }
  }

  SDL_UnlockSurface(screen);
  SDL_Flip(screen);
}
