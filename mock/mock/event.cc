#include "event.h"

#include <glibmm/threads.h>
#include <queue>

namespace {
  Glib::Threads::Mutex      badge_event_mutex;
  Glib::Threads::Cond       badge_event_condition;
  std::queue<badge_event_t> badge_event_queue;
  uint8_t          volatile badge_event_input_state = 0;
}

uint8_t badge_event_current_input_state(void) {
  Glib::Threads::Mutex::Lock lock(badge_event_mutex);
  return badge_event_input_state;
}

void          badge_event_push(badge_event_t e) {
  Glib::Threads::Mutex::Lock lock(badge_event_mutex);

  badge_event_queue.push(e);

  if(badge_event_type(e) == BADGE_EVENT_USER_INPUT) {
    badge_event_input_state = badge_event_new_input_state(e);
  }

  badge_event_condition.signal();
}

badge_event_t badge_event_wait(void) {
  Glib::Threads::Mutex::Lock lock(badge_event_mutex);

  while(badge_event_queue.empty()) {
    badge_event_condition.wait(badge_event_mutex);
  }

  badge_event_t result = badge_event_queue.front();
  badge_event_queue.pop();

  return result;
}
