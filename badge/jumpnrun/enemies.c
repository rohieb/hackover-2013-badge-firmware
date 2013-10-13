#include "enemies.h"

#include "collision.h"
#include "tiles.h"
#include "jumpnrun.h"

static badge_sprite const anim_cat[] = {
  { 8, 5, (uint8_t const *) "\xc7\x3f\xce\x38\x11" },
  { 8, 5, (uint8_t const *) "\xd7\x7d\xc6\x19\x25" }
};

static badge_sprite const anim_mushroom[] = {
  { 7, 7, (uint8_t const *) "\x10\x0c\x9f\xcf\xc7\x40" },
  { 7, 7, (uint8_t const *) "\x20\x18\x1e\x8f\x87\x81" },
  { 7, 7, (uint8_t const *) "\x10\x0c\x9f\xcf\xc7\x40" },
  { 7, 7, (uint8_t const *) "\x08\x86\xdf\xef\x67\x20" },
  { 7, 7, (uint8_t const *) "\x04\xc3\xef\xf7\x33\x10" },
  { 7, 7, (uint8_t const *) "\x04\xc3\xe7\xf3\x31\x10" }
};

static badge_sprite const anim_kaninchen[] = {
  { 7, 5, (uint8_t const *) "\x60\x30\xbe\x31\x02" },
  { 7, 5, (uint8_t const *) "\x42\x30\xbe\x31\x01" },
  { 7, 5, (uint8_t const *) "\x42\x30\xae\x35\x01" },
  { 7, 5, (uint8_t const *) "\x60\x30\xae\x35\x02" },
  { 7, 5, (uint8_t const *) "\x60\x30\xbe\x31\x01" }
};

static void enemy_animation_advance(jumpnrun_enemy *enemy) {
  ++enemy->base.tick_minor;
  if(enemy->base.tick_minor == enemy->type->animation_ticks_per_frame) {
    enemy->base.tick_minor = 0;

    ++enemy->base.anim_frame;
    if(enemy->base.anim_frame >= enemy->type->animation_length) {
      enemy->base.anim_frame = 0;
    }
  }
}

