#include "2048.h"
#include "game.h"

#include "../ui/menu.h"
#include "../util/util.h"

uint8_t show_menu(uint8_t * selected) {
  uint8_t first_visible;

  char const * const menu[] = {
    "Neues Spiel",
    "Zurück"
  };

  first_visible = 0;

  return badge_menu(menu, ARRAY_SIZE(menu), &first_visible, *selected);
}

void play_2048(void) {
  uint8_t selected;

  do {
    selected = show_menu(&selected);

    if(selected == 0)
      new_2048();
  } while(selected != 1);
}
