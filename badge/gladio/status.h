#ifndef INCLUDED_BADGE_GLADIO_STATUS_H
#define INCLUDED_BADGE_GLADIO_STATUS_H

#include "../ui/display.h"

#include "player.h"
#include "game_state.h"

void gladio_render_status_bar(badge_framebuffer       *fb,
                              gladio_player     const *player,
                              gladio_game_state const *state);

#endif
