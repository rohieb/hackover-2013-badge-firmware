#ifndef INCLUDED_BADGE_GLADIO_SHOT_H
#define INCLUDED_BADGE_GLADIO_SHOT_H

#include "animation.h"
#include "object.h"
#include "../util/rectangle.h"

#include <stdint.h>

typedef struct gladio_shot {
  gladio_object base;
  uint8_t       type;
} gladio_shot;

typedef struct gladio_shot_type {
  gladio_animation animation;
  uint8_t          damage;
} gladio_shot_type;

gladio_shot_type const *gladio_get_shot_type_by_id(uint8_t id);

static inline gladio_shot_type const *gladio_get_shot_type(gladio_shot const *shot) {
  return gladio_get_shot_type_by_id(shot->type);
}

#endif
