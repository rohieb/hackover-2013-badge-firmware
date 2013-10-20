#ifndef INCLUDED_BADGE_UI_MENU_H
#define INCLUDED_BADGE_UI_MENU_H

#include <stddef.h>
#include <stdint.h>

size_t badge_menu(char const *const * menu,
                  size_t n,
                  size_t first_visible,
                  size_t preselected);

#endif
