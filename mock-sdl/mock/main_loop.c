#include "mock/display.h"
#include "mock/event.h"

#include <jumpnrun/jumpnrun.h>
#include <ui/display.h>
#include <ui/event.h>

#include <SDL/SDL.h>
#include <emscripten.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
  STATE_MENU,
  STATE_LIVES_SCREEN,
  STATE_JUMPNRUN,
  STATE_GAMEOVER,
  STATE_WON
};

enum {
  COUNTER_MAX     = 75,
  COUNTER_BARRIER = 25
};

static unsigned state   = STATE_MENU;
static unsigned counter = 0;

int poll_wait(void) {
  if(counter == COUNTER_MAX) {
    counter = 0;
    return 0;
  }

  ++counter;
  return 1;
}

void main_loop(void) {
  SDL_Event event;

  while(SDL_PollEvent(&event)) {
    mock_event_update_keystate(&event);
  }

  switch(state) {
  case STATE_MENU:
  case STATE_LIVES_SCREEN:
  case STATE_JUMPNRUN:
  case STATE_GAMEOVER:
  case STATE_WON:
    break;
  }
}
