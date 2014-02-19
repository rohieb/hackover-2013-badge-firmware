#include "menu.h"
#include "read_file.h"

#include "event.h"
#include "scroll.h"

#include <ui/event.h>
#include <ui/menu.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum {
  LEVELFILE_MAX        = 12,
  LEVELDESCRIPTION_MAX = 14,
  MENU_BUFLEN          = LEVELDESCRIPTION_MAX + 1 + LEVELFILE_MAX + 1
};

#define MENUFILE "hackio/levels.lst"

static uint8_t mock_menu_initialised   = 0;
static uint8_t mock_first_visible      = 0;
static uint8_t mock_selected           = 0;
static uint8_t mock_menu_already_shown = 0;

static struct string_array *mock_menu = NULL;
static char const **mock_menu_index;
static char const **mock_menu_fnames;

static void mock_menu_init(void) {
  mock_menu = read_file_hint(MENUFILE, MENU_BUFLEN);

  mock_menu_index  = malloc(sizeof(*mock_menu_index ) * (mock_menu->size + 1));
  mock_menu_fnames = malloc(sizeof(*mock_menu_fnames) * (mock_menu->size + 1));

  if(mock_menu        == NULL ||
     mock_menu_index  == NULL ||
     mock_menu_fnames == NULL) {
    free(mock_menu       ); mock_menu        = NULL;
    free(mock_menu_index ); mock_menu_index  = NULL;
    free(mock_menu_fnames); mock_menu_fnames = NULL;
    return;
  }

  for(size_t i = 0; i < mock_menu->size; ++i) {
    mock_menu_index[i] = mock_menu->data[i];

    char *p;

    for(p = mock_menu->data[i]; *p && *p != '|'; ++p)
      ;
    if(*p) {
      *p++ = '\0';
    }
    mock_menu_fnames[i] = p;
  }

  mock_menu_index [mock_menu->size] = "Credits";
  mock_menu_fnames[mock_menu->size] = NULL;

  mock_menu_initialised = 1;
  return;
}

int mock_menu_tick(void) {
  if(!mock_menu_initialised) {
    mock_menu_init();
  }

  badge_event_t ev;
  while(mock_event_poll(&ev)) {
    uint8_t new_buttons = mock_event_new_buttons(ev);

    if(badge_event_type(ev) == BADGE_EVENT_USER_INPUT) {
      scroll_reset();
    }

    if(new_buttons & (BADGE_EVENT_KEY_BTN_A | BADGE_EVENT_KEY_BTN_B)) {
      mock_menu_already_shown = 0;
      return mock_menu_fnames[mock_selected] ? MOCK_MENU_SELECTED : MOCK_MENU_CREDITS;
    } else if((new_buttons & BADGE_EVENT_KEY_UP  ) && mock_selected     > 0) {
      --mock_selected;
      mock_menu_already_shown = 0;
    } else if((new_buttons & BADGE_EVENT_KEY_DOWN) && mock_selected < mock_menu->size) {
      ++mock_selected;
      mock_menu_already_shown = 0;
    }
  }

  int scroll_direction = scroll_tick();
  if(scroll_direction == -1 && mock_selected     > 0) {
    --mock_selected;
    mock_menu_already_shown = 0;
  } else if(scroll_direction == 1 && mock_selected < mock_menu->size) {
    ++mock_selected;
    mock_menu_already_shown = 0;
  }

  if(!mock_menu_already_shown) {
    badge_menu_show(mock_menu_index,
                    mock_menu->size + 1,
                    &mock_first_visible,
                    mock_selected,
                    '*');
    mock_menu_already_shown = 1;
  }

  return MOCK_MENU_CONTINUE;
}

char const *mock_menu_selection(void) {
  return mock_menu_fnames[mock_selected];
}
