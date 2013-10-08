#ifndef INCLUDED_JUMPNRUN_ENEMIES_H
#define INCLUDED_JUMPNRUN_ENEMIES_H

#include <badge/ui/sprite.h>
#include <badge/util/rectangle.h>

#include "tiles.h"

struct jumpnrun_game_state;
struct jumpnrun_level;
struct jumpnrun_tile_range;

struct jumpnrun_enemy;

typedef struct jumpnrun_enemy_type {
  unsigned            animation_ticks_per_frame;
  size_t              animation_length;
  badge_sprite const *animation_frames;

  vec2d               spawn_inertia;

  void (*collision_tiles)(struct jumpnrun_enemy             *self,
                          vec2d                             *desired_position,
                          struct jumpnrun_level             *lv,
                          struct jumpnrun_tile_range  const *visible_tiles);
  void (*collision_player)(struct jumpnrun_enemy      *self,
                           struct jumpnrun_game_state *state);
  void (*game_tick)(struct jumpnrun_enemy            *self,
                    struct jumpnrun_game_state       *state,
                    struct jumpnrun_level            *lv,
                    struct jumpnrun_tile_range const *visible_tiles);
} jumpnrun_enemy_type;

typedef struct jumpnrun_enemy {
  vec2d                      spawn_pos;
  vec2d                      current_pos;
  vec2d                      inertia;
  unsigned                   flags;
  unsigned                   tick_counter;
  unsigned                   current_frame;

  jumpnrun_enemy_type const *type;
} jumpnrun_enemy;

static inline rectangle rect_from_enemy(jumpnrun_enemy const *enemy) {
  badge_sprite const *cur_sprite = &enemy->type->animation_frames[(enemy->tick_counter / enemy->type->animation_ticks_per_frame) % enemy->type->animation_length];
  rectangle r = { enemy->current_pos, { FIXED_POINT(cur_sprite->width, 0), FIXED_POINT(cur_sprite->height, 0) } };
  return r;
}

enum {
  JUMPNRUN_ENEMY_SPAWNED     = 1,
  JUMPNRUN_ENEMY_UNAVAILABLE = 2
};

enum {
  JUMPNRUN_ENEMY_TYPE_CAT,

  JUMPNRUN_ENEMY_TYPE_COUNT
};

extern jumpnrun_enemy_type const jumpnrun_enemy_type_data[JUMPNRUN_ENEMY_TYPE_COUNT];

void jumpnrun_process_enemy(jumpnrun_enemy                   *self,
                            badge_framebuffer                *fb,
                            struct jumpnrun_game_state       *state,
                            struct jumpnrun_level            *lv,
                            struct jumpnrun_tile_range const *visible_tiles);
#endif
