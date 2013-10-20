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

void badge_main_loop(void) {
  char const *const menu[] = {
    "smb",
    "skynet",
    "wrongturn",
    "lubiXOXO",
    "lubilove",
    "gnobbel",
    "foo",
    "mean",
    "xyzzy",
    "abc",
    "nonsense"
  };

  size_t choice = badge_menu(menu, ARRAY_SIZE(menu), 0, 0);

  printf("%zu\n", choice);

  for(;;) {
    FILE *fd = fopen("../badge/jumpnrun/levels.txt", "r");
    char buf[12];

    while(fgets(buf, sizeof(buf), fd)) {
      buf[strlen(buf) - 1] = '\0';
      char lvname[256];
      sprintf(lvname, "../badge/jumpnrun/%s.lvl", buf);
      while(jumpnrun_play(lvname) != JUMPNRUN_WON)
        ;
    }

    fclose(fd);
  }
}
