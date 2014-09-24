#include "shot.h"

#include "game_state.h"

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

void gladio_shot_despawn(gladio_shot *shot) {
  memset(shot, 0, sizeof(gladio_shot));
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

    rectangle r = gladio_shot_rectangle(shot);
    if(!rectangle_onscreen(&r)) {
      gladio_shot_despawn(shot);
    }
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
