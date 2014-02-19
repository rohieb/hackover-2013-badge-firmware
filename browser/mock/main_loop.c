#include "browser.h"
#include "display.h"
#include "event.h"
#include "jumpnrun.h"
#include "menu.h"
#include "scroll.h"

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
  STATE_CREDITS
};

static unsigned state   = STATE_MENU;

void main_loop(void) {
  SDL_Event event;

  while(SDL_PollEvent(&event)) {
    mock_event_update_keystate(&event);
  }

  switch(state) {
  case STATE_MENU:
  {
    uint8_t action = mock_menu_tick();

    if(action == MOCK_MENU_SELECTED) {
      if(mock_jumpnrun_start_level(mock_menu_selection()) != 0) {
        return;
      }
      scroll_reset();
      state = STATE_JUMPNRUN;
    } else if(action == MOCK_MENU_CREDITS) {
      mock_browse_open_file("/hackio/credits.txt");
      state = STATE_CREDITS;
    }
    break;
  }

  case STATE_CREDITS:
  {
    if(mock_browse_tick() == BROWSER_EXIT) {
      state = STATE_MENU;
    }
    break;
  }

  case STATE_JUMPNRUN:
    if(mock_jumpnrun_tick() != MOCK_JUMPNRUN_CONTINUE) {
      state = STATE_MENU;
      return;
    }

    break;
  }
}
