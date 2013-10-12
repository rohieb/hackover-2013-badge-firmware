#ifndef INCLUDED_JUMPNRUN_TILES_H
#define INCLUDED_JUMPNRUN_TILES_H

#include "../util/fixed_point.h"
#include "../util/rectangle.h"
#include "../ui/sprite.h"

#include <stdint.h>

typedef enum {
  JUMPNRUN_TILE_TOP    = 1,
  JUMPNRUN_TILE_BOTTOM = 2,
  JUMPNRUN_TILE_LEFT   = 4,
  JUMPNRUN_TILE_RIGHT  = 8
} jumpnrun_tile_sides;

enum {
  JUMPNRUN_TILE_PIXEL_WIDTH  = 5,
  JUMPNRUN_TILE_PIXEL_HEIGHT = 5
};

typedef struct jumpnrun_tile_position {
  uint16_t x;
  uint8_t  y;
} jumpnrun_tile_position;

typedef struct jumpnrun_tile_type {
  uint8_t      lethal_sides;
  badge_sprite sprite;
} jumpnrun_tile_type;

typedef struct jumpnrun_tile {
  jumpnrun_tile_position pos;
  uint8_t                type;
} jumpnrun_tile;

typedef struct jumpnrun_tile_range {
  size_t first;
  size_t last; // actually one past last.
} jumpnrun_tile_range;

static inline fixed_point tile_left  (jumpnrun_tile const *tile) { return FIXED_POINT(tile->pos.x * JUMPNRUN_TILE_PIXEL_WIDTH , 0); }
static inline fixed_point tile_top   (jumpnrun_tile const *tile) { return FIXED_POINT(tile->pos.y * JUMPNRUN_TILE_PIXEL_HEIGHT, 0); }
static inline fixed_point tile_right (jumpnrun_tile const *tile) { return fixed_point_add(tile_left(tile), FIXED_POINT(JUMPNRUN_TILE_PIXEL_WIDTH , 0)); }
static inline fixed_point tile_bottom(jumpnrun_tile const *tile) { return fixed_point_add(tile_top (tile), FIXED_POINT(JUMPNRUN_TILE_PIXEL_HEIGHT, 0)); }

/************************************/

enum {
  JUMPNRUN_TILE_TYPE_BRICK,
  JUMPNRUN_TILE_TYPE_SQUARE,
  JUMPNRUN_TILE_TYPE_TUBE_TOP_LEFT,
  JUMPNRUN_TILE_TYPE_TUBE_TOP_RIGHT,
  JUMPNRUN_TILE_TYPE_TUBE_LEFT,
  JUMPNRUN_TILE_TYPE_TUBE_RIGHT,

  JUMPNRUN_TILE_TYPE_COUNT
};

extern jumpnrun_tile_type const jumpnrun_tile_type_data[JUMPNRUN_TILE_TYPE_COUNT];

static inline jumpnrun_tile_type const *tile_type(jumpnrun_tile const *tile) {
  return &jumpnrun_tile_type_data[tile->type];
}

static inline rectangle rect_from_tile(jumpnrun_tile const *tile) {
  rectangle r = {
    {
      tile_left(tile),
      tile_top(tile)
    }, {
      FIXED_POINT(tile_type(tile)->sprite.width , 0),
      FIXED_POINT(tile_type(tile)->sprite.height, 0)
    }
  };
  return r;
}

#endif
