#include "scroll.h"

#include <ui/event.h>

enum {
  SCROLL_MAX = 25
};

static int scroll_direction = 0;
static int scroll_counter = 0;

static uint8_t map_scroll_direction(void) {
  if(scroll_direction == -1) {
    return BADGE_EVENT_KEY_UP;
  } else if(scroll_direction == 1) {
    return BADGE_EVENT_KEY_DOWN;
  }

  return 0;
}

int scroll_tick(void) {
  uint8_t state = badge_event_current_input_state();

  if(state == map_scroll_direction()) {
    if(scroll_counter == SCROLL_MAX) {
      scroll_counter = 0;
      return scroll_direction;
    } else {
      ++scroll_counter;
    }
  } else if(state == BADGE_EVENT_KEY_UP) {
    scroll_direction = -1;
    scroll_counter = 0;
  } else if(state == BADGE_EVENT_KEY_DOWN) {
    scroll_direction = 1;
    scroll_counter = 0;
  } else {
    scroll_direction = 0;
    scroll_counter = 0;
  }

  return 0;
}

void scroll_reset(void) {
  scroll_direction = scroll_counter = 0;
}
