#include <ui/display.h>
#include "lcd-window.hh"

namespace badge2013 {
  namespace {
    lcd_window *window;
  }

  void badge_framebuffer_set_backend(lcd_window *win) {
    window = win;
  }
}

void badge_framebuffer_flush(badge_framebuffer const *fb) {
  badge2013::window->push_framebuffer(*fb);
}
