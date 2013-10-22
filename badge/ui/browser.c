#include "browser.h"
#include "menu.h"

#include <drivers/fatfs/ff.h>

enum {
  LINE_LENGTH = 16
};

static void badge_browse_textfile_fd(FIL *fd) {
  unsigned linecount = 0;

  {
    char buf[LINE_LENGTH];
    while(f_gets(buf, LINE_LENGTH, fd)) {
      ++linecount;
    }
  }

  if(FR_OK != f_lseek(fd, 0)) {
    return;
  }

  char lines_buf[linecount][LINE_LENGTH];
  char const *lines[linecount];
  unsigned i;

  for(i = 0; i < linecount && f_gets(lines_buf[i], LINE_LENGTH, fd); ++i) {
    lines[i] = lines_buf[i];
  }

  badge_scroll_text(lines, i);
}

void badge_browse_textfile(char const *fname) {
  FIL fd;
  if(FR_OK == f_open(&fd, fname, FA_OPEN_EXISTING | FA_READ)) {
    badge_browse_textfile_fd(&fd);
    f_close(&fd);
  }
}
