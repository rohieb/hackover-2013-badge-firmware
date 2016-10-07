#ifndef INCLUDED_BADGE_GLADIO_SCREENS_H
#define INCLUDED_BADGE_GLADIO_SCREENS_H

#include "game_state.h"
#include "gladio.h"

void gladio_screen_level_intro(gladio_game_state *state, gladio_level_number const *lvnum);
void gladio_screen_game_over(gladio_game_state *state);
void gladio_screen_awesome(gladio_game_state *state);
void gladio_screen_scores(gladio_game_state *state);

#endif
