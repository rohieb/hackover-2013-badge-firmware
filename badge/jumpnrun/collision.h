#ifndef INCLUDED_COLLISION_H
#define INCLUDED_COLLISION_H

#include "jumpnrun.h"
#include "../util/rectangle.h"
#include <stdbool.h>

void collision_displace(vec2d           *desired_pos,
			rectangle const *current,
			rectangle const *obstacle,
			vec2d           *inertia_mod,
			bool            *touching_ground);

void collisions_tiles_displace(vec2d                     *desired_position,
			       rectangle           const *current,
			       jumpnrun_level      const *level,
			       jumpnrun_tile_range const *visible_tiles,
			       vec2d                     *inertia_mod,
			       bool                      *touching_ground);

#endif
