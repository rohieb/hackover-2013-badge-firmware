#ifndef INCLUDED_BADGE_GLADIO_SHOT_H
#define INCLUDED_BADGE_GLADIO_SHOT_H

#include "animation.h"
#include "object.h"
#include "../util/rectangle.h"

#include <stdint.h>

typedef struct {
  gladio_object base;
  uint8_t       type;
} gladio_shot;

typedef struct {
  gladio_animation animation;
  uint8_t          damage;
} gladio_shot_type;

#endif
