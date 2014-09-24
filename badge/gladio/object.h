#ifndef INCLUDED_BADGE_GLADIO_OBJECT_H
#define INCLUDED_BADGE_GLADIO_OBJECT_H

#include "../util/rectangle.h"

typedef struct gladio_object {
  vec2d   position;
  uint8_t anim_pos;
} __attribute__((packed)) gladio_object;

static inline rectangle gladio_object_rectangle(gladio_object obj, vec2d extents) {
  return rectangle_new(obj.position, extents);
}

static inline gladio_object gladio_object_new(vec2d position) {
  gladio_object obj;
  obj.position = position;
  obj.anim_pos = 0;
  return obj;
}

#endif
