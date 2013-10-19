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

vec2d hacker_extents(void) { return (vec2d) { FIXED_INT_I(5), FIXED_INT_I(8) }; }
static vec2d const shot_spawn_inertia = { FIXED_POINT_I(0, 800), FIXED_POINT_I(0, -800) };

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

static badge_sprite const anim_sickle[] = {
  { 3, 3, (uint8_t const *) "\xab\x01" },
  { 3, 3, (uint8_t const *) "\xee\x00" }
/*
  { 3, 3, (uint8_t const *) "\x8a\x01" },
  { 3, 3, (uint8_t const *) "\x6a" },
  { 3, 3, (uint8_t const *) "\xa3" },
  { 3, 3, (uint8_t const *) "\xac" }
*/
};

enum {
  JUMPNRUN_SHOT_EXTENT = 3,
  JUMPNRUN_SHOT_TICKS_PER_FRAME = 36
};

static void jumpnrun_shot_spawn(jumpnrun_shot *shot, jumpnrun_game_state const *state) {
  shot->tick        = 0;
  shot->inertia     = shot_spawn_inertia;

  if(state->player.anim_direction == BADGE_BLT_MIRRORED) {
    shot->current_box = rectangle_new((vec2d) { fixed_point_sub(rectangle_left(&state->player.current_box), FIXED_INT(JUMPNRUN_SHOT_EXTENT)), rectangle_top(&state->player.current_box) },
                                      (vec2d) { FIXED_INT(JUMPNRUN_SHOT_EXTENT), FIXED_INT(JUMPNRUN_SHOT_EXTENT) });
    shot->inertia.x   = fixed_point_neg(shot->inertia.x);
  } else {
    shot->current_box = rectangle_new((vec2d) { rectangle_right(&state->player.current_box), rectangle_top(&state->player.current_box) },
                                      (vec2d) { FIXED_INT(JUMPNRUN_SHOT_EXTENT), FIXED_INT(JUMPNRUN_SHOT_EXTENT) });
  }

  shot->old_box = shot->current_box;
  shot->inertia = vec2d_add(shot->inertia, state->player.inertia);
}

static inline int imax(int x, int y) {
  return x < y ? y : x;
}

