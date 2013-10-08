#ifndef INCLUDED_JUMPNRUN_LEVEL_H
#define INCLUDED_JUMPNRUN_LEVEL_H

#include "enemies.h"
#include "items.h"
#include "tiles.h"

#include <badge/util/rectangle.h>

#include <drivers/fatfs/ff.h>

#include <stddef.h>
#include <stdio.h>

typedef struct jumpnrun_level_header {
  size_t                tile_count;
  size_t                item_count;
  size_t                enemy_count;
} jumpnrun_level_header;

typedef struct jumpnrun_level {
  jumpnrun_level_header header;

  vec2d                 start_pos;

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

int jumpnrun_load_level_header_from_file(jumpnrun_level *dest, FIL *fd);
int jumpnrun_load_level_from_file       (jumpnrun_level *dest, FIL *fd);

// Use stack-local VLAs to store dynamic content.
#define JUMPNRUN_LEVEL_MAKE_SPACE(var)                    \
  jumpnrun_tile  var ## _tiles  [var.header.tile_count];  \
  jumpnrun_item  var ## _items  [var.header.item_count];  \
  jumpnrun_enemy var ## _enemies[var.header.enemy_count]; \
                                                          \
  var.tiles   = var ## _tiles;                            \
  var.items   = var ## _items;                            \
  var.enemies = var ## _enemies;

#endif
