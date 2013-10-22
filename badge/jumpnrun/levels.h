#ifndef INCLUDED_JUMPNRUN_LEVEL_H
#define INCLUDED_JUMPNRUN_LEVEL_H

#include "enemies.h"
#include "items.h"
#include "tiles.h"

#include "../util/rectangle.h"

#include <stddef.h>
#include <stdio.h>

#define JUMPNRUN_PATH "/hackio"

typedef struct jumpnrun_level_header {
  uint16_t tile_count;
  uint16_t item_count;
  uint16_t enemy_count;
} jumpnrun_level_header;

typedef struct jumpnrun_level {
  jumpnrun_level_header header;

  vec2d                 start_pos;
  uint8_t               start_lives;

  jumpnrun_tile        *tiles;
  jumpnrun_item        *items;
  jumpnrun_enemy       *enemies;
} jumpnrun_level;

enum {
  JUMPNRUN_LEVEL_LOAD_OK,
  JUMPNRUN_LEVEL_LOAD_ERROR
};

size_t jumpnrun_level_count(void);
void jumpnrun_levels_dump(void);

// Use stack-local VLAs to store dynamic content.
#define JUMPNRUN_LEVEL_MAKE_SPACE(var)                    \
  jumpnrun_tile  var ## _tiles  [var.header.tile_count];  \
  jumpnrun_item  var ## _items  [var.header.item_count];  \
  jumpnrun_enemy var ## _enemies[var.header.enemy_count]; \
                                                          \
  var.tiles   = var ## _tiles;                            \
  var.items   = var ## _items;                            \
  var.enemies = var ## _enemies;
#else

#ifdef __linux__

int jumpnrun_load_level_header_from_file(jumpnrun_level *dest, FILE *fd);
int jumpnrun_load_level_from_file       (jumpnrun_level *dest, FILE *fd);

#define JUMPNRUN_LEVEL_LOAD(lv, lvname)                         \
  memset(&(lv), 0, sizeof(lv));                                 \
  FILE *fd = fopen((lvname), "r");                              \
  if(fd == NULL) return JUMPNRUN_ERROR;                         \
  int err = jumpnrun_load_level_header_from_file(&(lv), fd);    \
  if(err != 0) {                                                \
    fclose(fd);                                                 \
    return JUMPNRUN_ERROR;                                      \
  }                                                             \
  JUMPNRUN_LEVEL_MAKE_SPACE(lv);                                \
  err = jumpnrun_load_level_from_file(&(lv), fd);               \
  fclose(fd);                                                   \
  if(err != 0) return JUMPNRUN_ERROR;
#else
#include <drivers/fatfs/ff.h>

int jumpnrun_load_level_header_from_file(jumpnrun_level *dest, FIL *fd);
int jumpnrun_load_level_from_file       (jumpnrun_level *dest, FIL *fd);

#define JUMPNRUN_LEVEL_LOAD(lv, lvname)                                 \
  memset(&(lv), 0, sizeof(lv));                                         \
  FIL fd;                                                               \
  if(FR_OK != f_chdir(JUMPNRUN_PATH) ||                                 \
     FR_OK != f_open(&fd, (lvname), FA_OPEN_EXISTING | FA_READ)) {      \
    return JUMPNRUN_ERROR;                                              \
  }                                                                     \
  if(0 != jumpnrun_load_level_header_from_file(&(lv), &fd)) {           \
    f_close(&fd);                                                       \
    return JUMPNRUN_ERROR;                                              \
  }                                                                     \
  JUMPNRUN_LEVEL_MAKE_SPACE(lv);                                        \
  int err = jumpnrun_load_level_from_file(&(lv), &fd);                  \
  f_close(&fd);                                                         \
  if(err != 0) {                                                        \
    return JUMPNRUN_ERROR;                                              \
  }
#endif

#endif
