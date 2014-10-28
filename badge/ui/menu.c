#include "menu.h"

#include "event.h"
#include "font.h"
#include "sprite.h"

#include <stdbool.h>

#include "../common/sprites.h"

enum {
  MENU_SCROLL_TICKS = 25,
  MENU_SCROLL_TICKS_TURBO = 10
};

enum {
  MENU_MARGIN_TOP      = 3,
  MENU_MARGIN_BOTTOM   = 2,
  MENU_MARGIN_LEFT     = 3,
  MENU_ENTRIES_HEIGHT  = 1 + BADGE_FONT_HEIGHT,
  MENU_ENTRIES_VISIBLE = (BADGE_DISPLAY_HEIGHT - MENU_MARGIN_TOP - MENU_MARGIN_BOTTOM) / MENU_ENTRIES_HEIGHT
};

void badge_menu_show(char const *const *menu,
                     uint8_t n,
                     uint8_t *first_visible,
                     uint8_t selected,
                     char   selector)
{
  badge_framebuffer fb = { { { 0 } } };
  bool arrow_up   = true;
  bool arrow_down = true;

  uint8_t first_used_row = 0;
  uint8_t used_rows = MENU_ENTRIES_VISIBLE;

  if(selected >= n) {
    selected = n - 1;
  }

  if(n <= MENU_ENTRIES_VISIBLE) {
    *first_visible = 0;
    used_rows      = n;
    first_used_row = (MENU_ENTRIES_VISIBLE - used_rows) / 2;
  } else if(*first_visible + MENU_ENTRIES_VISIBLE > n) {
    *first_visible = n - MENU_ENTRIES_VISIBLE;
  } else if(selected + 1 == n) {
    *first_visible = n - MENU_ENTRIES_VISIBLE;
  } else if(selected <= *first_visible) {
    *first_visible = selected == 0 ? 0 : selected - 1;
  } else if(selected - *first_visible + 2 > MENU_ENTRIES_VISIBLE) {
    *first_visible = selected - MENU_ENTRIES_VISIBLE + 2;
  }

  if(*first_visible == 0) {
    arrow_up = false;
  }

  if(*first_visible + MENU_ENTRIES_VISIBLE >= n) {
    arrow_down = false;
  }

  for(uint8_t i = 0; i < used_rows; ++i) {
    badge_framebuffer_render_text(&fb,
                                  (int8_t) (MENU_MARGIN_LEFT + BADGE_FONT_WIDTH),
                                  (int8_t) (MENU_MARGIN_TOP  + (first_used_row + i) * MENU_ENTRIES_HEIGHT),
                                  menu[*first_visible + i]);
  }

  badge_framebuffer_render_char(&fb, MENU_MARGIN_LEFT, MENU_MARGIN_TOP + MENU_ENTRIES_HEIGHT * (selected - *first_visible + first_used_row), selector);
  if(arrow_up  ) { badge_framebuffer_blt(&fb, MENU_MARGIN_LEFT, MENU_MARGIN_TOP,                                                    common_sprite(BADGE_COMMON_SPRITE_ARROW_UP  ), 0); }
  if(arrow_down) { badge_framebuffer_blt(&fb, MENU_MARGIN_LEFT, MENU_MARGIN_TOP + (MENU_ENTRIES_VISIBLE - 1) * MENU_ENTRIES_HEIGHT, common_sprite(BADGE_COMMON_SPRITE_ARROW_DOWN), 0); }

  badge_framebuffer_flush(&fb);
}

uint8_t badge_menu(char const *const *menu,
                  uint8_t  n,
                  uint8_t *first_visible,
                  uint8_t  selected)
{
  unsigned scroll_ticks = 0;
  int scroll_direction = 0;

  for(;;) {
    if(scroll_ticks == 0) {
      if       (scroll_direction ==  1 && selected + 1 <  n) {
        ++selected;
      } else if(scroll_direction == -1 && selected     != 0) {
        --selected;
      }

      badge_menu_show(menu, n, first_visible, selected, '*');

      scroll_ticks = (badge_event_current_input_state() & BADGE_EVENT_KEY_BTN_B) ? MENU_SCROLL_TICKS_TURBO : MENU_SCROLL_TICKS;
    }

    badge_event_t ev;

    ev = badge_event_wait();
    switch(badge_event_type(ev)) {
    case BADGE_EVENT_USER_INPUT:
    {
      uint8_t old_state = badge_event_old_input_state(ev);
      uint8_t new_state = badge_event_new_input_state(ev);
      uint8_t new_buttons = new_state & (old_state ^ new_state);

      if((new_buttons & BADGE_EVENT_KEY_BTN_A)) {
        return selected;
      } else if((new_buttons & BADGE_EVENT_KEY_UP  )) {
        scroll_direction = -1;
      } else if((new_buttons & BADGE_EVENT_KEY_DOWN)) {
        scroll_direction =  1;
      } else if(old_state != (new_state ^ BADGE_EVENT_KEY_BTN_B)) {
        scroll_direction =  0;
      }

      scroll_ticks = 0;
      break;
    }
    case BADGE_EVENT_GAME_TICK:
    {
      --scroll_ticks;
      break;
    }
    }
  }
}

void badge_scroll_text(char const *const *lines, uint8_t n) {
  uint8_t first_visible  = 0;
  int scroll_direction  = 0;
  unsigned scroll_ticks = 0;

  for(;;) {
    if(scroll_ticks == 0) {
      if       (scroll_direction ==  1 && first_visible + MENU_ENTRIES_VISIBLE < n) {
        ++first_visible;
      } else if(scroll_direction == -1 && first_visible != 0) {
        --first_visible;
      }

      badge_menu_show(lines, n, &first_visible, first_visible + (first_visible + 1 == n ? 0 : 1), ' ');

      scroll_ticks = MENU_SCROLL_TICKS;
    }

    badge_event_t ev;

    ev = badge_event_wait();
    switch(badge_event_type(ev)) {
    case BADGE_EVENT_USER_INPUT:
    {
      uint8_t old_state = badge_event_old_input_state(ev);
      uint8_t new_state = badge_event_new_input_state(ev);
      uint8_t new_buttons = new_state & (old_state ^ new_state);

      if(new_buttons & (BADGE_EVENT_KEY_BTN_A | BADGE_EVENT_KEY_BTN_B)) {
        return;
      } else if((new_buttons & BADGE_EVENT_KEY_UP  )) {
        scroll_direction = -1;
      } else if((new_buttons & BADGE_EVENT_KEY_DOWN)) {
        scroll_direction =  1;
      } else {
        scroll_direction =  0;
      }

      scroll_ticks = 0;
      break;
    }
    case BADGE_EVENT_GAME_TICK:
    {
      --scroll_ticks;
      break;
    }
    }
  }
}
