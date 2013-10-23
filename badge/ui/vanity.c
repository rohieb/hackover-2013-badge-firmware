#include "vanity.h"

#include "display.h"
#include "event.h"
#include <drivers/fatfs/ff.h>

#define VANITY_IMAGE_FILE "/vanity.dat"

void badge_vanity_read_from_fd(FIL *fd, badge_framebuffer *dest) {
  UINT bytes;
  f_read(fd, dest, sizeof(*dest), &bytes);
}

void badge_vanity_show(void) {
  FIL fd;
  badge_framebuffer fb;

  if(FR_OK == f_open(&fd, VANITY_IMAGE_FILE, FA_OPEN_EXISTING | FA_READ)) {
    badge_vanity_read_from_fd(&fd, &fb);
    f_close(&fd);

    badge_framebuffer_flush(&fb);

    badge_event_t ev;
    do {
      ev = badge_event_wait();
    } while(badge_event_type(ev) != BADGE_EVENT_USER_INPUT);

    for(;;);
  }
}
