#ifndef INCLUDED_BADGE2013_2048_RENDER_H
#define INCLUDED_BADGE2013_2048_RENDER_H

#include "../ui/display.h"

#include "game.h"

#define FIELD_SIZE 13

void render_intro(badge_framebuffer * fb);
void render_lost(badge_framebuffer * fb);
void invert_framebuffer(badge_framebuffer * fb);
void show_frame(badge_framebuffer * fb);
void render_score(badge_framebuffer * fb, uint32_t score);
void show_grid(badge_framebuffer * fb, uint8_t grid[GRID_SIZE][GRID_SIZE]);
void render_number(badge_framebuffer * fb, uint8_t number, uint8_t x, uint8_t y);
void clear_field(badge_framebuffer * fb, uint8_t x_grid, uint8_t y_grid);

#endif
