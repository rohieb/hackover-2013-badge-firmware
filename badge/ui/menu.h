#ifndef INCLUDED_BADGE_UI_MENU_H
#define INCLUDED_BADGE_UI_MENU_H

#include <stddef.h>
#include <stdint.h>

uint8_t badge_menu(char const *const * menu,
                   uint8_t  n,
                   uint8_t *first_visible,
                   uint8_t  preselected);

void badge_scroll_text(char const *const *lines, uint8_t n);

#endif
