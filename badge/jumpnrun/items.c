#include "items.h"
#include "jumpnrun.h"

static void on_collect_win(jumpnrun_game_state *state) {
  state->status = JUMPNRUN_WON;
}

jumpnrun_item_type const jumpnrun_item_type_data[JUMPNRUN_ITEM_TYPE_COUNT] = {
  { { 6, 7, (uint8_t const *) "\x7c\x61\xb5\x1a\xfc\x03" }, on_collect_win }
};
