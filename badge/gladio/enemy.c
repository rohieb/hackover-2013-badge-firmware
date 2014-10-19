#include "enemy.h"
#include "enemy_types.h"
#include "game_state.h"
#include "player.h"
#include "shot.h"

#include "../common/explosion.h"

static badge_sprite const anim_dummy = { 11, 9, (uint8_t const *) "\x10\x20\x40\xc0\x81\x83\x4d\xdb\xff\xff\xd9\xa0\x00" };

rectangle gladio_enemy_hitbox(gladio_enemy const *enemy) {
  rectangle r = gladio_enemy_type_get(enemy)->hitbox;
  rectangle_move_rel(&r, enemy->base.position);
  return r;
}

rectangle gladio_enemy_collisionbox(gladio_enemy const *enemy) {
  rectangle r = gladio_enemy_type_get(enemy)->collisionbox;
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

void gladio_enemy_spawn_at(struct gladio_game_state *state, uint8_t type_id, int8_t pos_x, int8_t pos_y) {
  for(uint8_t i = 0; i < GLADIO_MAX_ENEMIES; ++i) {
    gladio_enemy *dest = &state->active_enemies[i];

    if(!gladio_enemy_active(dest)) {
      gladio_enemy_type const *type = gladio_enemy_type_by_id(type_id);

      dest->base         = gladio_object_new(vec2d_new(FIXED_INT(pos_x), FIXED_INT(pos_y)));
      dest->type         = type_id;
      dest->hitpoints    = type->hitpoints;
      dest->move_counter = 0;
      dest->cooldown     = type->cooldown_initial;
      dest->flags        = GLADIO_ENEMY_ACTIVE;
      break;
    }
  }
}

void gladio_enemy_spawn(struct gladio_game_state *state, uint8_t type_id, int8_t pos_y) {
  gladio_enemy_spawn_at(state, type_id, gladio_enemy_type_by_id(type_id)->spawnpos_x, pos_y);
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
                          &gladio_enemy_type_get(enemy)->sprite,
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
      rectangle hitbox = gladio_enemy_hitbox(e);

      if(gladio_object_off_playfield(hitbox)) {
        gladio_enemy_despawn(e);
        continue;
      }

      ++e->move_counter;

      if(gladio_enemy_dying(e)) {
        if(e->move_counter >= EXPLOSION_TICKS) {
          gladio_enemy_despawn(e);
          continue;
        }
      } else {
        vec2d hitbox_lr = vec2d_new(rectangle_right (&hitbox),
                                    rectangle_bottom(&hitbox));

        uint8_t shot_ix = gladio_shot_lower_bound(hitbox.pos, state->shots_friendly, player_shot_count);

        while(shot_ix < player_shot_count && vec2d_xy_less(state->shots_friendly[shot_ix].base.position, hitbox_lr)) {
          if((state->shots_friendly[shot_ix].flags & GLADIO_SHOT_DESPAWNING) == 0) {
            rectangle shotbox = gladio_shot_rectangle(&state->shots_friendly[shot_ix]);
            if(rectangle_intersect(&hitbox, &shotbox)) {
              gladio_enemy_type_get(e)->collision_shots(e, &state->shots_friendly[shot_ix]);
            }
          }
          ++shot_ix;
        }

        rectangle collisionbox = gladio_enemy_collisionbox(e);

        if(gladio_enemy_active(e) && rectangle_intersect(&collisionbox, &box_player)) {
          gladio_enemy_type_get(e)->collision_player(e, state);
        }

        if(e->cooldown == 0) {
          gladio_enemy_type_get(e)->tick_shoot(e, state);
          e->cooldown = gladio_enemy_type_get(e)->cooldown_max;
        }
        --e->cooldown;
      }

      gladio_enemy_type_get(e)->tick_move(e, state);
    }
  }
}