void jumpnrun_process_enemy(jumpnrun_enemy                   *self,
			    badge_framebuffer                *fb,
			    struct jumpnrun_game_state       *state,
			    struct jumpnrun_level            *lv,
			    struct jumpnrun_tile_range const *visible_tiles,
			    vec2d                            *player_inertia_mod) {
  int const spawn_margin = 1 + self->type->animation_frames[self->base.anim_frame].width;

  if(self->flags & JUMPNRUN_ENEMY_SPAWNED) {
    if(fixed_point_lt(rectangle_left(enemy_box(self)), FIXED_POINT(state->left                       - spawn_margin, 0)) ||
       fixed_point_gt(rectangle_left(enemy_box(self)), FIXED_POINT(state->left + BADGE_DISPLAY_WIDTH + spawn_margin, 0)) ||
       fixed_point_gt(rectangle_top (enemy_box(self)), FIXED_POINT(BADGE_DISPLAY_HEIGHT                            , 0))) {
      self->flags &= ~JUMPNRUN_ENEMY_SPAWNED;
    } else {
      self->type->game_tick(self, state, lv, visible_tiles, player_inertia_mod);

      if(fb) {
	badge_framebuffer_blt(fb,
			      fixed_point_cast_int(rectangle_left(enemy_box(self))) - state->left,
			      fixed_point_cast_int(rectangle_top (enemy_box(self))),
			      enemy_sprite(self),
			      enemy_render_flags(self));
      }
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
    self->flags               |= JUMPNRUN_ENEMY_SPAWNED | JUMPNRUN_ENEMY_UNAVAILABLE;
    self->base.current_box     = rectangle_new(self->spawn_pos, self->type->extent);
    self->base.inertia         = self->type->spawn_inertia;
    self->base.anim_frame      = 0;
    self->base.tick_minor      = 0;
    self->base.touching_ground = false;
    self->base.jumpable_frames = 0;
  }
}

void enemy_collision_tiles_bounce_horiz(jumpnrun_enemy            *self,
					vec2d                     *desired_position,
					jumpnrun_level            *lv,
					jumpnrun_tile_range const *visible_tiles) {
  vec2d inertia_mod = self->base.inertia;

  collisions_tiles_displace(desired_position,
			    &self->base,
			    lv,
			    visible_tiles,
			    &inertia_mod);

  if(fixed_point_ne(inertia_mod.x, self->base.inertia.x)) {
    self->base.inertia.x = fixed_point_neg(self->base.inertia.x);
  }
}

void enemy_collision_player_jumpable(jumpnrun_enemy      *self,
				     jumpnrun_game_state *state,
				     vec2d               *player_inertia_mod)
{
  rectangle rect_self = enemy_hitbox(self);

  if(rectangle_intersect(&rect_self, &state->player.current_box)) {
    if(fixed_point_gt(state->player.inertia.y, FIXED_POINT(0, 0))) {
      self->flags &= ~JUMPNRUN_ENEMY_SPAWNED;
      player_inertia_mod->y = FIXED_POINT(0, -250);
      state->player.jumpable_frames = 12;
    } else {
      state->status = JUMPNRUN_DEAD;
    }
  }
}

void enemy_tick_cat(jumpnrun_enemy            *self,
		    jumpnrun_game_state       *state,
		    jumpnrun_level            *lv,
		    jumpnrun_tile_range const *visible_tiles,
		    vec2d                     *player_inertia_mod) {
  int screenpos = fixed_point_cast_int(rectangle_left(&self->base.current_box));

  if(screenpos + JUMPNRUN_MAX_SPAWN_MARGIN <  state->left ||
     screenpos                             >= state->left + BADGE_DISPLAY_WIDTH + JUMPNRUN_MAX_SPAWN_MARGIN) {
    return;
  }

  jumpnrun_passive_movement(&self->base.inertia);

  vec2d new_pos = vec2d_add(enemy_position(self), self->base.inertia);
  self->type->collision_tiles(self, &new_pos, lv, visible_tiles);
  self->type->collision_player(self, state, player_inertia_mod);
  rectangle_move_to(&self->base.current_box, new_pos);

  enemy_animation_advance(self);
}

jumpnrun_enemy_type const jumpnrun_enemy_type_data[JUMPNRUN_ENEMY_TYPE_COUNT] = {
  { 16, ARRAY_SIZE(anim_cat), anim_cat,
    { 
      FIXED_INT_I(8), FIXED_INT_I(5)
    }, {
      { FIXED_INT_I(0), FIXED_INT_I(0) },
      { FIXED_INT_I(8), FIXED_INT_I(5) }
    }, {
      FIXED_POINT_I(0, -100), FIXED_INT_I(0)
    },
    enemy_collision_tiles_bounce_horiz,
    enemy_collision_player_jumpable,
    enemy_tick_cat
  }, {
    12, ARRAY_SIZE(anim_mushroom), anim_mushroom,
    { 
      FIXED_INT_I(7), FIXED_INT_I(7)
    }, {
      { FIXED_INT_I(0), FIXED_INT_I(0) },
      { FIXED_INT_I(7), FIXED_INT_I(7) }
    }, {
      FIXED_POINT_I(0, -50), FIXED_INT_I(0)
    },
    enemy_collision_tiles_bounce_horiz,
    enemy_collision_player_jumpable,
    enemy_tick_cat
  }, {
    9, ARRAY_SIZE(anim_kaninchen), anim_kaninchen,
    {
      FIXED_INT_I(7), FIXED_INT_I(5)
    }, {
      { FIXED_INT_I(1), FIXED_INT_I(2) },
      { FIXED_INT_I(6), FIXED_INT_I(3) }
    }, {
      FIXED_POINT_I(0, -80), FIXED_POINT_I(0, 0)
    },
    enemy_collision_tiles_bounce_horiz,
    enemy_collision_player_jumpable,
    enemy_tick_cat
  }
};
