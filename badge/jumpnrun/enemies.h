#ifndef INCLUDED_JUMPNRUN_ENEMIES_H
#define INCLUDED_JUMPNRUN_ENEMIES_H

#include "../ui/sprite.h"
#include "../util/rectangle.h"

#include "moveable.h"
#include "tiles.h"

struct jumpnrun_game_state;
struct jumpnrun_level;
struct jumpnrun_tile_range;

struct jumpnrun_enemy;

typedef struct jumpnrun_enemy_type {
  unsigned            animation_ticks_per_frame;
  size_t              animation_length;
  badge_sprite const *animation_frames;

  vec2d               extent;
  rectangle           hitbox;
  vec2d               spawn_inertia;

  void (*collision_tiles)(struct jumpnrun_enemy             *self,
			  vec2d                             *desired_position,
			  struct jumpnrun_level             *lv,
			  struct jumpnrun_tile_range  const *visible_tiles);
  void (*collision_player)(struct jumpnrun_enemy      *self,
			   struct jumpnrun_game_state *state,
			   vec2d                      *player_inertia_mod);
  void (*game_tick)(struct jumpnrun_enemy            *self,
		    struct jumpnrun_game_state       *state,
                    struct jumpnrun_level            *lv,
                    struct jumpnrun_tile_range const *visible_tiles,
		    vec2d                            *player_inertia_mod);
} jumpnrun_enemy_type;

typedef struct jumpnrun_enemy {
  jumpnrun_moveable base;
  vec2d             spawn_pos;
  unsigned          flags;

  jumpnrun_enemy_type const *type;
} jumpnrun_enemy;

static inline rectangle    const *enemy_box         (jumpnrun_enemy const *enemy) { return &enemy->base.current_box   ; }
static inline vec2d               enemy_position    (jumpnrun_enemy const *enemy) { return enemy->base.current_box.pos; }
static inline rectangle           enemy_hitbox      (jumpnrun_enemy const *enemy) { rectangle r = enemy->type->hitbox; rectangle_move_rel(&r, enemy_position(enemy)); return r; }
static inline badge_sprite const *enemy_sprite      (jumpnrun_enemy const *enemy) { return &enemy->type->animation_frames[enemy->base.anim_frame]; }
static inline uint8_t             enemy_render_flags(jumpnrun_enemy const *enemy) { return fixed_point_lt(enemy->base.inertia.x, FIXED_POINT(0, 0)) ? 0 : BADGE_BLT_MIRRORED; }

enum {
  JUMPNRUN_ENEMY_SPAWNED     = 1,
  JUMPNRUN_ENEMY_UNAVAILABLE = 2
};

enum {
  JUMPNRUN_ENEMY_TYPE_CAT,
  JUMPNRUN_ENEMY_TYPE_MUSHROOM,
  JUMPNRUN_ENEMY_TYPE_BUNNY,
  JUMPNRUN_ENEMY_TYPE_SNAKE,
  JUMPNRUN_ENEMY_TYPE_SPIRAL,
  JUMPNRUN_ENEMY_TYPE_ROTOR,

  JUMPNRUN_ENEMY_TYPE_COUNT
};

extern jumpnrun_enemy_type const jumpnrun_enemy_type_data[JUMPNRUN_ENEMY_TYPE_COUNT];

void jumpnrun_process_enemy(jumpnrun_enemy                   *self,
			    badge_framebuffer                *fb,
			    struct jumpnrun_game_state       *state,
			    struct jumpnrun_level            *lv,
			    struct jumpnrun_tile_range const *visible_tiles,
			    vec2d                            *player_inertia_mod);
#endif
