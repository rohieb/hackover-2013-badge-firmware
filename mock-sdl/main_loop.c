#include "mock/display.h"
#include "mock/event.h"

#include <ui/display.h>
#include <ui/event.h>

#include <SDL/SDL.h>
#include <emscripten.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void main_loop(void) {
  SDL_Event event;

  while(SDL_PollEvent(&event)) {
    mock_event_update_keystate(&event);
  }
}

int main(void) {
  mock_display_init();

  emscripten_set_main_loop(main_loop, 30, 1);

  return 0;
}
