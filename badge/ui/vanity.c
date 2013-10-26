#include "vanity.h"

#include "display.h"
#include "event.h"
#include <drivers/fatfs/ff.h>

#define VANITY_IMAGE_FILE "/titleimg.dat"

void badge_vanity_read_from_fd(FIL *fd, badge_framebuffer *dest) {
  UINT bytes;
  f_read(fd, dest, sizeof(*dest), &bytes);
}

void badge_vanity_show(void) {
  FIL fd;
  badge_framebuffer fb;

  unsigned min_ticks = 50;

  if(FR_OK == f_open(&fd, VANITY_IMAGE_FILE, FA_OPEN_EXISTING | FA_READ)) {
    badge_vanity_read_from_fd(&fd, &fb);
    f_close(&fd);

    badge_framebuffer_flush(&fb);

    badge_event_t ev;

    do {
      ev = badge_event_wait();

      if(badge_event_type(ev) == BADGE_EVENT_GAME_TICK) {
        --min_ticks;
      }
    } while(min_ticks != 0);

    do {
      ev = badge_event_wait();
    } while(badge_event_type(ev) != BADGE_EVENT_USER_INPUT);
  }
}
