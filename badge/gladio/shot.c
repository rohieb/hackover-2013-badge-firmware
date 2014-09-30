#include "shot.h"

#include "game_state.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

badge_sprite const gladio_shot_sprite = { 3, 3, (uint8_t const *) "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" };

static fixed_point gladio_shot_size = FIXED_INT_I(3);

rectangle gladio_shot_rectangle(gladio_shot const *shot) {
  return gladio_object_rectangle(shot->base, vec2d_new(gladio_shot_size, gladio_shot_size));
}

uint8_t gladio_shot_active(gladio_shot const *shot) {
  return
    fixed_point_ne(FIXED_INT(0), shot->inertia.x) ||
    fixed_point_ne(FIXED_INT(0), shot->inertia.y);
}

static inline uint8_t posless(vec2d lhs, vec2d rhs) {
  return fixed_point_lt(lhs.x, rhs.x) || (fixed_point_eq(lhs.x, lhs.y) && fixed_point_lt(lhs.y, rhs.y));
}

void gladio_shot_despawn(gladio_shot *shot) {
  memset(shot, 0, sizeof(gladio_shot));
}

static uint8_t gladio_shot_onscreen(gladio_shot const *shot) {
  rectangle r = gladio_shot_rectangle(shot);
  return rectangle_onscreen(&r);
}

uint8_t gladio_shot_friendly_despawn_and_compress(struct gladio_game_state *state) {
  uint8_t pos_r = 0;

  // Skip initial active shots
  while(pos_r < GLADIO_MAX_SHOTS_FRIENDLY                   &&
        gladio_shot_active  (&state->shots_friendly[pos_r]) &&
        gladio_shot_onscreen(&state->shots_friendly[pos_r])) {
    ++pos_r;
  }

  uint8_t pos_w = pos_r;

  while(pos_r < GLADIO_MAX_SHOTS_FRIENDLY &&
        gladio_shot_active(&state->shots_friendly[pos_r])) {
    gladio_shot *shot_r = &state->shots_friendly[pos_r];
    gladio_shot *shot_w = &state->shots_friendly[pos_w];

    if(gladio_shot_onscreen(shot_r)) {
      *shot_w = *shot_r;
      ++pos_w;
    }

    ++pos_r;
  }

  uint8_t r = pos_w;

  while(pos_w != pos_r) {
    gladio_shot_despawn(&state->shots_friendly[pos_w]);
    ++pos_w;
  }

  return r;
}

static uint8_t gladio_shot_lower_bound(vec2d needle,
                                       gladio_shot const *haystack,
                                       uint8_t len) {
  uint8_t first = 0;

  while(len > 0) {
    uint8_t half = len / 2;
    if(posless(haystack[first + half].base.position, needle)) {
      first += half + 1;
      len   -= half + 1;
    } else {
      len = half;
    }
  }

  return first;
}

void gladio_shot_friendly_spawn(gladio_game_state *state,
                                uint8_t            where,
                                vec2d              position) {
  // lucky coincidence
  uint8_t spawncount = where;
  uint8_t shotcount = gladio_shot_friendly_despawn_and_compress(state);

  if(spawncount == 0) {
    return;
  }

  // If spawning multiple shots, spawn the one that will move upwards first so ordering is preserved.
  fixed_point speed_y  = (where & GLADIO_SHOT_FRIENDLY_SIDEGUN) ? FIXED_POINT(0, 250) : FIXED_INT  (0);
  fixed_point dspeed_y = (where & GLADIO_SHOT_FRIENDLY_FRONT  ) ? FIXED_POINT(0, 250) : FIXED_POINT(0, 500);

  uint8_t pos_insert = gladio_shot_lower_bound(position, state->shots_friendly, shotcount);
  uint8_t pos_shift  = pos_insert + spawncount;

  uint8_t shiftcount;

  if(shotcount + spawncount <= GLADIO_MAX_SHOTS_FRIENDLY) {
    shiftcount = shotcount - pos_insert;
  } else {
    shiftcount = GLADIO_MAX_SHOTS_FRIENDLY - pos_shift;
  }

  memmove(&state->shots_friendly[pos_shift],
          &state->shots_friendly[pos_insert],
          sizeof(gladio_shot) * shiftcount);

  for(uint8_t i = 0; i < spawncount; ++i) {
    gladio_shot *shot_w = &state->shots_friendly[pos_insert + i];

    shot_w->base.position = position;
    shot_w->base.anim_pos = 0;
    shot_w->type          = 0;
    shot_w->inertia       = vec2d_new(FIXED_INT(1), speed_y);

    speed_y = fixed_point_sub(speed_y, dspeed_y);
  }
}

void gladio_shot_spawn(struct gladio_game_state *state, uint8_t shot_type, vec2d position, vec2d movement) {
  gladio_shot *shots     = shot_type == GLADIO_SHOT_FRIENDLY ? state->shots_friendly     : state->shots_hostile;
  uint8_t      shots_max = shot_type == GLADIO_SHOT_FRIENDLY ? GLADIO_MAX_SHOTS_FRIENDLY : GLADIO_MAX_SHOTS_HOSTILE;

  position.y = fixed_point_sub(position.y, fixed_point_div(FIXED_INT(gladio_shot_sprite.height), FIXED_INT(2)));
  position.x = fixed_point_sub(position.x, fixed_point_div(FIXED_INT(gladio_shot_sprite.width ), FIXED_INT(2)));

  for(uint8_t i = 0; i < shots_max; ++i) {
    if(!gladio_shot_active(shots + i)) {
      shots[i].base.position = position;
      shots[i].base.anim_pos = 0;
      shots[i].type          = 0;
      shots[i].inertia       = movement;
      break;
    }
  }
}

void gladio_shot_tick(gladio_shot *shot) {
  if(gladio_shot_active(shot)) {
    shot->base.position = vec2d_add(shot->base.position, shot->inertia);
  }
}

void gladio_shot_render(badge_framebuffer *fb, gladio_shot const *shot) {
  if(gladio_shot_active(shot)) {
    badge_framebuffer_blt(fb,
                          fixed_point_cast_int(shot->base.position.x),
                          fixed_point_cast_int(shot->base.position.y),
                          &gladio_shot_sprite,
                          0);
  }
}
