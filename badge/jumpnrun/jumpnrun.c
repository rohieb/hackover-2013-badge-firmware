#include "jumpnrun.h"
#include "collision.h"

#include <badge/ui/display.h>
#include <badge/ui/event.h>
#include <badge/ui/sprite.h>

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof*(arr))

static vec2d       const gravity      = { FIXED_POINT_I(0,   0), FIXED_POINT_I(0,  56) };
static vec2d       const move_max     = { FIXED_POINT_I(1, 200), FIXED_POINT_I(1, 300) };
static fixed_point const accel_horiz  =   FIXED_POINT_I(0, 100);
static fixed_point const accel_vert   =   FIXED_POINT_I(0, 250);
static fixed_point const drag_factor  =   FIXED_POINT_I(0, 854);
static fixed_point const speed_jump_x =   FIXED_POINT_I(1, 200);

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
static inline fixed_point hacker_right (vec2d const *pos, jumpnrun_game_state const *state) { return fixed_point_add(hacker_left(pos, state), FIXED_POINT(anim_hacker[state->anim_frame].width , 0)); }
static inline fixed_point hacker_bottom(vec2d const *pos, jumpnrun_game_state const *state) { return fixed_point_add(hacker_top (pos, state), FIXED_POINT(anim_hacker[state->anim_frame].height, 0)); }

static inline rectangle hacker_rect(vec2d const *pos,
                                    jumpnrun_game_state const *state) {
  return (rectangle) { { hacker_left(pos, state), hacker_top(pos, state) }, { FIXED_POINT(anim_hacker[state->anim_frame].width, 0), FIXED_POINT(anim_hacker[state->anim_frame].height, 0) } };
}

rectangle hacker_rect_current(jumpnrun_game_state const *state) {
  return hacker_rect(&state->current_pos, state);
}

