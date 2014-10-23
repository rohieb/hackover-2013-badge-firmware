#include "screens.h"

#include "../ui/display.h"
#include "../ui/event.h"
#include "../ui/font.h"

#include <stdint.h>

// FIXME: I should really replace all these magic numbers with something sensible. Too late now.

void gladio_screen_game_over(gladio_game_state *state) {
  uint16_t i = 750;

  do {
    badge_event_t ev;

    ev = badge_event_wait();

    switch(badge_event_type(ev)) {
    case BADGE_EVENT_USER_INPUT:
      if(i < 700 && badge_event_new_buttons(ev)) {
        i = 0;
      }
      break;
    case BADGE_EVENT_GAME_TICK:
      {
        badge_framebuffer fb = { { { 0 } } };

        gladio_background_tick(state);

        gladio_background_render(&fb, &state->persistent->background);
        gladio_status_render(&fb, state);

        badge_framebuffer_render_text(&fb, 21, 32, "GAME OVER");

        badge_framebuffer_flush(&fb);
        --i;
      }
    }
  } while(i != 0);
}

void gladio_screen_level_intro(gladio_game_state *state, gladio_level_number const *lvnum) {
  char msg[4] = " - ";
  msg[0] = '0' + lvnum->world;
  msg[2] = '0' + lvnum->level;
  uint8_t i = 75;

  do {
    badge_event_t ev = badge_event_wait();

    if(badge_event_type(ev) == BADGE_EVENT_GAME_TICK) {
      badge_framebuffer fb = { { { 0 } } };

      gladio_background_tick(state);

      gladio_background_render(&fb, &state->persistent->background);
      gladio_status_render(&fb, state);

      badge_framebuffer_render_text(&fb, 33, 28, "Stage");
      badge_framebuffer_render_text(&fb, 36, 37, msg);

      badge_framebuffer_flush(&fb);

      --i;
    }
  } while(i != 0);
}

void gladio_screen_awesome(gladio_game_state *state) {
  uint16_t i = 0;

  do {
    badge_event_t ev;

    ev = badge_event_wait();

    switch(badge_event_type(ev)) {
    case BADGE_EVENT_USER_INPUT:
      break;
    case BADGE_EVENT_GAME_TICK:
      {
        badge_framebuffer fb = { { { 0 } } };

        gladio_background_tick(state);

        gladio_background_render(&fb, &state->persistent->background);
        gladio_status_render(&fb, state);

        badge_framebuffer_render_text(&fb, 27, 28, "You");
        if(i >= 75) {
          badge_framebuffer_render_text(&fb, 51, 28, "are");
        }

        int8_t left = 24;

        if(i >= 300) {
          left -= 9;
          badge_framebuffer_render_text(&fb, 66, 37, "1!!");
        }

        if(i >= 225) {
          left -= 3;
          badge_framebuffer_render_text(&fb, left + 48, 37, "!");
        }

        if(i >= 150) {
          badge_framebuffer_render_text(&fb, left, 37, "awesome!");
        }

        badge_framebuffer_flush(&fb);
        ++i;
      }
    }
  } while(i < 525);
}

static void gladio_render_score(badge_framebuffer *fb, uint32_t score, int8_t pos_y) {
  uint8_t pos_x = 72;

  if(score > GLADIO_SCORE_MAX) {
    badge_framebuffer_render_text(fb, 30, pos_y, "Cheater!");
  } else {
    do {
      badge_framebuffer_render_char(fb, pos_x, pos_y, score % 10 + '0');
      pos_x -= 6;
      score /= 10;
    } while(score != 0);
  }
}

void gladio_screen_scores(gladio_game_state *state) {
#ifdef __tumb__
  gladio_highscores hs = gladio_highscores_load();
  gladio_highscores_update(&hs, state->persistent->score);
  gladio_highscores_save(&hs);
#else
  gladio_highscores hs = { { 1234567, 12345, 1234 } };
#endif

  uint16_t i = 750;

  do {
    badge_event_t ev;

    ev = badge_event_wait();

    switch(badge_event_type(ev)) {
    case BADGE_EVENT_USER_INPUT:
      if(i < 700 && badge_event_new_buttons(ev)) {
        i = 0;
      }
      break;
    case BADGE_EVENT_GAME_TICK:
      {
        badge_framebuffer fb = { { { 0 } } };

        gladio_background_tick(state);

        gladio_background_render(&fb, &state->persistent->background);
        gladio_status_render(&fb, state);

        badge_framebuffer_render_text(&fb, 48, 10, "Score");
        gladio_render_score(&fb, state->persistent->score, 20);

        badge_framebuffer_render_text(&fb, 18, 30, "Highscores");
        gladio_render_score(&fb, hs.scores[0], 40);
        gladio_render_score(&fb, hs.scores[1], 49);
        gladio_render_score(&fb, hs.scores[2], 58);

        badge_framebuffer_flush(&fb);
        --i;
      }
    }
  } while(i != 0);
}
