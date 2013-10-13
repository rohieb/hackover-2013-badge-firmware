#ifndef INCLUDED_COLLISION_H
#define INCLUDED_COLLISION_H

#include "moveable.h"
#include "jumpnrun.h"
#include "../util/rectangle.h"
#include <stdbool.h>

void collision_displace(vec2d             *desired_pos,
			jumpnrun_moveable *current,
			rectangle   const *obstacle,
			vec2d             *inertia_mod);

void collisions_tiles_displace(vec2d                     *desired_position,
			       jumpnrun_moveable         *thing,
			       jumpnrun_level      const *level,
			       jumpnrun_tile_range const *visible_tiles,
			       vec2d                     *inertia_mod);

#endif