int jumpnrun_level_assert_left_side(jumpnrun_game_state const *state) {
  static int const lmargin = 20;
  static int const rmargin = 50;

  int pos_cur = fixed_point_cast_int(state->current_pos.x);
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

    if(fixed_point_lt(tile_right(&lv->tiles[mid]), FIXED_POINT(state->left - JUMPNRUN_MAX_SPAWN_MARGIN, 0))) {
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
                                    jumpnrun_game_state       *state) {
  switch(badge_event_current_input_state() &
         (BADGE_EVENT_KEY_LEFT |
          BADGE_EVENT_KEY_RIGHT)) {
  case BADGE_EVENT_KEY_LEFT:
    //    state->inertia.x = state->touching_ground ? fixed_point_sub(state->inertia.x, accel_horiz) : fixed_point_neg(speed_jump_x);
    state->inertia.x = fixed_point_sub(state->inertia.x, accel_horiz);
    state->anim_direction = BADGE_BLT_MIRRORED;
    break;
  case BADGE_EVENT_KEY_RIGHT:
    //    state->inertia.x = state->touching_ground ? fixed_point_add(state->inertia.x, accel_horiz) : speed_jump_x;
    state->inertia.x = fixed_point_add(state->inertia.x, accel_horiz);
    state->anim_direction = 0;
    break;
  default:
    if(state->touching_ground) {
      state->inertia.x = fixed_point_mul(state->inertia.x, drag_factor);
    } //else {
      //state->inertia.x = FIXED_POINT(0, 0);
    //}

    break;
  }

  if(state->jumpable_frames == 0) {
    // intentionally left blank.
  } else if(badge_event_current_input_state() & BADGE_EVENT_KEY_UP) {
    state->inertia.y = fixed_point_sub(state->inertia.y, accel_vert);
    // fixed_point_neg(move_max.y)
    --state->jumpable_frames;
  } else {
    state->jumpable_frames = 0;
  }

  jumpnrun_passive_movement(&state->inertia);

  state->inertia.x = fixed_point_min(fixed_point_max(fixed_point_neg(move_max.x), state->inertia.x), move_max.x);
  state->inertia.y = fixed_point_min(fixed_point_max(fixed_point_neg(move_max.y), state->inertia.y), move_max.y);

  vec2d new_pos = vec2d_add(state->current_pos, state->inertia);

  if(fixed_point_lt(new_pos.x, FIXED_POINT(state->left, 0))) {
    new_pos.x = FIXED_POINT(state->left, 0);
    state->inertia.x = FIXED_POINT(0, 0);
  }

  rectangle hacker_rect_c = hacker_rect(&state->current_pos, state);
  collisions_tiles_displace(&new_pos, &hacker_rect_c, lv, tilerange, &state->inertia, &state->touching_ground);

  state->current_pos = new_pos;

  if(fixed_point_gt(state->current_pos.y, FIXED_POINT(BADGE_DISPLAY_HEIGHT, 0))) {
    state->status = JUMPNRUN_DEAD;
  }
}

void jumpnrun_level_tick(jumpnrun_level      *lv,
                         jumpnrun_game_state *state)
{
  jumpnrun_tile_range tilerange = jumpnrun_visible_tiles(lv, state);
  jumpnrun_apply_movement(lv, &tilerange, state);

  state->left = jumpnrun_level_assert_left_side(state);

  if(state->tick_minor == 0) {
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
        rectangle hack_rect = hacker_rect(&state->current_pos, state);
        rectangle item_rect = rect_from_item(&lv->items[item]);

        if(rectangle_intersect(&hack_rect, &item_rect)) {
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
      jumpnrun_process_enemy(enemy, &fb, state, lv, &tilerange);
    }

    badge_framebuffer_blt(&fb,
                          fixed_point_cast_int(state->current_pos.x) - state->left,
                          fixed_point_cast_int(state->current_pos.y),
                          &anim_hacker[state->anim_frame],
                          state->anim_direction);

    badge_framebuffer_flush(&fb);

    if(!state->touching_ground) {
      state->anim_frame = 2;
    } else if(fixed_point_gt(fixed_point_abs(state->inertia.x), FIXED_POINT(0, 200))) {
      state->anim_frame = (state->anim_frame + 1) % ARRAY_SIZE(anim_hacker);
    } else {
      state->anim_frame = 0;
    }
  }

  state->tick_minor = (state->tick_minor + 1) % 2;
}

uint8_t jumpnrun_play(char const *lvname) {
  jumpnrun_level lv;

  memset(&lv, 0, sizeof(lv));

  // This part looks ugly. The reason it's done this way is that we don't know how much memory
  // we need for the level before parsing its header, and that the VLAs we use to store it have
  // block scope.
  // Still, better than opening the whole dynamic memory can of worms.
  FIL fd;
  int err;

  if(FR_OK != f_open(&fd, lvname, FA_OPEN_EXISTING | FA_READ)) { return JUMPNRUN_ERROR; }

  if(0 != jumpnrun_load_level_header_from_file(&lv, &fd)) {
    f_close(&fd);
    return JUMPNRUN_ERROR;
  }

  JUMPNRUN_LEVEL_MAKE_SPACE(lv);
  err = jumpnrun_load_level_from_file(&lv, &fd);

  f_close(&fd);
  if(err != 0) {
    return JUMPNRUN_ERROR;
  }

  jumpnrun_game_state gs;
  memset(&gs, 0, sizeof(gs));

  gs.current_pos = lv.start_pos;

  while(gs.status == JUMPNRUN_PLAYING) {
    badge_event_t ev = badge_event_wait();

    switch(badge_event_type(ev)) {
    case BADGE_EVENT_USER_INPUT:
      {
        uint8_t old_state = badge_event_old_input_state(ev);
        uint8_t new_state = badge_event_new_input_state(ev);
        uint8_t new_buttons = new_state & (old_state ^ new_state);

        if((new_buttons & BADGE_EVENT_KEY_UP) && gs.touching_ground) {
          gs.jumpable_frames = 8;
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
