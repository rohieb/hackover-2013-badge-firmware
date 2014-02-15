#ifndef INCLUDED_MOCK_SDL_MOCK_JUMPNRUN_H
#define INCLUDED_MOCK_SDL_MOCK_JUMPNRUN_H

enum {
  MOCK_JUMPNRUN_CONTINUE,
  MOCK_JUMPNRUN_GAMEOVER
};

int mock_jumpnrun_start_level(char const *fname);
int mock_jumpnrun_tick(void);

#endif
