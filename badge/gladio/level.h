#ifndef INCLUDED_BADGE_GLADIO_LEVEL_H
#define INCLUDED_BADGE_GLADIO_LEVEL_H

#include "common.h"

#include <stddef.h>
#include <stdint.h>

#define GLADIO_PATH "/gladio"

typedef struct gladio_level_spawn_spec {
  uint16_t time;
  uint8_t  type;
  uint8_t  pos_y;
} gladio_level_spawn_spec;

typedef struct gladio_level {
  uint16_t len;
  gladio_level_spawn_spec *specs;
} gladio_level;

size_t gladio_level_count(void);

#define GLADIO_LEVEL_MAKE_SPACE(var)              \
  gladio_level_spawn_spec var ## _specs[var.len]; \
  var.specs = var ## _specs;

#ifdef __thumb__

#include <drivers/fatfs/ff.h>

#define GLADIO_LEVEL_LOAD(lv, lvname)                                   \
  memset(&(lv), 0, sizeof(lv));                                         \
  FIL fd;                                                               \
  if(FR_OK != f_chdir(GLADIO_PATH) ||                                   \
     FR_OK != f_open(&fd, (lvname), FA_OPEN_EXISTING | FA_READ)) {      \
    return GLADIO_ERROR;                                                \
  }                                                                     \
  if(0 != gladio_load_level_header_from_file(&(lv), &fd)) {             \
    f_close(&fd);                                                       \
    return GLADIO_ERROR;                                                \
  }                                                                     \
  GLADIO_LEVEL_MAKE_SPACE(lv);                                          \
  int err = gladio_load_level_from_file(&(lv), &fd);                    \
  f_close(&fd);                                                         \
  if(err != 0) {                                                        \
    return GLADIO_ERROR;                                                \
  }

#else

#include <stdio.h>

typedef FILE FIL;

#define GLADIO_LEVEL_LOAD(lv, lvname)                           \
  memset(&(lv), 0, sizeof(lv));                                 \
  FILE *fd = fopen((lvname), "r");                              \
  if(fd == NULL) {                                              \
    return GLADIO_ERROR;                                        \
  }                                                             \
  int err = gladio_load_level_header_from_file(&(lv), fd);      \
  if(err != 0) {                                                \
    fclose(fd);                                                 \
    return GLADIO_ERROR;                                        \
  }                                                             \
  GLADIO_LEVEL_MAKE_SPACE(lv);                                  \
  err = gladio_load_level_from_file(&(lv), fd);                 \
  fclose(fd);                                                   \
  if(err != 0) return GLADIO_ERROR;

#endif

int gladio_load_level_header_from_file(gladio_level *dest, FIL *fd);
int gladio_load_level_from_file       (gladio_level *dest, FIL *fd);

#endif
