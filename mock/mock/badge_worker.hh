#ifndef INCLUDED_BADGE2013_MOCKUP_BADGE_WORKER_HH
#define INCLUDED_BADGE2013_MOCKUP_BADGE_WORKER_HH

namespace badge2013 {
  class lcd_window;

  class badge_worker {
  public:
    badge_worker(lcd_window *win);

    void run();

  private:
    lcd_window *win_;
  };
}

#endif
