#ifndef INCLUDED_BADGE2013_2048_GAME_H
#define INCLUDED_BADGE2013_2048_GAME_H

#include "../ui/display.h"
#include "../ui/event.h"

#define GRID_SIZE 4

typedef struct {
  uint32_t score;
  uint8_t spawn_flag;
  uint8_t end_flag;
  uint8_t reached_flag;
} game_2048_state;

uint8_t move_possible(uint8_t grid[GRID_SIZE][GRID_SIZE], game_2048_state *state);
void move(uint8_t grid[GRID_SIZE][GRID_SIZE], uint8_t direction, game_2048_state *state);
void spawn_number(uint8_t grid[GRID_SIZE][GRID_SIZE], game_2048_state *state);
void handle_input_event(uint8_t grid[GRID_SIZE][GRID_SIZE], game_2048_state *state);
void new_2048(void);

#endif
