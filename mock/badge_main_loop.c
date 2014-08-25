#include "badge_main_loop.h"

#include "gladio/gladio.h"
#include "jumpnrun/jumpnrun.h"
#include "ui/display.h"
#include "ui/event.h"
#include "ui/menu.h"
#include "ui/sprite.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_LEVELS 1024

#define PATH_PREFIX "../badge/jumpnrun/levels/"

void jumpnrun_loop(void) {
  char menu_buf[MAX_LEVELS][32];
  FILE *fd = fopen("levels.txt", "r");

  int i;

  for(i = 0; i < MAX_LEVELS && fgets(menu_buf[i], sizeof(menu_buf[i]), fd); ++i) {
    menu_buf[i][strlen(menu_buf[i]) - 1] = '\0';
  }

  fclose(fd);

  char const *menu[i];
  for(int j = 0; j < i; ++j) {
    menu[j] = menu_buf[j];
  }

  uint8_t choice = 0;
  uint8_t first_visible = 0;

  for(;;) {
    choice = badge_menu(menu, ARRAY_SIZE(menu), &first_visible, choice);
    char lvname[256];
    sprintf(lvname, PATH_PREFIX "%s.lvl", menu[choice]);

    jumpnrun_play_level(lvname);
  }
}

void gladio_loop(void) {
  gladio_game_state state = { 0 };
  gladio_player p = { { { FIXED_INT(10), FIXED_INT(48) }, 0}, 3, 0 };
  badge_framebuffer fb = { { { 0 } } };

  gladio_render_status_bar(&fb, &p, &state);

  badge_framebuffer_flush(&fb);

  badge_event_t ev;

  do {
    ev = badge_event_wait();
  } while(badge_event_type(ev) != BADGE_EVENT_USER_INPUT ||
          (badge_event_new_buttons(ev) & (BADGE_EVENT_KEY_BTN_A | BADGE_EVENT_KEY_BTN_B)) == 0);
}

void badge_main_loop(void) {
  char const *menu[] = {
    "Gladio",
    "Super Hackio"
  };

  uint8_t choice = 0;
  uint8_t first_visible = 0;

  for(;;) {
    choice = badge_menu(menu, ARRAY_SIZE(menu), &first_visible, choice);

    if(choice == 0) {
      gladio_loop();
    } else {
      jumpnrun_loop();
    }
  }
}
