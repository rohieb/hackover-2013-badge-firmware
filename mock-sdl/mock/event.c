#include "event.h"

#include <stdint.h>

enum {
  MOCK_KEY_UP    = 1,
  MOCK_KEY_LEFT  = 2,
  MOCK_KEY_DOWN  = 4,
  MOCK_KEY_RIGHT = 8
};

static badge_event_t volatile event_buffer;
static uint8_t                keystate_raw;

void mock_event_update_keystate(SDL_Event const *event) {
  switch(event->type) {
  case SDL_KEYDOWN:
    switch(event->key.keysym.sym) {
    case SDLK_e: keystate_raw |= MOCK_KEY_UP   ; break;
    case SDLK_s: keystate_raw |= MOCK_KEY_LEFT ; break;
    case SDLK_d: keystate_raw |= MOCK_KEY_DOWN ; break;
    case SDLK_f: keystate_raw |= MOCK_KEY_RIGHT; break;
    }
    break;
  case SDL_KEYUP:
    switch(event->key.keysym.sym) {
    case SDLK_e: keystate_raw &= ~MOCK_KEY_UP   ; break;
    case SDLK_s: keystate_raw &= ~MOCK_KEY_LEFT ; break;
    case SDLK_d: keystate_raw &= ~MOCK_KEY_DOWN ; break;
    case SDLK_f: keystate_raw &= ~MOCK_KEY_RIGHT; break;
    }
    break;
  }
}

uint8_t badge_event_current_input_state(void) {
  return badge_event_new_input_state(event_buffer);
}

int mock_event_poll(badge_event_t *dest) {
  uint8_t state = badge_event_current_input_state();
  if(keystate_raw != state) {
    event_buffer = badge_event_new(BADGE_EVENT_USER_INPUT, state, keystate_raw);
    *dest = event_buffer;
    return 1;
  }

  return 0;
}
