#include "badge_main_loop.h"

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

void badge_main_loop(void) {
  char menu_buf[MAX_LEVELS][32];
  FILE *fd = fopen("../badge/jumpnrun/levels.txt", "r");

  int i;

  for(i = 0; i < MAX_LEVELS && fgets(menu_buf[i], sizeof(menu_buf[i]), fd); ++i) {
    menu_buf[i][strlen(menu_buf[i]) - 1] = '\0';
  }

  fclose(fd);

  char const *menu[i];
  for(int j = 0; j < i; ++j) {
    menu[j] = menu_buf[j];
  }

  size_t choice = 0;
  size_t first_visible = 0;

  for(;;) {
    choice = badge_menu(menu, ARRAY_SIZE(menu), &first_visible, choice);
    char lvname[256];
    sprintf(lvname, "../badge/jumpnrun/%s.lvl", menu[choice]);

    jumpnrun_play(lvname);
  }
}
