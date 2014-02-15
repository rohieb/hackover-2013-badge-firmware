#include "menu.h"

#include "event.h"

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

static char (*mock_menu_buffer)[MENU_BUFLEN];
static char const **mock_menu_index;
static char const **mock_menu_fnames;
static uint8_t      mock_menu_length;

static void mock_menu_init_from_fd(FILE *fd) {
  char buf[MENU_BUFLEN];
  uint8_t counter = 0;

  while(counter < 255 && fgets(buf, sizeof(buf), fd)) {
    ++counter;
  }

  if(fseek(fd, 0, SEEK_SET) == -1) {
    return;
  }

  mock_menu_buffer = malloc(sizeof(*mock_menu_buffer) * counter);
  mock_menu_index  = malloc(sizeof(*mock_menu_index ) * counter);
  mock_menu_fnames = malloc(sizeof(*mock_menu_fnames) * counter);

  if(mock_menu_buffer == NULL ||
     mock_menu_index  == NULL ||
     mock_menu_fnames == NULL) {
    free(mock_menu_buffer); mock_menu_buffer = NULL;
    free(mock_menu_index ); mock_menu_index  = NULL;
    free(mock_menu_fnames); mock_menu_fnames = NULL;
    return;
  }

  for(int i = 0; i < counter && fgets(mock_menu_buffer[i], sizeof(*mock_menu_buffer), fd); ++i) {
    mock_menu_index[i] = mock_menu_buffer[i];

    char *p;

    for(p = mock_menu_buffer[i]; *p && *p != '|'; ++p)
      ;
    if(*p) {
      *p++ = '\0';
    }
    mock_menu_fnames[i] = p;

    while(*p && *p != '\n') {
      ++p;
    }
    *p = '\0';
  }

  mock_menu_length      = counter;
  mock_menu_initialised = 1;
  return;
}

static void mock_menu_init(void) {
  FILE *fd = fopen(MENUFILE, "rb");

  if(fd != NULL) {
    mock_menu_init_from_fd(fd);
    fclose(fd);
  }
}

int mock_menu_tick(void) {
  if(!mock_menu_initialised) {
    mock_menu_init();
  }

  badge_event_t ev;
  while(mock_event_poll(&ev)) {
    uint8_t old_state = badge_event_old_input_state(ev);
    uint8_t new_state = badge_event_new_input_state(ev);
    uint8_t new_buttons = new_state & (old_state ^ new_state);

    if(new_buttons & (BADGE_EVENT_KEY_BTN_A | BADGE_EVENT_KEY_BTN_B)) {
      mock_menu_already_shown = 0;
      return MOCK_MENU_SELECTED;
    } else if((new_buttons & BADGE_EVENT_KEY_UP  ) && mock_selected     > 0) {
      --mock_selected;
      mock_menu_already_shown = 0;
    } else if((new_buttons & BADGE_EVENT_KEY_DOWN) && mock_selected + 1 < mock_menu_length) {
      ++mock_selected;
      mock_menu_already_shown = 0;
    }
  }

  if(!mock_menu_already_shown) {
    badge_menu_show(mock_menu_index,
                    mock_menu_length,
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
