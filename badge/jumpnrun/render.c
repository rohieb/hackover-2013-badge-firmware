#include "render.h"

static badge_sprite const anim_sickle[JUMPNRUN_SHOT_FRAMES] = {
  { 3, 3, (uint8_t const *) "\xab\x01" },
  { 3, 3, (uint8_t const *) "\xee" }
};

static badge_sprite const anim_player[JUMPNRUN_PLAYER_FRAMES ] = {
  { 5, 8, (uint8_t const *) "\x1c\xff\xfd\x04\x04" },
  { 5, 8, (uint8_t const *) "\x1c\xff\x3d\xc4\x04" },
  { 5, 8, (uint8_t const *) "\xdc\x3f\x1d\x24\xc4" },
  { 5, 8, (uint8_t const *) "\x1c\xff\x3d\xc4\x04" }
};

vec2d jumpnrun_player_extents(void) { return (vec2d) { FIXED_INT_I(5), FIXED_INT_I(8) }; }

void jumpnrun_render_moveable     (badge_framebuffer *fb, jumpnrun_game_state const *state, jumpnrun_moveable const *moveable, badge_sprite const *animation, vec2d sprite_offset) {
  vec2d render_pos = vec2d_sub(moveable->hitbox.pos, sprite_offset);
  badge_framebuffer_blt(fb,
                        fixed_point_cast_int(render_pos.x) - jumpnrun_screen_left(state),
                        fixed_point_cast_int(render_pos.y),
                        &animation[moveable->anim_frame],
                        (moveable->flags & JUMPNRUN_MOVEABLE_MIRRORED) ? BADGE_BLT_MIRRORED : 0);
}

void jumpnrun_render_player       (badge_framebuffer *fb, jumpnrun_game_state const *state) {
  jumpnrun_render_moveable(fb, state, &state->player.base, anim_player, (vec2d) { FIXED_INT(0), FIXED_INT(0) });
}

void jumpnrun_render_shot         (badge_framebuffer *fb, jumpnrun_game_state const *state, jumpnrun_shot           *shot    ) {
  /* show every position twice, because LCD switching time. This makes the shots more
   * visible on the nokia lcds.
   */
  badge_framebuffer_blt(fb,
                        fixed_point_cast_int(shot->old_box.pos.x) - jumpnrun_screen_left(state),
                        fixed_point_cast_int(shot->old_box.pos.y),
                        &anim_sickle[shot->tick / JUMPNRUN_SHOT_TICKS_PER_FRAME],
                        fixed_point_lt(shot->inertia.x, FIXED_INT(0)) ? BADGE_BLT_MIRRORED : 0);
  badge_framebuffer_blt(fb,
                        fixed_point_cast_int(shot->current_box.pos.x) - jumpnrun_screen_left(state),
                        fixed_point_cast_int(shot->current_box.pos.y),
                        &anim_sickle[shot->tick / JUMPNRUN_SHOT_TICKS_PER_FRAME],
                        fixed_point_lt(shot->inertia.x, FIXED_INT(0)) ? BADGE_BLT_MIRRORED : 0);

  shot->old_box = shot->current_box;
}

void jumpnrun_render_enemy        (badge_framebuffer *fb, jumpnrun_game_state const *state, jumpnrun_enemy    const *enemy   ) {
  jumpnrun_render_moveable(fb, state, &enemy->base, enemy->type->animation_frames, enemy->type->hitbox.pos);
}

void jumpnrun_render_tile         (badge_framebuffer *fb, jumpnrun_game_state const *state, jumpnrun_tile     const *tile    ) {
  badge_framebuffer_blt(fb,
                        tile->pos.x * JUMPNRUN_TILE_PIXEL_WIDTH - jumpnrun_screen_left(state),
                        tile->pos.y * JUMPNRUN_TILE_PIXEL_HEIGHT,
                        &tile_type(tile)->sprite,
                        0);
}

void jumpnrun_render_item         (badge_framebuffer *fb, jumpnrun_game_state const *state, jumpnrun_item     const *item    ) {
  badge_framebuffer_blt(fb,
                        fixed_point_cast_int(item->pos.x) - jumpnrun_screen_left(state),
                        fixed_point_cast_int(item->pos.y),
                        &item->type->sprite,
                        0);
}

void jumpnrun_render_player_symbol(badge_framebuffer *fb, int8_t x, int8_t y) {
  badge_framebuffer_blt(fb, x, y, &anim_player[0], 0);
}

void jumpnrun_render_key_symbol   (badge_framebuffer *fb, int8_t x, int8_t y) {
  badge_framebuffer_blt(fb, x, y, &jumpnrun_item_type_data[JUMPNRUN_ITEM_TYPE_KEY].sprite, 0);
}
