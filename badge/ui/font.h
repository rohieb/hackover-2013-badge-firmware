#ifndef INCLUDED_BADGE_UI_FONT_H
#define INCLUDED_BADGE_UI_FONT_H

#include "display.h"

enum {
  BADGE_FONT_WIDTH  = 6,
  BADGE_FONT_HEIGHT = 8
};

uint8_t badge_framebuffer_render_char(badge_framebuffer *fb, int8_t pos_x, int8_t pos_y, char c);
uint8_t badge_framebuffer_render_text(badge_framebuffer *fb, int8_t pos_x, int8_t pos_y, char const *text);

#endif
