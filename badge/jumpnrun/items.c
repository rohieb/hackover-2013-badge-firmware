#include "items.h"
#include "jumpnrun.h"

static void on_collect_win(jumpnrun_item       *self,
                           jumpnrun_game_state *state,
                           jumpnrun_level      *lv) {
  (void) lv;
  self->flags |= JUMPNRUN_ITEM_COLLECTED;
  state->status = JUMPNRUN_WON;
}

static void on_collect_checkpoint(jumpnrun_item       *self,
                                  jumpnrun_game_state *state,
                                  jumpnrun_level      *lv) {
  (void) state;
  self->flags |= JUMPNRUN_ITEM_COLLECTED;
  lv->start_pos = (vec2d) { self->pos.x,
                            fixed_point_sub(fixed_point_add(self->pos.y, FIXED_INT(self->type->sprite.height)), hacker_extents().y)
  };
}

jumpnrun_item_type const jumpnrun_item_type_data[JUMPNRUN_ITEM_TYPE_COUNT] = {
  { { 6,  7, (uint8_t const *) "\x7c\x61\xb5\x1a\xfc\x03"                             }, on_collect_win },
  { { 9, 12, (uint8_t const *) "\xff\x1f\x04\x49\x10\x05\x49\x50\x04\x51\x10\x04\x7f" }, on_collect_checkpoint }
};
