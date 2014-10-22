#include <stdlib.h>

#include "../../core/systick/systick.h"

#include "game.h"
#include "render.h"

enum {
  SPAWN_FALSE,
  SPAWN_TRUE
};

enum {
  END_FLAG_NO,
  END_FLAG_LOST,
  END_FLAG_NOW
};

enum {
  MOVE_UP,
  MOVE_DOWN,
  MOVE_RIGHT,
  MOVE_LEFT
};

enum {
  MOVE_IMPOSSIBLE,
  MOVE_POSSIBLE
};

enum {
  COULDNT_MOVE,
  COULD_MOVE
};

enum {
  NOT_REACHED_2048,
  REACHED_2048
};

uint8_t move_possible(uint8_t grid[GRID_SIZE][GRID_SIZE], game_2048_state *state) {
  uint8_t value, neighbor;

  for(uint8_t x = 0; x < 4; x++) {
    for(uint8_t y = 0; y < 4; y++) {
      value = grid[x][y];

      if(value == 0)
        return MOVE_POSSIBLE;

      if(y > 0) {
        neighbor = grid[x][y - 1];
        if(value == neighbor)
          return MOVE_POSSIBLE;
      }

      if(y < 3) {
        neighbor = grid[x][y + 1];
        if(value == neighbor)
          return MOVE_POSSIBLE;
      }

      if(x > 0) {
        neighbor = grid[x - 1][y];
        if(value == neighbor)
          return MOVE_POSSIBLE;
      }

      if(x < 3) {
        neighbor = grid[x + 1][y];
        if(value == neighbor)
          return MOVE_POSSIBLE;
      }
    }
  }

  state->end_flag = END_FLAG_LOST;
  return MOVE_IMPOSSIBLE;
}

uint32_t pow2(uint8_t power) {
  uint32_t pow2;
  pow2 = 1;

  for(uint8_t i = 0; i < power; i++) {
    pow2 *= 2;
  }

  return pow2;
}

void move(uint8_t grid[GRID_SIZE][GRID_SIZE], uint8_t direction, game_2048_state *state) {
  int8_t tmp, move_flag;
  uint8_t merged[GRID_SIZE] = { 0 };

  move_flag = COULDNT_MOVE;

  switch(direction) {
  case MOVE_UP:
    for(int8_t x = 0; x < 4; x++) {
      for(int8_t y = 1; y < 4; y++) {
        tmp = y;

        while(tmp > 0) {
          if(grid[x][tmp] > 0 && grid[x][tmp - 1] == 0) {
            grid[x][tmp - 1] = grid[x][tmp];
            grid[x][tmp--] = 0;
            move_flag = COULD_MOVE;
          } else if(grid[x][tmp] > 0 && grid[x][tmp - 1] == grid[x][tmp] &&
                    !((merged[tmp - 1] >> (3 - x)) & 1)) {
            grid[x][tmp - 1] += 1;
            grid[x][tmp] = 0;
            merged[tmp - 1] |= 1 << (3 - x);
            state->score += pow2(grid[x][tmp - 1]);
            if(grid[x][tmp - 1] == 11)
              state->reached_flag = REACHED_2048;
            tmp = 0;
            move_flag = COULD_MOVE;
          } else {
            tmp = 0;
          }
        }
      }
    }
    break;
  case MOVE_DOWN:
    for(int8_t x = 0; x < 4; x++) {
      for(int8_t y = GRID_SIZE - 2; y >= 0; y--) {
        tmp = y;

        while(tmp < 3) {
          if(grid[x][tmp] > 0 && grid[x][tmp + 1] == 0) {
            grid[x][tmp + 1] = grid[x][tmp];
            grid[x][tmp++] = 0;
            move_flag = COULD_MOVE;
          } else if(grid[x][tmp] > 0 && grid[x][tmp + 1] == grid[x][tmp] &&
                    !((merged[tmp + 1] >> (3 - x)) & 1)) {
            grid[x][tmp + 1] += 1;
            grid[x][tmp] = 0;
            merged[tmp + 1] |= 1 << (3 - x);
            state->score += pow2(grid[x][tmp + 1]);
            if(grid[x][tmp + 1] == 11)
              state->reached_flag = REACHED_2048;
            tmp = 3;
            move_flag = COULD_MOVE;
          } else {
            tmp = 3;
          }
        }
      }
    }
    break;
  case MOVE_RIGHT:
    for(int8_t y = 0; y < 4; y++) {
      for(int8_t x = GRID_SIZE - 2; x >= 0; x--) {
        tmp = x;

        while(tmp < 3) {
          if(grid[tmp][y] > 0 && grid[tmp + 1][y] == 0) {
            grid[tmp + 1][y] = grid[tmp][y];
            grid[tmp++][y] = 0;
            move_flag = COULD_MOVE;
          } else if(grid[tmp][y] > 0 && grid[tmp + 1][y] == grid[tmp][y] &&
                    !((merged[y] >> (3 - (tmp + 1))) & 1)) {
            grid[tmp + 1][y] += 1;
            grid[tmp][y] = 0;
            state->score += pow2(grid[tmp + 1][y]);
            merged[y] |= 1 << (3 - (tmp + 1));
            if(grid[tmp + 1][y] == 11)
              state->reached_flag = REACHED_2048;
            tmp = 3;
            move_flag = COULD_MOVE;
          } else {
            tmp = 3;
          }
        }
      }
    }
    break;
  case MOVE_LEFT:
    for(int8_t y = 0; y < 4; y++) {
      for(int8_t x = 1; x < 4; x++) {
        tmp = x;

        while(tmp > 0) {
          if(grid[tmp][y] > 0 && grid[tmp - 1][y] == 0) {
            grid[tmp - 1][y] = grid[tmp][y];
            grid[tmp--][y] = 0;
            move_flag = COULD_MOVE;
          } else if(grid[tmp][y] > 0 && grid[tmp - 1][y] == grid[tmp][y] &&
                    !((merged[y] >> (3 - (tmp - 1))) & 1)) {
            grid[tmp - 1][y] += 1;
            grid[tmp][y] = 0;
            merged[y] |= 1 << (3 - (tmp - 1));
            state->score += pow2(grid[tmp - 1][y]);
            if(grid[tmp - 1][y] == 11)
              state->reached_flag = REACHED_2048;
            tmp = 0;
            move_flag = COULD_MOVE;
          } else {
            tmp = 0;
          }
        }
      }
    }
    break;
  }

  if(move_flag)
    state->spawn_flag = SPAWN_TRUE;
}

