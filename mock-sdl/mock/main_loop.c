#include "mock/display.h"
#include "mock/event.h"
#include "mock/jumpnrun.h"
#include "mock/menu.h"

#include <jumpnrun/game_state.h>
#include <jumpnrun/jumpnrun.h>
#include <ui/display.h>
#include <ui/event.h>
#include <ui/menu.h>

#include <SDL/SDL.h>
#include <emscripten.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

enum {
  STATE_MENU,
  STATE_JUMPNRUN,
};

static unsigned state   = STATE_MENU;

void main_loop(void) {
  SDL_Event event;

  while(SDL_PollEvent(&event)) {
    mock_event_update_keystate(&event);
  }

  switch(state) {
  case STATE_MENU:
    if(mock_menu_tick() == MOCK_MENU_SELECTED) {
      if(mock_jumpnrun_start_level(mock_menu_selection()) != 0) {
        return;
      }
      state = STATE_JUMPNRUN;
    }
    break;

  case STATE_JUMPNRUN:
    if(mock_jumpnrun_tick() != MOCK_JUMPNRUN_CONTINUE) {
      state = STATE_MENU;
      return;
    }

    break;
  }
}
