#include "jumpnrun.h"
#include "collision.h"
#include "levels.h"

#include "../ui/display.h"
#include "../ui/event.h"
#include "../ui/sprite.h"
#include "../util/util.h"

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>

static vec2d       const gravity      = { FIXED_POINT_I(0,   0), FIXED_POINT_I(0,  56) };
static vec2d       const move_max     = { FIXED_POINT_I(0, 600), FIXED_POINT_I(1, 300) };
static fixed_point const accel_horiz  =   FIXED_POINT_I(0,  50);
static fixed_point const accel_vert   =   FIXED_POINT_I(0, 167);
static fixed_point const drag_factor  =   FIXED_POINT_I(0, 854);
static fixed_point const speed_jump_x =   FIXED_POINT_I(0, 600);

static vec2d const hacker_extent = { FIXED_INT_I(5), FIXED_INT_I(8) };

static badge_sprite const anim_hacker[] = {
  { 5, 8, (uint8_t const *) "\x1c\xff\xfd\x04\x04" },
  { 5, 8, (uint8_t const *) "\x1c\xff\x3d\xc4\x04" },
  { 5, 8, (uint8_t const *) "\xdc\x3f\x1d\x24\xc4" },
  { 5, 8, (uint8_t const *) "\x1c\xff\x3d\xc4\x04" }

/*
  { 5, 8, (uint8_t const *) "\x46\xfc\x73\x8c\x31" },
  { 5, 8, (uint8_t const *) "\x46\xfc\x73\x8c\x52" },
  { 5, 8, (uint8_t const *) "\x46\xfc\x73\x94\x8c" },
  { 5, 8, (uint8_t const *) "\x46\xfc\x73\x8c\x52" }
*/
  /*
    { 6, 8, (uint8_t const *) "\x0c\xe1\x3b\x0e\xc3\x30" },
    { 6, 8, (uint8_t const *) "\x0c\xe1\x3b\x0e\x43\x51" },
    { 6, 8, (uint8_t const *) "\x0c\xe1\x3b\x0e\x35\x82" },
    { 6, 8, (uint8_t const *) "\x0c\xe1\x3b\x0e\x43\x51" }
  */
  /*
    { 6, 8, (uint8_t const *) "\xff\xff\xff\xff\xff\xff" },
    { 6, 8, (uint8_t const *) "\xff\xff\xff\xff\xff\xff" },
    { 6, 8, (uint8_t const *) "\xff\xff\xff\xff\xff\xff" },
    { 6, 8, (uint8_t const *) "\xff\xff\xff\xff\xff\xff" }
  */
};


static inline int imax(int x, int y) {
  return x < y ? y : x;
}

static inline fixed_point hacker_left  (vec2d const *pos, jumpnrun_game_state const *state) { (void) state; return pos->x; }
static inline fixed_point hacker_top   (vec2d const *pos, jumpnrun_game_state const *state) { (void) state; return pos->y; }
static inline fixed_point hacker_right (vec2d const *pos, jumpnrun_game_state const *state) { return fixed_point_add(hacker_left(pos, state), hacker_extent.x); }
static inline fixed_point hacker_bottom(vec2d const *pos, jumpnrun_game_state const *state) { return fixed_point_add(hacker_top (pos, state), hacker_extent.y); }

int jumpnrun_level_assert_left_side(jumpnrun_game_state const *state) {
  static int const lmargin = 20;
  static int const rmargin = 50;

  int pos_cur = fixed_point_cast_int(state->player.current_box.pos.x);
  int pos_rel = pos_cur - state->left;

  if(pos_rel < lmargin) {
    return imax(0, pos_cur - lmargin);
  } else if(pos_rel > BADGE_DISPLAY_WIDTH - rmargin) {
    return pos_cur - (BADGE_DISPLAY_WIDTH - rmargin);
  }

  return state->left;
}

