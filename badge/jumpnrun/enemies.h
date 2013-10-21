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
  uint8_t             animation_ticks_per_frame;
  uint8_t             animation_length;
  badge_sprite const *animation_frames;

  rectangle           hitbox;
  vec2d               spawn_inertia;

  void (*collision_tiles)(struct jumpnrun_enemy             *self,
                          vec2d                             *desired_position,
                          struct jumpnrun_level             *lv,
                          struct jumpnrun_tile_range  const *visible_tiles);
  void (*collision_player)(struct jumpnrun_enemy      *self,
                           struct jumpnrun_game_state *state,
                           vec2d                      *player_inertia_mod);
  void (*collision_shots)(struct jumpnrun_enemy      *self,
                          struct jumpnrun_game_state *state);

  void (*move_tick)(struct jumpnrun_enemy            *self,
                    struct jumpnrun_game_state       *state,
                    struct jumpnrun_level            *lv,
                    struct jumpnrun_tile_range const *visible_tiles,
                    vec2d                            *player_inertia_mod);
} jumpnrun_enemy_type;

typedef struct jumpnrun_enemy {
  jumpnrun_moveable base;
  vec2d             spawn_pos;

  jumpnrun_enemy_type const *type;
} jumpnrun_enemy;

enum {
// Do not collide with JUMPNRUN_MOVEABLE_* flags
  JUMPNRUN_ENEMY_SPAWNED        = 128,
  JUMPNRUN_ENEMY_UNAVAILABLE    =  64,
  JUMPNRUN_ENEMY_MOVING         =  32,
  JUMPNRUN_ENEMY_EVENT_TRIGGER1 =  16
};

static inline rectangle    const *enemy_hitbox(jumpnrun_enemy const *enemy) { return &enemy->base.hitbox; }
void jumpnrun_enemy_despawn(jumpnrun_enemy *self);
void jumpnrun_enemy_reset  (jumpnrun_enemy *self);

enum {
  JUMPNRUN_ENEMY_TYPE_CAT,
  JUMPNRUN_ENEMY_TYPE_MUSHROOM,
  JUMPNRUN_ENEMY_TYPE_BUNNY,
  JUMPNRUN_ENEMY_TYPE_SNAKE,
  JUMPNRUN_ENEMY_TYPE_SPIRAL,
  JUMPNRUN_ENEMY_TYPE_ROTOR,
  JUMPNRUN_ENEMY_TYPE_DOG,
  JUMPNRUN_ENEMY_TYPE_GIRAFFE,
  JUMPNRUN_ENEMY_TYPE_BIRD,
  JUMPNRUN_ENEMY_TYPE_BIRD_STRAIGHT,
  JUMPNRUN_ENEMY_TYPE_BIRD_DIP,

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
