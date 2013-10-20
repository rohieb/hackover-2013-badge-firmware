#ifndef INCLUDED_BADGE_JUMPNRUN_RENDER_H
#define INCLUDED_BADGE_JUMPNRUN_RENDER_H

#include "enemies.h"
#include "game_state.h"
#include "items.h"
#include "player.h"
#include "shots.h"
#include "tiles.h"

#include "../ui/display.h"
#include "../ui/sprite.h"
#include "../util/util.h"

void jumpnrun_render_moveable     (badge_framebuffer *fb, jumpnrun_game_state const *state, jumpnrun_moveable const *moveable, badge_sprite const *animation, vec2d sprite_offset);
void jumpnrun_render_player       (badge_framebuffer *fb, jumpnrun_game_state const *state);
void jumpnrun_render_shot         (badge_framebuffer *fb, jumpnrun_game_state const *state, jumpnrun_shot           *shot );
void jumpnrun_render_enemy        (badge_framebuffer *fb, jumpnrun_game_state const *state, jumpnrun_enemy    const *enemy);
void jumpnrun_render_tile         (badge_framebuffer *fb, jumpnrun_game_state const *state, jumpnrun_tile     const *tile );
void jumpnrun_render_item         (badge_framebuffer *fb, jumpnrun_game_state const *state, jumpnrun_item     const *item );

void jumpnrun_render_splosion     (badge_framebuffer *fb, jumpnrun_game_state const *state, jumpnrun_moveable const *moveable);

void jumpnrun_render_player_symbol(badge_framebuffer *fb, int8_t x, int8_t y);
void jumpnrun_render_key_symbol   (badge_framebuffer *fb, int8_t x, int8_t y);

enum {
  JUMPNRUN_SPLOSION_FRAMES          =  4,
  JUMPNRUN_SPLOSION_TICKS_PER_FRAME = 15
};

#endif