static int jumpnrun_bsearch_tile(jumpnrun_level const *lv, jumpnrun_game_state const *state) {
  int front = 0;
  int len   = lv->header.tile_count;

  while(len > 0) {
    int mid = front + len / 2;

    if(fixed_point_lt(tile_right(&lv->tiles[mid]), FIXED_INT(state->left - JUMPNRUN_MAX_SPAWN_MARGIN))) {
      front = mid + 1;
      len -= len / 2 + 1;
    } else {
      len /= 2;
    }
  }

  return front;
}

jumpnrun_tile_range jumpnrun_visible_tiles(jumpnrun_level const *lv,
                                           jumpnrun_game_state const *state) {
  jumpnrun_tile_range r;

  r.first = jumpnrun_bsearch_tile(lv, state);

  for(r.last = r.first;
      r.last < lv->header.tile_count && lv->tiles[r.last].pos.x * JUMPNRUN_TILE_PIXEL_WIDTH < state->left + BADGE_DISPLAY_WIDTH + JUMPNRUN_MAX_SPAWN_MARGIN;
      ++r.last)
    ;

  return r;
}

void jumpnrun_passive_movement(vec2d *inertia)
{
  *inertia = vec2d_add(*inertia, gravity);

  inertia->x = fixed_point_min(fixed_point_max(fixed_point_neg(move_max.x), inertia->x), move_max.x);
  inertia->y = fixed_point_min(fixed_point_max(fixed_point_neg(move_max.y), inertia->y), move_max.y);
}

static void jumpnrun_apply_movement(jumpnrun_level      const *lv,
                                    jumpnrun_tile_range const *tilerange,
                                    jumpnrun_game_state       *state,
				    vec2d                     *inertia_mod) {
  switch(badge_event_current_input_state() &
         (BADGE_EVENT_KEY_LEFT |
          BADGE_EVENT_KEY_RIGHT)) {
  case BADGE_EVENT_KEY_LEFT:
    //    state->player.inertia.x = state->player.touching_ground ? fixed_point_sub(state->player.inertia.x, accel_horiz) : fixed_point_neg(speed_jump_x);
    state->player.inertia.x = fixed_point_sub(state->player.inertia.x, accel_horiz);
    state->anim_direction = BADGE_BLT_MIRRORED;
    break;
  case BADGE_EVENT_KEY_RIGHT:
    //    state->player.inertia.x = state->player.touching_ground ? fixed_point_add(state->player.inertia.x, accel_horiz) : speed_jump_x;
    state->player.inertia.x = fixed_point_add(state->player.inertia.x, accel_horiz);
    state->anim_direction = 0;
    break;
  default:
    if(state->player.touching_ground) {
      state->player.inertia.x = fixed_point_mul(state->player.inertia.x, drag_factor);
    } //else {
      //state->player.inertia.x = FIXED_INT(0);
    //}

    break;
  }

  if(state->player.jumpable_frames == 0) {
    // intentionally left blank.
  } else if(badge_event_current_input_state() & BADGE_EVENT_KEY_BTN_A) {
    state->player.inertia.y = fixed_point_sub(state->player.inertia.y, accel_vert);
    --state->player.jumpable_frames;
  } else {
    state->player.jumpable_frames = 0;
  }

  jumpnrun_passive_movement(&state->player.inertia);

  vec2d new_pos = vec2d_add(state->player.current_box.pos, state->player.inertia);

  if(fixed_point_lt(new_pos.x, FIXED_INT(state->left))) {
    new_pos.x = FIXED_INT(state->left);
    state->player.inertia.x = FIXED_INT(0);
  }

  *inertia_mod = state->player.inertia;
  collisions_tiles_displace(&new_pos, &state->player, lv, tilerange, inertia_mod);
  state->player.inertia = *inertia_mod;

  if(fixed_point_gt(state->player.current_box.pos.y, FIXED_INT(BADGE_DISPLAY_HEIGHT))) {
    state->status = JUMPNRUN_DEAD;
  }
}

