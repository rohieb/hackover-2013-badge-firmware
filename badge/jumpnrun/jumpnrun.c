#include "game_state.h"
#include "jumpnrun.h"
#include "collision.h"
#include "levels.h"
#include "render.h"
#include "stats.h"

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

static inline int imax(int x, int y) {
  return x < y ? y : x;
}

static inline fixed_point hacker_left  (vec2d const *pos, jumpnrun_game_state const *state) { (void) state; return pos->x; }
static inline fixed_point hacker_top   (vec2d const *pos, jumpnrun_game_state const *state) { (void) state; return pos->y; }
static inline fixed_point hacker_right (vec2d const *pos, jumpnrun_game_state const *state) { return fixed_point_add(hacker_left(pos, state), jumpnrun_player_extents().x); }
static inline fixed_point hacker_bottom(vec2d const *pos, jumpnrun_game_state const *state) { return fixed_point_add(hacker_top (pos, state), jumpnrun_player_extents().y); }

int jumpnrun_level_assert_left_side(jumpnrun_game_state const *state) {
  static int const lmargin = 20;
  static int const rmargin = 50;

  int pos_cur = fixed_point_cast_int(state->player.base.hitbox.pos.x);
  int pos_rel = pos_cur - state->screen_left;

  if(pos_rel < lmargin) {
    return imax(0, pos_cur - lmargin);
  } else if(pos_rel > BADGE_DISPLAY_WIDTH - rmargin) {
    return pos_cur - (BADGE_DISPLAY_WIDTH - rmargin);
  }

  return state->screen_left;
}

static int jumpnrun_bsearch_tile(jumpnrun_level const *lv, jumpnrun_game_state const *state) {
  int front = 0;
  int len   = lv->header.tile_count;

  while(len > 0) {
    int mid = front + len / 2;

    if(fixed_point_lt(tile_right(&lv->tiles[mid]), FIXED_INT(state->screen_left - JUMPNRUN_MAX_SPAWN_MARGIN))) {
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
      r.last < lv->header.tile_count && lv->tiles[r.last].pos.x * JUMPNRUN_TILE_PIXEL_WIDTH < state->screen_left + BADGE_DISPLAY_WIDTH + JUMPNRUN_MAX_SPAWN_MARGIN;
      ++r.last)
    ;

  return r;
}

void jumpnrun_apply_gravity(vec2d *inertia) {
  *inertia = vec2d_add(*inertia, gravity);
}

void jumpnrun_passive_movement(vec2d *inertia)
{
  jumpnrun_apply_gravity(inertia);

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
    //    state->player.base.inertia.x = state->player.touching_ground ? fixed_point_sub(state->player.base.inertia.x, accel_horiz) : fixed_point_neg(speed_jump_x);
    state->player.base.inertia.x = fixed_point_sub(state->player.base.inertia.x, accel_horiz);
    state->player.base.flags |= JUMPNRUN_MOVEABLE_MIRRORED;
    break;
  case BADGE_EVENT_KEY_RIGHT:
    //    state->player.base.inertia.x = state->player.touching_ground ? fixed_point_add(state->player.base.inertia.x, accel_horiz) : speed_jump_x;
    state->player.base.inertia.x = fixed_point_add(state->player.base.inertia.x, accel_horiz);
    state->player.base.flags &= ~JUMPNRUN_MOVEABLE_MIRRORED;
    break;
  default:
    if(jumpnrun_moveable_touching_ground(&state->player.base)) {
      state->player.base.inertia.x = fixed_point_mul(state->player.base.inertia.x, drag_factor);
    } //else {
      //state->player.base.inertia.x = FIXED_INT(0);
    //}

    break;
  }

  if(state->player.base.jumpable_frames == 0) {
    // intentionally left blank.
  } else if(badge_event_current_input_state() & BADGE_EVENT_KEY_BTN_A) {
    state->player.base.inertia.y = fixed_point_sub(state->player.base.inertia.y, accel_vert);
    --state->player.base.jumpable_frames;
  } else {
    state->player.base.jumpable_frames = 0;
  }

  jumpnrun_passive_movement(&state->player.base.inertia);

  vec2d new_pos = vec2d_add(state->player.base.hitbox.pos, state->player.base.inertia);

  if(fixed_point_lt(new_pos.x, FIXED_INT(state->screen_left))) {
    new_pos.x = FIXED_INT(state->screen_left);
    state->player.base.inertia.x = FIXED_INT(0);
  }

  *inertia_mod = state->player.base.inertia;
  bool killed = collisions_tiles_displace(&new_pos, &state->player.base, lv, tilerange, inertia_mod);
  state->player.base.inertia = *inertia_mod;

  if(killed || fixed_point_gt(state->player.base.hitbox.pos.y, FIXED_INT(BADGE_DISPLAY_HEIGHT))) {
    state->player.base.flags |= JUMPNRUN_PLAYER_DEAD;
  }
}

