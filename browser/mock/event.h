#ifndef INCLUDED_MOCK_SDL_EVENT_H
#define INCLUDED_MOCK_SDL_EVENT_H

#include <SDL/SDL.h>

#include <ui/event.h>

void mock_event_update_keystate(SDL_Event const *event);
int mock_event_poll(badge_event_t *dest);

#endif
