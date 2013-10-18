#include "collision.h"

uint8_t collision_displace(vec2d             *desired_pos,
                           jumpnrun_moveable *current,
                           rectangle   const *obstacle,
                           vec2d             *inertia_mod) {
  rectangle desired = current->current_box;
  rectangle_move_to(&desired, *desired_pos);

  if(!rectangle_intersect(obstacle, &desired)) {
    return 0;
  }

  fixed_point x = FIXED_INT(1000), y = FIXED_INT(1000);
  fixed_point dx = desired_pos->x, dy = desired_pos->y;
  bool bottom_collision = false;

  uint8_t coll_x = 0;
  uint8_t coll_y = 0;
  uint8_t coll   = 0;

  if(fixed_point_le(rectangle_top   ( obstacle), rectangle_top(&desired)) &&
     fixed_point_gt(rectangle_bottom( obstacle), rectangle_top(&desired)) &&
     fixed_point_lt(rectangle_top   (&desired ), rectangle_top(&current->current_box))) {

    coll_y = JUMPNRUN_COLLISION_BOTTOM;
    y      = fixed_point_sub(rectangle_bottom(obstacle), rectangle_top(&desired));
    dy     =                 rectangle_bottom(obstacle);

  } else if(fixed_point_gt(rectangle_bottom( obstacle), rectangle_bottom(&desired)) &&
            fixed_point_le(rectangle_top   ( obstacle), rectangle_bottom(&desired)) &&
            fixed_point_gt(rectangle_top   (&desired ), rectangle_top   (&current->current_box))) {

    coll_y = JUMPNRUN_COLLISION_TOP;
    y      = fixed_point_sub(rectangle_bottom(&desired ), rectangle_top   ( obstacle));
    dy     = fixed_point_sub(rectangle_top   ( obstacle), rectangle_height(&desired ));
    bottom_collision = true;

  }

  if(fixed_point_le(rectangle_left ( obstacle), rectangle_left(&desired)) &&
     fixed_point_gt(rectangle_right( obstacle), rectangle_left(&desired)) &&
     fixed_point_lt(rectangle_left (&desired ), rectangle_left(&current->current_box))) {

    coll_x = JUMPNRUN_COLLISION_RIGHT;
    x      = fixed_point_sub(rectangle_right(obstacle), rectangle_left(&desired));
    dx     =                 rectangle_right(obstacle);

  } else if(fixed_point_gt(rectangle_right( obstacle), rectangle_right(&desired)) &&
            fixed_point_le(rectangle_left ( obstacle), rectangle_right(&desired)) &&
            fixed_point_gt(rectangle_left (&desired ), rectangle_left (&current->current_box))) {

    coll_x = JUMPNRUN_COLLISION_LEFT;
    x      = fixed_point_sub(rectangle_right(&desired ), rectangle_left ( obstacle));
    dx     = fixed_point_sub(rectangle_left ( obstacle), rectangle_width(&desired ));

  }

  if(fixed_point_eq(x, y)) {
    desired_pos->x = dx;
    desired_pos->y = dy;
    coll = coll_x | coll_y;
  } else if(fixed_point_gt(x, y)) {
    desired_pos->y = dy;
    inertia_mod->y = FIXED_INT(0);
    current->touching_ground = bottom_collision;
    coll = coll_y;
  } else {
    desired_pos->x = dx;
    inertia_mod->x = FIXED_INT(0);
    coll = coll_x;
  }

  return coll;
}

bool collisions_tiles_displace(vec2d                     *desired_position,
                               jumpnrun_moveable         *thing,
                               jumpnrun_level      const *lv,
                               jumpnrun_tile_range const *visible_tiles,
                               vec2d                     *inertia_mod)
{
  int collision_tile[] = { -1, -1, -1,
                           -1, -1, -1,
                           -1, -1, -1
  };
  static int const collision_order[] = { 7, 1, 3, 5, 6, 8, 0, 2 };

  vec2d midpoint = rectangle_mid(&thing->current_box);

  jumpnrun_tile_position midtile_pos = {
    fixed_point_cast_int(fixed_point_div(midpoint.x, FIXED_INT(JUMPNRUN_TILE_PIXEL_WIDTH ))),
    fixed_point_cast_int(fixed_point_div(midpoint.y, FIXED_INT(JUMPNRUN_TILE_PIXEL_HEIGHT)))
  };

  int tile;

  for(tile = visible_tiles->first; (size_t) tile < visible_tiles->last && lv->tiles[tile].pos.x < midtile_pos.x - 1; ++tile)
    ;
  for(; (size_t) tile < visible_tiles->last && lv->tiles[tile].pos.x < midtile_pos.x + 2; ++tile) {
    int xdiff = lv->tiles[tile].pos.x - midtile_pos.x;
    int ydiff = lv->tiles[tile].pos.y - midtile_pos.y;

    switch(xdiff) {
    case -1:
    {
      switch(ydiff) {
      case -1: collision_tile[0] = tile; break;
      case  0: collision_tile[1] = tile; break;
      case  1: collision_tile[2] = tile; break;
      }
      break;
    }
    case  0:
    {
      switch(ydiff) {
      case -1: collision_tile[3] = tile; break;
      case  0: collision_tile[4] = tile; break;
      case  1: collision_tile[5] = tile; break;
      }
      break;
    }
    case  1:
    {
      switch(ydiff) {
      case -1: collision_tile[6] = tile; break;
      case  0: collision_tile[7] = tile; break;
      case  1: collision_tile[8] = tile; break;
      }
      break;
    }
    }
  }

  /* collision: sort by priority (top/bottom, left/right, then diagonal) */
  bool lethal = false;
  thing->touching_ground = false;

  for(size_t collision_index = 0; collision_index < ARRAY_SIZE(collision_order); ++collision_index) {
    if(collision_tile[collision_order[collision_index]] == -1) {
      continue;
    }

    jumpnrun_tile *tile_obj = &lv->tiles[collision_tile[collision_order[collision_index]]];
    rectangle tile_rect = rect_from_tile(tile_obj);

    uint8_t coll = collision_displace(desired_position, thing, &tile_rect, inertia_mod);
    if(coll & tile_type(tile_obj)->lethal_sides) {
      lethal = true;
    }
  }

  rectangle_move_to(&thing->current_box, *desired_position);

  return lethal;
}
