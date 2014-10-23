#include "player.h"

#include "game_state.h"
#include "status.h"
#include "../common/explosion.h"
#include "../ui/display.h"
#include "../ui/sprite.h"

#include <stdio.h>

static badge_sprite const gladio_player_sprite = { GLADIO_PLAYER_WIDTH, GLADIO_PLAYER_HEIGHT, (uint8_t const *) "\x29\xdf\xff\xaf\x5a\xb5\x6f\xdf\xb6\x6d\x51\xe0\xc0\x01\x01\x02\x04" };

gladio_player gladio_player_new(void) {
  return (gladio_player) {
      { { FIXED_INT(10), FIXED_INT(GLADIO_STATUS_BAR_HEIGHT + BADGE_DISPLAY_HEIGHT / 2) } },
      GLADIO_PLAYER_MAX_HEALTH,
      0, 0, 0,
      GLADIO_PLAYER_INVULNERABILITY_DURATION,
      GLADIO_PLAYER_INVULNERABLE
    };
}

void gladio_player_render(badge_framebuffer *fb, gladio_player const *p) {
  if(p->status == GLADIO_PLAYER_DYING) {
    common_render_explosion(fb,
                            fixed_point_cast_int(p->base.position.x),
                            fixed_point_cast_int(p->base.position.y),
                            (EXPLOSION_TICKS / 3 - p->status_cooldown - 1) * 3);
    return;
  }

  if(p->status == GLADIO_PLAYER_INVULNERABLE && (p->status_cooldown & 1)) {
    return;
  }

  badge_framebuffer_blt(fb,
                        fixed_point_cast_int(p->base.position.x),
                        fixed_point_cast_int(p->base.position.y),
                        &gladio_player_sprite,
                        0);
}

void gladio_player_damage(gladio_game_state *state, uint8_t damage) {
  if(gladio_player_vulnerable(&state->player)) {
    if(state->player.health > damage) {
      state->player.health -= damage;
    } else {
      state->player.health = 0;
      gladio_player_die(state);
    }
  }
}

void gladio_player_heal(gladio_player *p, uint8_t damage) {
  if(p->health + damage < GLADIO_PLAYER_MAX_HEALTH) {
    p->health += damage;
  } else {
    p->health = GLADIO_PLAYER_MAX_HEALTH;
  }
}

void gladio_player_die(struct gladio_game_state *state) {
  if(state->persistent->lives > 0) {
    --state->persistent->lives;
  }

  state->player.status = GLADIO_PLAYER_DYING;
  state->player.status_cooldown = EXPLOSION_TICKS / 3;
}

void gladio_player_status_tick(struct gladio_game_state *state) {
  if(state->player.status == GLADIO_PLAYER_WINNING) {
    if(state->player.status_cooldown > 32) {
      fixed_point ticks = FIXED_INT(state->player.status_cooldown - 32);
      fixed_point dx = fixed_point_div(                               state->player.base.position.x , ticks);
      fixed_point dy = fixed_point_div(fixed_point_sub(FIXED_INT(25), state->player.base.position.y), ticks);

      state->player.base.position.x = fixed_point_sub(state->player.base.position.x, dx);
      state->player.base.position.y = fixed_point_add(state->player.base.position.y, dy);

      --state->player.status_cooldown;
    } else if(state->player.status_cooldown > 0) {
      state->player.base.position.x = fixed_point_add(state->player.base.position.x, FIXED_INT(3));
      --state->player.status_cooldown;
    }
  } else if(state->player.status_cooldown != 0) {
    --state->player.status_cooldown;
  } else {
    switch(state->player.status) {
    case GLADIO_PLAYER_INVULNERABLE:
      state->player.status = GLADIO_PLAYER_NORMAL;
      break;
    case GLADIO_PLAYER_DYING:
      if(state->persistent->lives > 0) {
        state->player = gladio_player_new();
      } else {
        state->player.status = GLADIO_PLAYER_LOST;
      }
      break;
    }
  }
}
