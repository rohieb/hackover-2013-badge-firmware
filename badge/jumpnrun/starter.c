#include "jumpnrun.h"
#include "../ui/menu.h"
#include "../ui/browser.h"

#include <drivers/fatfs/ff.h>

#define CREDITSFILE "credits.txt"

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

#define PROGRESS_FNAME "progress.dat"

static uint8_t jumpnrun_load_progress(void) {
  uint8_t progress = 0;
  FIL fd;


  if(FR_OK == f_chdir(JUMPNRUN_PATH) &&
     FR_OK == f_open(&fd, PROGRESS_FNAME, FA_OPEN_EXISTING | FA_READ)) {
    UINT bytes;
    f_read(&fd, &progress, sizeof(progress), &bytes);
    f_close(&fd);
  }

  return progress;
}

static void jumpnrun_save_progress(uint8_t progress) {
  FIL fd;

  if(FR_OK == f_chdir(JUMPNRUN_PATH) &&
     FR_OK == f_open(&fd, PROGRESS_FNAME, FA_CREATE_ALWAYS | FA_WRITE)) {
    UINT bytes;
    f_write(&fd, &progress, sizeof(progress), &bytes);
    f_close(&fd);
  }
}

static uint8_t jumpnrun_pick_level_from_fd(char *buf, size_t *first_visible, size_t *selected, uint8_t progress, FIL *fd) {
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

  size_t menulen   = levelcount + (levelcount <= progress) + 1;

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

  size_t creditspos = -1;
  size_t exitpos   = i;

  if(levelcount <= progress) {
    creditspos = i;
    strcpy(menu_buf[creditspos], "Credits");
    menu_index[creditspos] = menu_buf[creditspos];

    ++exitpos;
  }

  strcpy(menu_buf[exitpos], "Zurück");
  menu_index[exitpos] = menu_buf[exitpos];

  size_t choice = badge_menu(menu_index, exitpos + 1, first_visible, *selected);

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

static uint8_t jumpnrun_pick_level(char *buf, size_t *first_visible, size_t *selected, uint8_t progress) {
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
  char buf[LEVELFILE_MAX + 1];
  size_t first_visible = 0;
  size_t selected = 0;
  uint8_t progress = jumpnrun_load_progress();
  uint8_t choice;

  do {
    choice = jumpnrun_pick_level(buf, &first_visible, &selected, progress);

    switch(choice) {
    case CHOICE_LEVEL:
      if(JUMPNRUN_WON == jumpnrun_play_level(buf) && selected == progress) {
        selected = ++progress;
        jumpnrun_save_progress(progress);
      }
      break;
    case CHOICE_CREDITS:
      badge_browse_textfile(CREDITSFILE);
      break;
    }
  } while(choice != CHOICE_EXIT);
}
