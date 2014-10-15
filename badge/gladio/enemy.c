#include "enemy.h"
#include "game_state.h"
#include "player.h"
#include "shot.h"

#include "../common/explosion.h"

static badge_sprite const anim_dummy = { 11, 9, (uint8_t const *) "\x10\x20\x40\xc0\x81\x83\x4d\xdb\xff\xff\xd9\xa0\x00" };

static inline vec2d gladio_enemy_snout_position(gladio_enemy *self) {
  rectangle hitbox = gladio_enemy_hitbox(self);
  return vec2d_new(self->base.position.x, rectangle_mid_y(&hitbox));
}

static void tick_move_straight_ahead(gladio_enemy *self, gladio_game_state *state) {
  (void) state;

  self->base.position.x = fixed_point_sub(self->base.position.x, FIXED_INT(1));

  rectangle r = gladio_enemy_hitbox(self);
  if(fixed_point_lt(rectangle_right(&r), FIXED_INT(0))) {
    gladio_enemy_despawn(self);
  }
}

static void tick_shoot_targeted(gladio_enemy *self, gladio_game_state *state) {
  if(self->move_counter == 16) {
    vec2d diff = vec2d_sub(state->player.base.position, self->base.position);
    diff = vec2d_div(diff, fixed_point_mul(vec2d_length_approx(diff), FIXED_POINT(1, 500)));

    vec2d snout_pos  = gladio_enemy_snout_position(self);

    gladio_shot_hostile_spawn(state, snout_pos, diff);
  } else if(self->move_counter == 48) {
    self->move_counter = 0;
  }
}

static void collision_player_dummy(gladio_enemy *self, gladio_game_state *state) {
  --self->hitpoints;
  gladio_player_damage(&state->player, 1);

  if(self->hitpoints == 0) {
    gladio_enemy_schedule_death(self);
  }
}

static void collision_shots_dummy(gladio_enemy *self, gladio_shot *shot) {
  gladio_enemy_schedule_death(self);
  gladio_shot_despawn_later(shot);
}

static gladio_enemy_type const enemy_types[] = {
  {
    { 11, 9, (uint8_t const *) "\x10\x20\x40\xc0\x81\x83\x4d\xdb\xff\xff\xd9\xa0\x00" },
    { { FIXED_INT_I(1), FIXED_INT_I(1) }, { FIXED_INT_I(9), FIXED_INT_I(7) } },
    10,
    BADGE_DISPLAY_WIDTH,
    tick_move_straight_ahead,
    tick_shoot_targeted,
    collision_player_dummy,
    collision_shots_dummy
  }
};

gladio_enemy_type const *gladio_get_enemy_type_by_id(uint8_t id) {
  return &enemy_types[id];
}

rectangle gladio_enemy_hitbox(gladio_enemy const *enemy) {
  rectangle r = enemy_types[enemy->type].hitbox;
  rectangle_move_rel(&r, enemy->base.position);
  return r;
}

uint8_t gladio_enemy_active(gladio_enemy const *enemy) {
  return enemy->flags & GLADIO_ENEMY_ACTIVE;
}

uint8_t gladio_enemy_dying (gladio_enemy const *enemy) {
  return enemy->flags & GLADIO_ENEMY_DYING;
}

void gladio_enemy_schedule_death(gladio_enemy *enemy) {
  enemy->flags         = GLADIO_ENEMY_DYING | GLADIO_ENEMY_ACTIVE;
  enemy->move_counter  = 0;
  enemy->base.position = gladio_object_explosion_position(gladio_enemy_hitbox(enemy));
}

void gladio_enemy_despawn(gladio_enemy *enemy) {
  enemy->flags = 0;
}

void gladio_enemy_spawn(struct gladio_game_state *state, uint8_t type_id, int8_t pos_y) {
  for(uint8_t i = 0; i < GLADIO_MAX_ENEMIES; ++i) {
    gladio_enemy *dest = &state->active_enemies[i];

    if(!gladio_enemy_active(dest)) {
      gladio_enemy_type const *type = gladio_get_enemy_type_by_id(type_id);

      dest->base         = gladio_object_new(vec2d_new(FIXED_INT(type->spawnpos_x), FIXED_INT(pos_y)));
      dest->type         = type_id;
      dest->hitpoints    = type->hitpoints;
      dest->flags        = GLADIO_ENEMY_ACTIVE;
      dest->move_counter = 0;
      break;
    }
  }
}

void gladio_enemy_render(badge_framebuffer *fb, gladio_enemy const *enemy) {
  if(gladio_enemy_dying(enemy)) {
    common_render_explosion(fb,
                            fixed_point_cast_int(enemy->base.position.x),
                            fixed_point_cast_int(enemy->base.position.y),
                            enemy->move_counter);
  } else {
    badge_framebuffer_blt(fb,
                          fixed_point_cast_int(enemy->base.position.x),
                          fixed_point_cast_int(enemy->base.position.y),
                          &gladio_get_enemy_type(enemy)->sprite,
                          0);
  }
}

void gladio_enemy_tick(gladio_game_state *state) {
  uint8_t player_shot_count;
  rectangle box_player = gladio_player_hitbox(&state->player);

  for(player_shot_count = 0;
      player_shot_count < GLADIO_MAX_SHOTS_FRIENDLY
        && gladio_shot_active(&state->shots_friendly[player_shot_count]);
      ++player_shot_count)
    ;

  for(uint8_t i = 0; i < GLADIO_MAX_ENEMIES; ++i) {
    gladio_enemy *e = &state->active_enemies[i];

    if(gladio_enemy_active(e)) {
      ++e->move_counter;

      if(gladio_enemy_dying(e)) {
        if(e->move_counter >= EXPLOSION_TICKS) {
          gladio_enemy_despawn(e);
          continue;
        }
      } else {
        rectangle hitbox = gladio_enemy_hitbox(e);
        vec2d hitbox_lr = vec2d_new(rectangle_right (&hitbox),
                                    rectangle_bottom(&hitbox));

        uint8_t shot_ix = gladio_shot_lower_bound(hitbox.pos, state->shots_friendly, player_shot_count);

        while(shot_ix < player_shot_count && vec2d_xy_less(state->shots_friendly[shot_ix].base.position, hitbox_lr)) {
          rectangle shotbox = gladio_shot_rectangle(&state->shots_friendly[shot_ix]);
          if(rectangle_intersect(&hitbox, &shotbox)) {
            gladio_get_enemy_type(e)->collision_shots(e, &state->shots_friendly[shot_ix]);
          }
          ++shot_ix;
          shotbox = gladio_shot_rectangle(&state->shots_friendly[shot_ix]);
        }

        if(gladio_enemy_active(e) && rectangle_intersect(&hitbox, &box_player)) {
          gladio_get_enemy_type(e)->collision_player(e, state);
        }

        gladio_get_enemy_type(e)->tick_shoot(e, state);
      }

      gladio_get_enemy_type(e)->tick_move(e, state);
    }
  }
}
