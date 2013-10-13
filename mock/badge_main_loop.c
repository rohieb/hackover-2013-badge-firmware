#include "badge_main_loop.h"

#include "jumpnrun/jumpnrun.h"
#include "ui/display.h"
#include "ui/event.h"
#include "ui/sprite.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void badge_main_loop(void) {

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
