#include "browser.h"
#include "event.h"
#include "read_file.h"
#include "scroll.h"

#include <ui/menu.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

enum {
  LINE_LENGTH = 17,
};

static struct string_array * file_contents = NULL;
static uint8_t mb_first_visible = 0;
static uint8_t mb_need_update   = 1;

void mock_browse_open_file(char const *fname) {
  free(file_contents);
  file_contents = read_file_hint(fname, LINE_LENGTH);
  mb_first_visible = 0;
  scroll_reset();
}

int mock_browse_tick(void) {
  badge_event_t ev;

  while(mock_event_poll(&ev)) {
    uint8_t new_buttons = mock_event_new_buttons(ev);

    if(badge_event_type(ev) == BADGE_EVENT_USER_INPUT) {
      scroll_reset();
    }

    if(new_buttons & (BADGE_EVENT_KEY_BTN_A | BADGE_EVENT_KEY_BTN_B)) {
      return BROWSER_EXIT;
    } else if((new_buttons & BADGE_EVENT_KEY_UP  ) && mb_first_visible     > 0) {
      --mb_first_visible;
      mb_need_update = 1;
    } else if((new_buttons & BADGE_EVENT_KEY_DOWN) && mb_first_visible + 1 < file_contents->size) {
      ++mb_first_visible;
      mb_need_update = 1;
    }
  }

  int scroll_direction = scroll_tick();
  if(scroll_direction == -1 && mb_first_visible     > 0) {
    --mb_first_visible;
    mb_need_update = 1;
  } else if(scroll_direction == 1 && mb_first_visible + 1 < file_contents->size) {
    ++mb_first_visible;
    mb_need_update = 1;
  }

  if(mb_need_update) {
    badge_menu_show((char const *const*) file_contents->data,
                    file_contents->size,
                    &mb_first_visible,
                    mb_first_visible + (mb_first_visible + 1 == file_contents->size ? 0 : 1),
                    ' ');
    mb_need_update = 0;
  }

  return BROWSER_CONTINUE;
}