static inline fixed_point hacker_left  (vec2d const *pos, jumpnrun_game_state const *state) { (void) state; return pos->x; }
static inline fixed_point hacker_top   (vec2d const *pos, jumpnrun_game_state const *state) { (void) state; return pos->y; }
static inline fixed_point hacker_right (vec2d const *pos, jumpnrun_game_state const *state) { return fixed_point_add(hacker_left(pos, state), hacker_extents().x); }
static inline fixed_point hacker_bottom(vec2d const *pos, jumpnrun_game_state const *state) { return fixed_point_add(hacker_top (pos, state), hacker_extents().y); }

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
    state->player.anim_direction = BADGE_BLT_MIRRORED;
    break;
  case BADGE_EVENT_KEY_RIGHT:
    //    state->player.inertia.x = state->player.touching_ground ? fixed_point_add(state->player.inertia.x, accel_horiz) : speed_jump_x;
    state->player.inertia.x = fixed_point_add(state->player.inertia.x, accel_horiz);
    state->player.anim_direction = 0;
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
  bool killed = collisions_tiles_displace(&new_pos, &state->player, lv, tilerange, inertia_mod);
  state->player.inertia = *inertia_mod;

  if(killed || fixed_point_gt(state->player.current_box.pos.y, FIXED_INT(BADGE_DISPLAY_HEIGHT))) {
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
      jumpnrun_item *item_obj = &lv->items[item];

      if(item_obj->flags & JUMPNRUN_ITEM_COLLECTED) {
        continue;
      }

      int screenpos = fixed_point_cast_int(item_obj->pos.x) - state->left;
      if(screenpos > -item_obj->type->sprite.width &&
         screenpos < BADGE_DISPLAY_WIDTH) {
        rectangle item_rect = rect_from_item(item_obj);

        if(rectangle_intersect(&state->player.current_box, &item_rect)) {
          item_obj->type->on_collect(item_obj, state, lv);
        }

        badge_framebuffer_blt(&fb,
                              screenpos,
                              fixed_point_cast_int(item_obj->pos.y),
                              &item_obj->type->sprite,
                              0);
      }
    }

    for(size_t shot_ix = 0; shot_ix < JUMPNRUN_MAX_SHOTS; ++shot_ix) {
      jumpnrun_shot *shot = &state->shots[shot_ix];

      if(jumpnrun_shot_spawned(shot)) {
        rectangle_move_rel(&shot->current_box, shot->inertia);
        shot->inertia = vec2d_add(shot->inertia, gravity);

        if(fixed_point_gt(rectangle_top(&shot->current_box), FIXED_INT(BADGE_DISPLAY_HEIGHT))) {
          jumpnrun_shot_despawn(shot);
        }

        /* show every position twice, because LCD switching time. This makes the shots more
         * visible on the nokia lcds.
         */
        badge_framebuffer_blt(&fb,
                              fixed_point_cast_int(shot->old_box.pos.x) - state->left,
                              fixed_point_cast_int(shot->old_box.pos.y),
                              &anim_sickle[shot->tick / JUMPNRUN_SHOT_TICKS_PER_FRAME],
                              fixed_point_lt(shot->inertia.x, FIXED_INT(0)) ? BADGE_BLT_MIRRORED : 0);
        badge_framebuffer_blt(&fb,
                              fixed_point_cast_int(shot->current_box.pos.x) - state->left,
                              fixed_point_cast_int(shot->current_box.pos.y),
                              &anim_sickle[shot->tick / JUMPNRUN_SHOT_TICKS_PER_FRAME],
                              fixed_point_lt(shot->inertia.x, FIXED_INT(0)) ? BADGE_BLT_MIRRORED : 0);

        shot->old_box = shot->current_box;

        ++shot->tick;
        if(shot->tick == ARRAY_SIZE(anim_sickle) * JUMPNRUN_SHOT_TICKS_PER_FRAME) {
          shot->tick = 0;
        }
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
                          state->player.anim_direction);

    badge_framebuffer_flush(&fb);

    if(!state->player.touching_ground) {
      state->player.anim_frame = 2;
    } else if(fixed_point_gt(fixed_point_abs(state->player.inertia.x), FIXED_POINT(0, 200))) {
      state->player.anim_frame = (state->player.anim_frame + 1) % ARRAY_SIZE(anim_hacker);
    } else {
      state->player.anim_frame = 0;
    }
  } else {
    for(size_t shot_ix = 0; shot_ix < JUMPNRUN_MAX_SHOTS; ++shot_ix) {
      jumpnrun_shot *shot = &state->shots[shot_ix];

      if(jumpnrun_shot_spawned(shot)) {
        rectangle_move_rel(&shot->current_box, shot->inertia);
        shot->inertia = vec2d_add(shot->inertia, gravity);

        if(fixed_point_gt(rectangle_top(&shot->current_box), FIXED_INT(BADGE_DISPLAY_HEIGHT))) {
          jumpnrun_shot_despawn(shot);
        }

        ++shot->tick;
        if(shot->tick == ARRAY_SIZE(anim_sickle) * JUMPNRUN_SHOT_TICKS_PER_FRAME) {
          shot->tick = 0;
        }
      }
    }

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

  for(gs.lives = 3; gs.lives != 0; --gs.lives) {
    gs.status = JUMPNRUN_PLAYING;
    gs.player.current_box = rectangle_new(lv.start_pos,
                                          hacker_extents());

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

        if((new_buttons & BADGE_EVENT_KEY_BTN_B)) {
          uint8_t i;
          for(i = 0; i < JUMPNRUN_MAX_SHOTS && jumpnrun_shot_spawned(&gs.shots[i]); ++i)
            ;

          if(i < JUMPNRUN_MAX_SHOTS) {
            jumpnrun_shot_spawn(gs.shots + i, &gs);
          }
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
  }

  return gs.status;
}
