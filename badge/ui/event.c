#include "event.h"

#include <core/gpio/gpio.h>
#include <core/timer32/timer32.h>
#include <r0ketports.h>

// We depend on input being popped often, so no queue.
static badge_event_t volatile event_buffer;
static uint8_t       volatile event_flag;

enum {
  BADGE_EVENT_FLAG_INPUT = 1,
  BADGE_EVENT_FLAG_TIMER = 2
};

static uint8_t badge_input_raw(void) {
    uint8_t result = BTN_NONE;

    if (gpioGetValue(RB_BTN3) == 0) { result |= BADGE_EVENT_KEY_UP    ; }
    if (gpioGetValue(RB_BTN2) == 0) { result |= BADGE_EVENT_KEY_DOWN  ; }
    if (gpioGetValue(RB_BTN4) == 0) { result |= BADGE_EVENT_KEY_CENTER; }
    if (gpioGetValue(RB_BTN0) == 0) { result |= BADGE_EVENT_KEY_LEFT  ; }
    if (gpioGetValue(RB_BTN1) == 0) { result |= BADGE_EVENT_KEY_RIGHT ; }
    // TODO: Knöpfe.
    if (gpioGetValue(RB_HB0)  == 0) { result |= BADGE_EVENT_KEY_UP    ; }
    if (gpioGetValue(RB_HB1)  == 0) { result |= BADGE_EVENT_KEY_UP    ; }

    return result;
}

uint8_t badge_event_current_input_state(void) {
  return badge_event_new_input_state(event_buffer);
}

void badge_event_irq(void) {
  uint8_t old_state = badge_event_current_input_state();
  uint8_t new_state = badge_input_raw();

  if(new_state != old_state) {
    event_buffer = badge_event_new(BADGE_EVENT_USER_INPUT,
                                   old_state,
                                   new_state);
    event_flag |= BADGE_EVENT_FLAG_INPUT;
  }

  event_flag |= BADGE_EVENT_FLAG_TIMER;
}

badge_event_t badge_event_wait(void) {
  while(!event_flag)
    ;

  // User input takes precedence.
  if(event_flag & BADGE_EVENT_FLAG_INPUT) {
    badge_event_t event = event_buffer;
    event_flag &= ~BADGE_EVENT_FLAG_INPUT;
    return event;
  }

  uint8_t state = badge_event_current_input_state();
  event_flag &= ~BADGE_EVENT_FLAG_TIMER;
  return badge_event_new(BADGE_EVENT_GAME_TICK, state, state);
}

void badge_event_start(void) {
  timer32Init(0, TIMER32_CCLK_10MS * 2);
  timer32SetIntHandler(badge_event_irq);
  timer32Enable(0);
}
