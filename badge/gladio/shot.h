#ifndef INCLUDED_BADGE_GLADIO_SHOT_H
#define INCLUDED_BADGE_GLADIO_SHOT_H

#include "object.h"
#include "../util/rectangle.h"

#include <stdint.h>

struct gladio_game_state;

enum {
  GLADIO_SHOT_FRIENDLY,
  GLADIO_SHOT_HOSTILE,
};

enum {
  GLADIO_SHOT_FRIENDLY_FRONT   = 1,
  GLADIO_SHOT_FRIENDLY_SIDEGUN = 2
};

enum {
  GLADIO_SHOT_DESPAWNING = 1
};

typedef struct gladio_shot {
  gladio_object base;
  uint8_t       type;
  uint8_t       flags;
  vec2d         inertia;
} gladio_shot;

typedef struct gladio_shot_type {
  uint8_t          damage;
} gladio_shot_type;

gladio_shot_type const *gladio_get_shot_type_by_id(uint8_t id);

static inline gladio_shot_type const *gladio_get_shot_type(gladio_shot const *shot) {
  return gladio_get_shot_type_by_id(shot->type);
}

uint8_t gladio_shot_active(gladio_shot const *shot);
uint8_t gladio_shot_deadly(gladio_shot const *shot);

uint8_t gladio_shot_friendly_despawn_and_compress(struct gladio_game_state *state);

void gladio_shot_friendly_move(struct gladio_game_state *state);
void gladio_shot_friendly_spawn(struct gladio_game_state *state,
                                uint8_t                   where,
                                vec2d                     position);

void gladio_shot_hostile_tick(struct gladio_game_state *state);
void gladio_shot_hostile_spawn(struct gladio_game_state *state,
                               vec2d                     position,
                               vec2d                     movement);
void gladio_shot_despawn      (gladio_shot *shot);
void gladio_shot_despawn_later(gladio_shot *shot);

void gladio_shot_render(badge_framebuffer *fb, gladio_shot const *shot);

rectangle gladio_shot_rectangle(gladio_shot const *shot);

uint8_t gladio_shot_lower_bound(vec2d needle,
                                gladio_shot const *haystack,
                                uint8_t len);

#endif
