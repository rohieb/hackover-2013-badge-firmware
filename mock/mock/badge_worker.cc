#include "badge_worker.hh"
#include "badge_main_loop.h"
#include "lcd-window.hh"

namespace badge2013 {
  void badge_framebuffer_set_backend(lcd_window *win);

  badge_worker::badge_worker(lcd_window *win) : win_(win) { }

  void badge_worker::run() {
    badge_framebuffer_set_backend(win_);
    badge_main_loop();
    win_->request_close();
  }
}