void spawn_number(uint8_t grid[GRID_SIZE][GRID_SIZE], game_2048_state *state) {
  uint8_t x_grid, y_grid, value;

  for(;;) {
    x_grid = rand() % 4;
    y_grid = rand() % 4;

    if(grid[x_grid][y_grid] == 0) {
      value = rand() % 100;
      if(value < 90)
        grid[x_grid][y_grid] = 1;
      else
        grid[x_grid][y_grid] = 2;
      break;
    }
  }

  state->spawn_flag = SPAWN_FALSE;
}

void handle_input_event(uint8_t grid[GRID_SIZE][GRID_SIZE], game_2048_state *state) {
  switch(badge_event_current_input_state()) {
  case BADGE_EVENT_KEY_BTN_A:
  case BADGE_EVENT_KEY_BTN_B: state->end_flag = END_FLAG_NOW; break;
  case BADGE_EVENT_KEY_UP:    move(grid, MOVE_UP   , state); break;
  case BADGE_EVENT_KEY_DOWN:  move(grid, MOVE_DOWN , state); break;
  case BADGE_EVENT_KEY_RIGHT: move(grid, MOVE_RIGHT, state); break;
  case BADGE_EVENT_KEY_LEFT:  move(grid, MOVE_LEFT , state); break;
  }
}

void new_2048(void) {
  badge_framebuffer fb;
  badge_event_t ev;
  uint32_t old_score;
  uint8_t grid[GRID_SIZE][GRID_SIZE] = { { 0 } };
  game_2048_state state = { 0, 0, 0, 0 };

  srand(systickGetTicks());

  render_intro(&fb);

  state.spawn_flag = SPAWN_TRUE;
  state.end_flag = END_FLAG_NO;
  state.reached_flag = NOT_REACHED_2048;
  state.score = 0;
  old_score = 0;

  badge_framebuffer_clear(&fb);
  render_score(&fb, state.score);

  do {
    if(state.spawn_flag)
      spawn_number(grid, &state);

    show_frame(&fb);
    show_grid(&fb, grid);

    if(state.reached_flag == REACHED_2048) {
      for(uint8_t i = 0; i < 100;) {
        ev = badge_event_wait();
        if(badge_event_type(ev) == BADGE_EVENT_GAME_TICK)
          i++;
        if(i % 25 == 0)
          invert_framebuffer(&fb);
      }
      state.reached_flag = NOT_REACHED_2048;
    }

    if(state.score != old_score) {
      badge_framebuffer_clear(&fb);
      render_score(&fb, state.score);
      old_score = state.score;
    }

    if(!move_possible(grid, &state))
      break;

    ev = badge_event_wait();
    if(badge_event_type(ev) == BADGE_EVENT_USER_INPUT)
      handle_input_event(grid, &state);
  } while(state.end_flag == 0);

  if(state.end_flag == END_FLAG_NOW)
    return;
  else if(state.end_flag == END_FLAG_LOST) {
    render_lost(&fb);
    badge_framebuffer_clear(&fb);
    show_frame(&fb);
    show_grid(&fb, grid);
    render_score(&fb, state.score);

    for(uint8_t i = 0; i < 100;) {
      ev = badge_event_wait();
      if(badge_event_type(ev) == BADGE_EVENT_GAME_TICK)
        i++;
    }

    return;
  }
}
