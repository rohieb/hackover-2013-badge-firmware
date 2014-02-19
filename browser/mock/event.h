#ifndef INCLUDED_MOCK_SDL_EVENT_H
#define INCLUDED_MOCK_SDL_EVENT_H

#include <SDL/SDL.h>

#include <ui/event.h>

uint8_t mock_event_new_buttons(badge_event_t ev);
void    mock_event_update_keystate(SDL_Event const *event);
int     mock_event_poll(badge_event_t *dest);

#endif
