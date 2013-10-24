#include "jumpnrun.h"
#include "../ui/menu.h"
#include "../ui/browser.h"

#include <drivers/fatfs/ff.h>

#define CREDITS_FNAME  "credits.txt"
#define POSITION_FNAME "selected.dat"
#define PROGRESS_FNAME "progress.dat"

enum {
  LEVELFILE_MAX        = 12,
  LEVELDESCRIPTION_MAX = 14,
  MENU_BUFLEN          = LEVELDESCRIPTION_MAX + 1 + LEVELFILE_MAX + 1
};

enum {
  CHOICE_LEVEL,
  CHOICE_CREDITS,
  CHOICE_EXIT,
  CHOICE_ERROR
};

static uint8_t read_byte_from_file(char const *fname) {
  FIL fd;
  uint8_t x = 0;

  if(FR_OK == f_chdir(JUMPNRUN_PATH) &&
     FR_OK == f_open(&fd, fname, FA_OPEN_EXISTING | FA_READ)) {
    UINT bytes;
    f_read(&fd, &x, sizeof(x), &bytes);
    f_close(&fd);
  }

  return x;
}

static void save_byte_to_file(char const *fname, uint8_t x) {
  FIL fd;

  if(FR_OK == f_chdir(JUMPNRUN_PATH) &&
     FR_OK == f_open(&fd, fname, FA_CREATE_ALWAYS | FA_WRITE)) {
    UINT bytes;
    f_write(&fd, &x, sizeof(x), &bytes);
    f_close(&fd);
  }
}

static uint8_t jumpnrun_load_selected(void            ) { return read_byte_from_file(POSITION_FNAME          ); }
static void    jumpnrun_save_selected(uint8_t selected) {        save_byte_to_file  (POSITION_FNAME, selected); }

static uint8_t jumpnrun_load_progress(void            ) { return read_byte_from_file(PROGRESS_FNAME          ); }
static void    jumpnrun_save_progress(uint8_t progress) {        save_byte_to_file  (PROGRESS_FNAME, progress); }

static uint8_t jumpnrun_pick_level_from_fd(char *buf, uint8_t *first_visible, uint8_t *selected, uint8_t progress, FIL *fd) {
  unsigned levelcount = 0;

  {
    char buf[MENU_BUFLEN];
    while(f_gets(buf, MENU_BUFLEN, fd) && levelcount <= progress) {
      ++levelcount;
    }
  }

  if(FR_OK != f_lseek(fd, 0)) {
    return JUMPNRUN_ERROR;
  }

  uint8_t menulen   = levelcount + (levelcount <= progress) + 1;

  char        menu_buf  [menulen][MENU_BUFLEN];
  char const *menu_index[menulen];
  char const *fnames    [menulen];
  unsigned i;

  for(i = 0; i < levelcount && f_gets(menu_buf[i], MENU_BUFLEN, fd); ++i) {
    menu_index[i] = menu_buf[i];
    char *p;
    for(p = menu_buf[i]; *p && *p != '|'; ++p)
      ;
    if(*p) {
      *p++ = '\0';
    }
    fnames[i] = p;
  }

  uint8_t creditspos = -1;
  uint8_t exitpos   = i;

  if(levelcount <= progress) {
    creditspos = i;
    strcpy(menu_buf[creditspos], "Credits");
    menu_index[creditspos] = menu_buf[creditspos];

    ++exitpos;
  }

  strcpy(menu_buf[exitpos], "Zurück");
  menu_index[exitpos] = menu_buf[exitpos];

  uint8_t choice = badge_menu(menu_index, exitpos + 1, first_visible, *selected);

  if(choice == exitpos) {
    return CHOICE_EXIT;
  }

  *selected = choice;

  if(choice == creditspos) {
    return CHOICE_CREDITS;
  }

  strncpy(buf, fnames[*selected], LEVELFILE_MAX);
  buf[LEVELFILE_MAX] = '\0';

  return CHOICE_LEVEL;
}

static uint8_t jumpnrun_pick_level(char *buf, uint8_t *first_visible, uint8_t *selected, uint8_t progress) {
  FIL fd;

  if(FR_OK != f_chdir(JUMPNRUN_PATH) ||
     FR_OK != f_open(&fd, "levels.lst", FA_OPEN_EXISTING | FA_READ)) {
    return JUMPNRUN_ERROR;
  }

  uint8_t err = jumpnrun_pick_level_from_fd(buf, first_visible, selected, progress, &fd);

  f_close(&fd);

  return err;
}

void jumpnrun_play(void) {
  char    buf[LEVELFILE_MAX + 1];
  uint8_t selected = jumpnrun_load_selected();
  uint8_t progress = jumpnrun_load_progress();
  uint8_t first_visible = selected;
  uint8_t choice;
  uint8_t oldselected = selected;

  do {
    if(oldselected != selected) {
      jumpnrun_save_selected((uint8_t) selected);
      oldselected = selected;
    }

    choice = jumpnrun_pick_level(buf, &first_visible, &selected, progress);

    switch(choice) {
    case CHOICE_LEVEL:
      if(JUMPNRUN_WON == jumpnrun_play_level(buf) && selected == progress) {
        selected = ++progress;
        jumpnrun_save_progress(progress);
      }
      break;
    case CHOICE_CREDITS:
      badge_browse_textfile(CREDITS_FNAME);
      break;
    }
  } while(choice != CHOICE_EXIT);
}
