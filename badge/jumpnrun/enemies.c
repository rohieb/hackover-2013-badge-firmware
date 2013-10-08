#include "enemies.h"

#include "collision.h"
#include "tiles.h"
#include "jumpnrun.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof*(arr))

static badge_sprite const anim_cat[] = {
  { 8, 5, (uint8_t const *) "\xc7\x3f\xce\x38\x11" },
  { 8, 5, (uint8_t const *) "\xd7\x7d\xc6\x19\x25" }
};

void jumpnrun_process_enemy(jumpnrun_enemy                   *self,
			    badge_framebuffer                *fb,
			    struct jumpnrun_game_state       *state,
			    struct jumpnrun_level            *lv,
			    struct jumpnrun_tile_range const *visible_tiles) {
  int const spawn_margin = 1 + self->type->animation_frames[self->current_frame].width;

  if(self->flags & JUMPNRUN_ENEMY_SPAWNED) {
    if(fixed_point_lt(self->current_pos.x, FIXED_POINT(state->left                       - spawn_margin, 0)) ||
       fixed_point_gt(self->current_pos.x, FIXED_POINT(state->left + BADGE_DISPLAY_WIDTH + spawn_margin, 0)) ||
       fixed_point_cast_int(self->current_pos.y) > BADGE_DISPLAY_HEIGHT) {
      self->flags &= ~JUMPNRUN_ENEMY_SPAWNED;
    } else {
      self->type->game_tick(self, state, lv, visible_tiles);
      badge_framebuffer_blt(fb,
			    fixed_point_cast_int(self->current_pos.x) - state->left,
			    fixed_point_cast_int(self->current_pos.y),
			    &self->type->animation_frames[self->current_frame],
			    fixed_point_lt(self->inertia.x, FIXED_POINT(0, 0)) ? 0 : BADGE_BLT_MIRRORED);
    }
  } else if(self->flags & JUMPNRUN_ENEMY_UNAVAILABLE) { 
    if(state->left                                      > fixed_point_cast_int(self->spawn_pos.x) + spawn_margin ||
       state->left + BADGE_DISPLAY_WIDTH + spawn_margin < fixed_point_cast_int(self->spawn_pos.x)) {
      self->flags &= ~JUMPNRUN_ENEMY_UNAVAILABLE;
    }
  } else if((fixed_point_gt(self->spawn_pos.x, FIXED_POINT(state->left                       - spawn_margin, 0)) &&
	     fixed_point_lt(self->spawn_pos.x, FIXED_POINT(state->left - spawn_margin / 2, 0))) ||
	    (fixed_point_lt(self->spawn_pos.x, FIXED_POINT(state->left + BADGE_DISPLAY_WIDTH + spawn_margin, 0)) &&
	     fixed_point_gt(self->spawn_pos.x, FIXED_POINT(state->left + BADGE_DISPLAY_WIDTH, 0)))) {
    // enemy unspawned, available and in spawn zone.
    self->flags        |= JUMPNRUN_ENEMY_SPAWNED | JUMPNRUN_ENEMY_UNAVAILABLE;
    self->current_pos   = self->spawn_pos;
    self->inertia       = self->type->spawn_inertia;
    self->current_frame = 0;
    self->tick_counter  = 0;
  }
}

void enemy_collision_tiles_bounce_horiz(jumpnrun_enemy            *self,
					vec2d                     *desired_position,
					jumpnrun_level            *lv,
					jumpnrun_tile_range const *visible_tiles) {
  rectangle rect_self = rect_from_enemy(self);
  vec2d inertia_copy = self->inertia;
  bool touching_ground = false;

  collisions_tiles_displace(desired_position,
			    &rect_self,
			    lv,
			    visible_tiles,
			    &inertia_copy,
			    &touching_ground);

  if(fixed_point_ne(inertia_copy.x, self->inertia.x)) {
    self->inertia.x = fixed_point_neg(self->inertia.x);
  }
}

void enemy_collision_player_kill(jumpnrun_enemy      *self,
				 jumpnrun_game_state *state)
{
  rectangle rect_self   = rect_from_enemy(self);
  rectangle rect_hacker = hacker_rect_current(state);

  if(rectangle_intersect(&rect_self, &rect_hacker)) {
    state->status = JUMPNRUN_DEAD;
  }
}

void enemy_tick_cat(jumpnrun_enemy *self,
		    jumpnrun_game_state *state,
		    jumpnrun_level *lv,
		    jumpnrun_tile_range const *visible_tiles) {
  int screenpos = fixed_point_cast_int(self->current_pos.x);

  if(screenpos + JUMPNRUN_MAX_SPAWN_MARGIN <  state->left ||
     screenpos                             >= state->left + BADGE_DISPLAY_WIDTH + JUMPNRUN_MAX_SPAWN_MARGIN) {
    return;
  }

  jumpnrun_passive_movement(&self->inertia);

  vec2d new_pos = vec2d_add(self->current_pos, self->inertia);
  self->type->collision_tiles(self, &new_pos, lv, visible_tiles);
  self->type->collision_player(self, state);
  self->current_pos = new_pos;

  self->tick_counter  = (self->tick_counter + 1) % self->type->animation_ticks_per_frame;
  if(self->tick_counter == 0) {
    self->current_frame = (self->current_frame + 1) % self->type->animation_length;
  }
}

jumpnrun_enemy_type const jumpnrun_enemy_type_data[JUMPNRUN_ENEMY_TYPE_COUNT] = {
  { 2, ARRAY_SIZE(anim_cat), anim_cat,
    { FIXED_POINT_I(0, -800), FIXED_POINT_I(0, 0) },
    enemy_collision_tiles_bounce_horiz,
    enemy_collision_player_kill,
    enemy_tick_cat
  }
};
