#ifndef INCLUDED_BADGE_COMMON_SPRITES_H
#define INCLUDED_BADGE_COMMON_SPRITES_H

#include "../ui/sprite.h"

enum {
  BADGE_COMMON_SPRITE_ARROW_UP,
  BADGE_COMMON_SPRITE_ARROW_DOWN
};

badge_sprite const *common_sprite(int id);

#endif
