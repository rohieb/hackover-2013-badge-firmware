#include "levels.h"
#include "tiles.h"
#include "items.h"
#include "enemies.h"
#include "jumpnrun.h"

#ifndef __thumb__

typedef enum {
  FR_OK,
  FR_SOME_ERROR
} FRESULT;

typedef size_t UINT;

FRESULT f_read (FIL *fd, void *buf, UINT bufsize, UINT *bytes) {
  *bytes = fread(buf, 1, bufsize, fd);
  return *bytes == 0 ? FR_SOME_ERROR : FR_OK;
}

#endif

typedef struct {
  uint16_t x;
  uint8_t  y;
  uint8_t  type;
} level_thing;

static level_thing jumpnrun_level_parse_blob(unsigned char blob[3]) {
  level_thing result;

  result.y    = blob[0] >> 4;
  result.x    = ((blob[0] & 0x0f) << 8) | blob[1];
  result.type = blob[2];

  return result;
}

static void jumpnrun_level_make_tile(jumpnrun_tile *dest, level_thing thing) {
  memset(dest, 0, sizeof(*dest));

  dest->type  = thing.type;
  dest->pos.x = thing.x;
  dest->pos.y = thing.y;
}

static void jumpnrun_level_make_item(jumpnrun_item *dest, level_thing thing) {
  memset(dest, 0, sizeof(*dest));

  dest->type = &jumpnrun_item_type_data[thing.type];

  uint8_t xoff = (JUMPNRUN_TILE_PIXEL_WIDTH - (dest->type->sprite.width % JUMPNRUN_TILE_PIXEL_WIDTH)) / 2;
  uint8_t yoff = thing.type != JUMPNRUN_ITEM_TYPE_CHECKPOINT ? 1 : 0; // HACK: spezielle Ausnahme

  dest->pos.x = FIXED_POINT( thing.x      * JUMPNRUN_TILE_PIXEL_WIDTH                             + xoff, 0);
  dest->pos.y = FIXED_POINT((thing.y + 1) * JUMPNRUN_TILE_PIXEL_WIDTH - dest->type->sprite.height - yoff, 0);
}

static void jumpnrun_level_make_enemy(jumpnrun_enemy *dest, level_thing thing) {
  memset(dest, 0, sizeof(*dest));

  dest->type = &jumpnrun_enemy_type_data[thing.type];

  dest->spawn_pos.x = FIXED_INT( thing.x      * JUMPNRUN_TILE_PIXEL_WIDTH  + fixed_point_cast_int(dest->type->hitbox.pos.x));
  dest->spawn_pos.y = FIXED_INT((thing.y + 1) * JUMPNRUN_TILE_PIXEL_HEIGHT - fixed_point_cast_int(dest->type->hitbox.extent.y));
  jumpnrun_enemy_despawn(dest);
}

int jumpnrun_load_level_header_from_file(jumpnrun_level *dest, FIL *fd) {
  UINT count;
  uint16_t head[3];

  if(FR_OK != f_read(fd, head, sizeof(head), &count) || count != sizeof(head)) {
    return JUMPNRUN_LEVEL_LOAD_ERROR;
  }

  dest->header. tile_count = head[0];
  dest->header. item_count = head[1];
  dest->header.enemy_count = head[2];

  return JUMPNRUN_LEVEL_LOAD_OK;
}

int jumpnrun_load_level_from_file(jumpnrun_level *dest, FIL *fd) {
  UINT count;
  size_t i;
  unsigned char buf[3];
  uint16_t spos[2];

  if(FR_OK != f_read(fd, spos, sizeof(spos), &count) || count != sizeof(spos)) {
    return JUMPNRUN_LEVEL_LOAD_ERROR;
  } else {
    dest->start_pos.x = fixed_point_sub(FIXED_INT((spos[0] + 1) * JUMPNRUN_TILE_PIXEL_WIDTH ), jumpnrun_player_extents().x);
    dest->start_pos.y =                 FIXED_INT( spos[1]      * JUMPNRUN_TILE_PIXEL_HEIGHT);
  }

  if(FR_OK != f_read(fd, &dest->start_lives, sizeof(dest->start_lives), &count) || count != sizeof(dest->start_lives)) {
    return JUMPNRUN_LEVEL_LOAD_ERROR;
  }

  for(i = 0; i < dest->header.tile_count; ++i) {
    if(FR_OK != f_read(fd, buf, sizeof(buf), &count) || count != sizeof(buf)) {
      return JUMPNRUN_LEVEL_LOAD_ERROR;
    }

    jumpnrun_level_make_tile(&dest->tiles[i], jumpnrun_level_parse_blob(buf));
    if(i != 0 &&
       ((dest->tiles[i - 1].pos.x >  dest->tiles[i].pos.x) ||
        (dest->tiles[i - 1].pos.x == dest->tiles[i].pos.x && dest->tiles[i - 1].pos.y >= dest->tiles[i].pos.y))) {
      return JUMPNRUN_LEVEL_LOAD_ERROR;
    }
  }

  for(i = 0; i < dest->header.item_count; ++i) {
    if(FR_OK != f_read(fd, buf, sizeof(buf), &count) || count != sizeof(buf)) {
      return JUMPNRUN_LEVEL_LOAD_ERROR;
    }

    jumpnrun_level_make_item(&dest->items[i], jumpnrun_level_parse_blob(buf));
    if(i != 0 &&
       (fixed_point_gt(dest->items[i - 1].pos.x, dest->items[i].pos.x) ||
        (fixed_point_eq(dest->items[i - 1].pos.x, dest->items[i].pos.x) && fixed_point_ge(dest->items[i - 1].pos.y, dest->items[i].pos.y)))) {
      return JUMPNRUN_LEVEL_LOAD_ERROR;
    }
  }

  for(i = 0; i < dest->header.enemy_count; ++i) {
    if(FR_OK != f_read(fd, buf, sizeof(buf), &count) || count != sizeof(buf)) {
      return JUMPNRUN_LEVEL_LOAD_ERROR;
    }

    jumpnrun_level_make_enemy(&dest->enemies[i], jumpnrun_level_parse_blob(buf));
    if(i != 0 &&
       (fixed_point_gt(dest->enemies[i - 1].spawn_pos.x, dest->enemies[i].spawn_pos.x) ||
        (fixed_point_eq(dest->enemies[i - 1].spawn_pos.x, dest->enemies[i].spawn_pos.x) && fixed_point_ge(dest->enemies[i - 1].spawn_pos.y, dest->enemies[i].spawn_pos.y)))) {
      return JUMPNRUN_LEVEL_LOAD_ERROR;
    }
  }

  return JUMPNRUN_LEVEL_LOAD_OK;
}
