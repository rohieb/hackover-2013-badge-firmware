#include "fahrplan.h"
#include "ui/browser.h"

#include <drivers/fatfs/ff.h>

#define FAHRPLAN_PATH     "/fahrplan"
#define FAHRPLAN_MENUFILE "fahrplan.lst"
#define FAHRPLAN_POSFILE  "selected.dat"

static size_t badge_load_fahrplan_pos(void) {
  size_t pos = 0;
  FIL fd;

  if(FR_OK == f_open(&fd, FAHRPLAN_POSFILE, FA_OPEN_EXISTING | FA_READ)) {
    UINT bytes;
    size_t buf;
    if(FR_OK == f_read(&fd, &buf, sizeof(buf), &bytes) && bytes == sizeof(buf)) {
      pos = buf;
    }

    f_close(&fd);
  }

  return pos;
}

static void badge_save_fahrplan_pos(size_t pos) {
  FIL fd;

  if(FR_OK == f_open(&fd, FAHRPLAN_POSFILE, FA_CREATE_ALWAYS | FA_WRITE)) {
    UINT bytes;
    f_write(&fd, &pos, sizeof(pos), &bytes);
    f_close(&fd);
  }
}

void badge_fahrplan(void) {
  f_chdir(FAHRPLAN_PATH);

  size_t oldpos = badge_load_fahrplan_pos();
  size_t pos = oldpos;

  badge_browse_textfiles(FAHRPLAN_MENUFILE, &pos);

  if(oldpos != pos) {
    badge_save_fahrplan_pos(pos);
  }
}
