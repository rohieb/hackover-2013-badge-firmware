#ifndef INCLUDED_JUMPNRUN_ITEMS_H
#define INCLUDED_JUMPNRUN_ITEMS_H

#include <badge/ui/sprite.h>
#include <badge/util/rectangle.h>

struct jumpnrun_game_state;

typedef struct jumpnrun_item_type {
  badge_sprite sprite;
  void (*on_collect)(struct jumpnrun_game_state *state);
} jumpnrun_item_type;

typedef struct jumpnrun_item {
  vec2d pos;
  jumpnrun_item_type const *type;
} jumpnrun_item;

static inline rectangle rect_from_item(jumpnrun_item const *item) {
  rectangle r = { item->pos, { FIXED_POINT(item->type->sprite.width, 0), FIXED_POINT(item->type->sprite.height, 0) } };
  return r;
}

/************************************/

enum {
  JUMPNRUN_ITEM_TYPE_DOCUMENT,

  JUMPNRUN_ITEM_TYPE_COUNT
};

extern jumpnrun_item_type const jumpnrun_item_type_data[JUMPNRUN_ITEM_TYPE_COUNT];

#endif
