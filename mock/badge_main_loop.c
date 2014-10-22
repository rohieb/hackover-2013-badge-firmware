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

#define JNR_PATH_PREFIX "../badge/jumpnrun/levels/"
#define GLD_PATH_PREFIX "../badge/gladio/levels/"

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
    sprintf(lvname, JNR_PATH_PREFIX "%s.lvl", menu[choice]);

    jumpnrun_play_level(lvname);
  }
}

void gladio_loop(void) {
  char menu_buf[MAX_LEVELS][32];
  FILE *fd = fopen("gladio_levels.txt", "r");

  gladio_game_state_persistent persistent_state = gladio_game_state_persistent_new();

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
    sprintf(lvname, GLD_PATH_PREFIX "%s.lvl", menu[choice]);

    gladio_level_number lvnum = { 1, 1 };

    gladio_play_level(lvname, &persistent_state, &lvnum);
  }
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
