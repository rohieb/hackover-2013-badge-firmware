#include "browser.h"
#include "menu.h"

#include <drivers/fatfs/ff.h>
#include <string.h>

enum {
  LINE_LENGTH = 16,
  FNAME_MAX = 12,
  BUFLEN = LINE_LENGTH + 1 + FNAME_MAX + 1
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

static uint8_t badge_count_lines_in_file(FIL *fd) {
  uint8_t count = 0;

  if(FR_OK == f_lseek(fd, 0)) {
    char buf[BUFLEN];
    while(f_gets(buf, BUFLEN, fd)) {
      ++count;
    }
  }

  return count;
}

static uint8_t badge_browse_pick_filename_from_fd(char *buf, uint8_t *first_visible, uint8_t *selected, FIL *fd) {
  unsigned linecount = badge_count_lines_in_file(fd);

  if(FR_OK != f_lseek(fd, 0)) {
    return 1;
  }

  char        menu_buf  [linecount + 1][BUFLEN];
  char const *menu_index[linecount + 1];
  char       *fnames    [linecount + 1];
  unsigned i;

  for(i = 0; i < linecount && f_gets(menu_buf[i], BUFLEN, fd); ++i) {
    menu_index[i] = menu_buf[i];
    char *p;
    for(p = menu_buf[i]; *p && *p != '|'; ++p)
      ;
    if(*p) {
      *p++ = '\0';
    }
    fnames[i] = p;
  }

  strcpy(menu_buf[i], "Zurück");
  menu_index[i] = menu_buf[i];
  uint8_t choice = badge_menu(menu_index, i + 1, first_visible, *selected);

  if(choice == linecount) {
    return 1; // exit
  }

  *selected = choice;
  strncpy(buf, fnames[*selected], FNAME_MAX);
  buf[FNAME_MAX] = '\0';

  return 0;
}

static uint8_t badge_pick_filename(char *buf, char const *menufile, uint8_t *first_visible, uint8_t *selected) {
  FIL fd;
  uint8_t err = 1;

  if(FR_OK == f_open(&fd, menufile, FA_OPEN_EXISTING | FA_READ)) {
    err = badge_browse_pick_filename_from_fd(buf, first_visible, selected, &fd);
    f_close(&fd);
  }

  return err;
}

void badge_browse_textfile(char const *fname) {
  FIL fd;

  if(FR_OK == f_open(&fd, fname, FA_OPEN_EXISTING | FA_READ)) {
    badge_browse_textfile_fd(&fd);
    f_close(&fd);
  }
}

void badge_browse_textfiles(char const *menufile, uint8_t *selected) {
  char buf[FNAME_MAX + 1];
  uint8_t first_visible = *selected;

  while(0 == badge_pick_filename(buf, menufile, &first_visible, selected)) {
    badge_browse_textfile(buf);
  }
}