void jumpnrun_level_tick(jumpnrun_level      *lv,
                         jumpnrun_game_state *state)
{
  jumpnrun_tile_range tilerange = jumpnrun_visible_tiles(lv, state);
  vec2d inertia_mod = state->player.inertia;

  jumpnrun_apply_movement(lv, &tilerange, state, &inertia_mod);
  state->left = jumpnrun_level_assert_left_side(state);

  if(state->player.tick_minor == 0) {
    badge_framebuffer fb;
    badge_framebuffer_clear(&fb);

    for(size_t tile = tilerange.first; tile < tilerange.last; ++tile) {
      badge_framebuffer_blt(&fb,
                            fixed_point_cast_int(tile_left(&lv->tiles[tile])) - state->left,
                            fixed_point_cast_int(tile_top (&lv->tiles[tile])),
                            &tile_type(&lv->tiles[tile])->sprite,
                            0);
    }

    for(size_t item = 0; item < lv->header.item_count; ++item) {
      int screenpos = fixed_point_cast_int(lv->items[item].pos.x) - state->left;
      if(screenpos > -lv->items[item].type->sprite.width &&
         screenpos < BADGE_DISPLAY_WIDTH) {
        rectangle item_rect = rect_from_item(&lv->items[item]);

        if(rectangle_intersect(&state->player.current_box, &item_rect)) {
          lv->items[item].type->on_collect(state);
        }

        badge_framebuffer_blt(&fb,
                              screenpos,
                              fixed_point_cast_int(lv->items[item].pos.y),
                              &lv->items[item].type->sprite,
                              0);
      }
    }

    for(size_t enemy_ix = 0; enemy_ix < lv->header.enemy_count; ++enemy_ix) {
      jumpnrun_enemy *enemy = &lv->enemies[enemy_ix];
      jumpnrun_process_enemy(enemy, &fb, state, lv, &tilerange, &inertia_mod);
    }

    badge_framebuffer_blt(&fb,
                          fixed_point_cast_int(state->player.current_box.pos.x) - state->left,
                          fixed_point_cast_int(state->player.current_box.pos.y),
                          &anim_hacker[state->player.anim_frame],
                          state->anim_direction);

    badge_framebuffer_flush(&fb);

    if(!state->player.touching_ground) {
      state->player.anim_frame = 2;
    } else if(fixed_point_gt(fixed_point_abs(state->player.inertia.x), FIXED_POINT(0, 200))) {
      state->player.anim_frame = (state->player.anim_frame + 1) % ARRAY_SIZE(anim_hacker);
    } else {
      state->player.anim_frame = 0;
    }
  } else {
    for(size_t enemy_ix = 0; enemy_ix < lv->header.enemy_count; ++enemy_ix) {
      jumpnrun_enemy *enemy = &lv->enemies[enemy_ix];
      jumpnrun_process_enemy(enemy, NULL, state, lv, &tilerange, &inertia_mod);
    }
  }

  state->player.inertia = inertia_mod;
  ++state->player.tick_minor;
  if(state->player.tick_minor == 3) {
    state->player.tick_minor = 0;
  }
}

uint8_t jumpnrun_play(char const *lvname) {
  jumpnrun_level lv;

  JUMPNRUN_LEVEL_LOAD(lv, lvname);

  jumpnrun_game_state gs;
  memset(&gs, 0, sizeof(gs));

  gs.player.current_box = rectangle_new(lv.start_pos, hacker_extent);

  while(gs.status == JUMPNRUN_PLAYING) {
    badge_event_t ev = badge_event_wait();

    switch(badge_event_type(ev)) {
    case BADGE_EVENT_USER_INPUT:
      {
        uint8_t old_state = badge_event_old_input_state(ev);
        uint8_t new_state = badge_event_new_input_state(ev);
        uint8_t new_buttons = new_state & (old_state ^ new_state);

        if((new_buttons & BADGE_EVENT_KEY_BTN_A) && gs.player.touching_ground) {
	  gs.player.jumpable_frames = 12;
        }

        break;
      }
    case BADGE_EVENT_GAME_TICK:
      {
        jumpnrun_level_tick(&lv, &gs);
        break;
      }
    }
  }

  return gs.status;
}
