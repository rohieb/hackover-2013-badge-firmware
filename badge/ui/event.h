#ifndef INCLUDED_BADGE2013_BADGE_UI_EVENT_H
#define INCLUDED_BADGE2013_BADGE_UI_EVENT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

  enum {
    BADGE_EVENT_USER_INPUT,
    BADGE_EVENT_GAME_TICK
  };

  enum {
    BADGE_EVENT_KEY_UP     =  1,
    BADGE_EVENT_KEY_LEFT   =  2,
    BADGE_EVENT_KEY_DOWN   =  4,
    BADGE_EVENT_KEY_RIGHT  =  8,
    BADGE_EVENT_KEY_CENTER = 16,
    BADGE_EVENT_KEY_BTN_A  = 32,
    BADGE_EVENT_KEY_BTN_B  = 64
  };

  typedef uint16_t badge_event_t;

  static inline uint8_t badge_event_type           (badge_event_t event) { return (uint8_t) (event      & 0x03); }
  static inline uint8_t badge_event_old_input_state(badge_event_t event) { return (uint8_t) (event >> 2 & 0x7f); }
  static inline uint8_t badge_event_new_input_state(badge_event_t event) { return (uint8_t) (event >> 9       ); }

  uint8_t badge_input_raw(void);
  uint8_t badge_event_current_input_state(void);

  static inline badge_event_t badge_event_new(uint8_t type, uint8_t old_input_state, uint8_t new_input_state) {
    return (badge_event_t) ((type & 0x3) | ((badge_event_t) (old_input_state & 0x7f) << 2) | ((badge_event_t) (new_input_state & 0x7f) << 9));
  }

  badge_event_t badge_event_wait(void);
  void          badge_event_start(void);

#ifdef __cplusplus
}
#endif

#endif
