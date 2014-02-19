#ifndef INCLUDED_MOCK_SDL_MOCK_MENU_H
#define INCLUDED_MOCK_SDL_MOCK_MENU_H

enum {
  MOCK_MENU_CONTINUE,
  MOCK_MENU_SELECTED,
  MOCK_MENU_CREDITS
};

int         mock_menu_tick     (void);
char const *mock_menu_selection(void);

#endif