void jumpnrun_level_tick(jumpnrun_level      *lv,
                         jumpnrun_game_state *state)
{
  jumpnrun_tile_range tilerange = jumpnrun_visible_tiles(lv, state);
  vec2d inertia_mod = state->player.base.inertia;

  jumpnrun_apply_movement(lv, &tilerange, state, &inertia_mod);
  state->screen_left = jumpnrun_level_assert_left_side(state);

  if(state->player.base.tick_minor == 0) {
    badge_framebuffer fb;
    badge_framebuffer_clear(&fb);

    for(size_t tile = tilerange.first; tile < tilerange.last; ++tile) {
      jumpnrun_render_tile(&fb, state, &lv->tiles[tile]);
    }

    for(size_t item = 0; item < lv->header.item_count; ++item) {
      jumpnrun_item *item_obj = &lv->items[item];

      if(item_obj->flags & JUMPNRUN_ITEM_COLLECTED) {
        continue;
      }

      int screenpos = fixed_point_cast_int(item_obj->pos.x) - state->screen_left;
      if(screenpos > -item_obj->type->sprite.width &&
         screenpos < BADGE_DISPLAY_WIDTH) {
        rectangle item_rect = rect_from_item(item_obj);

        if(rectangle_intersect(&state->player.base.hitbox, &item_rect)) {
          item_obj->type->on_collect(item_obj, state, lv);
        }

        jumpnrun_render_item(&fb, state, item_obj);
      }
    }

    for(size_t shot_ix = 0; shot_ix < JUMPNRUN_MAX_SHOTS; ++shot_ix) {
      jumpnrun_shot *shot = &state->shots[shot_ix];
      jumpnrun_shot_process(shot);
      if(jumpnrun_shot_spawned(shot)) {
        jumpnrun_render_shot(&fb, state, shot);
      }
    }

    for(size_t enemy_ix = 0; enemy_ix < lv->header.enemy_count; ++enemy_ix) {
      jumpnrun_enemy *enemy = &lv->enemies[enemy_ix];
      jumpnrun_process_enemy(enemy, &fb, state, lv, &tilerange, &inertia_mod);
    }

    jumpnrun_render_player(&fb, state);

    badge_framebuffer_flush(&fb);

    if(!jumpnrun_moveable_touching_ground(&state->player.base)) {
      state->player.base.anim_frame = 2;
    } else if(fixed_point_gt(fixed_point_abs(state->player.base.inertia.x), FIXED_POINT(0, 200))) {
      state->player.base.anim_frame = (state->player.base.anim_frame + 1) % JUMPNRUN_PLAYER_FRAMES;
    } else {
      state->player.base.anim_frame = 0;
    }
  } else {
    for(size_t shot_ix = 0; shot_ix < JUMPNRUN_MAX_SHOTS; ++shot_ix) {
      jumpnrun_shot_process(&state->shots[shot_ix]);
    }

    for(size_t enemy_ix = 0; enemy_ix < lv->header.enemy_count; ++enemy_ix) {
      jumpnrun_enemy *enemy = &lv->enemies[enemy_ix];
      jumpnrun_process_enemy(enemy, NULL, state, lv, &tilerange, &inertia_mod);
    }
  }

  state->player.base.inertia = inertia_mod;
  ++state->player.base.tick_minor;
  if(state->player.base.tick_minor == 3) {
    state->player.base.tick_minor = 0;
  }
}

uint8_t jumpnrun_play(char const *lvname) {
  jumpnrun_level lv;

  JUMPNRUN_LEVEL_LOAD(lv, lvname);

  jumpnrun_game_state gs;

  for(jumpnrun_game_state_init(&gs, &lv); (gs.flags & JUMPNRUN_STATE_WON) == 0 && gs.player.lives != 0; --gs.player.lives) {
    jumpnrun_show_lives_screen(&gs);
    jumpnrun_game_state_respawn(&gs, &lv);

    while((gs.player.base.flags & JUMPNRUN_PLAYER_DEAD) == 0 &&
          (gs.            flags & JUMPNRUN_STATE_WON  ) == 0) {
      badge_event_t ev = badge_event_wait();

      switch(badge_event_type(ev)) {
      case BADGE_EVENT_USER_INPUT:
      {
        uint8_t old_state = badge_event_old_input_state(ev);
        uint8_t new_state = badge_event_new_input_state(ev);
        uint8_t new_buttons = new_state & (old_state ^ new_state);

        if((new_buttons & BADGE_EVENT_KEY_BTN_A) && jumpnrun_moveable_touching_ground(&gs.player.base)) {
          gs.player.base.jumpable_frames = 12;
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

  if(gs.flags & JUMPNRUN_STATE_WON) { return JUMPNRUN_WON; }
  if(gs.player.lives == 0) return JUMPNRUN_LOST;
  return JUMPNRUN_ERROR;
}
